#include "includes.h"

void printTemperature(double temperature) {
	switch ((int) temperature) {
	    case FAULT_OPEN:
			printf("FAULT_OPEN");
			break;
	    case FAULT_VOLTAGE:
	    	printf("FAULT_VOLTAGE");
	    	break;
	    case NO_MAX31856:
			printf("NO_MAX31856");
	    	break;
	    default:
			printf("%f", temperature);
			break;
	}
}

int main(int argc, char **argv)
{
	double dataIn = 0;

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

	//let first samples come in
	bcm2835_delay(200);

	while(1)
	{
		dataIn = readJunction(SPI0_CS1_CUST);
		printf("\nJunction temp: ");
		printTemperature(dataIn);
		printf("\tTC temp: ");
		dataIn = readThermocouple(SPI0_CS1_CUST);
		printTemperature(dataIn);
		bcm2835_delay(1000);
	}

	bcm2835_spi_end();
	bcm2835_close();
   	return 0;
}
