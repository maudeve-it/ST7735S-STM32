
<br>

_**Piu' sotto, al termine della lingua inglese trovi il testo in italiano </i>**_

<br>

# Source and header files

- Copy the .c files into the /Core/Src folder in your STM32CubeIDE project
- Copy the .h files into the /Core/Inc folder in your STM32CubeIDE project
- into the /Core/Inc folder, open the main.h file and add (into "USER CODE BEGIN Includes" section?)include directive for every .h file copied

That should be enough.

"z_displ_ST7735_test" files (.c and .h) must be copied into the project only if you uant to use test/demo functions. You don't need to add it in the production project.



