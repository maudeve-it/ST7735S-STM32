/*
 * z_gfx_functions.c
 *
 *  Created on: Oct 9, 2021
 *      Author: mauro
 */


#include "main.h"

extern SPI_HandleTypeDef DISPLAY_SPI_PORT;

static volatile uint32_t dispSpiAvailable=1;  		// 0 if SPI is busy or 1 if it is free (transm cplt)

int16_t _width;       								///< (oriented) display width
int16_t _height;      								///< (oriented) display height
uint8_t _colstart;   								///< Some displays need this changed to offset
uint8_t _rowstart;       							///< Some displays need this changed to offset
uint8_t _xstart;
uint8_t _ystart;

static uint8_t dispBuffer1[SIZEBUF];
static uint8_t dispBuffer2[SIZEBUF];
static uint8_t *dispBuffer=dispBuffer1;



/****** ST7735 driver's DEFINITIONS ******/
#define DELAY 					0x80
#define ST7735_RESET_DELAY		5
#define ST7735_FLAG_LONG_DELAY	255
#define ST7735_LONG_DELAY		500

#define ST7735_MADCTL_MY  0x80
#define ST7735_MADCTL_MX  0x40
#define ST7735_MADCTL_MV  0x20
#define ST7735_MADCTL_ML  0x10
#define ST7735_MADCTL_RGB 0x00
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MH  0x04

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1
/****** ST7735 driver's DEFINITIONS ******/







const uint8_t
init_cmds1[] = {            // Init for 7735R, part 1 (red or green tab)
  15,                       // 15 commands in list:
  ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
    150,                    //     150 ms delay
  ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
  ST7735_FLAG_LONG_DELAY,       //     500 ms delay
  ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
    0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
  ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
    0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
  ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
    0x01, 0x2C, 0x2D,       //     Dot inversion mode
    0x01, 0x2C, 0x2D,       //     Line inversion mode
  ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
    0x07,                   //     No inversion
  ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
    0xA2,
    0x02,                   //     -4.6V
    0x84,                   //     AUTO mode
  ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
    0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
  ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
    0x0A,                   //     Opamp current small
    0x00,                   //     Boost frequency
  ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
    0x8A,                   //     BCLK/2, Opamp current small & Medium low
    0x2A,
  ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
    0x8A, 0xEE,
  ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
    0x0E,
  ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
  ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
    0x05 },                 //     16-bit color

#if (defined(ST7735_IS_128X128) || defined(ST7735_IS_160X128))
init_cmds2[] = {            // Init for 7735R, part 2 (1.44" display)
  2,                        //  2 commands in list:
  ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
    0x00, 0x00,             //     XSTART = 0
    0x00, 0x7F,             //     XEND = 127
  ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
    0x00, 0x00,             //     XSTART = 0
    0x00, 0x7F },           //     XEND = 127
#endif // ST7735_IS_128X128

#ifdef ST7735_IS_160X80
init_cmds2[] = {            // Init for 7735S, part 2 (160x80 display)
  3,                        //  3 commands in list:
  ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
    0x00, 0x00,             //     XSTART = 0
    0x00, 0x4F,             //     XEND = 79
  ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
    0x00, 0x00,             //     XSTART = 0
    0x00, 0x9F ,            //     XEND = 159
  ST7735_INVON, 0 },        //  3: Invert colors
#endif

init_cmds3[] = {            // Init for 7735R, part 3 (red or green tab)
  4,                        //  4 commands in list:
  ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
    0x02, 0x1c, 0x07, 0x12,
    0x37, 0x32, 0x29, 0x2d,
    0x29, 0x25, 0x2B, 0x39,
    0x00, 0x01, 0x03, 0x10,
  ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
    0x03, 0x1d, 0x07, 0x06,
    0x2E, 0x2C, 0x29, 0x2D,
    0x2E, 0x2E, 0x37, 0x3F,
    0x00, 0x00, 0x02, 0x10,
  ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
    10,                     //     10 ms delay
  ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
    100 };                  //     100 ms delay



void ST7735_Reset()
{
  HAL_GPIO_WritePin(DISPL_RST_GPIO_Port, DISPL_RST_Pin, GPIO_PIN_RESET);
  HAL_Delay(ST7735_RESET_DELAY);
  HAL_GPIO_WritePin(DISPL_RST_GPIO_Port, DISPL_RST_Pin, GPIO_PIN_SET);
}


