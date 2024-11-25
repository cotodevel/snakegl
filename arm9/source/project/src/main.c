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

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#ifdef ARM9
#include <ctype.h>
#include <stdlib.h>
#include <_ansi.h>
#include <reent.h>
#include "videoTGDS.h"
#include "posixHandleTGDS.h"
#include "consoleTGDS.h"
#include "debugNocash.h"
#include "main.h"
#include "spitscTGDS.h"
#include "timerTGDS.h"
#include "keypadTGDS.h"
#include "biosTGDS.h"
#include "InterruptsARMCores_h.h"
#include "interrupts.h"
#include "ipcfifoTGDSUser.h"
#include <ctype.h>
#include "loader.h"

//ARM7 VRAM core
#include "arm7vram.h"
#include "arm7vram_twl.h"

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
u32 * getTGDSARM7VRAMCore(){
	if(__dsimode == false){
		return (u32*)&arm7vram[0];	
	}
	else{
		return (u32*)&arm7vram_twl[0];
	}
}

#ifdef __cplusplus
extern "C"{
#endif

extern int vsnprintf(char *str, size_t size, const char *format, va_list ap);

#ifdef __cplusplus
}
#endif

#endif

#include "Scene.h"

#ifndef _MSC_VER
					// //
#define ARM9 1		// Enable only if not real GCC + NDS environment
#undef _MSC_VER		// //
#undef WIN32		// //
#endif

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0"))) 
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int main(int argc, char *argv[])
{
	#ifdef _MSC_VER
	startTGDSProject(argc, argv);
	#endif

	#ifdef ARM9
	/*			TGDS 1.6 Standard ARM9 Init code start	*/
	//Save Stage 1: IWRAM ARM7 payload: NTR/TWL (0x03800000)
	memcpy((void *)TGDS_MB_V3_ARM7_STAGE1_ADDR, (const void *)0x02380000, (int)(96*1024));
	coherent_user_range_by_size((uint32)TGDS_MB_V3_ARM7_STAGE1_ADDR, (int)(96*1024));
	
	//Execute Stage 2: VRAM ARM7 payload: NTR/TWL (0x06000000)
	u32 * payload = getTGDSARM7VRAMCore();
	executeARM7Payload((u32)0x02380000, 96*1024, payload);
	
	bool isTGDSCustomConsole = true;	//set default console or custom console: custom console 
	GUI_init(isTGDSCustomConsole);
	GUI_clear();

	bool isCustomTGDSMalloc = true;
	setTGDSMemoryAllocator(getProjectSpecificMemoryAllocatorSetup(isCustomTGDSMalloc));
	sint32 fwlanguage = (sint32)getLanguage();

	int ret=FS_init();
	if (ret != 0){
		printf("%s: FS Init error: %d >%d", TGDSPROJECTNAME, ret, TGDSPrintfColor_Red);
		while(1==1){
			swiDelay(1);
		}
	}
	
	asm("mcr	p15, 0, r0, c7, c10, 4");
	flush_icache_all();
	flush_dcache_all();	
	/*			TGDS 1.6 Standard ARM9 Init code end	*/
	
	setupDisabledExceptionHandler();
	REG_IME = 0;
	set0xFFFF0000FastMPUSettings();
	//TGDS-Projects -> legacy NTR TSC compatibility
	if(__dsimode == true){
		TWLSetTouchscreenTWLMode(); //guaranteed TSC on TWL Mode through Unlaunch
	}
	REG_IME = 1;
	
	setBacklight(POWMAN_BACKLIGHT_TOP_BIT | POWMAN_BACKLIGHT_BOTTOM_BIT); //Dual3D or debug session enabled screens
	
	/*
	#ifdef NO_VIDEO_PLAYBACK
	argv[2] = (char*)0x02000000; //debug, if enabled, disables video intro
	#endif
	
	//Play game intro if coldboot
	if(argv[2] == NULL){
		char tmpName[256];
		strcpy(tmpName, videoIntro);
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		char startPath[MAX_TGDSFILENAME_LENGTH+1];
		strcpy(startPath,"/");
		if(__dsimode == true){
			strcpy(curChosenBrowseFile, "0:/ToolchainGenericDS-videoplayer.srl");
		}
		else{
			strcpy(curChosenBrowseFile, "0:/ToolchainGenericDS-videoplayer.nds");
		}
		//Send args
		printf("[Booting %s]", curChosenBrowseFile);
		printf("Want to send argument?");
		printf("(A) Yes: (Start) Choose arg.");
		printf("(B) No. ");
		
		int argcCount = 0;
		argcCount++;
		printf("[Booting... Please wait] >%d", TGDSPrintfColor_Red);
		
		char thisArgv[3][MAX_TGDSFILENAME_LENGTH];
		memset(thisArgv, 0, sizeof(thisArgv));
		strcpy(&thisArgv[0][0], TGDSPROJECTNAME);	//Arg0:	This Binary loaded
		strcpy(&thisArgv[1][0], curChosenBrowseFile);	//Arg1:	NDS Binary reloaded
		strcpy(&thisArgv[2][0], tmpName);					//Arg2: NDS Binary ARG0
		u32 * payload = getTGDSARM7VRAMCore();
		if(TGDSMultibootRunNDSPayload(curChosenBrowseFile, (u8*)payload, 3, (char*)&thisArgv) == false){ //should never reach here, nor even return true. Should fail it returns false
			printf("Invalid NDS/TWL Binary >%d", TGDSPrintfColor_Yellow);
			printf("or you are in NTR mode trying to load a TWL binary. >%d", TGDSPrintfColor_Yellow);
			printf("or you are missing the TGDS-multiboot payload in root path. >%d", TGDSPrintfColor_Yellow);
			printf("Press (A) to continue. >%d", TGDSPrintfColor_Yellow);
			while(1==1){
				scanKeys();
				if(keysDown()&KEY_A){
					scanKeys();
					while(keysDown() & KEY_A){
						scanKeys();
					}
					break;
				}
			}
			menuShow();
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
	else if(strncmp(argv[2], videoIntro, strlen(videoIntro)) == 0){
		//video intro already played, skip.
	}
	else if(strncmp(argv[2], videoTest, strlen(videoTest)) == 0){
		//play videoTest maybe?
	}
	*/
	
	//init sound
	bgMusicEnabled = false;

	clrscr();
	printf("---");
	printf("---");
	printf("---");
	printf("starting TGDS Project");

	ret = startTGDSProject(argc, argv);

	clrscr();
	printf("---");
	printf("---");
	printf("---");
	printf("ending TGDS Project. Halt");
	while(1==1){

	}
	#endif
	
	return 0;
}

