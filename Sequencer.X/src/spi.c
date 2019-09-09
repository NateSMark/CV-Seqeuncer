
/*! @file   spi.c
 *  @brief  API for SPI initialization and data transmission.
 * 
 * Configures the SPI0 interface on the ATMEGA4809 and provides simple 
 * API for transmitting data.
 * 
 *  @author A. Bunker \<<abunker97@gmail.com>\>
 *  @author S. Farnsworth \<<sfarnsworth@hawk.iit.edu>\>
 *  @date   May, 29, 2019
 *  @bug    No known bugs
 * 
 *  @VERSION: 1.1
 */

#include "spi.h"

static register8_t* PORTx_OUTSET[6] = {
    &(PORTA.OUTSET), 
    &(PORTB.OUTSET), 
    &(PORTC.OUTSET),
    &(PORTD.OUTSET),
    &(PORTE.OUTSET),
    &(PORTF.OUTSET),
};

static register8_t* PORTx_OUTCLR[6] = {
    &(PORTA.OUTCLR), 
    &(PORTB.OUTCLR), 
    &(PORTC.OUTCLR),
    &(PORTD.OUTCLR),
    &(PORTE.OUTCLR),
    &(PORTF.OUTCLR),
};

static uint8_t PINx_bm[8] = {
    PIN0_bm, PIN1_bm, PIN2_bm, PIN3_bm, PIN4_bm, PIN5_bm, PIN6_bm, PIN7_bm
};

/*  Initializes and configures the SPI0 module
 *
 *  SPI0 module is multiplex to different ports based on the muxSel value.
 *  Note: SS Pins are user defined. Use SPI0_select when addressing slave devices.
 * 
 * If muxSel == 1:
 *      - C0 = MOSI
 *      - C1 = MISO
 *      - C2 = SCK
 *      - C3 = SS (disabled)
 * 
 * If muxSel == 2:
 *      - E0 = MOSI
 *      - E1 = MISO
 *      - E2 = SCK
 *      - E3 = SS (disabled)
 * 
 * If muxSel == Anything Else:
 *      - A4 = MOSI
 *      - A5 = MISO
 *      - A6 = SCK
 *      - A7 = SS (disabled)
 */
void SPI0_init(uint8_t muxSel)
{ 
    spi_port_config = (PORT_t){ 
        .DIRSET = PIN0_bm | PIN2_bm,
        .DIRCLR = PIN1_bm,
    };
    spi0_config = (SPI_t){
        .CTRLA = (SPI_CLK2X_bm | SPI_ENABLE_bm | SPI_MASTER_bm | SPI_PRESC_DIV16_gc),
        .CTRLB = (SPI_SSD_bm),
    };
    
    //enables the spi peripheral
    SPI0 = spi0_config; 
    
    //SPI0 on PC[3:0]
    if(muxSel == 1){
        PORTMUX.TWISPIROUTEA |= PORTMUX_SPI0_ALT1_gc; 
        PORTC = spi_port_config;
    }
    //SPI0 on PE[3:0]
    else if(muxSel == 2){
        PORTMUX.TWISPIROUTEA |= PORTMUX_SPI0_ALT2_gc;
        PORTE = spi_port_config;
    }
    //SPI0 on PA[7:4]
    else{
        PORTMUX.TWISPIROUTEA |= PORTMUX_SPI0_DEFAULT_gc;
        PORTA.DIRSET = PIN4_bm | PIN6_bm;
        PORTA.DIRCLR = PIN5_bm;
    }
    
    return;
}

/*  Begins SPI data transmission.
 * 
 *  Writes data to be transmitted over the SPI0 lane to the SPIx.DATA register.
 *  Then waits for data to be successfully transmitted before returning. If the
 *  transmission is successful, SPIx.DATA will return '0x00'.
 */
uint8_t SPI0_transmit(uint8_t data)
{ 
    SPI0.DATA = data;
    
    while(!(SPI0.INTFLAGS & SPI_IF_bm)){;}
    
    return SPI0.DATA; 
}

/* Selects the slave pin.
 * 
 * If sel is 1 selects the slave by setting its corresponding pin low
 * If sel is 0 selects the slave by setting its corresponding pin high
 *  
 * The addr is broken into 2 parts. the left 4 bits correspond to the port
 *  A = 0, B = 1, C = 2, D = 3, E = 4, F = 5
 * 
 * the right 4 bits correspond to the pin       
 */
void SPI0_select(uint8_t addr, uint8_t sel)
{
    if(sel){
        *PORTx_OUTCLR[addr >> 4] = PINx_bm[addr & 0xF];
    }
    else{
        *PORTx_OUTSET[addr >> 4] = PINx_bm[addr & 0xF];
    }
}
