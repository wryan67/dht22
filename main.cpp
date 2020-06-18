
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <wiringPi.h>

#include <chrono>

#include "threads.h"
#include "dht22.h"


// Input/Output RPi GPIO Pins
static volatile int   dhtPin = 25;

std::time_t lastReading = 0;
static volatile float humidity = -9999;
static volatile float temperature = -9999;


void *readDHT22Loop(void *) {
	float h;
	float t;

	while (true) {
		if (readDHT22Data(dhtPin, &h, &t)) {
			auto now    = std::chrono::system_clock::now();
			lastReading = std::chrono::system_clock::to_time_t(now);

			humidity = h;
			temperature = t;
			delay(2000);
		} else {
			delay(500);
		}
	}
}


bool usage() {
	fprintf(stderr, "usage: dht22 [-p pin]\n");
	fprintf(stderr, "p = dht22 data pin\n");

	return false;
}

bool commandLineOptions(int argc, char **argv) {
	int c, index;

	if (argc < 2) {
		return usage();
	}

	while ((c = getopt(argc, argv, "p:")) != -1)
		switch (c) {
			case 'p':
				sscanf(optarg, "%d", &dhtPin);
				break;
			case '?':
				if (optopt == 'm' || optopt=='t')
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint(optopt))
					fprintf(stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf(stderr,	"Unknown option character \\x%x.\n", optopt);

				return usage();
			default:
				abort();
		}

	return true;
}

bool setup() {
	printf("Program initialization\n");

	if (int ret = wiringPiSetup()) {
		fprintf(stderr, "Wiring Pi setup failed, ret=%d\n", ret);
		return false;
	}

	pinMode(dhtPin, INPUT);
	
	return true;
}

int main(int argc, char **argv)
{
	setuid(0);

	if (!commandLineOptions(argc, argv)) {
		return 1;
	}

	if (!setup()) {
		printf("setup failed\n");
		return 1;
	}

	printf("setup event triggers\n"); fflush(stdout);

	threadCreate(readDHT22Loop, "read dht sensor");


        while (1) {
           printf("humidity=%4.2f   temperature=%4.2f\n", humidity, temperature); 
	   fflush(stdout);
           sleep(1);
        }
}
