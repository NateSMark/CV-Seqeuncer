
/*! @file   spi.h 
 *  @brief  Function prototypes and include dependencies for the SPI.
 * 
 *  This file includes all the API currently implemented for the
 *  ATmega4809 SPI module, as well as any necessary dependencies.
 * 
 *  @author A. Bunker \<<abunker97@gmail.com>\>
 *  @author S. Farnsworth \<<sfarnsworth@hawk.iit.edu>\>
 *  @date   June 7, 2019
 *  @bug    No known bugs
 * 
 *  @VERSION: 1.1
 */

#ifndef _SPI_H
#define	_SPI_H_

#include <avr/io.h>

/*! @brief Configuration structure for PORTC SPI initialization
 */ 
PORT_t spi_port_config;
/*! @brief Configuration structure for ATmega4809 SPI module
 */
SPI_t spi0_config;

/*! @brief  Initializes and configures the SPI0 module
 *
 * SPI0 module is multiplex to different ports based on the muxSel value.
 * 
 *  @param  none
 *  @return none
 */
void SPI0_init(uint8_t);
/*! @brief  Begins SPI data transmission.
 * 
 *  Writes data to be transmitted over the SPI0 lane to the SPIx.DATA register. \n
 *  Then waits for data to be successfully transmitted before returning. If the \n
 *  transmission is successful, SPIx.DATA will return '0x00'.
 * 
 *  @param[in]  data : the byte to be transmitted
 *  @return     (uint8_t)SPIx.DATA
 */
uint8_t SPI0_transmit(uint8_t);
/*! @brief Selects the slave device
 *   
 * If sel is 1 selects the slave by setting its corresponding pin low \n
 * If sel is 0 selects the slave by setting its corresponding pin high \n
 *  
 * The addr is broken into 2 parts. the left 4 bits correspond to the port \n
 *  A = 0, B = 1, C = 2, D = 3, E = 4, F = 5 \n
 * 
 * the right 4 bits correspond to the pin   \n
 * 
 * @param[in]    addr : the address of the slave
 *               sel : the sel/deselect bit
 * @return  none   
 */
void SPI0_select(uint8_t, uint8_t);

#endif	/* _SPI_H_ */