void ST7735_InitCmds(const uint8_t *addr)
{
	uint8_t numCommands, numArgs;
	uint16_t ms;

	numCommands = *addr++;
	while(numCommands--) {
		uint8_t cmd = *addr++;
		Displ_WriteCommand(cmd);

		numArgs = *addr++;
		// If high bit set, delay follows args
		ms = numArgs & DELAY;
		numArgs &= ~DELAY;
		if(numArgs) {
// send 1 command each data transfer in init functions
//			Displ_WriteData((uint8_t*)addr, numArgs);
//			addr += numArgs;
			for (uint8_t k=0;k<numArgs;k++){
				Displ_WriteData((uint8_t*)addr++, 1);
			}
		}

		if(ms) {
			ms = *addr++;
			if(ms == ST7735_FLAG_LONG_DELAY) ms = ST7735_LONG_DELAY;
			HAL_Delay(ms);
		}
	}
}



void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	// column address set
	Displ_WriteCommand(ST7735_CASET);
	uint8_t data[] = { 0x00, x0 + _xstart, 0x00, x1 + _xstart };
	Displ_WriteData(data, sizeof(data));

	data[1] = y0 + _ystart;
	data[3] = y1 + _ystart;
	// row address set
	Displ_WriteCommand(ST7735_RASET);
	Displ_WriteData(data, sizeof(data));

	// write to RAM
	Displ_WriteCommand(ST7735_RAMWR);
}




void ST7735_SetRotation(Displ_Orientat_e m){

	uint8_t madctl = 0;

	switch (m){
	case Displ_Orientat_0:
#if ST7735_IS_160X80
		madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_BGR;
#else
		madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_RGB;
		_height = ST7735_HEIGHT;
		_width = ST7735_WIDTH;
		_xstart = _colstart;
		_ystart = _rowstart;
#endif
		break;
	case Displ_Orientat_90:
#if ST7735_IS_160X80
		madctl = ST7735_MADCTL_MY | ST7735_MADCTL_MV | ST7735_MADCTL_BGR;
#else
		madctl = ST7735_MADCTL_MY | ST7735_MADCTL_MV | ST7735_MADCTL_RGB;
		_width = ST7735_HEIGHT;
		_height = ST7735_WIDTH;
		_ystart = _colstart;
		_xstart = _rowstart;
#endif
		break;
	case Displ_Orientat_180:
#if ST7735_IS_160X80
		madctl = ST7735_MADCTL_BGR;
#else
		madctl = ST7735_MADCTL_RGB;
		_height = ST7735_HEIGHT;
		_width = ST7735_WIDTH;
		_xstart = _colstart;
		_ystart = _rowstart;
#endif
		break;
	case Displ_Orientat_270:
#if ST7735_IS_160X80
		madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MV | ST7735_MADCTL_BGR;
#else
		madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MV | ST7735_MADCTL_RGB;
		_width = ST7735_HEIGHT;
		_height = ST7735_WIDTH;
		_ystart = _colstart;
		_xstart = _rowstart;
#endif
		break;
	}
	//  ST7735_Select();
	Displ_WriteCommand(ST7735_MADCTL);
	Displ_WriteData(&madctl,1);
	//  ST7735_Unselect();
}




void ST7735_Init(Displ_Orientat_e orientation)
{
	//    ST7735_Select();
	ST7735_Reset();
	ST7735_InitCmds(init_cmds1);
	ST7735_InitCmds(init_cmds2);
	ST7735_InitCmds(init_cmds3);
#if ST7735_IS_160X80
	_colstart = 24;
	_rowstart = 0;
	/*****  IF Doesn't work, remove the code below (before #elif) *****/
	uint8_t data = 0xC0;
	//ST7735_Select();
	Displ_WriteCommand(ST7735_MADCTL);
	Displ_WriteData(&data,1);
	//ST7735_Unselect();

#elif ST7735_IS_128X128
	_colstart = 2;
	_rowstart = 3;
#else
	_colstart = 0;
	_rowstart = 0;
#endif
	ST7735_SetRotation (orientation);

}








