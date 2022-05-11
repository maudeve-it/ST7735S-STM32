_**Piu' sotto, al termine della lingua inglese trovi il testo in italiano. </i>**_
_**Below English text you'll find the Italian version</i>**_

<br>
<br>

# Handling backlight

this library can handle display backlight in two different modes:
- as a switched on-off light
- as a dimming backlight

The backlight mode is defined by the macro define "DISPLAY_DIMMER_MODE" in "z_DISPL_st7735.h" file
- If this definition is commented backight is switching on-off
- Uncomment the macro define enabling the dimming hendling 

# Switching ON-OFF Backlight
("#define DISPLAY_DIMMER_MODE" commented in "z_DISPL_st7735.h" file)
Define a GPIO pin in output mode as described in [HOWTO](../HOWTO) page giving it the name DISPL_LED
connect the LED pin of the display to DISPL_LED

That's all: function


