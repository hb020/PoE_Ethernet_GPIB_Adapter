#ifdef INTERFACE_VXI11
// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wtype-limits"
// #pragma GCC diagnostic ignored "-Wunused-variable"

// STATUS:
// WIP, just tested an echo server on 4 ports. This uses the AR488_EthernetStream class
// that uses a String as output buffer (flushed upon \n). That could be improved and changed to a static buffer.
// But main problem is the auto switching between LISTEN and TALK. VXI-11 is better for that, as it is explicit.

// VXI-11.2

// Example of connection string:

// 'normal' vxi-11: TCPIP::192.168.1.105::INSTR           implies inst0, see below
//                  TCPIP::192.168.1.105::instN::INSTR    
//  VXI-11.2        TCPIP::192.168.1.105::gpibN,A::INSTR  (or hpibN) A is gpib bus primary address. 0 reserved for the controller
// in all cases, N = interface number. Default is 0. When 0, it can be omitted
//
// VXI-11 Requires 3 servers:
// * 2 port mappers on UDP and TCP that point to the VXI-11 RPC server
// * 1 VXI-11 RPC server. VXI-11 consists of 3 separate RPC programs with the numeric identifiers: 0x0607AF (DEVICE_CORE), 0x0607B0 (DEVICE_ASYNC) and 0x0607B1 (DEVICE_INTR).
// * you may also publish the service presence via mDNS, but this is not required.
//
// For compatibility with basic pyvisa use, you only need DEVICE_CORE, but the guidelines say you should also support the other RPC programs.
//
// While in theory one could re-use an existing VXI-11 RPC server socket for multiple connections, 
// pyvisa requires 1 RPC client socket per device. It doesn't just use create_link on the same socket with a different device name.
//
// VXI-11 works this way:
// * a request for an open port is sent to the port mapper (in UDP or TCP), which returns a port number.
// * the client then connects to the RPC server on that port, with a CREATE_LINK request and device to connect to (inst0, gpib,2 etc). 
// * the RPC server checks if there are resources available and if so, replies to the client with a link ID.
// * the client then uses that port and link ID for DEVICE_WRITE and DEVICE_READ operations
// * when done, the client sends a DESTROY_LINK command to the RPC server, which will close the link and closes the socket.
//
// The link ID is used to map with the device address

#ifdef __AVR__
#include <avr/wdt.h>
#endif

// #pragma GCC diagnostic pop

#include "config.h"
#include "AR488_Config.h"
#include "AR488_GPIBbus.h"
#include "AR488_ComPorts.h"
#include "AR488_Eeprom.h"

#include "24AA256UID.h"
#include "AR488_EthernetStream.h"
#include "user_interface.h"
#include "rpc_bind_server.h"
#include "vxi_server.h"

/****** Global variables with volatile values related to controller state *****/

// External EEPROM with MAC and unique ID.
_24AA256UID eeprom(0x50, true);

// GPIB bus object
GPIBbus gpibBus;

#pragma region SCPI handler

// #define DUMMY_DEVICE

/**
 * @brief a helper class to capture data printed by receiveData (using Stream.write(uint8_t ch)) to an external buffer
 */
class bufStream : public Stream {
   public:
    bufStream(char *buf, size_t size) : buffer(buf), bufferSize(size) {}

    size_t write(uint8_t ch) override {
        // debugPort.print((char)ch);
        if (buffer_pos < bufferSize) {
            buffer[buffer_pos++] = ch;
            return 1;
        }
        return 0;
    }

    int available() { return 0; }  // dummy
    int read() { return 0; }       // dummy
    int peek() { return 0; }       // dummy

    size_t len(void) { return buffer_pos; }

    void flush() {
        buffer_pos = 0;  // clear the buffer
    }

   private:
    char *buffer;
    size_t bufferSize;
    size_t buffer_pos = 0;
};

/**
 * @brief SCPI handler interface
 *
 * This class handles the communication between the VXI servers and the SCPI parser or the devices.
 */
class SCPI_handler : public SCPI_handler_interface {
   public:
    SCPI_handler() {}

    void write(int address, const char *data, size_t len) override {
#ifdef DUMMY_DEVICE
        debugPort.print(F("SCPI write: "));
        printBuf(data, len);
#else
        if (address == 0) return;

        // Send data to the GPIB bus
        gpibBus.cfg.paddr = address;
        if (!gpibBus.haveAddressedDevice()) gpibBus.addressDevice(gpibBus.cfg.paddr, LISTEN);
        gpibBus.sendData(data, len);
        gpibBus.unAddressDevice();
#endif
    }

