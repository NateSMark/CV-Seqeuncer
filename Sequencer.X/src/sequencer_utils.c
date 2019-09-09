/* 
 * File:   sequencer_utils.c
 * Author: N Mark
 *
 * Created on June 16, 2019
 */

#include "sequencer_utils.h"

/* 
 * local variables
 */
volatile uint8_t dacCommandH; // d8-d15 of data to the DAC
volatile uint8_t dacCommandL; // d0-d7 of data to the DAC
volatile uint8_t rotaryPrevPos = 0;  // tracks previous position of the encoder
volatile uint8_t rotaryPos;

/* @NAME: io_init
 * 
 * @DESCRIPTION: Initializes all port IO functionality
 *               
 * @NOTE: 
 * 
 */
void io_init(void) {
    
    // step LED pins on PD0-PD3 and PC4-PC7
    PORTD.DIR |= (PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm);
    PORTC.DIR |= (PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm);

    
    // step button pins on PORTA
    // INTERRUPT on PORTA
    PORTA.PIN0CTRL |= PORT_ISC_FALLING_gc | PORT_PULLUPEN_bm;
    PORTA.PIN1CTRL |= PORT_ISC_FALLING_gc | PORT_PULLUPEN_bm;
    PORTA.PIN2CTRL |= PORT_ISC_FALLING_gc | PORT_PULLUPEN_bm;
    PORTA.PIN3CTRL |= PORT_ISC_FALLING_gc | PORT_PULLUPEN_bm;
    PORTA.PIN4CTRL |= PORT_ISC_FALLING_gc | PORT_PULLUPEN_bm;
    PORTA.PIN5CTRL |= PORT_ISC_FALLING_gc | PORT_PULLUPEN_bm;
    PORTA.PIN6CTRL |= PORT_ISC_FALLING_gc | PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL |= PORT_ISC_FALLING_gc | PORT_PULLUPEN_bm;
    
    // Rotary encoder dial (change pattern) on PB4-PB5
    // INTERRUPT on PORTB
    PORTB.PIN4CTRL |= PORT_ISC_BOTHEDGES_gc;
    PORTB.PIN5CTRL |= PORT_ISC_BOTHEDGES_gc;
    
    // Rotary encoder button (record enable) on PF2
    // INTERRUPT on PORTF
    PORTF.PIN2CTRL |= PORT_ISC_FALLING_gc | PORT_PULLUPEN_bm;
    
    // Record LED on PB3
    PORTB.DIR |= PIN3_bm;
    PORTB.OUTCLR = PIN3_bm;
    
    // Playback mode button on PD4
    // INTERRUPT on PORTD
    PORTD.PIN4CTRL |= PORT_ISC_FALLING_gc | PORT_PULLUPEN_bm;
    
    //Program mode LED on PB2
    PORTB.DIR |= PIN2_bm;
    PORTB.OUTCLR = PIN2_bm;
    
    // saveContext button on PC0
    PORTC.PIN0CTRL |= PORT_ISC_FALLING_gc | PORT_PULLUPEN_bm;
    
    return;
    
}

/* @NAME: rtc_pit_init
 * 
 * @DESCRIPTION: Initializes periodic interrupt timer 
 *               
 * @NOTE: UNTESTED and UNUITILIZED
 * 
 */
void rtc_pit_init(void) {
    
    while(RTC.PITSTATUS && RTC_CTRLABUSY_bm){
        ;
    }
    
    RTC.CTRLA |= RTC_PRESCALER_DIV1_gc;
    
    // configure Clock CLK_RTC
    CLKCTRL.MCLKCTRLA = CLKCTRL_CLKSEL_XOSC32K_gc;
    RTC.CLKSEL = RTC_CLKSEL_TOSC32K_gc; 
    
    // period set to every 8 clk cycles
    RTC.PITCTRLA = 0x10;
    
    return;
    
}

/* @NAME: sequencer_init
 * 
 * @DESCRIPTION: Initializes sequencer variables and attributes to factory settings
 *               
 * @NOTE: Only for initializing system to FACTORY SETTINGS!
 * 
 */
