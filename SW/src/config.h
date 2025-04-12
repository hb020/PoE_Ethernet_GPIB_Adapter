#pragma once

#define LED_R 13
#define LED_G 39
#define LED_B 38
#define DEBUG_ENABLE

#define VXI11_PORT 9010
#define MAX_VXI_CLIENTS 30

#define DEVICE_NAME "Ethernet2GPIB Gateway v1.1 (AR488 v0.53.03)\n"

#ifndef INTERFACE_PROLOGIX
#define INTERFACE_VXI11
#endif

// and if you define both, well, you'll have to deal with the compiler telling you there is not enough ROM.