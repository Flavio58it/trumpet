/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) Nuvoton Technology Corp. All rights reserved.                                              */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

// ---------------------------------------------------------------------------------------------------------
//	Functions:
//		- System clock configuration.
//		- Keypad configuration.
//		- SPI Flash configuration.
//		- Speaker configuration.
//		- MIC configuration.
//		- Output pin configuration.
//		- UltraIO configuration.
//		- Application Initiation.
//		- Processing loop:
//			* Codec processing(use functions in "AppFunctions.c").
//			* Voice effect processing(use functions in "AppFunctions.c").
//			* Keypad check and execution actions(use functions in "InputKeyActions.c").
//			* Etc.
//
//	Reference "Readme.txt" for more information.
// ---------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>

#include "App.h"
#include "Framework.h"
#include "Keypad.h"
#include "SPIFlash.h"
#include "ConfigSysClk.h"
#include "MicSpk.h"

#include "DrvUart.h"
#include "DrvI2C.h"

#if( !defined(__CHIP_SERIES__) )
#error "Please update SDS version >= v5.0."
#endif

// SPI flash handler.
S_SPIFLASH_HANDLER g_sSpiFlash;
// Application control.
volatile UINT8 g_u8AppCtrl;
// Application handler.
S_APP g_sApp;

#ifdef NUONE_ATC
volatile uint8_t g_u8AtcCmd = 0;
volatile uint8_t g_u8AtcCmd2 = 0;
uint32_t g_u32AtcParam;

extern BOOL App_StartPlay(void);
extern BOOL App_StopPlay(void);
extern void App_PowerDown(void);
#endif

extern void App_Initiate(void);
extern BOOL App_StopPlay(void);
extern BOOL App_ProcessPlay(void);

UINT8 SPIFlash_Initiate(void)
{
	UINT16 ui16Temp;
	UINT32 ui32Temp;
	UINT32 u32Count;

	// SPI0: GPA1=SSB00, GPA2=SCLK0, GPA3=MISO0, GPA4=MOSI0
	SYS->GPA_MFP  =
		(SYS->GPA_MFP & (~(SYS_GPA_MFP_PA0MFP_Msk|SYS_GPA_MFP_PA1MFP_Msk|SYS_GPA_MFP_PA2MFP_Msk|SYS_GPA_MFP_PA3MFP_Msk)) )
		| (SYS_GPA_MFP_PA0MFP_SPI_MOSI0|SYS_GPA_MFP_PA1MFP_SPI_SCLK|SYS_GPA_MFP_PA2MFP_SPI_SSB0|SYS_GPA_MFP_PA3MFP_SPI_MISO0);

	// Reset IP module
	CLK_EnableModuleClock(SPI0_MODULE);
	SYS_ResetModule(SPI0_RST);
	SPIFlash_Open(SPI0, SPI_SS0, SPI0_CLOCK, &g_sSpiFlash );

	// Make SPI flash leave power down mode if some where or some time had made it entring power down mode
	SPIFlash_PowerDown(&g_sSpiFlash, FALSE);

	// Check SPI flash is ready for accessing
	u32Count = ui32Temp = 0;
	while(u32Count!=100)
	{
		SPIFlash_Read(&g_sSpiFlash, 0, (PUINT8) &ui16Temp, 2);
		if ( ui32Temp != (UINT32)ui16Temp )
		{
			ui32Temp = (UINT32)ui16Temp;
			u32Count = 0;
		}
		else
			u32Count++;
	}

	// The following code can be remove to save code if the flash size is not necessary for this application
	SPIFlash_GetChipInfo(&g_sSpiFlash);
	if (g_sSpiFlash.u32FlashSize == 0)
		return 0;

	// The above code can be remove to save code if the flash size is not necessary for this application
	return 1;
}

#ifdef NUONE_ATC
void ATC_NUONE_CHECK(void)
{
	if (!g_u8AtcCmd) {
		return ;
	}

	g_u8AtcCmd = 0;
	switch (g_u32AtcParam) {
		case 0: // PLAY
			printf("PLAY\r\n");
			if ((g_u8AppCtrl & APPCTRL_PLAY) == 0) {
				if (g_sApp.u32TotalAudioNum > 0) {
					App_StartPlay();
				}
			} else {
				App_StopPlay();
			}
			break;
		case 1: // NEXT
			printf("NEXT\r\n");
			if (g_u8AppCtrl & APPCTRL_PLAY) {
				Playback_StopPlay();
			}

			if (g_sApp.u32TotalAudioNum > 0) {
				if ((g_sApp.u32PlayID+=1) >= g_sApp.u32TotalAudioNum) {
					g_sApp.u32PlayID = 0;
				}

				App_StartPlay();
			}
			break;
		case 2: // PREV
			printf("PREV\r\n");
			if (g_u8AppCtrl & APPCTRL_PLAY) {
				Playback_StopPlay();
			}

			if (g_sApp.u32TotalAudioNum > 0) {
				if (g_sApp.u32PlayID == 0) {
					g_sApp.u32PlayID = g_sApp.u32TotalAudioNum-1;
				} else {
					g_sApp.u32PlayID--;
				}

				App_StartPlay();
			}
			break;
		case 3: // PAUSE
			printf("PAUSE\r\n");
			if ((g_u8AppCtrl&APPCTRL_PAUSE) == 0) {
				Playback_PauseCtrl(0, TRUE);
				g_u8AppCtrl |= APPCTRL_PAUSE;
			} else {
				Playback_PauseCtrl(0, FALSE);
				g_u8AppCtrl &= ~APPCTRL_PAUSE;
			}
			break;
		case 4: // STOP
			printf("STOP\r\n");
			if (g_u8AppCtrl & APPCTRL_PLAY) {
				Playback_StopPlay();
			}
			break;
		case 5: // MUTE
			printf("MUTE\r\n");
			if ((g_u8AppCtrl&APPCTRL_MUTE) == 0) {
				Playback_MuteCtrl(0, TRUE);
				g_u8AppCtrl |= APPCTRL_MUTE;
			} else {
				Playback_MuteCtrl(0, FALSE);
				g_u8AppCtrl &= ~APPCTRL_MUTE;
			}
			break;
		default:
			break;
	}
}