void sequencer_init(void) {
    
    // initialize all patterns w/ 0th step, steps enabled, empty value,
    // repeat and counter at 0
    for (uint8_t i = 0; i < 8; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            patterns[i].steps[j].enable = true;
            patterns[i].steps[j].value = 0;
            patterns[i].steps[j].repeat = 0;
            patterns[i].steps[j].counter = 0;
        }
        // set all sequence lengths to 8
        patterns[i].seqLength = 8;
        // set initial pattern to 0th
        patterns[i].idx = i;
    }
    
    // initialize sequencer status struct
    status.currPatternIdx = 0;
    status.currStepIdx = 0;
    status.freeRun = true;
    status.patternMode = 0;
    status.recordEnable = false;
    status.saved = false;
    
    // set initial current pattern to 0th
    currPattern = &patterns[0];
    
    return;
    
}

/* @NAME: toggleSteps
 * 
 * @DESCRIPTION: step buttons toggle steps on/off and adjust a step's repeat attribute 
 *               
 * @NOTE: 
 * 
 */
void toggleSteps(void) {
    
    // ~PORTA.IN&0xFF is a bitmap for the step buttons
    // cases are 2^n but simply map to steps n = 0-7
    switch (~PORTA.IN&0xFF) {
        
        case 1: 
            if (currPattern->steps[0].enable) {
                   // if step's enabled, add a step repeat 
                   if(currPattern->steps[0].repeat < 2){
                        currPattern->steps[0].repeat++;
                        currPattern->steps[0].counter++;
                        currPattern->seqLength++;
                    } 
                   // else no step repeats
                   else {
                       currPattern->steps[0].enable = false;
                       currPattern->steps[0].repeat = 0;
                       currPattern->steps[0].counter = 0;
                       currPattern->seqLength -= 2;
                   }
            }
            //if disabled ;  enable 
            else {
                currPattern->steps[0].enable = true;
                currPattern->seqLength++;   
            }        
            break;
            
        case 2:
            if (currPattern->steps[1].enable) {
                    //if enabled and repeat = 0 ; repeat = 1 
                   if(currPattern->steps[1].repeat < 2){
                        currPattern->steps[1].repeat++;
                        currPattern->steps[1].counter++;
                        currPattern->seqLength++;
                    }  
                    //if enabled and repeat = 1 ; repeat = 2
                   else {
                       currPattern->steps[1].enable = false;
                       currPattern->steps[1].repeat = 0;
                       currPattern->steps[1].counter = 0;
                       currPattern->seqLength -= 2;
                   }
            }
            //if disabled ;  enable 
            else {
                currPattern->steps[1].enable = true;
                currPattern->seqLength++;   
            }
            break;
            
        case 4:
            if (currPattern->steps[2].enable) {
                    //if enabled and repeat = 0 ; repeat = 1 
                   if(currPattern->steps[2].repeat < 2){
                        currPattern->steps[2].repeat++;
                        currPattern->steps[2].counter++;
                        currPattern->seqLength++;
                    } else {
                       currPattern->steps[2].enable = false;
                       currPattern->steps[2].repeat = 0;
                       currPattern->steps[2].counter = 0;
                       currPattern->seqLength -= 2;
                   }
            }
            //if disabled ;  enable 
            else {
                currPattern->steps[2].enable = true;
                currPattern->seqLength++;   
            }
            break;
            
        case 8:
            if (currPattern->steps[3].enable) {
                    //if enabled and repeat = 0 ; repeat = 1 
                   if(currPattern->steps[3].repeat < 2){
                        currPattern->steps[3].repeat++;
                        currPattern->steps[3].counter++;
                        currPattern->seqLength++;
                    } else {
                       currPattern->steps[3].enable = false;
                       currPattern->steps[3].repeat = 0;
                       currPattern->steps[3].counter = 0;
                       currPattern->seqLength -= 2;
                   }
            }
            //if disabled ;  enable 
            else {
                currPattern->steps[3].enable = true;
                currPattern->seqLength++;   
            }
            break;
            
        case 16:
            if (currPattern->steps[4].enable) {
                    //if enabled and repeat = 0 ; repeat = 1 
                   if(currPattern->steps[4].repeat < 2){
                        currPattern->steps[4].repeat++;
                        currPattern->steps[4].counter++;
                        currPattern->seqLength++;
                    }  
                    //if enabled and repeat = 1 ; repeat = 2
                   else {
                       currPattern->steps[4].enable = false;
                       currPattern->steps[4].repeat = 0;
                       currPattern->steps[4].counter = 0;
                       currPattern->seqLength -= 2;
                   }
            }
            //if disabled ;  enable 
            else {
                currPattern->steps[4].enable = true;
                currPattern->seqLength++;   
            }
            break;
            
        case 32:
            if (currPattern->steps[5].enable) {
                    //if enabled and repeat < 3 ; repeat++ 
                   if(currPattern->steps[5].repeat < 2){
                        currPattern->steps[5].repeat++;
                        currPattern->steps[5].counter++;
                        currPattern->seqLength++;
                    } else {
                       currPattern->steps[5].enable = false;
                       currPattern->steps[5].repeat = 0;
                       currPattern->steps[5].counter = 0;
                       currPattern->seqLength -= 2;
                   }
            }
            //if disabled ;  enable 
            else {
                currPattern->steps[5].enable = true;
                currPattern->seqLength++;   
            }
            break;
            
        case 64:
            if (currPattern->steps[6].enable) {
                    //if enabled and repeat < 3 ; repeat++ 
                   if(currPattern->steps[6].repeat < 2){
                        currPattern->steps[6].repeat++;
                        currPattern->steps[6].counter++;
                        currPattern->seqLength++;
                    } else {
                       currPattern->steps[6].enable = false;
                       currPattern->steps[6].repeat = 0;
                       currPattern->steps[6].counter = 0;
                       currPattern->seqLength -= 2;
                   }
            }
            //if disabled ;  enable 
            else {
                currPattern->steps[6].enable = true;
                currPattern->seqLength++;   
            }
            break;
            
        case 128:
            if (currPattern->steps[7].enable) {
                    //if enabled and repeat < 3 ; repeat++ 
                   if(currPattern->steps[7].repeat < 2){
                        currPattern->steps[7].repeat++;
                        currPattern->steps[7].counter++;
                        currPattern->seqLength++;
                    } else {
                       currPattern->steps[7].enable = false;
                       currPattern->steps[7].repeat = 0;
                       currPattern->steps[7].counter = 0;
                       currPattern->seqLength -= 2;
                   }
            }
            //if disabled ;  enable 
            else {
                currPattern->steps[7].enable = true;
                currPattern->seqLength++;   
            }
            break;                        
      
    }      
    
    return;

}

