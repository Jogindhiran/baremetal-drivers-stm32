#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "uart.h"
#include "adc.h"

//Program to read the key and turn on LED if key '1' is pressed

uint32_t sensor_value;

int main(void) {

	uart2_tx_init();
	pa1_adc_init();
	start_conversion();

	while(1) {

		sensor_value = adc_read();
		printf("Sensor value: %d\n\r",(int)sensor_value); //Imp: Typecast uint32_t data to int because %d works only for int
	}
}


