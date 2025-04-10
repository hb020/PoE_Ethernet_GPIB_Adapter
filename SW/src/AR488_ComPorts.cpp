#include <Arduino.h>
#include "AR488_ComPorts.h"
#include "24AA256UID.h"
#include <DEVNULL.h>
/***** AR488_ComPorts.cpp, ver. 0.51.18, 26/02/2023 *****/

/***************************************/
/***** Serial Port implementations *****/
/***************************************/


/****************************/ 
/***** DATA SERIAL PORT *****/
/****************************/

#ifdef DATAPORT_ENABLE

#ifdef AR_ETHERNET_PORT
  #include "AR488_EthernetStream.h"
  EthernetStream* ethernetPort = nullptr;
#else
  #ifdef AR_SOFTWARE_SERIAL
    #include <SoftwareSerial.h>
    SoftwareSerial swSerial(SW_SERIAL_RX_PIN, SW_SERIAL_TX_PIN);
  #endif
#endif

Stream* dataPort = nullptr;

void startDataPort(byte* mac, IPAddress ip) {
  #ifdef AR_ETHERNET_PORT
    //byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Temporary hard coded MAC
    //IPAddress ip(0, 0, 0, 0); // Same
    ethernetPort = new EthernetStream(mac, ip, 1234);
    
    dataPort = ethernetPort;
  
    ethernetPort->begin();

    
  #else
  #ifdef AR_SOFTWARE_SERIAL
    swSerial.begin(AR_SERIAL_SPEED);
    dataPort = &swSerial;
  #else
    AR_SERIAL_PORT.begin(AR_SERIAL_SPEED);
    dataPort = &AR_SERIAL_PORT;
  #endif
  #endif
}

void maintainDataPort() {
#ifdef AR_ETHERNET_PORT
    if (ethernetPort) {
        ethernetPort->maintain();
    }
#endif
}


#else

  DEVNULL _dndata;
  Stream& dataPort = _dndata;

#endif  // DATAPORT_ENABLE



/*****************************/ 
/***** DEBUG SERIAL PORT *****/
/*****************************/

#ifdef DEBUG_ENABLE
  #ifdef DB_SERIAL_SWPORT

    SoftwareSerial debugPort(SW_SERIAL_RX_PIN, SW_SERIAL_TX_PIN);

    void startDebugPort() {
      debugPort.begin(DB_SERIAL_SPEED);
    }
  
  #else

    Stream& debugPort = DB_SERIAL_PORT;

    void startDebugPort() {
      DB_SERIAL_PORT.begin(DB_SERIAL_SPEED);
    }

  #endif

  void printBuf(const char *data, size_t len) {
    debugPort.print("\"");
    for (size_t i = 0; i < len; i++) {
      char ch = data[i];
      if (ch == '\n') {
        debugPort.print("\\n");
      } else if (ch == '\r') {
        debugPort.print("\\r");
      } else if (ch == '\t') {
        debugPort.print("\\t");
      } else if (ch < 0x20 || ch > 0x7E) {
        debugPort.print("\\x");
        debugPort.print(ch, HEX);
      } else {
        debugPort.print(ch);
      }
    }
    debugPort.print("\" (length: ");
    debugPort.print(len);
    debugPort.print(")\n");
  }

  void printHex(uint8_t byteval) {
    char x[4] = {'\0'};
    sprintf(x,"%02X ", byteval);
    debugPort.print(x);
  }

  void printHexBuf(char * buf, size_t bsize){
    for (size_t i = 0; i < bsize; i++) {
      printHex(buf[i]);
    }
    debugPort.println();
  }
  
  void printHexArray(uint8_t barray[], size_t asize){
    for (size_t i = 0; i < asize; i++) {
      printHex(barray[i]);
    }
    debugPort.println();
  }

#else

  DEVNULL _dndebug;
  Stream& debugPort = _dndebug;

#endif  // DEBUG_ENABLE