/* @NAME: step
 * 
 * @DESCRIPTION: Steps through each pattern according to patternMode;  
 *               called on rising gate/clock edge (see analog comparator ISR)
 *               
 * @NOTE: includes lighting of step LEDs
 *        ALSO, note recursive call below; this is what causes steps to 
 *        be skipped when they are disabled
 * 
 */
void step(void) {
    switch (status.patternMode) {
        case 0:   
            // repeat countdown
            // if step is to repeat, decrease it's counter 
            if (currPattern->steps[status.currStepIdx].counter != 0 ){
                currPattern->steps[status.currStepIdx].counter--;
            } else {
                currPattern->steps[status.currStepIdx].counter = currPattern->steps[status.currStepIdx].repeat;
                if (status.currStepIdx == 7) {
                 status.currStepIdx = 0;
                 } else {
                   status.currStepIdx++;
                 }
            }
            break;

        case 1: 
            // repeat countdown
            // if step is to repeat, decrease it's counter 
            if (currPattern->steps[status.currStepIdx].counter != 0 ){
                currPattern->steps[status.currStepIdx].counter--; 
            } else {
                currPattern->steps[status.currStepIdx].counter = currPattern->steps[status.currStepIdx].repeat;
                if (status.currStepIdx == 0) {
                    status.currStepIdx = 7;
                 } else {
                    status.currStepIdx--;
                 }
            }
            break;
    }

    // RECURSIVE CALL to step()
    // skips current step if it's not enabled
    if (!currPattern->steps[status.currStepIdx].enable) {
        step();
    }
    
    // light up current step's LED
    stepLedsToggle(false);
    switch(status.currStepIdx) {
        case 0:
            PORTD.OUTSET = PIN0_bm;
            break;
        case 1:
            PORTD.OUTSET = PIN1_bm;
            break;
        case 2:
            PORTD.OUTSET = PIN2_bm;
            break;
        case 3:
            PORTD.OUTSET = PIN3_bm;
            break;
        case 4:
            PORTC.OUTSET = PIN4_bm;
            break;
        case 5:
            PORTC.OUTSET = PIN5_bm;
            break;
        case 6:
            PORTC.OUTSET = PIN6_bm;
            break;
        case 7:
            PORTC.OUTSET = PIN7_bm;
            break;
    }
     
    return;
    
}

