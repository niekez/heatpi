#include "includes.h"


// MAX31856 Initial settings (see MAX31856.h and the MAX31856 datasheet)
// The default noise filter is 60Hz, suitable for the USA
#define CR0_INIT  (CR0_AUTOMATIC_CONVERSION + CR0_OPEN_CIRCUIT_FAULT_TYPE_K) //+ CR0_NOISE_FILTER_50HZ)
#define CR1_INIT  (CR1_AVERAGE_2_SAMPLES + CR1_THERMOCOUPLE_TYPE_K)
#define MASK_INIT (~(MASK_VOLTAGE_UNDER_OVER_FAULT + MASK_THERMOCOUPLE_OPEN_FAULT))

void printTemperature(double temperature);

int main(int argc, char **argv)
{
	double dataIn = 0;
	uint32_t faultData;
	uint8_t regData;

	if (!bcm2835_init())
   {
	 printf("bcm2835_init failed. Are you running as root??\n");
	 return 1;
   }

   if (!bcm2835_spi_begin())
    {
      printf("bcm2835_spi_begin failed. Are you running as root??\n");
      return 1;
    }

	InitMAX31856();

	// Initializing the MAX31855's registers
	writeRegister(REGISTER_CR0, CR0_INIT, SPI0_CS1_CUST);
	writeRegister(REGISTER_CR1, CR1_INIT, SPI0_CS1_CUST);
	writeRegister(REGISTER_MASK, MASK_INIT, SPI0_CS1_CUST);

	bcm2835_delay(2000);

	dataIn = verifyMAX31856(SPI0_CS1_CUST);
	printf("Connected?: %s\n", dataIn == NO_MAX31856 ? "No :(" : "Yes!:D");

	dataIn = verifyMAX31856(SPI0_CS1_CUST);
	printf("Connected?: %s\n", dataIn == NO_MAX31856 ? "No :(" : "Yes!:D");

	// writeByte(READ_OPERATION(0x0F));
	// // Read 4 registers
	// faultData = readData();
	// //faultData  >>= 24;
	// printf("Fault: %f\n", faultData);
	delay(200);

	while(1)
	{
		dataIn = readJunction(SPI0_CS1_CUST);
		printTemperature(dataIn);
		delay(200);
	}
}

void printTemperature(double temperature) {
	switch ((int) temperature) {
	    case FAULT_OPEN:
			puts("FAULT_OPEN");
			break;
	    case FAULT_VOLTAGE:
	    	puts("FAULT_VOLTAGE");
	    	break;
	    case NO_MAX31856:
			puts("NO_MAX31856");
	    	break;
	    default:
			printf("Temp: %f\n", temperature);
			break;
	}
}
