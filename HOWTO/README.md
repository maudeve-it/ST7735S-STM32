_**Piu' sotto, al termine della lingua inglese trovi il testo in italiano. </i>**_
_**Below English text you'll find the Italian version</i>**_

<br>
<br>

## HOW TO setup a project with this software

- CubeMX setup

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



---

## Project Folders

Software forder is here: https://github.com/maudeve-it/ST7735S-STM32/tree/main/SOURCE

"How to" add it to a CubeIDE project:
"How to" handle display backlight:

---
