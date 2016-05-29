all: heatpi

heatpi: main.o MAX31856.o
	gcc main.o MAX31856.o -lbcm2835 -o heatpi

main.o: main.c
	gcc -c main.c

MAX31856.o: MAX31856.o
	gcc -c MAX31856.c

clean:
	rm -rf *o output_file_name
