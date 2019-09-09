/* 
 * File:   W25Q32JV_memory.h
 * Author: Nathan
 *
 * Created on June 13, 2019, 3:07 PM
 */

#ifndef W25Q32JV_MEMORY_H
#define	W25Q32JV_MEMORY_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>


#include "spi.h"
#include "sequencer_utils.h"

void mem_init(void);
void mem_writeEnable(bool);
void mem_pageProgram(uint32_t, uint16_t);
void mem_readInit(uint32_t);
uint8_t mem_readData(void);
void mem_display(uint32_t, uint32_t, char*);
uint8_t mem_readSR1(void);
uint8_t mem_readSR2(void);
uint8_t mem_readSR3(void);
void mem_waitBusy(void);
void mem_sectorErase(uint32_t);
void mem_erase(void);




#endif	/* W25Q32JV_MEMORY_H */

