#include	"MAX31856.h"

//Globals
//Default register values
uint8_t reg[NUM_REGISTERS] = {0x00,0x03,0xff,0x7f,0xc0,0x7f,0xff,0x80,0,0,0,0};
uint8_t regSave[NUM_REGISTERS];


// Define which pins are connected to the MAX31856.  The DRDY and FAULT outputs
// from the MAX31856 are not used in this library.
void InitMAX31856(void)
{
	uint8_t i = 0;

	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);	// The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);					// The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64);	//period of 256ns
    bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE);				// Set to none, handling it ourselfs
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);	// chipsel pol is low

	//custom chip select as output and level high
	bcm2835_gpio_write(SPI0_CS1_CUST, HIGH);
	bcm2835_gpio_fsel(SPI0_CS1_CUST, BCM2835_GPIO_FSEL_OUTP);

    //pull up on data in for error handling
    bcm2835_gpio_set_pud(SPI0_MIS0, BCM2835_GPIO_PUD_UP);

    //init register state saved
    for(i=0; i<NUM_REGISTERS; i++){
    	regSave[i] = reg[i];
    }

}


// Write the given data to the MAX31856 register
void writeRegister(uint8_t registerNum, uint8_t data, uint8_t cs)
{
	// Sanity check on the register number
    if (registerNum >= NUM_REGISTERS)
        return;

    // Select the MAX31856 chip
    bcm2835_gpio_write(cs, LOW);

	// Write the register number, with the MSB set to indicate a write
	writeByte(WRITE_OPERATION(registerNum));

	// Write the register value
	writeByte(data);

	// // Write the register number, with the MSB set to indicate a write
	// bcm2835_spi_transfer(WRITE_OPERATION(registerNum));
	//
    // // Write the register value
	// bcm2835_spi_transfer(data);

    // Deselect MAX31856 chip
	bcm2835_gpio_write(cs, HIGH);

    // Save the register value, in case the registers need to be restored
    regSave[registerNum] = data;
}


// Read the thermocouple temperature either in Degree Celsius or Fahrenheit. Internally,
// the conversion takes place in the background within 155 ms, or longer depending on the
// number of samples in each reading (see CR1).
// Returns the temperature, or an error (FAULT_OPEN, FAULT_VOLTAGE or NO_MAX31856)
double readThermocouple(uint8_t cs)
{
    double temperature;
    long data;

    // Select the MAX31856 chip
    bcm2835_gpio_write(cs, LOW);

	// Read data starting with register 0x0c
	writeByte(READ_OPERATION(0x0c));

	// Read 4 registers
	data = readData();

	// //transfer read operation and receive data, cast addres of long int function
	// operate = READ_OPERATION(0x0c);
    // bcm2835_spi_transfernb(&operate, (char*)&data, 4);

    // Deselect MAX31856 chip
    bcm2835_gpio_write(cs, HIGH);

    // If there is no communication from the IC then data will be all 1's because
    // of the internal pullup on the data line (INPUT_PULLUP)
    if (data == 0xFFFFFFFF)
        return NO_MAX31856;

    // If the value is zero then the temperature could be exactly 0.000 (rare), or
    // the IC's registers are uninitialized.
    if (data == 0 && verifyMAX31856(cs) == NO_MAX31856)
        return NO_MAX31856;

	// Was there an error?
	if (data & SR_FAULT_OPEN)
		temperature = FAULT_OPEN;
	else if (data & SR_FAULT_UNDER_OVER_VOLTAGE)
		temperature = FAULT_VOLTAGE;
	else{
		// Strip the unused bits and the Fault Status Register
		data = data >> 13;

		// Negative temperatures have been automagically handled by the shift above :-)

		// Convert to Celsius
		temperature = (double) data * 0.0078125;
	}

    // Return the temperature
    return (temperature);
}