void ST7735_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
  if((x >= _width) || (y >= _height)) return;
  if((x + w - 1) >= _width) return;
  if((y + h - 1) >= _height) return;

  //ST7735_Select();
  ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);
  Displ_WriteData((uint8_t*)data, sizeof(uint16_t)*w*h);
  //ST7735_Unselect();
}


void ST7735_InvertColors(uint8_t invert) {
  //ST7735_Select();
  Displ_WriteCommand(invert ? ST7735_INVON : ST7735_INVOFF);
  //ST7735_Unselect();
}










/*****************************
 * @brief	fill a rectangle with a color
 * @param	x, y	top left corner of the rectangle
 * 			w, h 	width and height of the rectangle
 ******************************/
void Displ_FillArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
	uint16_t k,x1,y1,area,times;

	uint32_t data32;

    if((x >= _width) || (y >= _height) || (w == 0) || (h == 0)) return;//

    x1=x + w - 1;
    if (x1 > _width) {
    	x1=_width;
    }

    y1=y + h - 1;
    if (y1 > _height) {
    	y1=_height;
    }


	data32=(color>>8) | (color<<8) | (color<<24); 	// supposing color is 0xABCD, data32 becomes 0xCDABCDAB - set a 32 bit variable with swapped endians
    area=((y1-y+1)*(x1-x+1)); 						// area to fill in 16bit pixels
	uint32_t *buf32Pos=(uint32_t *)dispBuffer; 		//dispBuffer defined in bytes, buf32Pos access it as 32 bit words
	if (area<(SIZEBUF>>1)) 							// if area is smaller than dispBuffer
		times=(area>>1)+1; 							// number of times data32 has to be loaded into buffer
	else
		times=(SIZEBUF>>2);  						// dispBuffer size as 32bit-words
	for (k = 0; k < times; k++)
		*(buf32Pos++)=data32; 						// loads buffer moving 32bit-words
	times=(area>>(BUFLEVEL-1));  					//how many times buffer must be sent via SPI. It is (BUFFLEVEL-1) because area is 16-bit while dispBuffer is 8-bit

	ST7735_SetAddressWindow(x, y, x1, y1);

	for  (k=0;k<times;k++) {
		Displ_WriteData(dispBuffer,SIZEBUF);
	}
	Displ_WriteData(dispBuffer,(area<<1)-(times<<BUFLEVEL));
	dispBuffer = (dispBuffer==dispBuffer1 ? dispBuffer2 : dispBuffer1); // swapping buffer

}





/***********************
 * @brief	print a single pixel
 * @params	x, y	pixel position on display
 * 			color	... to be printed
 ***********************/
void Displ_Pixel(uint16_t x, uint16_t y, uint16_t color) {
    if((x >= _width) || (y >= _height))
        return;
    Displ_FillArea(x, y, 1, 1, color);

}


/***********************
 * @brief	print an empty rectangle of a given thickness
 * @params	x, y	top left corner
 * 			w, h	width and height
 * 			t		border thickness
 * 			color	border color, inner part unchanged
 ***********************/
void Displ_Border(int16_t x, int16_t y, int16_t w, int16_t h, int16_t t,  uint16_t color){
	Displ_FillArea(x, y, w, t, color);
	Displ_FillArea(x, y+h-t, w, t, color);
	Displ_FillArea(x, y, t, h, color);
	Displ_FillArea(x+w-t, y, t, h, color);
}



/************************
 * @brief	draws a line from "x0","y0" to "x1","y1" of the given "color"
 ************************/
