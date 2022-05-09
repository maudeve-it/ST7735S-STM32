_**Piu' sotto, al termine della lingua inglese trovi il testo in italiano. </i>**_
_**Below English text you'll find the Italian version</i>**_

<br>
<br>

## HOW TO setup a project with this software


##### CubeMX setup
- create a new project on STM32CubeIDE
- enable an SPI port. Configuration is:
  - mode: Half-Duplex Master
  - NSS: Hardware NSS Output Signal
  - (as per manufacturer specifications) Baud Rate: <= 15 MBit/s  
  - all other parameter as default:
    - Motorola frame format
    - 8 bit
    - MSB first
    - CPOL low
    - CPHA 1 Edge
    - no CRC
    - NSS Output Hw
  - if DMA communication:
    - enable only TX DMA
    - enable also SPI global interrupt
- assign these names yo SPI pins:
  - NSS=DISPL_CS
  - MOSI=DISPL_MOSI
  - SCK=DISPL_SCK
- enable 3 more pins as GPIO_Output:
  - configuration is:
    - Output push pull
    - No pull-up/down 
- assign these names yo GPIO pins:
  - DISPL_LED
  - DISPL_DC
  - DISPL_RST

##### Downloading SW
- download and install software as described [here](../SOURCE)

##### Parameters setup
into "z_displ_ST7735.h" file you have to setup this configuration:
- section 1 - Port Parameters: here you have to set two macro constant with the SPI port name connecting display
- section 2 - SPI communication mode: uncomment the macro definition related to enabled communication mode (Polling moode, Interrupt mode or DMA mode). You must uncomment no less and no more than ONE definition here
- section 3 - Backlight Timer: see [BACKLIGHT page](../BACKLIGHT)  
- section 4 - Buffer size: define the "Buffer Level" you like (between 10 and 16 included) see [youtube video](../BACKLIGHT) for information and analisys.  
- section 5 - Display size: despite the SPI driver can handle displays of any size and color modes, this library was tensed only on a 128x160 display running in RBG565.  
 


--


---
