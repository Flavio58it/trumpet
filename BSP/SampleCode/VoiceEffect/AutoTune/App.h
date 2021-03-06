/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) Nuvoton Technology Corp. All rights reserved.                                              */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef __APP_H__
#define __APP_H__
  	 
#include "ConfigApp.h"
#include "Framework.h"

// -------------------------------------------------------------------------------------------------------------------------------
// g_u8AppCtrl Bit Field Definitions
// -------------------------------------------------------------------------------------------------------------------------------
#define APPCTRL_NO_ACTION			0
#define APPCTRL_PLAY				BIT0
#define APPCTRL_PLAY_STOP			BIT1
#define APPCTRL_RECORD				BIT2

// -------------------------------------------------------------------------------------------------------------------------------
// Application Related Definitions 
// -------------------------------------------------------------------------------------------------------------------------------
#include "AutoTuneApp/AutoTuneApp.h"
#include "PlaybackRecord.h"
#include "BufCtrl.h"
#include "ConfigIO.h"


typedef struct
{
	S_AUTOTUNEAPP	sAutoTuneApp;				// data structure for AutoTune app

	BOOL			bEnableAutoTune;			// disable / enable AutoTune
	INT8 			i8AutoTunePitchShift;		// pitch shift number in semitone for AutoTune

} S_APP;


// ------------------------------------------------------------------------------------------------------------------------------
// Initiate AutoTune memory and parameters.
// ------------------------------------------------------------------------------------------------------------------------------
void 
App_Initiate(void);

// ------------------------------------------------------------------------------------------------------------------------------
// Start AutoTune, start record, start playback.
// ------------------------------------------------------------------------------------------------------------------------------
void 
App_StartPlay(void);

// ------------------------------------------------------------------------------------------------------------------------------
// Stop record, stop playback, stop AutoTune.
// ------------------------------------------------------------------------------------------------------------------------------
void 
App_StopPlay(void);

// ------------------------------------------------------------------------------------------------------------------------------
// Operation in main loop for playing.
// ------------------------------------------------------------------------------------------------------------------------------
BOOL 						// TRUE: continue playback, FALSE: finish playback
App_ProcessPlay(void);

// ------------------------------------------------------------------------------------------------------------------------------
// Operation for power-down request.
// ------------------------------------------------------------------------------------------------------------------------------
void 				
App_PowerDown(void);


#endif //#ifndef __APP_H__

