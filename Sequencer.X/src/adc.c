/* 
 * File:   adc.c
 *
 * Created on May 31, 2019, 1:24 PM
 */

#include "adc.h"

void ADC0mux_init(void){
    
    /*
     * ADC Channel Configurations
     * 
     * Disable digital input buffer
     * Disable pull-up resistor
     */
    register8_t adc_pinconfig = (PORT_ISC_INPUT_DISABLE_gc
            & ~PORT_ISC_gm
            & ~PORT_PULLUPEN_bm);
    
    PORT_t portd_config = {
        .PIN0CTRL = adc_pinconfig,
        .PIN1CTRL = adc_pinconfig,
        .PIN2CTRL = adc_pinconfig,
        .PIN3CTRL = adc_pinconfig,
        .PIN4CTRL = adc_pinconfig,
        .PIN5CTRL = adc_pinconfig,
        .PIN6CTRL = adc_pinconfig,
        .PIN7CTRL = adc_pinconfig,
    };
    PORTD = portd_config;


    /*
     * ADC0 Configurations
     * MUXPOS : ADC Input Pin 0 => PD0
     * CTRLA : ADC enabled in 8-bit resolution mode
     * CTRLB : ADC Sample Accumulation - 8 Samples
     * CRTLC : CLK_PER divided by 4 and internal reference
     */
    uint8_t adc_pinmux[ADC_MAX_CHANNEL] = {
        ADC_MUXPOS_AIN0_gc,
        ADC_MUXPOS_AIN1_gc,
        ADC_MUXPOS_AIN2_gc,
        ADC_MUXPOS_AIN3_gc,
        ADC_MUXPOS_AIN4_gc,
        ADC_MUXPOS_AIN5_gc,
        ADC_MUXPOS_AIN6_gc,
        ADC_MUXPOS_AIN7_gc,
    };
    
    ADC_t adc0_config = {
        .MUXPOS = adc_pinmux[0],
        .CTRLA = ADC_ENABLE_bm | ADC_RESSEL_8BIT_gc,
        .CTRLB = ADC_SAMPNUM_ACC4_gc,
        .CTRLC = ADC_PRESC_DIV4_gc | ADC_REFSEL_VDDREF_gc,
    };
    ADC0 = adc0_config;
    
    return;
}

void ADC0free_init()
{

   PORTF.PIN3CTRL &= ~PORT_ISC_gm;      //Disable digital input buffer
   PORTF.PIN3CTRL |= PORT_ISC_INPUT_DISABLE_gc;
   PORTF.PIN3CTRL &= ~PORT_PULLUPEN_bm; //Disable Pull-up Resistor

   ADC0.CTRLC = ADC_PRESC_DIV2_gc     //CLK_PER divided by 2
              | ADC_REFSEL_VDDREF_gc; //VDD Reference

   ADC0.CTRLA = ADC_ENABLE_bm         //ADC Enable: enabled
              | ADC_RESSEL_10BIT_gc   //10 bit mode
              | ADC_FREERUN_bm;       //Enable Free Run Mode

   ADC0.MUXPOS = ADC_MUXPOS_AIN13_gc; //Select ADC Channel
   
}

uint8_t ADC0mux_read(void){
    
    //start ADC conversion cycle
    ADC0.COMMAND = ADC_STCONV_bm;
  
    //wait for results
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));
    
    //reset result interrupt flag
    ADC0.INTFLAGS = ADC_RESRDY_bm;
    
    //return results
    return ADC0.RES >> ADC_SAMPNUM_ACCDIV;
    
}

uint16_t ADC0free_read(void){
   /* Clear the interrupt flag by writing 1: */
   ADC0.INTFLAGS = ADC_RESRDY_bm;

   return ADC0.RES;
}

void ADC0_start(void){
   ADC0.COMMAND = ADC_STCONV_bm;
}

void ADC0_stop(void){
   ADC0.COMMAND &= ~ADC_STCONV_bm;
}

bool ADC0_conversionDone(void){
   return (ADC0.INTFLAGS & ADC_RESRDY_bm);
}

void ADC0_mux(uint8_t n){
    
    ADC0.MUXPOS = n;
    return;
    
}
