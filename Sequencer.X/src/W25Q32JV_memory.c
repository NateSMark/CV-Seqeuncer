
/* 
 * File:   W25Q32JV_memory.c 
 * Author: N Mark
 *
 * Created on June 13, 2019
 */

#include "W25Q32JV_memory.h"

volatile uint8_t data;

/* 
 * 
 * SS -> PC3
 * MISO -> PE1
 * MOSI & CLK shared on SPI0ALT2 -> PE0 & PE2 respectively
 *  
 */

/* @NAME: mem_init
 * 
 * @DESCRIPTION: Initializes memory writing operations
 *               
 * @NOTE: It seems this write enable must occur before any others...
 * 
 */
void mem_init(void) {
    /* seems necessary for writing to work properly */
    mem_writeEnable(false);
}

/* @NAME: mem_writeEnable
 * 
 * @DESCRIPTION: Write enable/disable function
 *               
 * @PARAM: 
 *          toggle: true for write enable; false for write disable
 * 
 * @NOTE: MUST be used before any write operation including page program or erase
 * 
 */
void mem_writeEnable(bool toggle) {
    SPI0_select(0x23, 1);
    if (toggle) {
        SPI0_transmit(0x06); // write enable 
    } else {
        SPI0_transmit(0x04); // write disable
    }
    SPI0_select(0x23, 0);
    
}

/* @NAME: mem_pageProgramWord
 * 
 * @DESCRIPTION: Writes a word (16-bits) to data at start address
 *               
 * @PARAM: 
 *          stAddr:   starting address of write operation
 *          dataWord: word sized value to write to flash
 * 
 * @NOTE: A write enable MUST occur directly prior to execution of this instruction.
 *        Must be started at an even address, or only first byte will write
 * 
 */
void mem_pageProgramWord(uint32_t stAddr, uint16_t dataWord) {
    
    uint8_t addr_l = stAddr & 0xFF;
    uint8_t addr_m = (stAddr>>8) & 0xFF;
    uint8_t addr_h = (stAddr>>16);
    
    uint8_t dataWord_l = dataWord & 0xFF;
    uint8_t dataWord_h = dataWord>>8;
    
    mem_writeEnable(true);
    
    SPI0_select(0x23, 1);
    SPI0_transmit(0x02);
    SPI0_transmit(addr_h);
    SPI0_transmit(addr_m);
    SPI0_transmit(addr_l);
    SPI0_transmit(dataWord_h);
    SPI0_transmit(dataWord_l);
    SPI0_select(0x23, 0);
    
    mem_waitBusy();
    
    mem_writeEnable(false);
}

/* @NAME: mem_pageProgramInit
 * 
 * @DESCRIPTION: First step in a loopable write operation. Sends instruction and start address.
 *               
 * @PARAM: 
 *          stAddr: starting address of write operation
 * 
 * @NOTE: A write enable MUST occur directly prior to execution of this instruction.
 *        NOT A STANDALONE FUNCTION. MUST be followed by mem_pageProgramData, 
 *        SPI0_select(0x23, 0), mem_waitBusy, and mem_writeEnable(false);
 * 
 */
void mem_pageProgramInit(uint32_t stAddr) {
    
    uint8_t addr_l = stAddr & 0xFF;
    uint8_t addr_m = (stAddr>>8) & 0xFF;
    uint8_t addr_h = (stAddr>>16);
    
    mem_writeEnable(true);
    
    SPI0_select(0x23, 1);
    SPI0_transmit(0x02);
    SPI0_transmit(addr_h);
    SPI0_transmit(addr_m);
    SPI0_transmit(addr_l);

}

/* @NAME: mem_pageProgramData
 * 
 * @DESCRIPTION: Second step in a loopable write operation. This is a loopable 
 *               function to write up to 256 bytes (a page). Address auto-incremented.
 *               Refer to page program in datasheet for caveats.
 *               
 * @PARAM: 
 *          data: byte of data to write
 * 
 * @NOTE: NOT A STANDALONE FUNCTION. 
 *        MUST be used after mem_pageProgramInit and MUST be followed by 
 *        SPI0_select(0x23, 0), mem_waitBusy, and mem_writeEnable(false);
 *        
 */
void mem_pageProgramData(uint8_t data) {
    SPI0_transmit(data);
}