/* @NAME: selectPattern
 * 
 * @DESCRIPTION: Utilizes rotary encoder to select the current pattern  
 *               
 * @NOTE: 
 * 
 */
void selectPattern(void) {
    
    // turn off record enable for new pattern
    status.recordEnable = false;
    PORTB.OUTCLR = PIN3_bm;
    
    rotaryTwist(true);
    
    currPattern = &patterns[status.currPatternIdx];
    
    return;
    
}

/* @NAME: freeSampleOnGate
 * 
 * @DESCRIPTION: Samples ADC input while it's above the AC's threshold
 *               
 * @NOTE: not currently in use, but will likely be used for full gate sampling
 * 
 */
void freeSampleOnGate(void) {
    
    ADC0_start();
    
    while(AC0.STATUS & AC_STATE_bm) {
        while(!ADC0_conversionDone()) {
            ;
        }
        
        adcVal = ADC0free_read();
        sendDacCommand(adcVal); 
    }
    
    ADC0_stop();
    
    return;
    
}

/* @NAME: freeRunSample
 * 
 * @DESCRIPTION: Generic free running ADC sampling
 *               
 * @NOTE: not currently in use; more of a debugging utility
 * 
 */
void freeRunSample(void) {
    ADC0_start();
    
    while(!ADC0_conversionDone()) {
        ;
    }
        
    adcVal = ADC0free_read();    
    sendDacCommand(adcVal);
    
    ADC0_stop();
    
    return;
    
}

/* @NAME: oneShotSample
 * 
 * @DESCRIPTION: Samples a one-shot of the ADC input
 *               
 * @NOTE: 
 * 
 */
uint16_t oneShotSample(void) {
    
    ADC0_start();
    
    while (!ADC0_conversionDone()) {
        ;
    }
    
    adcVal = ADC0free_read();
        
    ADC0_stop();
    
    return adcVal;
   
}

/* @NAME: recordSample
 * 
 * @DESCRIPTION: Records a given value into the current step's value attribute 
 *               
 * @PARAM: 
 *          val: sample value to record to current pattern->current step's value
 * 
 */
void recordSample(uint16_t val) {
    
    if (status.recordEnable) {
        currPattern->steps[status.currStepIdx].value = val;
    }
    
    return;
    
}

/* @NAME: setRecordEnable
 * 
 * @DESCRIPTION: Simple utility for enabling recordEnable and the record LED
 *               
 * @NOTE: 
 * 
 */
void setRecordEnable(void) {
    
    if (!status.recordEnable) {
        status.recordEnable = true;
        recLedToggle(true);
    } else if (status.recordEnable) {
        status.recordEnable = false;
        recLedToggle(false);
    }

    return;
    
}

