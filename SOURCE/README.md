_**Piu' sotto, al termine della lingua inglese trovi il testo in italiano. </i>**_
_**Below English text you'll find the Italian version</i>**_

<br>
<br>

# Source and header files

- Copy the .c files into the /Core/Src folder in your STM32CubeIDE project
- Copy the .h files into the /Core/Inc folder in your STM32CubeIDE project
- into the /Core/Inc folder, open the main.h file and add (into "USER CODE BEGIN Includes" section?)include directive for every .h file copied

That should be enough.

"z_displ_ST7735_test" files (.c and .h) must be copied into the project only if you uant to use test/demo functions. You don't need to add it in the production project.

That should be enough, now you can move to these sections:
- ["How to" add it to a CubeIDE project](./HOWTO)
- ["How to" handle display backlight](./BACKLIGHT)



---

<br>
<br>

# File sorgente e di testata

- Copia i file .c nella cartella /Core/Src del progetto STM32CubeIDE
- Copia i file .h nella cartella /Core/Inc del progetto STM32CubeIDE
- Nella cartella /Core/Inc, apri il file main.h e aggiungi la direttiva include (nella sezione "USER CODE BEGIN Includes"?) per ogni file .h copiato.

I file "z_displ_ST7735_test" (.c e .h) devono essere copiati nel progetto solo se vuoi usare le funzioni di demo e test. Non occorrono questi file nella versione definitiva del progetto.

Dovrebbe bastrare, pra puoi andare alle sezioni:
- [Guida per integrare il software in un progetto con CubeIDE](./HOWTO)
- [Guida alla gestione della retroilluminazione](./BACKLIGHT)