void ATC_PLAYBACK_CHECK(void)
{
	uint8_t ch = 0;
	uint16_t idx = 0;

	if (!g_u8AtcCmd2) {
		return ;
	}

	g_u8AtcCmd2 = 0;

	ch = (g_u32AtcParam >> 24) & 0xFF;
	idx = g_u32AtcParam & 0xFFFF;
	printf("PLAY, Ch - %d, Idx - %d\r\n", ch, idx);
	// Check if idx is in range
	if (idx >= g_sApp.u32TotalAudioNum) {
		// TODO
	}
	if ((g_u8AppCtrl & APPCTRL_PLAY)) {
		App_StopPlay();
	}
	// Set PlayID to the idx which is going to play
	g_sApp.u32PlayID = idx;
	App_StartPlay();
}
#endif

//---------------------------------------------------------------------------------------------------------
// Main Function
//---------------------------------------------------------------------------------------------------------
INT32 main()
{

	SYSCLK_INITIATE();				// Configure CPU clock source and operation clock frequency.
									// The configuration functions are in "SysClkConfig.h"

	CLK_EnableLDO(CLK_LDOSEL_3_3V);	// Enable interl 3.3 LDO.

	UART_Init();

	printf("\r\n");
	printf("+------------------------------------------------------------------------+\r\n");
	printf("|                    ISD9100 NuOne                                       |\r\n");
	printf("+------------------------------------------------------------------------+\r\n");

	if (! SPIFlash_Initiate())		// Initiate SPI interface and checking flows for accessing SPI flash.
		while(1); 					// loop here for easy debug

	OUTPUTPIN_INITIATE();			// Initiate output pin configuration.
									// The output pins configurations are defined in "ConfigIO.h".

	ULTRAIO_INITIATE();				// Initiate ultraio output configurations.
									// The ultraio output pin configurations are defined in "ConfigUltraIO.h"

	KEYPAD_INITIATE();				// Initiate keypad configurations including direct trigger key and matrix key
									// The keypad configurations are defined in "ConfigIO.h".

	PDMA_INITIATE();				// Initiate PDMA.
									// After initiation, the PDMA engine clock NVIC are enabled.
									// Use PdmaCtrl_Open() to set PDMA service channel for desired IP.
									// Use PdmaCtrl_Start() to trigger PDMA operation.
									// Reference "PdmaCtrl.h" for PDMA related APIs.
									// PDMA_INITIATE() must be call before SPK_INITIATE() and MIC_INITIATE(), if open MIC or speaker.

	SPK_INITIATE();					// Initiate speaker including pop-sound canceling.
									// After initiation, the APU is paused.
									// Use SPK_Resume(0) to start APU operation.
									// Reference "MicSpk.h" for speaker related APIs.

	MIC_INITIATE();					// Initiate MIC.
									// After initiation, the ADC is paused.
									// Use ADC_Resume() to start ADC operation.
									// Reference "MicSpk.h" for MIC related APIs.

	I2C_Init();

	App_Initiate();					// Initiate application for audio decode.

	while (1)
	{
		if (g_u8AppCtrl & APPCTRL_PLAY)
		{
			if (App_ProcessPlay() == FALSE)
			{
				App_StopPlay();
			}
		}

		TRIGGER_KEY_CHECK();		// Check and execute direct trigger key actions defined in "InputKeyActions.c"
									// Default trigger key handler is "Default_KeyHandler()"
									// The trigger key configurations are defined in "ConfigIO.h".

		MATRIX_KEY_CHECK();			// Check and execute matrix key actions defined in "InputKeyActions.c"
									// Default matrix key handler is "Default_KeyHandler()"
									// The matrix key configurations are defined in "ConfigIO.h".

		TOUCH_KEY_CHECK();			// Check and execute touch key actions defined in "InputKeyActions.c"
									// Default touch key handler is "Default_KeyHandler()"
									// The touch key configurations are defined in "ConfigIO.h".

#ifdef NUONE_ATC
		ATC_NUONE_CHECK();

		ATC_PLAYBACK_CHECK();
#endif
	}
}


/* vim: set ts=4 sw=4 tw=0 noexpandtab : */