/* @NAME: saveContext
 * 
 * @DESCRIPTION: Save context of system on external flash; dedicated button on PC0
 *               
 * @NOTE: Context stored from 0x000000 to 0x00010C
 * 
 */
void saveContext(void) {
    
    USART3_sendByte(status.saved);
    USART3_sendString("\n\r");
    
    status.saved = true;
    
    mem_sectorErase(0x000000);
    
    mem_writeEnable(true);
    
    SPI0_select(0x23, 1);
    
    SPI0_transmit(0x02);
    
    // address is 0x000000
    SPI0_transmit(0x00);
    SPI0_transmit(0x01);
    SPI0_transmit(0x00);
    
    SPI0_transmit(status.saved);
    SPI0_transmit(status.currPatternIdx);
    SPI0_transmit(status.currStepIdx);
    SPI0_transmit(status.freeRun);
    SPI0_transmit(status.patternMode);
    SPI0_transmit(status.recordEnable);
    
    for (int pidx = 0; pidx < 8; pidx++) {
        SPI0_transmit(patterns[pidx].idx);
        SPI0_transmit(patterns[pidx].seqLength);
    }
    
    SPI0_select(0x23, 0);

    mem_waitBusy();
    
    mem_writeEnable(false);
    
    mem_writeEnable(true);
    
    SPI0_select(0x23, 1);
    
    SPI0_transmit(0x02);
    
    // address is 0x000000
    SPI0_transmit(0x00);
    SPI0_transmit(0x00);
    SPI0_transmit(0x00);
    
    for (int pidx = 0; pidx < NUM_PATTERNS; pidx++) {
        for (int sidx = 0; sidx < NUM_STEPS; sidx++) {
            SPI0_transmit(patterns[pidx].steps[sidx].enable);
            SPI0_transmit(patterns[pidx].steps[sidx].value>>8);
            SPI0_transmit(patterns[pidx].steps[sidx].value&0xFF);
            SPI0_transmit(patterns[pidx].steps[sidx].repeat);
        }
    }
    
    SPI0_select(0x23, 0);
    
    mem_waitBusy();
    
    mem_writeEnable(false);
    
    USART3_sendByte(status.saved);
    USART3_sendString("\n\r");

}

/* @NAME: restoreContext
 * 
 * @DESCRIPTION: Restore context of system from external flash; runs on boot up
 *               
 * @NOTE: Context stored from 0x000000 to 0x00010C
 * 
 */
void restoreContext(void) {

    SPI0_select(0x23, 1);

    mem_readInit(0x000100);
            
    status.saved = SPI0_transmit(0);
    status.currPatternIdx = SPI0_transmit(0);
    status.currStepIdx = SPI0_transmit(0);
    status.freeRun = SPI0_transmit(0);
    status.patternMode = SPI0_transmit(0);
    status.recordEnable = SPI0_transmit(0);
    


    for (int pidx = 0; pidx < NUM_PATTERNS; pidx++) {
        patterns[pidx].idx = SPI0_transmit(0);
        patterns[pidx].seqLength = SPI0_transmit(0);
    }
    
    SPI0_select(0x23, 0);
    
    if (status.saved) {

        SPI0_select(0x23, 1);

        mem_readInit(0x000000);

        for (int pidx = 0; pidx < NUM_PATTERNS; pidx++) {
            for (int sidx = 0; sidx < NUM_STEPS; sidx++) {
                patterns[pidx].steps[sidx].enable = SPI0_transmit(0);
                patterns[pidx].steps[sidx].value = SPI0_transmit(0)<<8;
                patterns[pidx].steps[sidx].value += SPI0_transmit(0);
                patterns[pidx].steps[sidx].repeat = SPI0_transmit(0);
            }
        }

        SPI0_select(0x23, 0);

        currPattern = &patterns[status.currPatternIdx];
    } else {
        sequencer_init();
    }
    
}

/* @NAME: setPlaybackEnable
 * 
 * @DESCRIPTION: Simple utility for enabling freeRun and the pattern playback LED
 *               
 * @NOTE: 
 * 
 */
