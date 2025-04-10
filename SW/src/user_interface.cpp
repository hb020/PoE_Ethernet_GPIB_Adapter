#include <Arduino.h>
#include "AR488_Config.h"
#include "AR488_ComPorts.h"
#include "user_interface.h"

#ifndef DEBUG_ENABLE
#error "DEBUG_ENABLE must be defined"
#endif

#pragma region LEDS

void setup_led() {
    pinMode(LED_R, OUTPUT);
    pinMode(LED_G, OUTPUT);
    pinMode(LED_B, OUTPUT);
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, HIGH);
}

uint16_t calculateBrightness(uint32_t phase) {
    uint16_t p = phase >> 10;
    if (p > 32768)
        p = 65535 - p;
    return p + p;
}

uint32_t pulseR = 0;
uint32_t pulseG = 0;
uint32_t pulseB = 0;
void LEDPulse(void) {
    // Increment phases
    pulseR += 248;
    pulseG += 420;
    pulseB += 690;

    // Calculate brightness for each LED using triangle wave logic
    uint16_t brightnessR = calculateBrightness(pulseR);
    uint16_t brightnessG = calculateBrightness(pulseG);
    uint16_t brightnessB = calculateBrightness(pulseB);

    // Set LEDs based on calculated brightness
    analogWrite(LED_R, brightnessR >> 8);  // Scale down to 8-bit value
    analogWrite(LED_G, brightnessG >> 8);  // Scale down to 8-bit value
    analogWrite(LED_B, brightnessB >> 8);  // Scale down to 8-bit value
}

// TODO: use connection status for led color
void loop_led() {
    LEDPulse();
}

#pragma endregion

#pragma region SERIAL

int freeRam() {
    /* for AVR, not ARM */
    extern int __heap_start, *__brkval;
    int v;
    return (int)&v - (__brkval == 0
                          ? (int)&__heap_start
                          : (int)__brkval);
}

void display_freeram() {
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

void setup_serial(const __FlashStringHelper* helloStr) {
    startDebugPort();
    debugPort.println(helloStr);
    display_freeram();
    debugPort.println("");
}

/**
 * @brief print debug information
 * 
 * @param pBusy pointer to busy flag of each of the servers
 * @param num_servers number of servers
 */
void loop_serial(bool *pBusy, size_t num_servers) {
    if (debugPort.available()) {
        char c = Serial.read();
        debugPort.print(F("Received: "));
        debugPort.println(c);
    }
    if (onceASecond(false)) {
        display_freeram();
        debugPort.print(F(", Servers: "));
        for (int i = 0; i < num_servers; i++) {
            debugPort.print(pBusy[i]);
            debugPort.print(" ");
        }
        debugPort.println();
    }
}

void end_of_setup() {
    debugPort.println(F("Setup complete."));
    onceASecond(true);
}

#pragma endregion