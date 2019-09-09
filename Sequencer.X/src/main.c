/*
 * File:   main.c
 * Authors: N Mark, P Fitzpatrick, and H Thayyil
 *
 * Created on June 5, 2019
 */

#define F_CPU 3333333

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "sequencer_utils.h"

/* 
 * local variables
 */
volatile uint8_t intflags; // variable for clearing interrupt flags

int main(void) {

    /* Periodic interrupt initializer */
//    rtc_pit_init();
    /* SPI0 Initalizer */
    SPI0_init(2);
    /* PORT IO initializer */
    io_init();
    /* USART Initializer */
    USART3_init();
    /* ADC Initializer - on PORTD pin 6 */
    ADC0free_init();
    /* AC Initializer */
    AC0redge_init();
    /* W25Q32JV memory initializer */
    mem_init();
    /* Sequencer initializer */    
    restoreContext();
    
    sei();
   
    while(1)
    {
        ;
    }
    
    return (EXIT_SUCCESS);
    
}
/*
-----------------------------------------------------------------------------
 Interrupt service routines:
 AC0_AC: Analog Comparator interrupt. Runs on rising gate/clock edge
 RTC_PIT: Real time counter periodic interrupt timer interrupt. UNUSED
 PORTA, PORTB, PORTD, PORTF, PORTC_PORT: Button/rotary encoder interrupts
                                         Specifics below
-----------------------------------------------------------------------------
*/

ISR(AC0_AC_vect) {
    
    step();

    if (status.freeRun) {
        adcVal = oneShotSample();
        sendDacCommand(adcVal);
        recordSample(adcVal);
    } else {
        playbackPattern();
    } 
    
    /* Clear Int flag */
    AC0.STATUS = AC_CMP_bm;
    
}

ISR(RTC_PIT_vect) {
    
    adcVal = oneShotSample();
    sendDacCommand(adcVal);
    recordSample(adcVal);
    
    // clear int flag
    intflags = RTC.PITINTFLAGS;
    RTC.PITINTFLAGS = intflags;
    
}

/* Routine for PORTA handles button step toggles */
ISR(PORTA_PORT_vect) {
    
    toggleSteps();
    
    intflags = PORTA.INTFLAGS;
    PORTA.INTFLAGS = intflags;
    
}

/* Routine for PORTB handles program pattern select knob */
ISR(PORTB_PORT_vect) {
    
    selectPattern();
    
    // clear int flag
    intflags = PORTB.INTFLAGS;
    PORTB.INTFLAGS = intflags;
    
}

/* Routine for PORTD handles playback enable button */
ISR(PORTD_PORT_vect) {
    
    setPlaybackEnable();
    
    // clear int flag
    intflags = PORTD.INTFLAGS;
    PORTD.INTFLAGS = intflags;
    
}

/* Routine for PORTF handles record enable button */
ISR(PORTF_PORT_vect) {
    
    setRecordEnable();
    
    // clear int flag
    intflags = PORTF.INTFLAGS;
    PORTF.INTFLAGS = intflags;
    
}

/* Routine for PORTC handles saveContext button 
 * Temporary button perhaps? 
 */
ISR(PORTC_PORT_vect) {
    saveContext();
    
    // clear int flag
    intflags = PORTC.INTFLAGS;
    PORTC.INTFLAGS = intflags;
    
}