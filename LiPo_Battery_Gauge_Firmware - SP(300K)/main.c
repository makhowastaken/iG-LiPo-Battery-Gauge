/*
 LiPo Battery Gauge
 Version: 1.2
 Author: Alex from insideGadgets (http://www.insidegadgets.com)
 Created: 13/10/2019
 Last Modified: 1/11/2019
 
*/

#define F_CPU 1000000 // 1 MHz

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include "setup.c"

int main (void) {
	setup();
	
	while(1) {
		// Power pin (PD4) low, go to sleep
		if (!(PIND & (1<<POWER_ON_PIN))) {
			_delay_ms(200);
			
			// Turn off LEDs
			PORTD &= ~((1<<LED1) | (1<<LED2) | (1<<LED3) | (1<<LED4) | (1<<LED6) | (1<<LED7) | (1<<LED8));
			PORTB &= ~((1<<LED5) | (1<<LED9) | (1<<LED10));
			
			// Disable ADC/interrupt
			cbi(ADCSRA, ADEN);
			cbi(ADCSRA, ADIE);
			
			// Enable pin change interrupt
			sbi(PCICR, PCIE2);
			system_sleep();
			
			// Enable ADC/interrupt
			sbi(ADCSRA, ADEN);
			sbi(ADCSRA, ADIE);
			
			// Disable pin change interrupt
			cbi(PCICR, PCIE2);
			_delay_ms(100);
			
			animate_leds(); // Animate LEDs at start
		}
		
		
		// 5 samples over 25ms
		uint16_t batteryVoltageLevel = 0;
		for (uint8_t x = 0; x < 5; x++) {
			batteryVoltageLevel += adc_read(batteryADCPin, REF_1_1V);
			_delay_ms(5);
		}
		batteryVoltageLevel = batteryVoltageLevel / 5;
		
		
		// Red
		// Low voltage cutoff on SP is 3.4v which with 300k resistor instead of recommended 320k, that trigger is at the change from three LEDs to two
		// Set lower bound from 675 to 750?
		// LED2 triggers at value 720 which is too low 
		if (batteryVoltageLevel >= 750) {
			PORTD |= (1<<LED1);
		}
		else {
			PORTD &= ~(1<<LED1);
		}
		if (batteryVoltageLevel >= 770) {
			PORTD |= (1<<LED2);
		}
		else {
			PORTD &= ~(1<<LED2);
		}
		if (batteryVoltageLevel >= 790) {
			PORTD |= (1<<LED3);
		}
		else {
			PORTD &= ~(1<<LED3);
		}
		
		// Blue
		if (batteryVoltageLevel >= 805) {
			PORTD |= (1<<LED4);
		}
		else {
			PORTD &= ~(1<<LED4);
		}
		if (batteryVoltageLevel >= 820) {
			PORTB |= (1<<LED5);
		}
		else {
			PORTB &= ~(1<<LED5);
		}
		if (batteryVoltageLevel >= 835) {
			PORTD |= (1<<LED6);
		}
		else {
			PORTD &= ~(1<<LED6);
		}
		if (batteryVoltageLevel >= 850) {
			PORTD |= (1<<LED7);
		}
		else {
			PORTD &= ~(1<<LED7);
		}
		
		// Green
		if (batteryVoltageLevel >= 865) {
			PORTD |= (1<<LED8);
		}
		else {
			PORTD &= ~(1<<LED8);
		}
		if (batteryVoltageLevel >= 880) {
			PORTB |= (1<<LED9);
		}
		else {
			PORTB &= ~(1<<LED9);
		}
		if (batteryVoltageLevel >= 900) {
			PORTB |= (1<<LED10);
		}
		else {
			PORTB &= ~(1<<LED10);
		}
		// Full charge (4.2v) is between 920 and 930
		// Setting Full for 4.1v so gauge isn't immediately depleted one bar after a minute of use
		// 4.1v is between 910 and 920, setting LED10 from 930 to 915
		// Sleep for 1 second
		watchdog_sleep(T1S);
	}
	
	return 0;
}

EMPTY_INTERRUPT(ADC_vect);
EMPTY_INTERRUPT(WDT_vect);

ISR(PCINT2_vect) {
	_delay_ms(200);
}