// Read the junction (IC) temperature either in Degree Celsius or Fahrenheit.
// This routine also makes sure that communication with the MAX31856 is working and
// will return NO_MAX31856 if not.
double readJunction(uint8_t cs)
{
    double temperature;
    long data, temperatureOffset;
	char operate;

    // Select the MAX31856 chip
    bcm2835_gpio_write(cs, LOW);

	// Read data starting with register 8
	writeByte(READ_OPERATION(8));
	// Read 4 registers
	data = readData();

    // //transfer read operation and receive data, cast addres of long int function
	// operate  = READ_OPERATION(8);
    // bcm2835_spi_transfernb(&operate,(char*)&data,4);

    // Deselect MAX31856 chip
    bcm2835_gpio_write(cs, HIGH);

    // If there is no communication from the IC then data will be all 1's because
    // of the internal pullup on the data line (INPUT_PULLUP)
    if (data == 0xFFFFFFFF)
        return NO_MAX31856;

    // If the value is zero then the temperature could be exactly 0.000 (rare), or
    // the IC's registers are uninitialized.
    if (data == 0 && verifyMAX31856(cs) == NO_MAX31856)
        return NO_MAX31856;

    // Register 9 is the temperature offset
    temperatureOffset = (data & 0x00FF0000) >> 16;

    // Is this a negative number?
    if (temperatureOffset & 0x80)
        temperatureOffset |= 0xFFFFFF00;

    // Strip registers 8 and 9, taking care of negative numbers
    if (data & 0x8000)
        data |= 0xFFFF0000;
    else
        data &= 0x0000FFFF;

    // Remove the 2 LSB's - they aren't used
    data = data >> 2;

    // Add the temperature offset to the temperature
    temperature = data + temperatureOffset;

    // Convert to Celsius
    temperature *= 0.015625;

    // Return the temperature
    return (temperature);
}


// When the MAX31856 is uninitialzed and either the junction or thermocouple temperature is read it will return 0.
// This is a valid temperature, but could indicate that the registers need to be initialized.
double verifyMAX31856(uint8_t cs)
{
    long data, reg;
	char operate;

    // Select the MAX31856 chip
    bcm2835_gpio_write(cs, LOW);

	// Read data starting with register 0
	writeByte(READ_OPERATION(0));

	// Read 4 registers
	data = readData();

	// Deselect MAX31856 chip
	bcm2835_gpio_write(cs, HIGH);

    // If there is no communication from the IC then data will be all 1's because
    // of the internal pullup on the data line (INPUT_PULLUP)
    if (data == 0xFFFFFFFF)
        return NO_MAX31856;

    // Are the registers set to their correct values?
    reg = ((long)regSave[0]<<24) + ((long)regSave[1]<<16) + ((long)regSave[2]<<8) + regSave[3];
    if (reg == data)
        return 0;

    // Communication to the IC is working, but the register values are not correct
    // Select the MAX31856 chip
    bcm2835_gpio_write(cs, LOW);

    // Start writing from register 0
	writeByte(WRITE_OPERATION(0));

    // Write the register values
	for (int i=0; i< NUM_REGISTERS; i++)
        writeByte(regSave[i]);

    //bcm2835_spi_writenb(regSave, NUM_REGISTERS);

    // Deselect MAX31856 chip
    bcm2835_gpio_write(cs, HIGH);

    // For now, return an error but soon valid temperatures will be returned
    return NO_MAX31856;
}


// Read in 32 bits of data from MAX31856 chip. Minimum clock pulse width is 100 ns
// so no delay is required between signal toggles.
long readData(void)
{
	uint32_t data = 0xFFFFFFFF;
	bcm2835_spi_transfernb((char*)&data, 4)
    return(data);
}


// Write out 8 bits of data to the MAX31856 chip. Minimum clock pulse width is 100 ns
// so no delay is required between signal toggles.
void writeByte(uint8_t data)
{
    bcm2835_spi_transfer(data);
}
