![TouchGFX](./TouchGFX.jpg)



_**Piu' sotto, al termine della lingua inglese trovi il testo in italiano. </i>**_
_**Below English text you'll find the Italian version</i>**_

<br>


# So...

Now that you have created a new CubeIDE project as per these instructions:

- ["How to" create a CubeIDE project for this library](../1-HOWTO)

Now that you have chosen the backlight handling mode and set it as per these instructions:

- ["How to" handle display backlight](../2-BACKLIGHT)


You are ready to go integrating TouchGFX!<br>
Follow the below instructions if you like handling the display through this GUI systems.<br>
Otherwise, if you want to directly draw on the display through the library functions, do not proceed here and go to [(Direct handling) "How to" add this library to the created project](./3B-DIRECT)
<br>

## Parameters setup
Your project is almost ready. You must "tune" the software parameters into "z_displ_ST7735.h":
- section 0 - TouchGFX parameter: ok, if you are here reading, it means you have to remove the comment sign to parameter in section 0, enabling TouchGFX handling
- section 1 - Port Parameters: here you have to set two macro constant with the SPI port name connecting display
- section 2 - SPI communication mode: uncomment the macro definition related to enabled communication mode (Polling mode, Interrupt mode or DMA mode). You must uncomment no less and no more than ONE definition here
- section 3 - Backlight Mode: see [BACKLIGHT page](../2-BACKLIGHT)  
- section 4 - Buffer size: the TouchGFX enabling will disable library buffer, so parameter set here is not used 
- section 5 - Display size: despite the ST7735 SPI driver can handle displays of any size and color modes, this library was tested only on a 128x160 display running in RBG565.  


## TouchGFX package setup

- enter in uC configuration (CubeMX) doubleclicking the ".ioc" file<br>
- goto "Software Packs", then to "Select components" and install "TouchGFX" if not yet done in your CubeIDE<br>
- after installation click on TouchGFX details and select it<br>
- close the Software packs component selector<br>
- goto Computing->CRC and activate it<br>
- goto Software Packs->TouchGFX and activate it<br>

now in TouchGFX generator, set these parameters:
|Parameter|Value|
|---|---|
|Framebuffer PixelFormat|RGB565|
|Width|160/128|   
|Height|128/160|
|Framebuffer Strategy|Partial Buffer|
all other parameters as per default<br>

Save and generate (or update) software.<br>




## main.c setup

in main.c you now need to initialize display before entering in the main loop, this way:<br>
<br>
  (main.c)
  ```sh
  ...
  /* USER CODE BEGIN 2 */			// "USER CODE BEGIN 2" is after all system initializations and before entering the main loop. Good place to initialize display
  Displ_Init(Displ_Orientat_0);		// initialize the display and set the initial display orientation (here is orientaton: 0°) - THIS FUNCTION MUST PRECEED ANY OTHER DISPLAY FUNCTION CALL.  
  touchgfxSignalVSync();			// asks TouchGFX to start initial display drawing
  Displ_BackLight('I');  			// initialize backlight and turn it on at init level
  /* USER CODE END 2 */
  ...
  ```

<br>
then, in the main loop, add the cycling activation of TouchGFX.<br>
You may ask a continuous TouchGFX refresh as here below:<br>
(an easier handling but a higher uC overload)<br>

  ```sh
  ...
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) 			
  {
       touchgfxSignalVSync();		// asks TouchGFX to get events and redraw screen
  
    /* USER CODE END WHILE */

       MX_TouchGFX_Process();
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
...
  ```
<br>
otherwise you can submit TouchGFX activation only to the events that need to be considered, see below: <br>
(having to take care of every event involving TouchGFX, but uC overload will be lower a lot)<br>

  ```sh
  ...
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) 			
  {
	if ("event occourred needing a display update")
		touchgfxSignalVSync();		// asks TouchGFX to get events and redraw screen
  
    /* USER CODE END WHILE */

	MX_TouchGFX_Process();
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
...
  ```


## using TouchGFX  in your projects

That's all, start using TouchGFX designer to setup your GUI!



## compiler warnings

When you first compile your project, it may stop on these two errors: <br> 

  ```sh
...
#error "A user must call touchgfx::startNewTransfer(); once touchgfxDisplayDriverTransmitBlock() has succesfully sent a block."
#error "A user must implement C-methods touchgfxDisplayDriverTransmitActive() and touchgfxDisplayDriverTransmitBlock() used by the Partial Framebuffer Strategy."
...
  ```

It is just a reminder for you to develop the interface routines to TouchGFX.<br>
They are already available in the library.<br>
So, just delete the two "#error" lines, the two warnings, in source file and re-compile the project.<br>
They will not appear anymore.<rb>

<br>
<br>

---

# Dunque...
Ora che hai creato un nuovo progetto con CubeIDE project seguendo queste istruzioni:

- [Guida per configurare un progetto con CubeIDE](./1-HOWTO)

Ora che hai scelto la gestione della retroilluminazione, seguendo queste istruzioni:

- [Guida alla gestione della retroilluminazione](./2-BACKLIGHT)