void Displ_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
	int16_t l,x,steep,ystep,err,dx, dy;

    if (x0==x1){  // fast solve vertical lines
    	if (y1>y0){
    		Displ_FillArea(x0, y0, 1, y1-y0+1, color);
    	}
    	else {
    		Displ_FillArea(x0, y1, 1, y0-y1+1, color);
    	}
    	return;
    }
    if (y0==y1){ // fast solve horizontal lines
    	if (x1>x0)
    		Displ_FillArea(x0, y0, x1-x0+1, 1, color);
    	else
    		Displ_FillArea(x1, y1, x0-x1+1, 1, color);
    	return;
    }

    steep = (y1>y0 ? y1-y0 : y0-y1) > (x1>x0 ? x1-x0 : x0-x1);

    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    dx = x1 - x0;
    err = dx >> 1;
    if (y0 < y1) {
        dy = y1-y0;
        ystep =  1 ;
    } else {
        dy = y0-y1;
        ystep =  -1 ;
    }

    l=00;
    for (x=x0; x<=x1; x++) {
    	l++;
        err -= dy;
        if (err < 0) {
        	if (steep) {
        		Displ_FillArea(y0, x0, 1, l, color);
            } else {
            	Displ_FillArea(x0, y0, l, 1, color);
            }
            y0 += ystep;
            l=0;
            x0=x+1;
            err += dx;
        }
    }
    if (l!=0){
    	if (steep) {
    		Displ_FillArea(y0, x0, 1, l-1, color);
    	} else {
    		Displ_FillArea(x0, y0, l-1,1, color);
    	}
    }
}





/***********************
 * @brief	display one character on the display
 * @param 	x,y: top left corner of the character to be printed
 * 			ch, font, color, bgcolor: as per parameter name
 * 			size: (1 or 2) single or double wided printing
 **********************/
void Displ_WChar(uint16_t x, uint16_t y, char ch, sFONT font, uint8_t size, uint16_t color, uint16_t bgcolor) {
    uint32_t i, b, bytes, j, bufSize, mask;
    uint16_t *dispBuffer16=(uint16_t *)dispBuffer;

    const uint8_t *pos;
	uint8_t wsize=font.Width; //printing char width
	uint16_t color1, bgcolor1;

	if (size==2)
		wsize<<= 1;
	bufSize=0;
	bytes=font.Height * font.Size ;
	pos=font.table+(ch - 32) * bytes ;//that's char position in table
	switch (font.Size) {
		case 3:
			mask=0x800000;
			break;
		case 2:
			mask=0x8000;
			break;
		default:
			mask=0x80;
	}
	color1 = ((color & 0xFF)<<8 | (color >> 8));      		//swapping byte endian: STM32 is little endian, ST7735 is big endian
	bgcolor1 = ((bgcolor & 0xFF)<<8 | (bgcolor >> 8));		//swapping byte endian: STM32 is little endian, ST7735 is big endian


	for(i = 0; i < (bytes); i+=font.Size){
		b=0;
		switch (font.Size) {
			case 3:
				b=pos[i]<<16 | pos[i+1]<<8 | pos[i+2];
				break;
			case 2:
				b=pos[i]<<8 | pos[i+1];
				break;
			default:
				b=pos[i];
		}

		for(j = 0; j < font.Width; j++) {
			if((b << j) & mask)  {
				dispBuffer16[bufSize++] = color1;
				if (size==2){
					dispBuffer16[bufSize++] = color1;
				}
			} else {
				dispBuffer16[bufSize++] = bgcolor1;
				if (size==2) {
					dispBuffer16[bufSize++] = bgcolor1;
				}
			}
		}
	}
	ST7735_SetAddressWindow(x, y, x+wsize-1, y+font.Height-1);
	Displ_WriteData(dispBuffer,bufSize<<1);
	dispBuffer = (dispBuffer==dispBuffer1 ? dispBuffer2 : dispBuffer1); // swapping buffer


}





/************************
 * @brief	print a string on display starting from a defined position
 * @params	x, y	top left area-to-print corner
 * 			str		string to print
 * 			font	to bu used
 * 			size	1 (normal), 2 (double width)
 * 			color	font color
 * 			bgcolor	background color
 ************************/
void Displ_WString(uint16_t x, uint16_t y, const char* str, sFONT font, uint8_t size, uint16_t color, uint16_t bgcolor) {
	uint16_t delta=font.Width;
	if (size>1)
		delta<<=1;

    while(*str) {
/*
 *  these rows split string in more screen lines
        if(x + font.Width >= _width) {
            x = 0;
            y += font.Height;
            if(y + font.Height >= _height) {
                break;
            }
            if(*str == ' ') {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }
 */

        Displ_WChar(x, y, *str, font, size, color, bgcolor);
        x += delta;
        str++;
    }
}





/************************
 * @brief	print a string on display centering into a defined area
 * @params	x0, y0	top left area corner
 * 			x1, y1	bottom right corner
 * 			str		string to print
 * 			font	to bu used
 * 			size	1 (normal), 2 (double width)
 * 			color	font color
 * 			bgcolor	background color
 ************************/
