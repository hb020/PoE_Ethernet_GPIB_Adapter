#include <Arduino.h>
#include <Ethernet.h>
#include "config.h"
#include "AR488_ComPorts.h"
#include "user_interface.h"
#ifdef USE_SERIALMENU
#include <SerialMenuCmd.h>
#endif
#ifdef USE_WEBSERVER
#include "web_server.h"
BasicWebServer webServer;
#endif

#ifndef DEBUG_ENABLE
#error "DEBUG_ENABLE must be defined"
#endif

#ifdef USE_SERIALMENU
#pragma region Serial Menu
SerialMenuCmd myMenu;

void cmd1_DoIt(void) {}
void cmd2_DoIt(void) {}

tMenuCmdTxt txt1_DoIt[] = "a - Set the IP address";
tMenuCmdTxt txt2_DoIt[] = "b - Reboot";
tMenuCmdTxt txt3_DisplayMenu[] = "? - Menu";
tMenuCmdTxt txt_Prompt[] = "";

stMenuCmd list[] = {
    {txt1_DoIt, 'a', cmd1_DoIt},
    {txt2_DoIt, 'b', cmd2_DoIt},
    {txt3_DisplayMenu, '?', []() { myMenu.ShowMenu();
        myMenu.giveCmdPrompt();}}};

#define NbCmds sizeof(list) / sizeof(stMenuCmd)

void setup_serial_menu(void) {
    myMenu.begin(list, NbCmds, txt_Prompt);
}


void loop_serial_menu(void) {
    int8_t ret = myMenu.UserRequest();
    if (ret > 0) {
        myMenu.ExeCommand(ret);
    }
}

#pragma endregion
#endif

#pragma region IP ADDRESS checking

static IPAddress _previous_address(0, 0, 0, 0);
static bool ip_address_is_wrong = false;

// forward declarations
bool is_valid_IP_assigned(IPAddress current_address);
void LEDRed(void);

/**
 * @brief Set the up ipaddress surveillance
 * 
 * @return true if address is valid
 */
bool setup_ipaddress_surveillance_and_show_address(void) {
    _previous_address = Ethernet.localIP();
    debugPort.print(F("IP Address: "));
    debugPort.println(_previous_address);
    if (!is_valid_IP_assigned(_previous_address)) {
        debugPort.println(F("!! No valid IP address assigned. Please check your network settings."));
        ip_address_is_wrong = true;
        LEDRed();
        return false;
    }
    return true;
}

bool has_address_changed_since_start(void) {
    IPAddress current_address = Ethernet.localIP();
    if (current_address != _previous_address) {
        // previous_address = current_address;        
        ip_address_is_wrong = true;
        return true;
    }
    return false;
}

bool is_valid_IP_assigned(IPAddress current_address) {
    // TODO should also look at those MS style 'local addresses', but don't know if Arduino has that.
    return (current_address[0] != 0 || current_address[1] != 0 || current_address[2] != 0 || current_address[3] != 0);
}

#pragma endregion

#pragma region LEDS

uint8_t calculateBrightness(uint16_t count, uint16_t scale) {
    // Scale the count to fit within the range of 0-256 in a triangular way
    // so must get 0-511 first
    uint32_t p = (count * 512L) / scale;
    if (p > 511) {
        p = 0;
    }
    if (p > 255) {
        p = 511 - p;
    }
    return (uint8_t)p;
}

void LEDPulse(bool pulse_r = true, bool pulse_g = true, bool pulse_b = true) {
    // This configuration is based on different speeds. 
    // You can also use same speed but different phases
    unsigned long currentMillis = millis();
    uint16_t scale;
    uint16_t offset = 0;
    if (pulse_r) {
        scale = 5000; // 5 secs cycle
        uint16_t brightnessR = calculateBrightness((currentMillis + offset)%scale, scale); // Calculate brightness
        analogWrite(LED_R, brightnessR);
    }
    if (pulse_g) {
        scale = 4000; // 4 secs cycle
        uint16_t brightnessG = calculateBrightness((currentMillis + offset)%scale, scale); // Calculate brightness
        analogWrite(LED_G, brightnessG);
    }
    if (pulse_b) {
        scale = 1000; // 1 secs cycle
        uint16_t brightnessB = calculateBrightness((currentMillis + offset)%scale, scale); // Calculate brightness
        analogWrite(LED_B, brightnessB);
    }
}

