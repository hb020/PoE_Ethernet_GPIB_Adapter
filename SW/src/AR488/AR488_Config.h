#ifndef AR488_CONFIG_H
#define AR488_CONFIG_H

/*********************************************/
/***** AR488 GLOBAL CONFIGURATION HEADER *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/


/***** Firmware version *****/
#define FWVER "AR488 GPIB controller, ver. 0.51.29-eth_addon, 18/05/2024"


/***** BOARD CONFIGURATION *****/
/*
 * Platform will be selected automatically based on 
 * Arduino definition.
 * Only ONE board/layout should be selected per platform
 * Only ONE Serial port can be used to receive output
 */


/*** Custom layout ***/
/*
 * Uncomment to use custom board layout
 */
#define AR488_CUSTOM

/*
 * Configure the appropriate board/layout section
 * below as required
*/
#ifdef AR488_CUSTOM
  /* Board layout */
  /*
   * Define board layout in the AR488 CUSTOM LAYOUT
   * section below
   */
    #define LED_R 13
    #define LED_G 39
    #define LED_B 38
  /* Default serial port type */
  //#define AR_SERIAL_TYPE_HW
  #define AR_ETHERNET_PORT
//#define DEBUG_ENABLE
/*** UNO and NANO boards ***/
#elif __AVR_ATmega328P__
/* Board/layout selection */
#define AR488_UNO
//#define AR488_NANO
//#define AR488_MCP23S17

#endif  // Board/layout selection



/***** SERIAL PORT CONFIGURATION *****/
/*
* Note: On most boards the primary serial device is named Serial. On boards that have a secondary
*       UART port this is named Serial1. The Mega2560 also supports Serial2, Serial3 and Serial4.
*       When using layout AR488_MEGA2560_D, Serial2 pins are assigned as GPIO pins for the GPIB bus
*       so Serial2 is not available.
*/
/***** Communication port *****/
#define DATAPORT_ENABLE
#ifdef DATAPORT_ENABLE
// Serial port device
#define AR_SERIAL_PORT Serial
// #define AR_SERIAL_SWPORT
// Set port operating speed
#define AR_SERIAL_SPEED 115200

#endif

/***** Debug port *****/
//#define DEBUG_ENABLE
#ifdef DEBUG_ENABLE
// Serial port device
#define DB_SERIAL_PORT Serial
// #define DB_SERIAL_SWPORT
// Set port operating speed
#define DB_SERIAL_SPEED 115200
#endif

/***** Configure SoftwareSerial Port *****/
/*
* Configure the SoftwareSerial TX/RX pins and baud rate here
* Note: SoftwareSerial support conflicts with PCINT support
* When using SoftwareSerial, disable USE_INTERRUPTS.
*/
#if defined(AR_SERIAL_SWPORT) || defined(DB_SERIAL_SWPORT)
#define SW_SERIAL_RX_PIN 11
#define SW_SERIAL_TX_PIN 12
#endif
/*
* Note: SoftwareSerial reliable only up to a MAX of 57600 baud only
*/



/***** SUPPORT FOR PERIPHERAL CHIPS *****/
/*
* This sections priovides configuration to enable/disable support
* for SN7516x chips and the MCP23S17 GPIO expander.
*/




/***** MISCELLANEOUS OPTIONS *****/
/*
* Miscellaneous options
*/


/***** Device mode local/remote signal (LED) *****/
//#define REMOTE_SIGNAL_PIN 7



/***** Acknowledge interface is ready *****/
//#define SAY_HELLO



/***** DEBUG LEVEL OPTIONS *****/
/*
* Configure debug level options
*/

#ifdef DEBUG_ENABLE
// Main module
//#define DEBUG_SERIAL_INPUT    // serialIn_h(), parseInput_h()
//#define DEBUG_CMD_PARSER      // getCmd()
//#define DEBUG_SEND_TO_INSTR   // sendToInstrument();
//#define DEBUG_SPOLL           // spoll_h()
//#define DEBUG_DEVICE_ATN      // attnRequired()
//#define DEBUG_IDFUNC          // ID command

// AR488_GPIBbus module
//#define DEBUG_GPIBbus_RECEIVE // GPIBbus::receiveData(), GPIBbus::readByte()
//#define DEBUG_GPIBbus_SEND    // GPIBbus::sendData()
//#define DEBUG_GPIBbus_CONTROL // GPIBbus::setControls() 
//#define DEBUG_GPIB_COMMANDS   // GPIBbus::sendCDC(), GPIBbus::sendLLO(), GPIBbus::sendLOC(), GPIBbus::sendGTL(), GPIBbus::sendMSA() 
//#define DEBUG_GPIB_ADDRESSING // GPIBbus::sendMA(), GPIBbus::sendMLA(), GPIBbus::sendUNT(), GPIBbus::sendUNL() 
//#define DEBUG_GPIB_DEVICE     // GPIBbus::unAddressDevice(), GPIBbus::addressDevice