void Displ_CString(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const char* str, sFONT font, uint8_t size, uint16_t color, uint16_t bgcolor) {
	uint16_t x,y;
	uint16_t wsize=font.Width;
	static uint8_t cambia=0;
	if (size>1)
		wsize<<=1;
	if ((strlen(str)*wsize)>(x1-x0+1))
		x=x0;
	else
		x=(x1+x0+1-strlen(str)*wsize) >> 1;
	if (font.Height>(y1-y0+1))
		y=y0;
	else
		y=(y1+y0+1-font.Height) >> 1;

	if (x>x0){
		Displ_FillArea(x0,y0,x-x0,y1-y0+1,bgcolor);
	} else
		x=x0; // fixing here mistake could be due to roundings: x lower than x0.
	if (x1>(strlen(str)*wsize+x0))
		Displ_FillArea(x1-x+x0-1,y0,x-x0+1,y1-y0+1,bgcolor);

	if (y>y0){
		Displ_FillArea(x0,y0,x1-x0+1,y-y0,bgcolor);
	} else
		y=y0; //same comment as above
	if (y1>=(font.Height+y0))
		Displ_FillArea(x0,y1-y+y0,x1-x0+1,y-y0+1,bgcolor);

	cambia = !cambia;

	Displ_WString(x, y, str, font, size, color, bgcolor);

}





/*****************
 * @brief	first display initialization.
 * @param	orientation: display orientation
 *****************/
void Displ_Init(Displ_Orientat_e orientation){
	ST7735_Init(orientation);
}


/*****************
 * @brief	set display orientation.
 * @param	orientation: display orientation
 *****************/
void Displ_Orientation(Displ_Orientat_e orientation){
	ST7735_SetRotation(orientation);
}


/*****************
 * @brief	clear display with a color.
 * @param	bgcolor
 *****************/
void Displ_CLS(uint16_t bgcolor){
	Displ_FillArea(0, 0, _width, _height, bgcolor);
}



void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	__NOP();
	if (hspi->Instance==DISPLAY_SPI) {
		dispSpiAvailable=1;
	}
}




void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
	if (hspi->Instance==DISPLAY_SPI) {
		dispSpiAvailable=1;
	}
}




/**************************
 * @BRIEF	engages SPI port communicating with displayDC_Status
 * 			depending on the macro definition makes transmission in Polling/Interrupt/DMA mode
 * @PARAM	DC_Status 	indicates if sending command or data
 * 			data		buffer data to send
 * 			dataSize	number of bytes in "data" to be sent
***************************/
void Displ_Transmit(GPIO_PinState DC_Status, uint8_t* data, uint16_t dataSize ){

while (!dispSpiAvailable) {};  // waiting for a free SPI port. Flag is set to 1 by transmission-complete interrupt callback

HAL_GPIO_WritePin(DISPL_DC_GPIO_Port, DISPL_DC_Pin, DC_Status);

#ifndef DISPLAY_SPI_POLLING_MODE
	if (dataSize<DISPL_DMA_CUTOFF) {
		dispSpiAvailable=0;
		HAL_SPI_Transmit(&DISPLAY_SPI_PORT , data, dataSize, HAL_MAX_DELAY);
		dispSpiAvailable=1;
	}
	else {
		dispSpiAvailable=0;
		#ifdef DISPLAY_SPI_DMA_MODE
			HAL_SPI_Transmit_DMA(&DISPLAY_SPI_PORT , data, dataSize);
		#endif
		#ifdef DISPLAY_SPI_INTERRUPT_MODE
			HAL_SPI_Transmit_IT(&DISPLAY_SPI_PORT , data, dataSize);
		#endif
	}
#endif

#ifdef DISPLAY_SPI_POLLING_MODE
	dispSpiAvailable=0;
	HAL_SPI_Transmit(&DISPLAY_SPI_PORT , data, dataSize, HAL_MAX_DELAY);
	dispSpiAvailable=1;
#endif
}






/* using NSS Hardware Device Select
 *
 */
void Displ_WriteCommand(uint8_t cmd)
{
	Displ_Transmit(SPI_COMMAND, &cmd, sizeof(cmd));
}







