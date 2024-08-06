/*

			Copyright (C) 2017  Coto
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
USA
*/

#include "main.h"
#include "biosTGDS.h"
#include "spifwTGDS.h"
#include "posixHandleTGDS.h"
#include "pff.h"
#include "ipcfifoTGDSUser.h"
#include "loader.h"
#include "dldi.h"
#include "exceptionTGDS.h"
#include "dmaTGDS.h"
#include "busTGDS.h"
#include "wifi_arm7.h"
#include "ima_adpcm.h"
#include "soundTGDS.h"
#include "timerTGDS.h"
#include "InterruptsARMCores_h.h"

////////////////////////////////TGDS-MB v3 VRAM Bootcode start////////////////////////////////
__attribute__((section(".iwram64K")))
IMA_Adpcm_Player backgroundMusicPlayer;	//Actual PLAYER Instance. See ima_adpcm.cpp -> [PLAYER: section

__attribute__((section(".iwram64K")))
IMA_Adpcm_Player SoundEffect0Player;

__attribute__((section(".iwram64K")))
FATFS FatfsFILEBgMusic; //Sound stream handle

__attribute__((section(".iwram64K")))
FATFS FatfsFILESoundSample0; //Sound effect handle #0

__attribute__((section(".iwram64K")))
FATFS fileHandle;					// Petit-FatFs work area 

struct soundPlayerContext soundData;
char fname[256];
char debugBuf7[256];

/*
//If NTR/TWL Binary
	int isNTRTWLBinary = isNTROrTWLBinaryTGDSMB7(fh);
	//Trying to boot a TWL binary in NTR mode? 
	if(!(isNTRTWLBinary == isNDSBinaryV1) && !(isNTRTWLBinary == isNDSBinaryV2) && !(isNTRTWLBinary == isNDSBinaryV3) && !(isNTRTWLBinary == isTWLBinary) && !(isNTRTWLBinary == isNDSBinaryV1Slot2)){
	}
*/

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int isNTROrTWLBinaryTGDSMB7(FATFS * currentFH){
	int mode = notTWLOrNTRBinary;
	return mode;
}


