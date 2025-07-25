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

#ifndef __main9_h__
#define __main9_h__

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "fatfslayerTGDS.h"
#include "utilsTGDS.h"
#include "videoGL.h"

#define Texture_MetalCubeID ((int)0)

//project 1 multimedia list
#define videoIntro ((char*)"0:/sth1.tvs") //video intro
#define videoTest ((char*)"0:/test.tvs") 

//tracks

//sound effects

#endif


#ifdef __cplusplus
extern "C" {
#endif

extern u32 * getTGDSMBV3ARM7Bootloader(); //Required by ToolchainGenericDS-multiboot v3
extern u32 * getTGDSMBV3ARM7Stage1(); //required by TGDS-mb v3's ARM7 @ 0x03800000

extern void menuShow();
extern int main(int argc, char **argv);
extern char curChosenBrowseFile[MAX_TGDSFILENAME_LENGTH];
extern struct fd * _FileHandleVideo; 
extern struct fd * _FileHandleAudio;

//TGDS Soundstreaming API
extern int internalCodecType;
extern struct fd * _FileHandleVideo; 
extern struct fd * _FileHandleAudio;
extern bool stopSoundStreamUser();
extern void closeSoundUser();
extern bool get_pen_delta( int *dx, int *dy );
extern int pendPlay;
extern bool NDSDual3DCameraFlag;
extern struct task_Context * internalTGDSThreads;
extern void onThreadOverflowUserCode(u32 * args);
#ifdef __cplusplus
}
#endif