/* using NSS Hardware Device Select
 *
 */
void Displ_WriteData(uint8_t* buff, size_t buff_size)
{
	if (buff_size==0) return;
	Displ_Transmit(SPI_DATA, buff, buff_size);
}





/****************************************************************************
 ************** demo functions from previous library *************************
 ************** not checked, nor optimized
 ****************************************************************************/



void drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        if (cornername & 0x4) {
            Displ_Pixel(x0 + x, y0 + y, color);
            Displ_Pixel(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
        	Displ_Pixel(x0 + x, y0 - y, color);
        	Displ_Pixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
        	Displ_Pixel(x0 - y, y0 + x, color);
        	Displ_Pixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
        	Displ_Pixel(x0 - y, y0 - x, color);
        	Displ_Pixel(x0 - x, y0 - y, color);
        }
    }
}





void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t corners, int16_t delta, uint16_t color){
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;
    int16_t px    = x;
    int16_t py    = y;

    delta++; // Avoid some +1's in the loop

    while(x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        // These checks avoid double-drawing certain lines, important
        // for the SSD1306 library which has an INVERT drawing mode.
        if(x < (y + 1)) {
//            if(corners & 1) drawFastVLine(x0+x, y0-y, 2*y+delta, color);
            if(corners & 1) Displ_Line(x0+x, y0-y, x0+x, y0-1+y+delta, color);
//            if(corners & 2) drawFastVLine(x0-x, y0-y, 2*y+delta, color);
            if(corners & 2) Displ_Line(x0-x, y0-y, x0-x, y0-1+y+delta, color);
        }
        if(y != py) {
//            if(corners & 1) drawFastVLine(x0+py, y0-px, 2*px+delta, color);
            if(corners & 1) Displ_Line(x0+py, y0-px, x0+py, y0-1+px+delta, color);
//            if(corners & 2) drawFastVLine(x0-py, y0-px, 2*px+delta, color);
            if(corners & 1) Displ_Line(x0-py, y0-px, x0-py, y0-1+px+delta, color);
            py = y;
        }
        px = x;
    }
}






void Displ_drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
    int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
    if(r > max_radius) r = max_radius;
    // smarter version
//    drawFastHLine(x+r  , y    , w-2*r, color); // Top
    Displ_Line(x+r, y, x+w-r-1, y, color);
//    drawFastHLine(x+r, y+h-1, w-2*r, color); // Bottom
    Displ_Line(x+r, y+h-1, x-1+w-r, y+h-1, color);
//    drawFastVLine(x, y+r, h-2*r, color); // Left
    Displ_Line(x, y+r, x, y-1+h-r, color); // Left
//    drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
    Displ_Line(x+w-1, y+r, x+w-1, y-1+h-r, color); // Right
    // draw four corners
    drawCircleHelper(x+r    , y+r    , r, 1, color);
    drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
    drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
    drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}





void Displ_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
    int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
    if(r > max_radius) r = max_radius;
    // smarter version
    Displ_FillArea(x+r, y, w-2*r, h, color);
    // draw four corners
    fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
    fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}







void Displ_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{

    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
    }
    if (y1 > y2) {
        _swap_int16_t(y2, y1); _swap_int16_t(x2, x1);
    }
    if (y0 > y1) {
        _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
    }

    if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if(x1 < a)      a = x1;
        else if(x1 > b) b = x1;
        if(x2 < a)      a = x2;
        else if(x2 > b) b = x2;
//        drawFastHLine(a, y0, b-a+1, color);
        Displ_Line(a, y0, b, y0, color);
        return;
    }

    int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
    int32_t
    sa   = 0,
    sb   = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if(y1 == y2) last = y1;   // Include y1 scanline
    else         last = y1-1; // Skip it

    for(y=y0; y<=last; y++) {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) _swap_int16_t(a,b);
//        drawFastHLine(a, y, b-a+1, color);
        Displ_Line(a, y, b, y, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = (int32_t)dx12 * (y - y1);
    sb = (int32_t)dx02 * (y - y0);
    for(; y<=y2; y++) {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) _swap_int16_t(a,b);
//      drawFastHLine(a, y, b-a+1, color);
        Displ_Line(a, y, b, y, color);
    }
}