void TGDSAPPExit(u32 fn_address){
#ifdef ARM9
	clrscr();
	printf("----");
	printf("----");
	printf("----");
	printf("TGDSAPP:Exit(); From Addr: %x", fn_address);
	while(1==1){
		swiDelay(1);
	}
#endif
}

/// Animates the scene. This function just renders the scene every
/// 25 milliseconds. A timer is used to give smooth animation at the
/// same rate on differnt computers. idle function draws the scenes
/// at way too different speeds on different computers
void animateScene(int type)
{
#ifdef _MSC_VER
	glutPostRedisplay();
	glutTimerFunc(25, animateScene, 0);
#endif
}



#ifdef _MSC_VER
void load_image(const char* filename)
{
    int width, height;
    unsigned char* image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);
}
#endif

void load_resources()
{
	#ifdef WIN32
    glGenTextures(TEXTURE_COUNT, texturesSnakeGL);

    glBindTexture(GL_TEXTURE_2D, texturesSnakeGL[GROUND_TEXTURE]);
    load_image("../src/resources/grass.png");

    glBindTexture(GL_TEXTURE_2D, texturesSnakeGL[FOOD_TEXTURE]);
    load_image("../src/resources/apple.png");

    glBindTexture(GL_TEXTURE_2D, texturesSnakeGL[BARRIER_TEXTURE]);
    load_image("../src/resources/box.png");

    glBindTexture(GL_TEXTURE_2D, texturesSnakeGL[SNAKE_TEXTURE]);
    load_image("../src/resources/snake.png");

    glBindTexture(GL_TEXTURE_2D, texturesSnakeGL[MENU_TEXTURE]);
    load_image("../src/resources/menu.png");

    glBindTexture(GL_TEXTURE_2D, texturesSnakeGL[BRICK_TEXTURE]);
    load_image("../src/resources/brick.png");
	#endif
}

void unload_resources()
{
    #ifdef WIN32
	glDeleteTextures(TEXTURE_COUNT, texturesSnakeGL);
	#endif
}

//ARM9 printf nocash debugger
int TWLPrintf(const char *fmt, ...){
#ifdef ARM9
	va_list args;
	va_start (args, fmt);
	vsnprintf ((sint8*)ConsolePrintfBuf, 64, fmt, args);
	va_end (args);
	
	nocashMessage((char*)ConsolePrintfBuf);
#endif
	return 0;
}

/////////////////////////////////////////////////////////// TGDS Project ARM9 specifics ///////////////////////////////////////
#ifdef ARM9

__attribute__((section(".dtcm")))
int pendPlay = 0;

char curChosenBrowseFile[MAX_TGDSFILENAME_LENGTH];

/////////////////////////////////////////////////////////////////////////////////////

//true: pen touch
//false: no tsc activity
#ifdef ARM9
#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__((optnone))
#endif
#endif
bool get_pen_delta( int *dx, int *dy ){
	static int prev_pen[2] = { 0x7FFFFFFF, 0x7FFFFFFF };
	
	// TSC Test.
	struct touchPosition touch;
	XYReadScrPosUser(&touch);
	
	if( (touch.px == 0) && (touch.py == 0) ){
		prev_pen[0] = prev_pen[1] = 0x7FFFFFFF;
		*dx = *dy = 0;
		return false;
	}
	else{
		if( prev_pen[0] != 0x7FFFFFFF ){
			*dx = (prev_pen[0] - touch.px);
			*dy = (prev_pen[1] - touch.py);
		}
		prev_pen[0] = touch.px;
		prev_pen[1] = touch.py;
	}
	return true;
}

void menuShow(){
	clrscr();
	printf(" ---- ");
	printf(" ---- ");
	printf(" ---- ");
	printf("[%s] running. >%d", TGDSPROJECTNAME, TGDSPrintfColor_Yellow);
	printf("Free Mem: %d KB >%d", ((int)TGDSARM9MallocFreeMemory()/1024), TGDSPrintfColor_Cyan);
}

//TGDS Soundstreaming API
int internalCodecType = SRC_NONE; //Returns current sound stream format: WAV, ADPCM or NONE
struct fd * _FileHandleVideo = NULL; 
struct fd * _FileHandleAudio = NULL;

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
bool stopSoundStreamUser(){
	return stopSoundStream(_FileHandleVideo, _FileHandleAudio, &internalCodecType);
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void closeSoundUser(){
	//Stubbed. Gets called when closing an audiostream of a custom audio decoder
}

bool NDSDual3DCameraFlag = false;

#endif