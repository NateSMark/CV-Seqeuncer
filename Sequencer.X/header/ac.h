/* 
 * File:   ac.h
 * Author: Pete
 *
 * Created on June 5, 2019, 1:42 PM
 */

#ifndef AC_H
#define	AC_H

/* V_DACREF = DACREF * VREF / 256 = 0.55V */
#define DACREF_VALUE    255

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h> /* for itoa() */
#include <avr/io.h>
#include <avr/interrupt.h>

void AC0redge_init(void);

#endif	/* AC_H */