Puoi finalmente configurare gli ultimi parametri ed eseguire il programma!<br>
Segui le istruzioni sotto se vuoi gestire direttamente il display attraverso le funzioni grafiche della libreria.<br>
Se invece vuoi usare il pacchetto STM TouchGFX, non procedere qui e passa a [(gestione diretta) Guida per aggiungere la libreria al progetto creato](./2B-DIRECT)
<br>

## Configurazione Parametri
Il progetto è quasi pronto. Devi mettere a punti i parametri nel file "z_displ_ST7735.h":
- sezione 0 - TouchGFX: se stai leggendo qui devi togliere il commento alla la macro nella sezione 0, abilitando la gestione TouchGFX  
- sezione 1 - Port Parameters: qui devi impostare due costanti macro con il nome della porta SPI a cui è connesso il display
- section 2 - SPI communication mode: togli il commento alla definizione definition relativa al tipo di comunicazione attivato (Polling moode, Interrupt mode o DMA mode). Devi togliere il commento ad una riga (NON di più e NON di meno) 
- section 3 - Backlight Mode: vedi la [pagina BACKLIGHT](../2-BACKLIGHT)  
- section 4 - Dimensione del Buffer: TouchGFX non utilizza i buffer della libreria, che vengono disabilitati. Il parametro impostato qui non è utilizzato.
- section 5 - Dimensione Display: anche se il driver SPI ST7735 può gestire display di dimensioni diverse, questa libreria è stata testata solo su un display di 128x160 pixel in modalità RBG565.  



## Configurazione TouchGFX package

- entra nella configurazione del uC (CubeMX) con un doppio click sul file ".ioc"<br>
- vai in "Software Packs", poi in "Select components" e installa "TouchGFX" se non lo hai ancora fatto sul tuo CubeIDE<br>
- dopo l'installazione clicca sulla tendina TouchGFX e selezionalo<br>
- chiudi il Software packs component selector<br>
- vai in Computing->CRC e attivalo<br>
- vai in Software Packs->TouchGFX e attivalo<br>

ora, in TouchGFX generator, imposta questi parametri:
|Parametro|Valore|
|---|---|
|Framebuffer PixelFormat|RGB565|
|Width|160/128|
|Height|128/160|
|Framebuffer Strategy|Partial Buffer|
tutti gli altri parametri come default<br>

Salva e genera (o aggiorna) il software.<br>



## impostazione di main.c 

in main.c ora devi inizializzare il display prima di entrare nel main loop, in questo modo:

  (main.c)
  ```sh
  ...
  /* USER CODE BEGIN 2 */			// "USER CODE BEGIN 2" is after all system initializations and before entering the main loop. Good place to initialize display
  Displ_Init(Displ_Orientat_0);		// initialize the display and set the initial display orientation (here is orientaton: 0°) - THIS FUNCTION MUST PRECEED ANY OTHER DISPLAY FUNCTION CALL.  
  touchgfxSignalVSync();		// asks TouchGFX to start initial display drawing
  Displ_BackLight('I');  			// initialize backlight and turn it on at init level
  /* USER CODE END 2 */
  ...
  ```

<br>poi, nel loop principale, aggiungi l'attivazione ciclica di TouchGFX.<br>
Puoi chiedere un aggiornamento continuo di TouchGFX come qui sotto:<br>
(gestione semplice ma con un maggiore sovraccarico del uC)<br>

  ```sh
  ...
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) 			// into the main loop you can add the test functions
  {
  	touchgfxSignalVSync();		// asks TouchGFX to get events and redraw screen
  
    /* USER CODE END WHILE */

	  MX_TouchGFX_Process();
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
...
  ```

<br>oppure puoi sottoporre l'attivazione di TouchGFX solo agli eventi che richiedono di essere considerati, vedi qui: <br>
(dovrai fare attenzione ad attivare TouchGFX in ogni specifico evento, ma il uC il sovraccarico al uC è molto minore )<br>

  ```sh
  ...
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) 			// into the main loop you can add the test functions
  {
	if ("e' accaduto un evento che richiede l'aggiornamento del display")
		touchgfxSignalVSync();		// asks TouchGFX to get events and redraw screen
  
    /* USER CODE END WHILE */

	  MX_TouchGFX_Process();
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
...
  ```



## usare TouchGFX nel tup progetto

E' tutto. Inizia ad usare TouchGFX designer per impostare la tua GUI!



## compiler warnings

Quando compili il progetto per la prima volta, potrebbe fermarsi su questi due errori:<br> 

  ```sh
...
#error "A user must call touchgfx::startNewTransfer(); once touchgfxDisplayDriverTransmitBlock() has succesfully sent a block."
#error "A user must implement C-methods touchgfxDisplayDriverTransmitActive() and touchgfxDisplayDriverTransmitBlock() used by the Partial Framebuffer Strategy."
...
  ```

Si tratta semplicemente di un avviso per ricordarti di sviluppare le routine di interfaccia a TouchGFX.<br>
Queste sono già disponibili nella libreria.<br>
Quindi, semplicemente cancella le due righe "#error", i due avvisi, nel file sorgente e ricompila il progetto.<br>
Non appariranno più.<br>