#include <stdio.h>

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void strrev(char *arr, int start, int end)
{
    char temp;

    if (start >= end)
        return;

    temp = *(arr + start);
    *(arr + start) = *(arr + end);
    *(arr + end) = temp;

    start++;
    end--;
    strrev(arr, start, end);
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
char *itoa(int number, char *arr, int base)
{
    int i = 0, r, negative = 0;

    if (number == 0)
    {
        arr[i] = '0';
        arr[i + 1] = '\0';
        return arr;
    }

    if (number < 0 && base == 10)
    {
        number *= -1;
        negative = 1;
    }

    while (number != 0)
    {
        r = number % base;
        arr[i] = (r > 9) ? (r - 10) + 'a' : r + '0';
        i++;
        number /= base;
    }

    if (negative)
    {
        arr[i] = '-';
        i++;
    }

    strrev(arr, 0, i - 1);

    arr[i] = '\0';

    return arr;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void playSoundStreamARM7(){
	uint32 * fifomsg = (uint32 *)NDS_CACHED_SCRATCHPAD;			
	UINT br;
	uint8_t fresult;
	bool loop = fifomsg[34];
	FATFS * currentFH;
	u32 streamType = (u32)fifomsg[35];
	struct sIPCSharedTGDSSpecific* sharedIPC = getsIPCSharedTGDSSpecific();
	char * filename = (char*)&sharedIPC->filename[0];
	strcpy((char*)fname, filename);
	
	if(streamType == FIFO_PLAYSOUNDSTREAM_FILE){
		currentFH = &FatfsFILEBgMusic;
	}
	else if(streamType == FIFO_PLAYSOUNDEFFECT_FILE){
		currentFH = &FatfsFILESoundSample0;
	}
	fresult = pf_mount(currentFH);
	if (fresult != FR_OK) { 
		
		//Throw exception
		int stage = 10;
		handleDSInitOutputMessage("playSoundStreamARM7(): pf_mount() failed");
		handleDSInitError7(stage, (u32)savedDSHardware);
		
		fifomsg[33] = 0xAABBCCDD;
	}
	fresult = pf_open(fname, currentFH);
	if (fresult != FR_OK) { 
		strcpy((char*)0x02000000, "soundfile failed to open:");
		strcat((char*)0x02000000, filename);
	}
	else{
		strcpy((char*)0x02000000, "soundfile open OK:"); //ok so far
		strcat((char*)0x02000000, filename);
	}
	
	pf_lseek(0, currentFH);
	
	int argBuffer[MAXPRINT7ARGVCOUNT];
	memset((unsigned char *)&argBuffer[0], 0, sizeof(argBuffer));
	argBuffer[0] = 0xc070ffff;
	
	//decode audio here
	bool loop_audio = loop;
	bool automatic_updates = false;
	if(streamType == FIFO_PLAYSOUNDSTREAM_FILE){
		if(backgroundMusicPlayer.play(loop_audio, automatic_updates, ADPCM_SIZE, stopSoundStreamUser, currentFH, streamType) == 0){
			//ADPCM Playback!
		}
	}
	else if(streamType == FIFO_PLAYSOUNDEFFECT_FILE){
		if(SoundEffect0Player.play(loop_audio, automatic_updates, ADPCM_SIZE, stopSoundStreamUser, currentFH, streamType) == 0){
			//ADPCM Sample Playback!
		}
	}
	fifomsg[33] = (u32)fresult;
}


#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void handleARM7FSRender(){
	uint32 * fifomsg = (uint32 *)NDS_CACHED_SCRATCHPAD;
	int fileOffset = (int)fifomsg[32];
	int bufferSize = (int)fifomsg[33];
	fifomsg[34] = (u32)0;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------
	
	//Copy ARM7i sections from VRAM -> IWRAM if we're already at VRAM
	extern u32 __arm7iwram_lma__;
	extern u32 __arm7iwram_lma_end__;
	extern u32 __iwram_startFast;
	extern u32 __iwram_topFast;
	int iwramSectionSize = (int) ((u32)&__arm7iwram_lma_end__ - (u32)&__iwram_startFast);
	dmaTransferWord(0, (uint32)&__arm7iwram_lma__, (uint32)&__iwram_startFast, iwramSectionSize);
	
	/*			TGDS 1.6 Standard ARM7 Init code start	*/
	installWifiFIFO();
	while(!(*(u8*)0x04000240 & 2) ){} //wait for VRAM_D block
	ARM7InitDLDI(TGDS_ARM7_MALLOCSTART, TGDS_ARM7_MALLOCSIZE, TGDSDLDI_ARM7_ADDRESS);
	SendFIFOWords(FIFO_ARM7_RELOAD, 0xFF); //ARM7 Reload OK -> acknowledge ARM9
    /*			TGDS 1.6 Standard ARM7 Init code end	*/
	REG_IE|=(IRQ_VBLANK); //X button depends on this
	while (1) {
		handleARM7SVC();	/* Do not remove, handles TGDS services */
		HaltUntilIRQ(); //Save power until next Vblank
	}
	return 0;
}

bool stopSoundStreamUser(){

}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void playerStopARM7(){
	memset((void *)strpcmL0, 0, 2048);
	memset((void *)strpcmL1, 0, 2048);
	memset((void *)strpcmR0, 0, 2048);
	memset((void *)strpcmR1, 0, 2048);

	REG_IE&=~IRQ_TIMER2;
	
	TIMERXDATA(1) = 0;
	TIMERXCNT(1) = 0;
	TIMERXDATA(2) = 0;
	TIMERXCNT(2) = 0;
	for(int ch=0;ch<4;++ch)
	{
		SCHANNEL_CR(ch) = 0;
		SCHANNEL_TIMER(ch) = 0;
		SCHANNEL_LENGTH(ch) = 0;
		SCHANNEL_REPEAT_POINT(ch) = 0;
	}
}

////////////////////////////////TGDS-MB v3 VRAM Bootcode end////////////////////////////////