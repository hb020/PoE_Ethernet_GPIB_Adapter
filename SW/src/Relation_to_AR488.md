# The relation to AR488

The GPIB part of this program is "forked" from https://github.com/Twilight-Logic/AR488, from ver. 0.53.03, 08/04/2025.

Since a proper fork was not possible seen the amount of changes and the specifics of the hardware, this file documents how the integration of the AR488 code was performed. Hoping it will help future code updates.

The files starting with `AR488` were copied to `\SW\src` and modified:

## AR488.ino

This is the 'main' file. It received a complete code shuffle, as the structure was not compatible with cohabitation with other socket servers and it was lacking forward declarations, making it incompatible with 'standard' compilers.

The file was renamed to 'prologix_server.cpp'. The code sections that remain mostly original, are marked as such.

## AR488_ComPorts.cpp and AR488_ComPorts.h

* DEVNULL externalised to become a dependency
* `startDataPort()` reworked into `void startDataPort(byte* mac,IPAddress ip)` so it can point to a `EthernetStream`.
* `void maintainDataPort()` added
* `printBuf()` added

## AR488_Config.h

* Set `#define AR488_CUSTOM`, plus content of the 2 relevant sections

## AR488_Eeprom.cpp and AR488_Eeprom.h

Not modified

## AR488_GPIBbus.cpp and AR488_GPIBbus.h

* Changed `void sendData(char *data, uint8_t dsize);` into `void sendData(const char *data, uint8_t dsize);`  (const)

## AR488_Layouts.cpp and AR488_Layouts.h

Not modified
