
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

//TGDS required version: IPC Version: 1.3

//IPC FIFO Description: 
//		struct sIPCSharedTGDS * TGDSIPC = TGDSIPCStartAddress; 														// Access to TGDS internal IPC FIFO structure. 		(ipcfifoTGDS.h)
//		struct sIPCSharedTGDSSpecific * TGDSUSERIPC = (struct sIPCSharedTGDSSpecific *)TGDSIPCUserStartAddress;		// Access to TGDS Project (User) IPC FIFO structure	(ipcfifoTGDSUser.h)

#include "ipcfifoTGDS.h"
#include "ipcfifoTGDSUser.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#include "InterruptsARMCores_h.h"
#include "libutilsShared.h"
#include "microphoneShared.h"
#ifdef ARM7
#include <string.h>

#include "main.h"
#include "wifi_arm7.h"
#include "spifwTGDS.h"

#if defined(ARM7VRAMCUSTOMCORE)
#include "pff.h"
#include "ima_adpcm.h"
#endif

#endif

#ifdef ARM9

#include <stdbool.h>
#include "main.h"
#include "wifi_arm9.h"
#include "dswnifi_lib.h"
#include "soundTGDS.h"
#include "biosTGDS.h"
#include "dmaTGDS.h"
#include "timerTGDS.h"

//TGDS Project sound effects
#include "ah_sampleEffect.h"
#include "munch_sampleEffect.h"