void LEDOff(bool set_r = true, bool set_g = true, bool set_b = true) {
    if (set_r) {
        digitalWrite(LED_R, HIGH);
    }
    if (set_g) {
        digitalWrite(LED_G, HIGH);
    }
    if (set_b) {
        digitalWrite(LED_B, HIGH);
    }
}
void LEDOn(void) {
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, LOW);
}
void LEDRed(void) {
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, HIGH);
}
void LEDGreen(bool set_r = true, bool set_b = true) {
    if (set_r) {
        digitalWrite(LED_R, HIGH);
    }
    digitalWrite(LED_G, LOW);
    if (set_b) {
        digitalWrite(LED_B, HIGH);
    }
}
void LEDBlue(void) {
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, LOW);
}

void setup_led(void) {
    pinMode(LED_R, OUTPUT);
    pinMode(LED_G, OUTPUT);
    pinMode(LED_B, OUTPUT);
    LEDBlue();
}

void loop_led(bool has_clients) {
    if (ip_address_is_wrong) {
        LEDRed();
    } else {
        if (has_clients) {
            // no red, half green, pulse blue
            digitalWrite(LED_R, LOW);
            analogWrite(LED_B, 128);
            LEDPulse(false, false, true);
        } else {
            // all off, pulse green
            LEDOff(true, false, true);
            LEDPulse(false, true, false);
        }
    }

}

#pragma endregion

#pragma region UI Controller

int freeRam(void) {
    /* for AVR, not ARM */
    extern int __heap_start, *__brkval;
    int v;
    return (int)&v - (__brkval == 0
                          ? (int)&__heap_start
                          : (int)__brkval);
}

void display_freeram(void) {
    debugPort.print(F("- SRAM left: "));
    debugPort.print(freeRam());
}

bool onceASecond(bool start = false) {
    static const unsigned long REFRESH_INTERVAL = 1000;  // ms
    static unsigned long lastRefreshTime = 0;

    if (start) {
        // Reset the last refresh time
        lastRefreshTime = millis();
        return false;
    }

    if (millis() - lastRefreshTime >= REFRESH_INTERVAL) {
        lastRefreshTime += REFRESH_INTERVAL;
        return true;
    } else {
        return false;
    }
}

/// This function is called once at startup. It initializes the LED, and serial port.
// It is to be called at the very start, before network initialization.
void setup_serial_ui_and_led(const __FlashStringHelper* helloStr) {
    setup_led();
    startDebugPort();
    debugPort.println(helloStr);
    display_freeram();
    debugPort.println("");
}

// This function is called once at the end of setup.
// It is to be called at the end of setup, after network initialization.
void end_of_setup(void) {
    onceASecond(true);
    if (ip_address_is_wrong) {
        LEDRed();
    } else {
        LEDGreen();
    }
#ifdef USE_WEBSERVER
    debugPort.println(F("Starting Web server..."));
    webServer.begin();
#endif

#ifdef USE_SERIALMENU
    debugPort.println(F("Starting Serial Menu..."));
    setup_serial_menu();
#endif

    debugPort.println(F("Setup complete."));
    
#ifdef USE_SERIALMENU
    myMenu.ShowMenu();
    myMenu.giveCmdPrompt();
#endif
}

int counter = 0;
/**
 * @brief run the UI (led, web, serial, whatever is configured)
 * 
 * @param nrConnections number of clients connected
 */
void loop_serial_ui_and_led(int nrConnections) {

    loop_led(nrConnections > 0);

#ifdef USE_SERIALMENU
    loop_serial_menu();
#endif
#ifdef USE_WEBSERVER
    webServer.loop(nrConnections);
#endif

    if (onceASecond(false)) {
        if (ip_address_is_wrong) {
            debugPort.print(F("IP Address "));
            debugPort.print(Ethernet.localIP());
            debugPort.println(F(" is wrong. Please reboot!"));
            LEDRed();
        } else if (has_address_changed_since_start()) {
            debugPort.print(F("!! IP Address changed: "));
            debugPort.print(Ethernet.localIP());
            debugPort.println(F("  Please reboot!"));
            LEDRed();
        }

        debugPort.print('\r');
        display_freeram();
        debugPort.print(F(", Clients: "));
        debugPort.print(nrConnections);
        debugPort.print('\r');
        /*
        // LED test
        counter++;
        switch (counter/2){
            case 0:
                LEDRed();
                debugPort.println(F("Red"));
                break;
            case 1:
                LEDGreen();
                debugPort.println(F("Green"));
                break;
            case 2:
                LEDBlue();
                debugPort.println(F("Blue"));
                break;
            case 3:
                LEDOff();
                debugPort.println(F("Off"));
                break;
            case 4:
                LEDOn();
                debugPort.println(F("On"));
                break;
            default:
                counter = 0;
                break;
        }
        */
    }
}


#pragma endregion