/* 
 * File:   adc.h
 *
 * Created on May 31, 2019, 1:26 PM
 */

#ifndef ADC_H
#define	ADC_H

#define ADC_SAMPNUM_ACCDIV      0x2
#define ADC_NUM_CHANNELS        0x2
#define ADC_MAX_CHANNEL         0x8

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h> /* for itoa() */
#include <avr/io.h>
#include <avr/interrupt.h>

void ADC0mux_init(void);
void ADC0free_init(void);
uint8_t ADC0mux_read(void);
uint16_t ADC0free_read(void);
void ADC0_start(void);
void ADC0_stop(void);
bool ADC0_conversionDone(void);
void ADC0_mux(uint8_t);

#endif	/* ADC_H */