#endif

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
struct sIPCSharedTGDSSpecific* getsIPCSharedTGDSSpecific(){
	struct sIPCSharedTGDSSpecific* sIPCSharedTGDSSpecificInst = (struct sIPCSharedTGDSSpecific*)(TGDSIPCUserStartAddress);
	return sIPCSharedTGDSSpecificInst;
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void HandleFifoNotEmptyWeakRef(u32 cmd1, uint32 cmd2){
	switch (cmd1) {
		//NDS7: 
		#ifdef ARM7
		
			#if defined(ARM7VRAMCUSTOMCORE)
			case(FIFO_STOP_ARM7_VRAM_CORE):{
				playerStopARM7();
				uint32 * fifomsg = (uint32 *)NDS_UNCACHED_SCRATCHPAD;
				setValueSafe(&fifomsg[34], (uint32)0);
			}
			break;
			
			case(FIFO_STOPSOUNDSTREAM_FILE):{
				stopBGMusic7();
			}
			break;
			
			case(FIFO_PLAYSOUNDSTREAM_FILE):{
				playSoundStreamARM7();
			}
			break;

			case(FIFO_WRITE_AUDIO_HARDWARE):{
				uint32 * fifomsg = (uint32 *)NDS_UNCACHED_SCRATCHPAD;
				struct soundItem * sndItem = (struct soundItem *)getValueSafe(&fifomsg[34]);
				if(sndItem->channel == 5){
					SCHANNEL_CR(6) = (u32)0; //clear up or hardware audio stalls
					//SCHANNEL_TIMER(6) = (u32)0; 	//can't be enabled or hardware audio stalls
					//SCHANNEL_SOURCE(6) = (u32)0;	//can't be enabled or hardware audio stalls
					//SCHANNEL_LENGTH(6) = (u32)0;	//can't be enabled or hardware audio stalls
				}
				else{
					SCHANNEL_CR(5) = (u32)0; //clear up or hardware audio stalls
					//SCHANNEL_TIMER(5) = (u32)0;	//can't be enabled or hardware audio stalls
					//SCHANNEL_SOURCE(5) = (u32)0;	//can't be enabled or hardware audio stalls
					//SCHANNEL_LENGTH(5) = (u32)0;	//can't be enabled or hardware audio stalls
				}
				
				SCHANNEL_CR(sndItem->channel) = (u32)(sndItem->cnt);
				SCHANNEL_TIMER(sndItem->channel) = SOUND_FREQ(sndItem->freq);
				SCHANNEL_SOURCE(sndItem->channel) = (u32)(sndItem->soundBuffer);
				SCHANNEL_LENGTH(sndItem->channel) = (sndItem->len);
			}break;
			
			#endif
			
		#endif
		
		//NDS9: 
		#ifdef ARM9
		
		#endif
	}
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void HandleFifoEmptyWeakRef(uint32 cmd1,uint32 cmd2){
}

//project specific stuff

#ifdef ARM9

static bool firstTimeSoundEffect = true; //for some reason sound effects are triggered on the 3rd try 

bool soundGameOverEmitted = false;
void gameoverSound(){
	BgMusicOff();
	if(firstTimeSoundEffect == true){
		playStreamEffect("0:/ah.wav", false);
		playStreamEffect("0:/ah.wav", false);
		playStreamEffect("0:/ah.wav", false);
		playStreamEffect("0:/ah.wav", false);
		firstTimeSoundEffect = false;
	}
	playStreamEffect("0:/ah.wav", false); //one way effect sound then stop
}

void MunchFoodSound(){
	if(firstTimeSoundEffect == true){
		playStreamEffect("0:/munch.wav", false);
		playStreamEffect("0:/munch.wav", false);
		playStreamEffect("0:/munch.wav", false);
		playStreamEffect("0:/munch.wav", false);
		firstTimeSoundEffect = false;
	}
	playStreamEffect("0:/munch.wav", false); //one way effect sound then stop
}

//TGDSProject3D .WAV -> NDS Sound hardware implementation

static int curChannel = 5;
__attribute__((section(".dtcm")))
static u8 panning = 1;
struct soundItem soundsCached[MAX_SOUNDS_BUFFERED];

static int curEntry = 0;
#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
__attribute__((section(".itcm")))
void playStreamEffect(char * fname, bool loopStream){
	struct sIPCSharedTGDSSpecific* sharedIPC = getsIPCSharedTGDSSpecific();
	
	//cache upcoming sample by allocating a new blank entry
	u8 * targetBuffer = NULL;
	int targetBufferSize = 0;
	
	bool found = false;
	int NDSChannel = 0;
	for(NDSChannel = 0; NDSChannel < MAX_SOUNDS_BUFFERED; NDSChannel++){
		if(strlen((char*)&soundsCached[NDSChannel].soundFile[0]) > (ENTRIES_TO_COMPARE-1) ){
			if(strncmpi((char*)&soundsCached[NDSChannel].soundFile[0], (char*)&fname[2], ENTRIES_TO_COMPARE) == 0){
				found = true;
				break;
			}
		}
	}
	
	if(found == false){
		if(strcmpi((char*)&fname[0], (char*)"0:/munch.wav") == 0){
			targetBuffer = &munch_sampleEffect[0];
			targetBufferSize = munch_sampleEffect_size;
		}
		
		else if(strcmpi((char*)&fname[0], (char*)"0:/ah.wav") == 0){
			targetBuffer = &ah_sampleEffect[0];
			targetBufferSize = ah_sampleEffect_size;
		}
		
		//Allocate a new entry.
		soundsCached[curEntry].soundBuffer = targetBuffer;
		soundsCached[curEntry].soundLen = targetBufferSize;
		strncpy((char*)&soundsCached[curEntry].soundFile[0], (char*)&fname[2], ENTRIES_TO_COMPARE);

		NDSChannel = curEntry; //point to current entry to use the allocated sound effect

		curEntry++;
		if(curEntry > (MAX_SOUNDS_BUFFERED-1)){
			curEntry = 0;
		}
	}
	else{
		//if soundfile exists, re-use it
		//printf("cached entry: %s", (char*)&soundsCached[i].soundFile[0]);
		targetBuffer = soundsCached[NDSChannel].soundBuffer;
		targetBufferSize = soundsCached[NDSChannel].soundLen;
	}
	
	struct soundItem * sndItem = (struct soundItem *)&soundsCached[NDSChannel];
	u32 cnt   = SCHANNEL_ENABLE | SOUND_ONE_SHOT | SOUND_VOL(0x7F) | SOUND_PAN(64 * panning) | SOUND_8BIT; // Bit29-30  Format       (0=PCM8, 1=PCM16, 2=IMA-ADPCM, 3=PSG/Noise)
	int len = (int)( ( ( ((int)targetBufferSize / 4)  ) + 3) & (~3) ); //align to 4 bytes otherwise audio hardware stalls
	u16 freq = 22050;
	//Channels 0 - 3 are used by ARM7 background music
	//Channels 4 and 15 are unused
	sndItem->cnt = cnt;
	sndItem->len = len;
	sndItem->freq = freq;
	sndItem->channel = curChannel;
	if(len > 16){
		writeARM7SoundChannelFromTGDSProject3D(sndItem); //implementation prevents audio stalls 
		if(curChannel == 5){
			curChannel = 6;
		}
		else{
			curChannel = 5;
		}
		if(panning == 1){
			panning = 2;
		}
		else{
			panning = 1;
		}
	}
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void writeARM7SoundChannelFromTGDSProject3D(struct soundItem * sndItem){
	if(sndItem != NULL){
		uint32 * fifomsg = (uint32 *)NDS_UNCACHED_SCRATCHPAD;
		setValueSafe(&fifomsg[34], (uint32)sndItem);
		coherent_user_range_by_size((uint32)sndItem, (int)sizeof(struct soundItem));
		SendFIFOWords(FIFO_WRITE_AUDIO_HARDWARE, (u32)0xFFFFFFFF);
	}
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void BgMusic(char * filename){
	//ARM7 ADPCM playback 
	char * filen = FS_getFileName(filename);
	strcat(filen, ".ima");
	u32 streamType = FIFO_PLAYSOUNDSTREAM_FILE;
	playSoundStreamFromFile((char*)&filen[2], true, streamType);
}

bool bgMusicEnabled = false;

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void BgMusicOff(){
	SendFIFOWords(FIFO_STOPSOUNDSTREAM_FILE, 0xFF);
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void haltARM7(){
	uint32 * fifomsg = (uint32 *)NDS_UNCACHED_SCRATCHPAD;
	setValueSafe(&fifomsg[34], (uint32)FIFO_STOP_ARM7_VRAM_CORE);
	SendFIFOWords(FIFO_STOP_ARM7_VRAM_CORE, 0xFF);
	while( getValueSafe(&fifomsg[34]) == (uint32)FIFO_STOP_ARM7_VRAM_CORE){
		swiDelay(1);
	}
	
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void updateStreamCustomDecoder(u32 srcFrmt){

}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void freeSoundCustomDecoder(u32 srcFrmt){

}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
u32 playSoundStreamFromFile(char * videoStructFDFilename, bool loop, u32 streamType){
	struct sIPCSharedTGDSSpecific* sharedIPC = getsIPCSharedTGDSSpecific();
	char * filename = (char*)&sharedIPC->filename[0];
	strcpy(filename, videoStructFDFilename);
	
	uint32 * fifomsg = (uint32 *)NDS_UNCACHED_SCRATCHPAD;
	setValueSafe(&fifomsg[33], (uint32)0xFFFFCCAA);
	setValueSafe(&fifomsg[34], (uint32)loop);
	setValueSafe(&fifomsg[35], (uint32)streamType);
	SendFIFOWords(FIFO_PLAYSOUNDSTREAM_FILE, 0xFF);
	while( getValueSafe(&fifomsg[33]) == (uint32)0xFFFFCCAA){
		swiDelay(1);
	}
	return fifomsg[33];
}

#endif

//Libutils setup: TGDS project uses WIFI, ARM7 malloc, except custom sound + bootloader.
#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void setupLibUtils(){
	//libutils:
	
	//Stage 0
	#ifdef ARM9
	initializeLibUtils9(
		(HandleFifoNotEmptyWeakRefLibUtils_fn)&libUtilsFIFONotEmpty, //ARM7 & ARM9
		(timerWifiInterruptARM9LibUtils_fn)&Timer_50ms, //ARM9 
		(SoundStreamStopSoundStreamARM9LibUtils_fn)&stopSoundStream,	//ARM9: bool stopSoundStream(struct fd * tgdsStructFD1, struct fd * tgdsStructFD2, int * internalCodecType)
		(SoundStreamUpdateSoundStreamARM9LibUtils_fn)&updateStream, //ARM9: void updateStream() 
		(wifiDeinitARM7ARM9LibUtils_fn)&DeInitWIFI, //ARM7 & ARM9: DeInitWIFI()
		(wifiswitchDsWnifiModeARM9LibUtils_fn)&switch_dswnifi_mode, //ARM9: bool switch_dswnifi_mode(sint32 mode)
		(GdbStubUserCodeHandlerLibUtils_fn)&userCodeGDBStubProcess	//ARM9: void userCodeGDBStubProcess()
	);
	#endif
	
	//Stage 1
	#ifdef ARM7
	initializeLibUtils7(
		(HandleFifoNotEmptyWeakRefLibUtils_fn)&libUtilsFIFONotEmpty, //ARM7 & ARM9
		(wifiUpdateVBLANKARM7LibUtils_fn)&Wifi_Update, //ARM7 : Wifi7
		(wifiInterruptARM7LibUtils_fn)&Wifi_Interrupt,  //ARM7 : Wifi7
		NULL, //ARM7: void TIMER1Handler()	//Custom ARM7 Sound
		NULL, 	//ARM7: void stopSound()	//Custom ARM7 Sound
		NULL,	//ARM7: void setupSound()	//Custom ARM7 Sound
		(initMallocARM7LibUtils_fn)&initARM7Malloc, //ARM7: void initARM7Malloc(u32 ARM7MallocStartaddress, u32 ARM7MallocSize);
		(wifiDeinitARM7ARM9LibUtils_fn)&DeInitWIFI,  //ARM7 & ARM9: DeInitWIFI()
		(MicInterruptARM7LibUtils_fn)&micInterrupt, //ARM7: micInterrupt()
		(DeInitWIFIARM7LibUtils_fn)&DeInitWIFI, //ARM7: DeInitWIFI()
		(wifiAddressHandlerARM7LibUtils_fn)&wifiAddressHandler	//ARM7: void wifiAddressHandler( void * address, void * userdata )
	);
	#endif
}


#ifdef ARM9

//Initializes NTR/TWL hardware context for the ToolchainGenericDS ecosystem in ARM7 / ARM9 and sets the current NTR/TWL(__dsimode) mode globally.
#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void initHardwareCustom(u8 DSHardware) {
//---------------------------------------------------------------------------------
	swiDelay(15000);
	#ifdef TWLMODE
	u32 SCFG_EXT9 = 0x80070180; //yes, override the ones from TWL header to ensure TGDS TWL projects do work even with tampered extended headers
	
	//7     Revised Card Interface Circuit (0=NITRO, 1=Revised) = TWL
	//8     Extended ARM9 Interrupts       (0=NITRO, 1=Extended) = TWL
	//12    Extended LCD Circuit           (0=NITRO, 1=Extended) = NTR
	//13    Extended VRAM Access           (0=NITRO, 1=Extended) = NTR
	//14-15 Main Memory RAM Limit (0..1=4MB/DS, 2=16MB/DSi, 3=32MB/DSiDebugger) = 4MB + (2) mirrors NTR mode
	//16    Access to New DMA Controller   (0=Disable, 1=Enable) (40041xxh) = TWL
	
	*(u32*)0x04004008 = SCFG_EXT9;
	
	int TGDSInitLoopCount = 0;
	while( ((u32)(*(u32*)0x04004008) != SCFG_EXT9) && (DSHardware == 0x57) ) {
		if(TGDSInitLoopCount > 1048576){
			u8 fwNo = *(u8*)(0x027FF000 + 0x5D);
			int stage = 5;
			handleDSInitError(stage, (u32)fwNo);			
		}
		TGDSInitLoopCount++;
	}
	
	setCpuClock(false);	//true: 133Mhz / false: 66Mhz (TWL Mode only)
	#endif
	
	//Disable mpu
	CP15ControlRegisterDisable(CR_M);
	
	//Hardware ARM9 Init
	{	
		register int i;
		//clear out ARM9 DMA channels
		for (i=0; i<4; i++) {
			DMAXCNT(i) = 0;
			DMAXSAD(i) = 0;
			DMAXDAD(i) = 0;
			TIMERXCNT(i) = 0;
			TIMERXDATA(i) = 0;
		}

		#ifdef ARM9
		VRAM_CR = 0x80808080;
		VRAM_E_CR = 0x80;
		VRAM_F_CR = 0x80;
		VRAM_G_CR = 0x80;
		VRAM_H_CR = 0x80;
		VRAM_I_CR = 0x80;

		// clear vram
		uint16 * vram = (uint16 *)0x06800000;
		memset(vram, 0, 656 * 1024);

		// clear video palette
		memset(BG_PALETTE, 0, 2048 );	//BG_PALETTE[0] = RGB15(1,1,1);
		memset(BG_PALETTE_SUB, 0, 2048 );	//BG_PALETTE[0] = RGB15(1,1,1);

		// clear video object attribution memory
		memset(OAM, 0, 2048 );	//BG_PALETTE[0] = RGB15(1,1,1);
		memset(OAM_SUB, 0, 2048 );	//BG_PALETTE[0] = RGB15(1,1,1);

		// clear video object data memory
		memset(SPRITE_GFX, 0, 128 * 1024 );	//BG_PALETTE[0] = RGB15(1,1,1);
		memset(SPRITE_GFX_SUB, 0, 128 * 1024 );	//BG_PALETTE[0] = RGB15(1,1,1);

		// clear main display registers
		memset((void*)0x04000000, 0, 0x6c );	//BG_PALETTE[0] = RGB15(1,1,1);

		// clear sub display registers
		memset((void*)0x04001000, 0, 0x6c );	//BG_PALETTE[0] = RGB15(1,1,1);

		// clear maths registers
		memset((void*)0x04000280, 0, 0x40 );	//BG_PALETTE[0] = RGB15(1,1,1);

		REG_DISPSTAT = 0;
		SETDISPCNT_MAIN(0);
		SETDISPCNT_SUB(0);
		VRAM_A_CR = 0;
		VRAM_B_CR = 0;
		VRAM_C_CR = 0;
		VRAM_D_CR = 0;
		VRAM_E_CR = 0;
		VRAM_F_CR = 0;
		VRAM_G_CR = 0;
		VRAM_H_CR = 0;
		VRAM_I_CR = 0;
		VRAM_CR   = 0x03000000;
		REG_POWERCNT  = 0x820F; //NDS9 - POWCNT1 

		//set WORKRAM 32K to ARM7 by default
		WRAM_CR = WRAM_0KARM9_32KARM7;
		
		IRQInit(DSHardware);
	}
	//Enable mpu
	CP15ControlRegisterEnable(CR_M);
	
	//Library init code
	
	//Newlib init
	//Stream Devices Init: see devoptab_devices.c
	//setbuf(stdin, NULL);
	setbuf(stdout, NULL);	//iprintf directs to DS Framebuffer (printf already does that)
	//setbuf(stderr, NULL);
	
	TryToDefragmentMemory();
	
	//Enable TSC
	setTouchScreenEnabled(true);
	
	handleARM9InitSVC();
	#endif
	
	savedDSHardware = (u32)DSHardware; //Global DS Firmware ARM7/ARM9
	
	//Shared ARM Cores
	disableTGDSDebugging(); //Disable debugging by default
}

#endif