    bool read(int address, char *data, size_t *len, size_t max_len) override {
#ifdef DUMMY_DEVICE
        // Simulate a device response
        memset(data, 0, max_len);
        *len = snprintf(data, max_len, "SCPI response");
        return true;
#else
        // dummy reply if I am addressed
        if (address == 0) {
            strncpy(data, "Ethernet to GPIB device\n", max_len);
            *len = strlen(data);
            return true;  // no address
        }
        bufStream buf = bufStream(data, max_len);  ///< Buffer stream for incoming data

        bool readWithEoi = true;
        bool detectEndByte = false;
        uint8_t endByte = 0;

        gpibBus.cfg.paddr = address;
        gpibBus.addressDevice(gpibBus.cfg.paddr, TALK);     // tel device 'paddr' to talk. If you do this and the device has nothing to say, you might get an error.
        gpibBus.receiveData(buf, readWithEoi, detectEndByte, endByte);  // get the data from the bus and send out
        *len = buf.len();
        return true;
#endif
    }

    bool claim_control() override {
        // not needed for the GPIB bus, is done differently
        return true;
    }
    void release_control() override {
        // not needed for the GPIB bus, is done differently
    }

};

#pragma endregion

#pragma region VXI related Socket servers and helpers

static SCPI_handler scpi_handler;                    ///< The bridge from the vxi server to the SCPI command handler
static VXI_Server vxi_server(scpi_handler);          ///< The vxi server
static RPC_Bind_Server rpc_bind_server(vxi_server);  ///< The RPC_Bind_Server for the vxi server

#pragma endregion

#pragma region Setup and loop functions

#define LOG_DETAILS true
/**
 * @brief Setup function
 *
 * This function is called once at startup. It initializes the LED, serial port, and Ethernet connection.
 * It also sets up the EEPROM and GPIB bus configuration. The function tries to wait for DHCP to assign an IP address.
 */
void setup() {
    setup_serial_ui_and_led(F("Starting VXI-11.2 socket server GPIB interface..."));

    // Disable the watchdog (needed to prevent WDT reset loop)
#ifdef __AVR__
    wdt_disable();
#endif

    eeprom.begin();
    uint8_t macAddress[6];
    eeprom.getMACAddress(macAddress);

    debugPort.print(F("MAC Address: "));
    printHexArray(macAddress, 6);
    debugPort.println(F("Waiting for DHCP..."));

    // Initialise dataport, serial or ethernet as defined
    IPAddress ip = (0, 0, 0, 0);
    Ethernet.init(7);
    if (ip == IPAddress(0, 0, 0, 0)) {
        // Use DHCP
        Ethernet.begin(macAddress);
    } else {
        // Use static IP
        Ethernet.begin(macAddress, ip);
    }

    // print the IP address
    setup_ipaddress_surveillance_and_show_address();
    // for now, just ignore if we have a good address

    debugPort.println(F("Starting TCP server..."));
    vxi_server.begin(5025, LOG_DETAILS);

    debugPort.println(F("Starting port mappers on TCP and UDP..."));
    rpc_bind_server.begin(LOG_DETAILS);
    debugPort.println(F("VXI-11 servers started"));

    // The following section is not needed if you do not use EEPROM to store gpibBus.cfg between startups.
#ifdef E2END
    // debugPort.println(F("EEPROM detected"));
    // Read data from non-volatile memory
    //(will only read if previous config has already been saved)
    if (!isEepromClear()) {
        debugPort.println(F("EEPROM has data, restoring gpib bus config from EEPROM."));
        if (!epReadData(gpibBus.cfg.db, GPIB_CFG_SIZE)) {
            // CRC check failed - config data does not match EEPROM
            debugPort.println(F("CRC check failed. Erasing EEPROM...."));
            epErase();
            gpibBus.setDefaultCfg();
            //      initAR488();
            epWriteData(gpibBus.cfg.db, GPIB_CFG_SIZE);
            debugPort.println(F("EEPROM data set to default."));
        }
    }
#endif

    // Start the interface in the configured mode
    debugPort.println(F("Starting GPIB bus..."));
    gpibBus.begin();

    end_of_setup();
}

/***** ARDUINO MAIN LOOP *****/

/**
 * @brief This is the main loop.
 */
void loop() {

    loop_serial_ui_and_led(vxi_server.nr_connections());
    // loop_ethernet();
    rpc_bind_server.loop();
    vxi_server.loop();
}
#pragma endregion
#endif  // INTERFACE_VXI11