void Displ_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    Displ_Line(x0, y0, x1, y1, color);
    Displ_Line(x1, y1, x2, y2, color);
    Displ_Line(x2, y2, x0, y0, color);
}





void Displ_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
//    drawFastVLine(x0, y0-r, 2*r+1, color);
    Displ_Line(x0, y0-r, x0, y0+r, color);

    fillCircleHelper(x0, y0, r, 3, 0, color);
}





void Displ_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

//    writePixel(x0  , y0+r, color);
    Displ_Pixel(x0  , y0+r, color);
    Displ_Pixel(x0  , y0-r, color);
    Displ_Pixel(x0+r, y0  , color);
    Displ_Pixel(x0-r, y0  , color);

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        Displ_Pixel(x0 + x, y0 + y, color);
        Displ_Pixel(x0 - x, y0 + y, color);
        Displ_Pixel(x0 + x, y0 - y, color);
        Displ_Pixel(x0 - x, y0 - y, color);
        Displ_Pixel(x0 + y, y0 + x, color);
        Displ_Pixel(x0 - y, y0 + x, color);
        Displ_Pixel(x0 + y, y0 - x, color);
        Displ_Pixel(x0 - y, y0 - x, color);
    }
}


/********************************************
 ********************************************
 ********************************************/







/**************************************
 * @brief		set backlight level
 * 				PLEASE NOTE: if not in "DIMMING MODE" only 'F', '1', '0' and 'Q' available
 * @param	cmd	'S'		put display in stby (light level=BKLIT_STBY_LEVEL)
 * 				'W' 	wake-up from stdby restoring previous level
 *				'+'		add 1 step to the current light level
 *				'-'		reduce 1 step to the current light level
 *				'F','1'	set the display level to max
 *				'0'		set the display level to 0 (off)
 *				'I'		'Initialize'  IT MUST BE in dimming mode
 *              'Q'		do nothing, just return current level
 * @return		current backlight level
 *
 */
uint32_t Displ_BackLight(uint8_t cmd) {

#ifdef DISPLAY_DIMMING_MODE
	static uint16_t memCCR1=0;  			//it stores CCR1 value while in stand-by
#endif

	switch (cmd) {
#ifndef DISPLAY_DIMMING_MODE
	case 'F':
	case '1':
		HAL_GPIO_WritePin(DISPL_LED_GPIO_Port, DISPL_LED_Pin, GPIO_PIN_SET);
		break;
	case '0':
		HAL_GPIO_WritePin(DISPL_LED_GPIO_Port, DISPL_LED_Pin, GPIO_PIN_RESET);
		break;
#else
	case 'F':
	case '1':
		BKLIT_TIMER->CCR1=BKLIT_TIMER->ARR;
		break;
	case '0':
		BKLIT_TIMER->CCR1=0;
		break;
	case 'W':
		BKLIT_TIMER->CCR1=memCCR1;					//restore previous level
		break;
	case 'S':
		memCCR1=BKLIT_TIMER->CCR1;
		if (BKLIT_TIMER->CCR1>=(BKLIT_STBY_LEVEL))	//set stby level only if current level is higher
			BKLIT_TIMER->CCR1=(BKLIT_STBY_LEVEL);
		break;
	case '+':
		if (BKLIT_TIMER->ARR>BKLIT_TIMER->CCR1)		// if CCR1 has not yet the highest value (ARR)
			++BKLIT_TIMER->CCR1;
		else
			BKLIT_TIMER->CCR1=BKLIT_TIMER->ARR;
		break;
	case '-':
		if (BKLIT_TIMER->CCR1>0)					// if CCR1 has not yet the lowest value (0)
			--BKLIT_TIMER->CCR1;
		else
			BKLIT_TIMER->CCR1=0;
		break;
	case 'I':
		Displ_BackLight(BKLIT_INIT_LEVEL);
		break;
#endif
	case 'Q':
		break;
	default:
		break;
	}
#ifndef DISPLAY_DIMMING_MODE
	return HAL_GPIO_ReadPin(DISPL_LED_GPIO_Port, DISPL_LED_Pin);
#else
	return (BKLIT_TIMER->CCR1);
#endif
}






