// This is a library for the Maxim MAX31856 thermocouple IC
// http://datasheets.maximintegrated.com/en/ds/MAX31856.pdf
//
// Written by Peter Easton (www.whizoo.com)
// Released under CC BY-SA 3.0 license
//
// Look for the MAX31856 breakout boards on www.whizoo.com and eBay (madeatrade)
// http://stores.ebay.com/madeatrade
//
// Looking to build yourself a reflow oven?  It isn't that difficult to
// do!  Take a look at the build guide here:
// http://www.whizoo.com
//
// Change History:
// 25 June 2015        Initial Version
// 31 July 2015        Fixed spelling and formatting problems

#ifndef MAX31856_H
#define MAX31856_H

#include "includes.h"

// MAX31856 Registers
// Register 0x00: CR0
#define CR0_AUTOMATIC_CONVERSION                0x80
#define CR0_ONE_SHOT                            0x40
#define CR0_OPEN_CIRCUIT_FAULT_TYPE_K           0x10    // Type-K is 10 to 20 Ohms
#define CR0_COLD_JUNCTION_DISABLED              0x08
#define CR0_FAULT_INTERRUPT_MODE                0x04
#define CR0_FAULT_CLEAR                         0x02
#define CR0_NOISE_FILTER_50HZ                   0x01
// Register 0x01: CR1
#define CR1_AVERAGE_1_SAMPLE                    0x00
#define CR1_AVERAGE_2_SAMPLES                   0x10
#define CR1_AVERAGE_4_SAMPLES                   0x20
#define CR1_AVERAGE_8_SAMPLES                   0x30
#define CR1_AVERAGE_16_SAMPLES                  0x40
#define CR1_THERMOCOUPLE_TYPE_B                 0x00
#define CR1_THERMOCOUPLE_TYPE_E                 0x01
#define CR1_THERMOCOUPLE_TYPE_J                 0x02
#define CR1_THERMOCOUPLE_TYPE_K                 0x03
#define CR1_THERMOCOUPLE_TYPE_N                 0x04
#define CR1_THERMOCOUPLE_TYPE_R                 0x05
#define CR1_THERMOCOUPLE_TYPE_S                 0x06
#define CR1_THERMOCOUPLE_TYPE_T                 0x07
#define CR1_VOLTAGE_MODE_GAIN_8                 0x08
#define CR1_VOLTAGE_MODE_GAIN_32                0x0C
// Register 0x02: MASK
#define MASK_COLD_JUNCTION_HIGH_FAULT           0x20
#define MASK_COLD_JUNCTION_LOW_FAULT            0x10
#define MASK_THERMOCOUPLE_HIGH_FAULT            0x08
#define MASK_THERMOCOUPLE_LOW_FAULT             0x04
#define MASK_VOLTAGE_UNDER_OVER_FAULT           0x02
#define MASK_THERMOCOUPLE_OPEN_FAULT            0x01
// Register 0x0F: SR
#define SR_FAULT_COLD_JUNCTION_OUT_OF_RANGE     0x80
#define SR_FAULT_THERMOCOUPLE_OUT_OF_RANGE      0x40
#define SR_FAULT_COLD_JUNCTION_HIGH             0x20
#define SR_FAULT_COLD_JUNCTION_LOW              0x10
#define SR_FAULT_THERMOCOUPLE_HIGH              0x08
#define SR_FAULT_THERMOCOUPLE_LOW               0x04
#define SR_FAULT_UNDER_OVER_VOLTAGE             0x02
#define SR_FAULT_OPEN                           0x01

// Set/clear MSB of first byte sent to indicate write or read
#define READ_OPERATION(x)                       (x & 0x7F)
#define WRITE_OPERATION(x)                      (x | 0x80)

// Register numbers
#define REGISTER_CR0                            0
#define REGISTER_CR1                            1
#define REGISTER_MASK                           2
#define NUM_REGISTERS                           12      // (read/write registers)

// Errors
#define	FAULT_OPEN                              10000   // No thermocouple
#define	FAULT_VOLTAGE                           10001   // Under/over voltage error.  Wrong thermocouple type?
#define NO_MAX31856                             10002   // MAX31856 not communicating or not connected
#define IS_MAX31856_ERROR(x)                    (x == FAULT_OPEN && x <= NO_MAX31856)

//SPI defines
#define SPI0_CLK								RPI_V2_GPIO_P1_23
#define SPI0_MOSI								RPI_V2_GPIO_P1_19
#define SPI0_MIS0								RPI_V2_GPIO_P1_21
#define SPI0_CS1_CUST							RPI_V2_GPIO_P1_15


//Functions
void InitMAX31856(void);
void writeRegister(uint8_t registerNum, uint8_t data, uint8_t cs);
double readThermocouple(uint8_t cs);
double readJunction(uint8_t cs);
double verifyMAX31856(uint8_t cs);

long readData();
void writeByte(uint8_t data);

#endif  // MAX31856_H
