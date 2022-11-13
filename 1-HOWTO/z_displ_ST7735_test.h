/*
 * z_displ_ST7735_test
 *  rel. 5
 *
 *  Created on: Mar 25, 2022
 *      Author: mauro
 *
 *  This is related to the functions testing features and showing performance
 *  you don't need this file in the production project
 *
*/

#ifndef INC_Z_DISPL_ST7735_TEST_H_
#define INC_Z_DISPL_ST7735_TEST_H_

#include <stdio.h>
#include <stdlib.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))


#define BGCOLOR BLACK

void Displ_PerfTest();
void Displ_ColorTest();
void Displ_TestAll ();

#endif /* INC_Z_DISPL_ST7735_TEST_H_ */