/* @NAME: mem_readInit
 * 
 * @DESCRIPTION: First step in a loopable read operation. Sends instruction and start address.
 *               
 * @PARAM: 
 *          stAddr: starting address of read operation
 * 
 * @NOTE: NOT A STANDALONE FUNCTION. 
 *        MUST be followed by mem_readData and SPI0_select(0x23, 0)
 * 
 * @EG:
 *          mem_readInit(0x003000);
 * 
 *          for (addr = 0x003000; addr < 0x003010; addr++);
 *              mem_readData(); 
 *          }   
 *      
 *          SPI0_select(0x23, 0);
 * 
 */
void mem_readInit(uint32_t stAddr) {
    uint8_t addr_l = stAddr & 0xFF;
    uint8_t addr_m = (stAddr>>8) & 0xFF;
    uint8_t addr_h = (stAddr>>16);
    
    SPI0_select(0x23, 1);
    SPI0_transmit(0x03);
    SPI0_transmit(addr_h);
    SPI0_transmit(addr_m);
    SPI0_transmit(addr_l);
}

/* @NAME: mem_readData
 * 
 * @DESCRIPTION: Second step in a loopable read operation. This is a loopable 
 *               function to read and return an arbitrary number of bytes.
 *               Address auto-incremented.       
 * 
 * @NOTE: NOT A STANDALONE FUNCTION.
 *        MUST be used after mem_readInit and MUST be followed by 
 *        SPI0_select(0x23, 0)
 * 
 */
uint8_t mem_readData(void) {
    data = SPI0_transmit(0x00);
    return data;
}

/* @NAME: mem_display
 * 
 * @DESCRIPTION: Displays memory locations between start and stop addresses 
 *               via USART3 through USB       
 * 
 * @NOTE: 
 * 
 */
void mem_display(uint32_t stAddr, uint32_t endAddr, char* buffer) {
    
    mem_readInit(stAddr);
    for (uint32_t addr = stAddr; addr < endAddr; addr++) {
        data = mem_readData();
        USART3_sendString("0x");
        USART3_sendHex(addr);
        USART3_sendString(": 0x");
        USART3_sendHex(data);
        USART3_sendString("\n\r");
    }
    
    SPI0_select(0x23, 0);
}

/* @NAME: mem_readSRx
 * 
 * @DESCRIPTION: Reads and returns the values from Status Register x       
 * 
 * @NOTE:
 * 
 */
uint8_t mem_readSR1(void) {
    SPI0_select(0x23, 1);
    SPI0_transmit(0x05);
    data = SPI0_transmit(0x00);
    SPI0_select(0x23, 0);
    
    return data;
}
uint8_t mem_readSR2(void) {
    SPI0_select(0x23, 1);
    SPI0_transmit(0x35);
    data = SPI0_transmit(0x00);
    SPI0_select(0x23, 0);
    
    return data;
}
uint8_t mem_readSR3(void) {
    SPI0_select(0x23, 1);
    SPI0_transmit(0x15);
    data = SPI0_transmit(0x00);
    SPI0_select(0x23, 0);
    
    return data;
}

/* @NAME: mem_waitBusy
 * 
 * @DESCRIPTION: Utility to wait for completion of certain operations.       
 * 
 * @NOTE: Polls the busy bit (b0 in SR1) until cleared
 * 
 */
void mem_waitBusy(void) {
    volatile uint8_t status = 1;
    SPI0_select(0x23, 1);
    
    while (status & 0x01) {
        status = mem_readSR1();
    }
    
    SPI0_select(0x23, 0);
    
}

/* @NAME: mem_waitBusy
 * 
 * @DESCRIPTION: Utility to clear a 4KB sector of memory at the starting address.       
 * 
 * @NOTE: Erased state is 0xFF
 * 
 */
void mem_sectorErase(uint32_t stAddr) {
    uint8_t addr_l = stAddr & 0xFF;
    uint8_t addr_m = (stAddr>>8) & 0xFF;
    uint8_t addr_h = (stAddr>>16);
    
    mem_writeEnable(true);
    SPI0_select(0x23, 1);
    SPI0_transmit(0x20);
    SPI0_transmit(addr_h);
    SPI0_transmit(addr_m);
    SPI0_transmit(addr_l);
    SPI0_select(0x23, 0);
    
    mem_waitBusy();
    mem_writeEnable(false);
}

/* @NAME: mem_waitBusy
 * 
 * @DESCRIPTION: Utility to clear entire memory chip.       
 * 
 * @NOTE: Cleared state is 0xFF
 * 
 */
void mem_erase(void) {
    mem_writeEnable(true);
    SPI0_select(0x23, 1);
    SPI0_transmit(0xC7);
    SPI0_select(0x23, 0);
    
    mem_waitBusy();
    mem_writeEnable(false);
}
