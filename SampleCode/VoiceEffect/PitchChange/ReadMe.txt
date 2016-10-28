---------------------------------------------------------------------------------------------------------
Purpose:
---------------------------------------------------------------------------------------------------------
		1. 	Demo how to input sound from microphone and then output sound that pitch is shifted up or down.


---------------------------------------------------------------------------------------------------------
Operation:
---------------------------------------------------------------------------------------------------------
	LED:
		1. 	GPA14: show system standby
		2. 	GPA15: On - in playback. Off - not in playback.

	Key:
		1.	GPB0: start or stop playback.
		2. 	GPB1: pitch up 1 semitone.
		3. 	GPB2: pitch down 1 semitone.
		4. 	GPB3: power down(keep pressing than 4 second)/wake up.


---------------------------------------------------------------------------------------------------------
Note:
---------------------------------------------------------------------------------------------------------


---------------------------------------------------------------------------------------------------------
Must Know:
---------------------------------------------------------------------------------------------------------
	1.	The stack size is defined by "Stack_Size" in "startup_ISD9100.s" or in "startup_ISD9300.s". Can change the stack size
		by modifing the content of "Stack_Size".
	2.	The chip ROM size and SRAM size is specified by "ROM_SIZE" and "SRAM_SIZE"in  linker settings
		Open the linker setting and find the --pd="-DSRAM_SIZE=0xNNNN" --pd="-DROM_SIZE=0xMMMMM" in "misc controls" 
	3.	When the error happen:
			"L6388E: ScatterAssert expression (ImageLimit(_SRAM) < (0xNNNNNNNN + 0xMMMM)) failed on line xx" 
		It means the size of variables+stack is over chip SRAM size.
		Please open map file to see the overed SRAM size and reduce variables or stack size to fit chip SRAM size.
	4.	When the error happen:
			"L6220E: Load region _ROM size (NNNNN bytes) exceeds limit (MMM bytes)."
		It means the size of programs is over chip ROM size.
		Please open map file to see the overed programs size and reduce programs size to fit chip ROM size.
