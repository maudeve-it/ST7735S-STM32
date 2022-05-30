/*
 * z_displ_ST7735.h
 *  rel. 5
 *
 *  Created on: Oct 9, 2021
 *      Author: mauro
 *
 **************************************************
 **************************************************
 *  integrating this library in your project, you
 *  MUST Follow the below STEPS 1 TO 5 setting
 *  configuration, after setting on CubeMX these pins:
 *  DISPL_SCK	SPI clock pin
 *  DISPL_MOSI	SPI MOSI pin
 *  DISPL_CS	SPI output NSS pin
 *  DISPL_DC	GPIO output push/pull, no pull up/down
 *  DISPL_RST	GPIO output push/pull, no pull up/down
 *  DISPL_LED	GPIO output push/pull, no pull up/down or PWM (see below step 3)
 **************************************************
 **************************************************
 *
 */

#ifndef INC_Z_GFX_FUNCTIONS_H_
#define INC_Z_GFX_FUNCTIONS_H_

#include <string.h>

/******************    STEP 1    ******************
 **************** PORT PARAMETERS *****************
 ** properly set the below the 2 defines to address
 ********  the SPI port defined on CubeMX *********/

#define DISPLAY_SPI_PORT 	hspi5
#define DISPLAY_SPI 		SPI5


/*****************     STEP 2      ****************
 ************* SPI COMMUNICATION MODE *************
 ** enable SPI mode wanted uncommenting ONE row ***
 ******* Same as configuration set on CubeMX ******/

//#define DISPLAY_SPI_POLLING_MODE
//#define DISPLAY_SPI_INTERRUPT_MODE
#define DISPLAY_SPI_DMA_MODE


/*****************     STEP 3      *****************
 ***************** Backlight timer *****************
 * if you want dimming backlight UNCOMMENT the
 * DISPLAY_DIMMING_MODE below define and properly
 * set other defines.
 *
 * Using backlight as a switch (only on/off) leave
 * DISPLAY_DIMMING_MODE commented
 *
 * if DIMMING:
 * On CubeMX set DISPL_LED pin as a timer PWM pin.
 * Timer COUNTER PERIOD (ARR) defines dimming light steps:
 * keep it low value - e.g. 10 - if dimming with buttons,
 * use higher value - e.g. 100 - if dimming with encoder, ...
 * Avoiding display flickering timer PRESCALER should
 * let timer clock to be higher than COUNTER PERIOD * 100 Hz.
 * Set all other defines below */


#define DISPLAY_DIMMING_MODE						// uncomment this define to enable dimming function otherwise there is an on/off switching function
#define BKLIT_TIMER 				TIM3			//timer used (PWMming DISPL_LED pin)
#define bklit_t 					htim3			//timer used
#define BKLIT_CHANNEL				TIM_CHANNEL_2	//channel used
#define BKLIT_CCR					CCR2			//preload register
#define BKLIT_STBY_LEVEL 			1				//Display backlight level when in stand-by (levels are CNT values)
#define BKLIT_INIT_LEVEL 			5				//Display backlight level on startup



/*****************     STEP 4      *****************
 ************* frame buffer DEFINITION *************
 * BUFLEVEL defines buffer size:
 * buffer size is 2^BUFLEVEL, 2 means 4 bytes buffer,
 * 9 means 512 bytes.
 * IT MUST BE BETWEEN 10 AND 16:
 * -	10 needed for 1 char in Font24 size
 * -	16 is a full screen 128x160 RGB565
***************************************************/
#define BUFLEVEL 11
#define SIZEBUF 1<<BUFLEVEL


/********     STEP 5      ********
 ****** ST7735 display size ******
 * (warning: I used only 160X128,
 * other sizes/types not tested)
 *
 * set here pixel format of the
 * display driven by ST7735
 * never tested (and probably uncomplete)
 * on sizes else than 160x128
 *********************************/
//#define ST7735_IS_160X80 1
//#define ST7735_IS_128X128 1
#define ST7735_IS_160X128 1
#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160

typedef enum {
	Displ_Orientat_0,
	Displ_Orientat_90,
	Displ_Orientat_180,
	Displ_Orientat_270
} Displ_Orientat_e;

extern int16_t _width;       								///< (oriented) display width
extern int16_t _height;      								///< (oriented) display height

#define SPI_COMMAND GPIO_PIN_RESET  //DISPL_DC_Pin level sending commands
#define SPI_DATA GPIO_PIN_SET		//DISPL_DC_Pin level sending data

/***************************
 * set data length lower limit to
 * enable DMA instead to direct polling
 ***************************/
#define DISPL_DMA_CUTOFF 	20

#define	RED			0xF800
#define	GREEN		0x07E0
#define	BLUE		0x001F
#define YELLOW		0xFFE0
#define MAGENTA		0xF81F
#define CYAN		0x07FF
#define	D_RED 		0xC000
#define	D_GREEN		0x0600
#define	D_BLUE		0x0018
#define D_YELLOW	0xC600
#define D_MAGENTA	0xC018
#define D_CYAN		0x0618
#define	DD_RED		0x8000
#define	DD_GREEN	0x0400
#define DD_BLUE		0x0010
#define DD_YELLOW	0x8400
#define DD_MAGENTA	0x8010
#define DD_CYAN		0x0410
#define WHITE		0xFFFF
#define D_WHITE		0xC618
#define DD_WHITE	0x8410
#define DDD_WHITE	0x4208
#define DDDD_WHITE	0x2104
#define	BLACK		0x0000
#define color565(r, g, b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3))

#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }


void Displ_Init(Displ_Orientat_e orientation);
void Displ_Orientation(Displ_Orientat_e orientation);
void Displ_WriteCommand(uint8_t cmd);
void Displ_WriteData(uint8_t* buff, size_t buff_size);

void Displ_FillArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void Displ_CLS(uint16_t bgcolor);
void Displ_Pixel(uint16_t x, uint16_t y, uint16_t color);
void Displ_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void Displ_Border(int16_t x, int16_t y, int16_t w, int16_t h, int16_t t,  uint16_t color);

void Displ_WChar(uint16_t x, uint16_t y, char ch, sFONT font, uint8_t size, uint16_t color, uint16_t bgcolor);
void Displ_WString(uint16_t x, uint16_t y, const char* str, sFONT font, uint8_t size, uint16_t color, uint16_t bgcolor);
void Displ_CString(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const char* str, sFONT font, uint8_t size, uint16_t color, uint16_t bgcolor);

void Displ_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);

void Displ_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void Displ_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void Displ_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void Displ_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void Displ_drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void Displ_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);

uint32_t Displ_BackLight(uint8_t cmd);

#endif /* INC_Z_GFX_FUNCTIONS_H_ */
