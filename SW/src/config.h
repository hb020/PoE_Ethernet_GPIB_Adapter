#pragma once

#define DEVICE_NAME "Ethernet2GPIB Gateway v1.1 (AR488 v0.53.03)\n"

#define LED_R 13
#define LED_G 39
#define LED_B 38

// This is needed, and debugPort should point to Serial if you want to use the serial menu
#define DEBUG_ENABLE
// define USE_SERIALMENU if you want a basic menu on the serial console. Unfortunately, this library is hardcoded to use Serial.
#define USE_SERIALMENU

// define USE_WEBSERVER if you want to use a very basic web page server that serves a static explanation page.
// It is not interactive, just a cherry on the cake.
// This is meant to be used with the VXI server, but it might work with the Prologix server too.
#define USE_WEBSERVER

#ifndef INTERFACE_PROLOGIX
#define INTERFACE_VXI11
#endif
// and if you define both, well, you'll have to deal with the compiler telling you there is not enough ROM.


// for the Prologix server: 
#define AR_ETHERNET_PORT
#define PROLOGIX_PORT 1234

// For the VXI server:
#define VXI11_PORT 9010
#define MAX_VXI_CLIENTS 30
// set LOG_VXI_DETAILS to 0 or 1, depending on whether you want to see VXI details on the debugPort
// setting to 1 messes up the serial menu a bit
#define LOG_VXI_DETAILS 1