void setPlaybackEnable(void) {
    
    if (!status.freeRun) {
        status.freeRun = true;
        playbackLedToggle(true);
    } else if (status.freeRun) {
        status.freeRun = false;
        playbackLedToggle(false);
    }

    return;
    
}

/* @NAME: playbackPattern
 * 
 * @DESCRIPTION: Sends the oneshot sample for current pattern->current step to DAC 
 *               
 * @NOTE: INCOMPLETE. Only accounts for oneshots
 * 
 */
void playbackPattern(void) {
    sendDacCommand(currPattern->steps[status.currStepIdx].value);
}

/* @NAME: sendDacCommand
 * 
 * @DESCRIPTION: Sends a DAC command to the MCP4922
 *               
 * @PARAM: 
 *          command: 16-bit ADC sampled voltage for D/A conversion
 * 
 */
void sendDacCommand(uint16_t command) {
    // 0x5000 sets up MCP4922 for DACA output, input buffer ON, x2 output gain
    dacCommandH = 0x50 + (command >> 8);
    dacCommandL = command & 0xFF;
    
    SPI0_dacSelect(true);
    SPI0_transmit(dacCommandH);
    SPI0_transmit(dacCommandL);
    SPI0_dacSelect(false);
    
    return;
    
}

/* @NAME: recLedToggle
 * 
 * @DESCRIPTION: Simple utility for lighting record enable LED
 *               
 * @PARAM: 
 *          toggle: true turns LED on, false turns LED off
 * 
 */
void recLedToggle(bool toggle) {
    if (toggle) {
        PORTB.OUTSET = PIN3_bm;
    } else {
        PORTB.OUTCLR = PIN3_bm;
    }
    
    return;
    
}

/* @NAME: playbackLedToggle
 * 
 * @DESCRIPTION: Simple utility for lighting pattern playback LED
 *               
 * @PARAM: 
 *          toggle: true turns LED on, false turns LED off
 * 
 */
void playbackLedToggle(bool toggle) {
    if (toggle) {
        PORTB.OUTCLR = PIN2_bm;
    } else {
        PORTB.OUTSET = PIN2_bm;
    }
    
    return;
    
}

/* @NAME: recLedToggle
 * 
 * @DESCRIPTION: Simple utility for lighting all step LED's
 *               
 * @PARAM: 
 *          toggle: true turns LED on, false turns LED off
 * 
 */
void stepLedsToggle(bool toggle) {
    if (toggle) {
        PORTD.OUTSET = (PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm);
        PORTC.OUTSET = (PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm);
    } else {
        PORTD.OUTCLR = (PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm);
        PORTC.OUTCLR = (PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm);
    }
    
    return;
    
}

/* @NAME: rotaryTwist
 * 
 * @DESCRIPTION: Handles twisting of rotary knob for patternSelect
 *               
 * @PARAM: 
 *          print: if true, currPatternIdx is printed via USART thru USB
 * 
 */
               void rotaryTwist(bool print) {
    //stores the current position of the rotary encoders
    rotaryPos = (PORTB.IN & PIN4_bm) | (PORTB.IN & PIN5_bm);
    
    //clockwise rotation
    if((rotaryPos == 32 && rotaryPrevPos == 48)) {
        if (status.currPatternIdx == 7) {
            status.currPatternIdx = 0;
        } else {
            status.currPatternIdx++;            
        }
        if (print) {
            USART3_sendNum(status.currPatternIdx);
        }
    }
    //counterclockwise rotation
    else if((rotaryPos == 16 && rotaryPrevPos == 48)) {
        if (status.currPatternIdx == 0) {
            status.currPatternIdx = 7;
        } else {
            status.currPatternIdx--;
        }
        if (print) {
            USART3_sendNum(status.currPatternIdx);
        }
    }  
    
    //updates the previous position
    rotaryPrevPos = rotaryPos;
    
    return;
    
}