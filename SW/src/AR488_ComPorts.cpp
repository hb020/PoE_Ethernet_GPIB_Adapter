#include <Arduino.h>
#include "AR488_ComPorts.h"
#include "24AA256UID.h"
/***** AR488_ComPorts.cpp, ver. 0.51.18, 26/02/2023 *****/


/***** DEVNULL Library *****
 *  AUTHOR: Rob Tillaart
 *  VERSION: 0.1.5
 *  PURPOSE: Arduino library for a /dev/null stream - useful for testing
 *  URL: https://github.com/RobTillaart/DEVNULL
 */

DEVNULL::DEVNULL()
{
  setTimeout(0);        //  no timeout.
  _bottomLessPit = -1;  //  nothing in the pit
}

int  DEVNULL::available()
{
  return 0;
};

int  DEVNULL::peek()
{
  return EOF;
};

int  DEVNULL::read()
{
  return EOF;
};

//  placeholder to keep CI happy
void DEVNULL::flush()
{
  return;
};

size_t DEVNULL::write(const uint8_t data)
{
  _bottomLessPit = data;
  return 1;
}

size_t DEVNULL::write( const uint8_t *buffer, size_t size)
{
  if (size > 0) _bottomLessPit = buffer[size - 1];
  return size;
}

int DEVNULL::lastByte()
{
  return _bottomLessPit;
}



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



