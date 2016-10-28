/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                                         	*/
/* Copyright(c) Nuvoton Technology Corp. All rights reserved.                                              	*/
/*                                                                                                      	*/
/*----------------------------------------------------------------------------------------------------------*/
purpose of this sample code:
		1. Demo how to input soud from microphone and output with auto tune effect.


---------------------------------------------------------------------------------------------------------
Operation:
---------------------------------------------------------------------------------------------------------
	LED:
		1. 	GPA14:  show system standby
		2. 	GPB4: show auto tune function 

	Key:
		1. 	GPB0: pitch up
		2. 	GPB1: pitch down
		3. 	GPB2: start or stop playback
		4. 	GPB3: enable/disable auto tune effect
		5.	GPB6: power down(keep pressing than 4 second)/wake up


---------------------------------------------------------------------------------------------------------
Note:
---------------------------------------------------------------------------------------------------------
	1. 	Default pitch shift frequency is defined in ConfigAutoTuneApp.h. 
	2. 	The input and output sampling rate are defined in ConfigAutoTuneApp.h.


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
