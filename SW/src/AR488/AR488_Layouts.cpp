#include <Arduino.h>

#include "AR488_Config.h"
#include "AR488_Layouts.h"

/***** AR488_Hardware.cpp, ver. 0.51.28, 16/02/2024 *****/

/*
 * Hardware layout function definitions
 */



/*************************************/
/***** CUSTOM PIN LAYOUT SECTION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvv *****/
#if defined (AR488_CUSTOM) || defined (NON_ARDUINO)


/***** Set the GPIB data bus to input pullup *****/
void readyGpibDbus() {
  // Set all PORTD pins (DIO1–DIO8) to input with pull-up resistors
  PORTD.DIRCLR = 0xFF;          // Set PORTD pins 0–7 as input
  PORTD.PIN0CTRL = PORT_PULLUPEN_bm; // Enable pull-up on PORTD pin 0
  PORTD.PIN1CTRL = PORT_PULLUPEN_bm; // Enable pull-up on PORTD pin 1
  PORTD.PIN2CTRL = PORT_PULLUPEN_bm; // Enable pull-up on PORTD pin 2
  PORTD.PIN3CTRL = PORT_PULLUPEN_bm; // Enable pull-up on PORTD pin 3
  PORTD.PIN4CTRL = PORT_PULLUPEN_bm; // Enable pull-up on PORTD pin 4
  PORTD.PIN5CTRL = PORT_PULLUPEN_bm; // Enable pull-up on PORTD pin 5
  PORTD.PIN6CTRL = PORT_PULLUPEN_bm; // Enable pull-up on PORTD pin 6
  PORTD.PIN7CTRL = PORT_PULLUPEN_bm; // Enable pull-up on PORTD pin 7
}

/***** Read the GPIB data bus wires to collect the byte of data *****/
uint8_t readGpibDbus() {
  // Read the entire PORTD directly and invert for GPIB logic levels
  return ~PORTD.IN;
}

/***** Set the GPIB data bus to output and with the requested byte *****/
void setGpibDbus(uint8_t db) {
  // Set PORTD pins (DIO1–DIO8) as outputs
  PORTD.DIRSET = 0xFF;

  // Write the inverted byte to PORTD to match GPIB logic levels
  PORTD.OUT = ~db;
}


/***** Set the direction and state of the GPIB control lines ****/
/*
   Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
   state: 0=LOW; 1=HIGH/INPUT_PULLUP
   dir  : 0=input; 1=output;
   mode:  0=set pin state; 1=set pin direction
*/

void setGpibState(uint8_t bits, uint8_t mask, uint8_t mode) {
  uint8_t portCb = 0; // Output value for PORTC
  uint8_t portCm = 0; // Mask for PORTC

  // Map GPIB control bits to PORTC pins
  portCb |= (bits & (1 << 0)) << 4; // IFC_PIN  (bit 0 -> PC4)
  portCb |= (bits & (1 << 1)) << 2; // NDAC_PIN (bit 1 -> PC3)
  portCb |= (bits & (1 << 2)) << 0; // NRFD_PIN (bit 2 -> PC2)
  portCb |= (bits & (1 << 3)) >> 2; // DAV_PIN  (bit 3 -> PC1)
  portCb |= (bits & (1 << 4)) >> 4; // EOI_PIN  (bit 4 -> PC0)
  portCb |= (bits & (1 << 5)) << 2; // REN_PIN  (bit 5 -> PC7)
  portCb |= (bits & (1 << 6)) >> 1; // SRQ_PIN  (bit 6 -> PC5)
  portCb |= (bits & (1 << 7)) >> 1; // ATN_PIN  (bit 7 -> PC6)

  portCm |= (mask & (1 << 0)) << 4; // IFC_PIN  (bit 0 -> PC4)
  portCm |= (mask & (1 << 1)) << 2; // NDAC_PIN (bit 1 -> PC3)
  portCm |= (mask & (1 << 2)) << 0; // NRFD_PIN (bit 2 -> PC2)
  portCm |= (mask & (1 << 3)) >> 2; // DAV_PIN  (bit 3 -> PC1)
  portCm |= (mask & (1 << 4)) >> 4; // EOI_PIN  (bit 4 -> PC0)
  portCm |= (mask & (1 << 5)) << 2; // REN_PIN  (bit 5 -> PC7)
  portCm |= (mask & (1 << 6)) >> 1; // SRQ_PIN  (bit 6 -> PC5)
  portCm |= (mask & (1 << 7)) >> 1; // ATN_PIN  (bit 7 -> PC6)

  switch (mode) {
    case 0: // Set pin states
      PORTC.OUT = (PORTC.OUT & ~portCm) | (portCb & portCm);
      break;
    case 1: // Set pin directions
      PORTC.DIR = (PORTC.DIR & ~portCm) | (portCb & portCm);
      break;
  }
}


#endif
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** CUSTOM PIN LAYOUT SECTION *****/
/*************************************/




/************************************/
/***** COMMON FUNCTIONS SECTION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvv *****/

uint8_t getGpibPinState(uint8_t pin){
  return digitalRead(pin);
}

/***** ^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** COMMON FUNCTIONS SECTION *****/
/************************************/
