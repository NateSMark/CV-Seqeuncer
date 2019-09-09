/* 
 * File:   terminalPrint.h
 * Author: N Mark
 *
 * Created on June 16, 2019
 *	
 * Contains structure and function instantiations for fundamental sequencer operations 
 * 
 * 
 * @VERSION: 1.1
 * 
 * @NOTE: Interrupt Service Routines are contained in the main file
 *
 */

#ifndef SEQUENCER_UTILS_H
#define	SEQUENCER_UTILS_H

#define F_CPU           3333333 // for util/delay.h
#define NUM_PATTERNS    8
#define NUM_STEPS       8
#define CONTEXT_PARAMS  10      // number of parameters to context store/restore

#include <util/delay.h>
#include "spi.h"
#include "adc.h"
#include "terminalPrint.h"
#include "ac.h"
#include "W25Q32JV_memory.h"


/*
 * global variables
 */
volatile uint16_t adcVal; // word read from ADC conversion

/*
-------------------------------------------------------------------------------
 Structure definitions:
 step:          NUM_STEPS # of steps in each step_pattern
 step_pattern:  NUM_PATTERNS # of programmable patterns total
 seq_status:    status structure contains sequencer status and count variables
-------------------------------------------------------------------------------
 */
typedef struct step {
    
    bool enable;        // when disabled, step is skipped (alters pattern length)
    uint16_t value;     // one shot sample stored in steps value
    uint8_t repeat;     // step repeat variable (currently 3 repeats supported)
    uint8_t counter;    // counter is a duplicate/countdown variable for repeat
    
} step_t;

typedef struct step_pattern {
    
    step_t steps[NUM_STEPS];    // each pattern has NUM_STEPS steps in an array
    uint8_t idx;                // useful index attribute for patterns array
    uint8_t seqLength;          // sequence length is altered often;
                                // can be larger than 8 with repeats (up to 24 currently)
    
} step_pattern_t;

typedef struct seq_status {
    
    bool freeRun;           // true: freeRun mode; false: pattern playback mode
    bool recordEnable;      // true: record mode; false: no recording
    bool saved;
    uint8_t currPatternIdx; // variable for the current pattern index
    uint8_t currStepIdx;    // variable for the current step index
    uint8_t patternMode;    // modes include forward and backwards traversal of pattern]
    
} seq_status_t;

/*
-------------------------------------------------------------------------------
 Structure instantiations:
 patterns[]:    contains all sequencer patterns
 *currPattern:  pointer to current pattern
 status:        status contains sequencer status and count variables
-------------------------------------------------------------------------------
 */
step_pattern_t patterns[NUM_PATTERNS];
step_pattern_t *currPattern;
seq_status_t status;

/* 
 * function prototypes
 */
void io_init(void);
void rtc_pit_init(void);
void sequencer_init(void);
void selectPattern(void);
void freeRunSample(void);
void freeSampleOnGate(void);
uint16_t oneShotSample(void);
void setRecordEnable(void);
void setPlaybackEnable(void);
void playbackPattern(void);
void recordSample(uint16_t);
void sendDacCommand(uint16_t);
void step(void);
void toggleSteps(void);
void saveContext(void);
void restoreContext(void);
void recLedToggle(bool);
void playbackLedToggle(bool);
void stepLedsToggle(bool);
void rotaryTwist(bool);


#endif	/* SEQUENCER_UTILS_H */