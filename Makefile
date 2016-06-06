all: heatpi

heatpi: main.o MAX31856.o
	gcc main.o MAX31856.o -lbcm2835 -o heatpi

main.o: ./src/main.c MAX31856.o ./inc/includes.h
	gcc -c ./src/main.c -Iinc -Ilib

MAX31856.o: ./lib/MAX31856.c ./lib/MAX31856.h
	gcc -c ./lib/MAX31856.c -Iinc -Ilib

clean:
	rm -rf *o heatpi