// GPIB layout
//#define DEBUG_LAYOUT

// EEPROM module
//#define DEBUG_EEPROM          // EEPROM

#endif


/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** AR488 GLOBAL CONFIGURATION HEADER *****/
/*********************************************/


/*******************************/
/***** AR488 CUSTOM LAYOUT *****/
/***** vvvvvvvvvvvvvvvvvvv *****/

#ifdef AR488_CUSTOM

#define DIO1_PIN  22  /*PD0 GPIB 1  */
#define DIO2_PIN  23  /*PD1 GPIB 2  */
#define DIO3_PIN  24  /*PD2 GPIB 3  */
#define DIO4_PIN  25  /*PD3 GPIB 4  */
#define DIO5_PIN  26  /*PD4 GPIB 13 */
#define DIO6_PIN  27  /*PD5 GPIB 14 */
#define DIO7_PIN  28  /*PD6 GPIB 15 */
#define DIO8_PIN  29  /*PD7 GPIB 16 */

#define EOI_PIN   14  /*PC0 GPIB 5  */
#define DAV_PIN   15  /*PC1 GPIB 6  */
#define NRFD_PIN  16  /*PC2 GPIB 7  */
#define NDAC_PIN  17  /*PC3 GPIB 8  */
#define IFC_PIN   18  /*PC4 GPIB 9  */
#define SRQ_PIN   19  /*PC5 GPIB 10 */
#define ATN_PIN   20  /*PC6 GPIB 11 */
#define REN_PIN   21  /*PC7 GPIB 17 */

#endif

/*
Data bus        DIO1-  1   13 - DIO5 Data bus
Data bus        DIO2-  2   14 - DIO6 Data bus
Data bus        DIO3-  3   15 - DIO7 Data bus
Data bus        DIO4-  4   16 - DIO8 Data bus
Management bus  EOI -  5   17 - REN (Remote Enable) Management bus
(End or Identify)                 (Data Valid)
Handshake bus   DAV -  6   18 - GND  (Ground)
(Data Valid)
Handshake bus  NRFD-  7   19 - GND  (Ground)
(Not Ready for Data)
Handshake bus  NDAC-  8   20 - GND  (Ground)
(No Data Accepted)
Management bus  IFC -  9   21 - GND  (Ground)
(Interface Clear)
Management bus  SRQ - 10   22 - GND  (Ground)
(Service Request)
Management bus  ATN - 11   23 - GND  (Ground)
(Attention)
GND            GND - 12   24 - Logic GND (Ground)
*/


/***** ^^^^^^^^^^^^^^^^^^^ *****/
/***** AR488 CUSTOM LAYOUT *****/
/*******************************/



/********************************/
/***** AR488 MACROS SECTION *****/
/***** vvvvvvvvvvvvvvvvvvvv *****/

/*
* (See the AR488 user manual for details)
*/

/***** Enable Macros *****/
/*
* Uncomment to enable macro support. The Startup macro allows the
* interface to be configured at startup. Macros 1 - 9 can be
* used to execute a sequence of commands with a single command
* i.e, ++macro n, where n is the number of the macro
* 
* USE_MACROS must be enabled to enable the macro feature including 
* MACRO_0 (the startup macro). RUN_STARTUP must be uncommented to 
* run the startup macro when the interface boots up
*/
//#define USE_MACROS    // Enable the macro feature
//#define RUN_STARTUP   // Run MACRO_0 (the startup macro)

#ifdef USE_MACROS

/***** Startup Macro *****/

#define MACRO_0 "\
++addr 9\n\
++auto 2\n\
*RST\n\
:func 'volt:ac'\
"
/* End of MACRO_0 (Startup macro)*/

/***** User macros 1-9 *****/

#define MACRO_1 "\
++addr 3\n\
++auto 0\n\
M3\n\
"
/*<-End of macro*/

#define MACRO_2 "\
"
/*<-End of macro 2*/

#define MACRO_3 "\
"
/*<-End of macro 3*/

#define MACRO_4 "\
"
/*<-End of macro 4*/

#define MACRO_5 "\
"
/*<-End of macro 5*/

#define MACRO_6 "\
"
/*<-End of macro 6*/

#define MACRO_7 "\
"
/*<-End of macro 7*/

#define MACRO_8 "\
"
/*<-End of macro 8*/

#define MACRO_9 "\
"
/*<-End of macro 9*/


#endif
/***** ^^^^^^^^^^^^^^^^^^^^ *****/
/***** AR488 MACROS SECTION *****/
/********************************/


/******************************************/
/***** !!! DO NOT EDIT BELOW HERE !!! *****/
/******vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv******/



/*********************************************/
/***** MISCELLANEOUS DECLARATIONS *****/
/******vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv******/

#define AR_CFG_SIZE 84

/******^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^******/
/***** MISCELLANEOUS DECLARATIONS *****/
/*********************************************/





#endif // AR488_CONFIG_H
