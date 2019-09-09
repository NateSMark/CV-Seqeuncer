/*
 * File:   ac.c
 * Author: Pete
 *
 * Created on June 5, 2019, 1:45 PM
 */

#include "ac.h"



void AC0redge_init(void){
    
    /* Negative input uses internal reference - voltage reference should be enabled */
    VREF.CTRLA = VREF_AC0REFSEL_1V5_gc; /* Voltage reference at 1.5V */
    VREF.CTRLB = VREF_AC0REFEN_bm; /* AC0 DACREF reference enabled */
    
    /* Positive Input Disable digital input buffer and internal pull up */
    PORTD.PIN6CTRL = PORT_ISC_INPUT_DISABLE_gc
            & ~PORT_ISC_gm
            & ~PORT_PULLUPEN_bm;
    
    AC0.DACREF = DACREF_VALUE; /*Set DAC voltage reference */
    
    /* Select inputs for comparator */
    AC0.MUXCTRLA = AC_MUXPOS_PIN2_gc        /* Positive Input - Analog Positive Pin 2 */
                 | AC_MUXNEG_DACREF_gc;     /* Negative Input - DAC Voltage Reference */
    
    /* output on PA7 currently disabled */
    AC0.CTRLA = AC_ENABLE_bm    /* Enable analog comparator */
              | AC_INTMODE_POSEDGE_gc;    /* RISING EDGE enabled */
    
    AC0.INTCTRL = AC_CMP_bm;    /* Analog Comparator 0 Interrupt enabled */
}