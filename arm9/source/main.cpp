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
#include <stdint.h>

#include "main.h"
#include "keypadTGDS.h"
#include "biosTGDS.h"
#include "dldi.h"
#include "fileBrowse.h"
#include "dswnifi_lib.h"
#include "ipcfifoTGDSUser.h"
#include "posixHandleTGDS.h"
#include "TGDSMemoryAllocator.h"
#include "TGDSLogoLZSSCompressed.h"
#include "VideoGL.h"
#include "ndsDisplayListUtils.h"
#include "Texture_Cube.h"
#include "imagepcx.h"
#include "spitscTGDS.h"
#include "timerTGDS.h"
#include "keypadTGDS.h"
#include "biosTGDS.h"

//snake GL
#include "base.h"
#include "game.h"
#include "snake.h"
#include "scenario.h"

#include "apple.h"
#include "boxbitmap.h"
#include "brick.h"
#include "grass.h"
#include "menu.h"
#include "snakegfx.h"
#include "Texture_Cube.h"

/////////////////////////////// Snake GL defs ///////////////////////////////////////
int width  = 640,
    height = 640;

bool is_game_over = false,
     is_running   = false;

Game* game = NULL;

GLuint texturesSnakeGL[TEXTURE_COUNT+1];

/////////////////////////////////////////////////////////////////////////////////////
char curChosenBrowseFile[MAX_TGDSFILENAME_LENGTH];
bool pendingPlay = false;

GLuint	texture[1];			// Storage For 1 Texture
GLuint	box;				// Storage For The Box Display List
GLuint	top;				// Storage For The Top Display List
GLuint	xloop;				// Loop For X Axis
GLuint	yloop;				// Loop For Y Axis

int	xrot;				// Rotates Cube On The X Axis
int	yrot;				// Rotates Cube On The Y Axis

GLfloat boxcol[5][3]=
{
	{1.0f,0.0f,0.0f},{1.0f,0.5f,0.0f},{1.0f,1.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,1.0f,1.0f}
};

GLfloat topcol[5][3]=
{
	{.5f,0.0f,0.0f},{0.5f,0.25f,0.0f},{0.5f,0.5f,0.0f},{0.0f,0.5f,0.0f},{0.0f,0.5f,0.5f} 
};

float rotateX = 0.0;
float rotateY = 0.0;

//true: pen touch
//false: no tsc activity
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
	printf("     ");
	printf("     ");
	
	printf("%s >%d", TGDSPROJECTNAME, TGDSPrintfColor_Yellow);
	printf("(Start): Boot NDS File/Audio. ");
	if(internalCodecType == SRC_WAVADPCM){
		printf("ADPCM Play: >%d", TGDSPrintfColor_Red);
	}
	else if(internalCodecType == SRC_WAV){	
		printf("WAVPCM Play: >%d", TGDSPrintfColor_Green);
	}
	else{
		printf("Player Inactive");
	}
	printf("Available heap memory: %d >%d", getMaxRam(), TGDSPrintfColor_Cyan);
}

//TGDS Soundstreaming API
int internalCodecType = SRC_NONE; //Returns current sound stream format: WAV, ADPCM or NONE
struct fd * _FileHandleVideo = NULL; 
struct fd * _FileHandleAudio = NULL;
bool stopSoundStreamUser(){
	return stopSoundStream(_FileHandleVideo, _FileHandleAudio, &internalCodecType);
}

void closeSoundUser(){
	//Stubbed. Gets called when closing an audiostream of a custom audio decoder
}

char args[8][MAX_TGDSFILENAME_LENGTH];
char *argvs[8];

int main(int argc, char **argv) {
	/*			TGDS 1.6 Standard ARM9 Init code start	*/
	bool isTGDSCustomConsole = true;	//set default console or custom console: default console
	GUI_init(isTGDSCustomConsole);
	GUI_clear();
	
	//xmalloc init removes args, so save them
	int i = 0;
	for(i = 0; i < argc; i++){
		argvs[i] = argv[i];
	}

	bool isCustomTGDSMalloc = true;
	setTGDSMemoryAllocator(getProjectSpecificMemoryAllocatorSetup(TGDS_ARM7_MALLOCSTART, TGDS_ARM7_MALLOCSIZE, isCustomTGDSMalloc, TGDSDLDI_ARM7_ADDRESS));
	sint32 fwlanguage = (sint32)getLanguage();
	
	//argv destroyed here because of xmalloc init, thus restore them
	for(i = 0; i < argc; i++){
		argv[i] = argvs[i];
	}

	int ret=FS_init();
	if (ret == 0)
	{
		printf("FS Init ok.");
	}
	else{
		printf("FS Init error: %d", ret);
	}
	
	switch_dswnifi_mode(dswifi_idlemode);
	asm("mcr	p15, 0, r0, c7, c10, 4");
	flush_icache_all();
	flush_dcache_all();	
	/*			TGDS 1.6 Standard ARM9 Init code end	*/
	
	/////////////////////////////////////////////////////////Reload TGDS Proj///////////////////////////////////////////////////////////
	#if !defined(ISEMULATOR)
	char tmpName[256];
	char ext[256];	
	char TGDSProj[256];
	char curChosenBrowseFile[256];
	strcpy(TGDSProj,"0:/");
	strcat(TGDSProj, "ToolchainGenericDS-multiboot");
	if(__dsimode == true){
		strcat(TGDSProj, ".srl");
	}
	else{
		strcat(TGDSProj, ".nds");
	}
	//Force ARM7 reload once 
	if( 
		(argc < 3) 
		&& 
		(strncmp(argv[1], TGDSProj, strlen(TGDSProj)) != 0) 	
	){
		REG_IME = 0;
		MPUSet();
		REG_IME = 1;
		char startPath[MAX_TGDSFILENAME_LENGTH+1];
		strcpy(startPath,"/");
		strcpy(curChosenBrowseFile, TGDSProj);
		
		char thisTGDSProject[MAX_TGDSFILENAME_LENGTH+1];
		strcpy(thisTGDSProject, "0:/");
		strcat(thisTGDSProject, TGDSPROJECTNAME);
		if(__dsimode == true){
			strcat(thisTGDSProject, ".srl");
		}
		else{
			strcat(thisTGDSProject, ".nds");
		}
		
		//Boot .NDS file! (homebrew only)
		strcpy(tmpName, curChosenBrowseFile);
		separateExtension(tmpName, ext);
		strlwr(ext);
		
		//pass incoming launcher's ARGV0
		char arg0[256];
		int newArgc = 3;
		if (argc > 2) {
			printf(" ---- test");
			printf(" ---- test");
			printf(" ---- test");
			printf(" ---- test");
			printf(" ---- test");
			printf(" ---- test");
			printf(" ---- test");
			printf(" ---- test");
			
			//arg 0: original NDS caller
			//arg 1: this NDS binary
			//arg 2: this NDS binary's ARG0: filepath
			strcpy(arg0, (const char *)argv[2]);
			newArgc++;
		}
		//or else stub out an incoming arg0 for relaunched TGDS binary
		else {
			strcpy(arg0, (const char *)"0:/incomingCommand.bin");
			newArgc++;
		}
		//debug end
		
		char thisArgv[4][MAX_TGDSFILENAME_LENGTH];
		memset(thisArgv, 0, sizeof(thisArgv));
		strcpy(&thisArgv[0][0], thisTGDSProject);	//Arg0:	This Binary loaded
		strcpy(&thisArgv[1][0], curChosenBrowseFile);	//Arg1:	Chainload caller: TGDS-MB
		strcpy(&thisArgv[2][0], thisTGDSProject);	//Arg2:	NDS Binary reloaded through ChainLoad
		strcpy(&thisArgv[3][0], (char*)&arg0[0]);//Arg3: NDS Binary reloaded through ChainLoad's ARG0
		addARGV(newArgc, (char*)&thisArgv);				
		if(TGDSMultibootRunNDSPayload(curChosenBrowseFile) == false){ //should never reach here, nor even return true. Should fail it returns false
			
		}
	}
	#endif
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(__dsimode == true){
		TWLSetTouchscreenTWLMode();
	}
	
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
		addARGV(3, (char*)&thisArgv);				
		if(TGDSMultibootRunNDSPayload(curChosenBrowseFile) == false){ //should never reach here, nor even return true. Should fail it returns false
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
	menuShow();
	
	// Simple triangle example
	/*
	//Simple Triangle GL init
	float rotateX = 0.0;
	float rotateY = 0.0;
	{
		//set mode 0, enable BG0 and set it to 3D
		SETDISPCNT_MAIN(MODE_0_3D);
		
		//this should work the same as the normal gl call
		glViewport(0,0,255,191);
		
		glClearColor(0,0,0);
		glClearDepth(0x7FFF);
		glInit(); //NDSDLUtils: Initializes a new videoGL context
	}
	while (1){
		if(pendingPlay == true){
			internalCodecType = playSoundStream(curChosenBrowseFile, _FileHandleVideo, _FileHandleAudio);
			
			if(internalCodecType == SRC_NONE){
				stopSoundStreamUser();
			}
			pendingPlay = false;
			menuShow();
		}
		
		scanKeys();
		
		int pen_delta[2];
		bool isTSCActive = get_pen_delta( &pen_delta[0], &pen_delta[1] );
		rotateY -= pen_delta[0];
		rotateX -= pen_delta[1];
		
		if( isTSCActive == false ){
			printfCoords(0, 16, " No TSC Activity ----");
		}
		else{
			
			printfCoords(0, 16, "TSC Activity ----");
			
			glReset();
	
			//any floating point gl call is being converted to fixed prior to being implemented
			gluPerspective(35, 256.0 / 192.0, 0.1, 40);

			gluLookAt(	0.0, 0.0, 1.0,		//camera possition 
						0.0, 0.0, 0.0,		//look at
						0.0, 1.0, 0.0);		//up

			glPushMatrix();

			//move it away from the camera
			glTranslate3f32(0, 0, floattof32(-1));
					
			glRotateX(rotateX);
			glRotateY(rotateY);			
			glMatrixMode(GL_MODELVIEW);
			
			//not a real gl function and will likely change
			glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);
			
			//glShadeModel(GL_FLAT); //forces the fill color to be the first glColor3b call
			
			//draw the obj
			glBegin(GL_TRIANGLE);
				
				glColor3b(31,0,0);
				glVertex3v16(inttov16(-1),inttov16(-1),0);

				glColor3b(0,31,0);
				glVertex3v16(inttov16(1), inttov16(-1), 0);

				glColor3b(0,0,31);
				glVertex3v16(inttov16(0), inttov16(1), 0);
				
			glEnd();
			glPopMatrix(1);
			glFlush();
		}
	}
	*/
	
	/* OpenGL 1.1 Dynamic Display List */
	InitGL();
	ReSizeGLScene(255, 191);

    // Init GL before this
    load_resources();

    #if defined(ARM9)
    startTimerCounter(tUnitsMilliseconds, 1);
    #endif

    game = new Game();


	#if defined(WIN32)
	glutMainLoop();
	#endif
	glMaterialShinnyness();

	while (1){
		DrawGLScene();
		handleARM9SVC();	/* Do not remove, handles TGDS services */
		IRQVBlankWait();
	}
	return 0;
}

//GL Display Lists Unit Test: Cube Demo
// Build Cube Display Lists
GLvoid BuildLists(){
	box=glGenLists(2);									// Generate 2 Different Lists
	glNewList(box,GL_COMPILE);							// Start With The Box List
		glBegin(GL_QUADS);
			// Bottom Face
			glNormal3f(0.5f,-1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
			// Front Face
			glNormal3f( 1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
			// Back Face
			glNormal3f( 1.0f, 1.0f,-1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
			// Right face
			glNormal3f( 1.0f, 0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
			// Left Face
			glNormal3f(-1.0f, 0.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glEnd();
	glEndList();
	top=box+1;											// Storage For "Top" Is "Box" Plus One
	glNewList(top,GL_COMPILE);							// Now The "Top" Display List
		glBegin(GL_QUADS);
			// Top Face
			glNormal3f( 1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		glEnd();
	glEndList();
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

static float camMov = -1.0;
int InitGL()										// All Setup For OpenGL Goes Here
{
	glInit(); //NDSDLUtils: Initializes a new videoGL context	
	glClearColor(255,255,255);		// Black Background
	glClearDepth(0x7FFF);		// Depth Buffer Setup
	glEnable(GL_ANTIALIAS);
	glEnable(GL_TEXTURE_2D); // Enable Texture Mapping 
	glEnable(GL_BLEND);

	//#1: Load a texture and map each one to a texture slot
	u32 arrayOfTextures[7];
	arrayOfTextures[0] = (u32)&apple; //0: apple.bmp  
	arrayOfTextures[1] = (u32)&boxbitmap; //1: boxbitmap.bmp  
	arrayOfTextures[2] = (u32)&brick; //2: brick.bmp  
	arrayOfTextures[3] = (u32)&grass; //3: grass.bmp
	arrayOfTextures[4] = (u32)&menu; //4: menu.bmp
	arrayOfTextures[5] = (u32)&snakegfx; //5: snakegfx.bmp
	arrayOfTextures[6] = (u32)&Texture_Cube; //6: Texture_Cube.bmp
	int texturesInSlot = LoadLotsOfGLTextures((u32*)&arrayOfTextures, (int*)&texturesSnakeGL, 7); //Implements both glBindTexture and glTexImage2D 
	int i = 0;
	for(i = 0; i < texturesInSlot; i++){
		printf("Texture loaded: %d:textID[%d] Size: %d", i, texturesSnakeGL[i], getTextureBaseFromTextureSlot(activeTexture));
	}
	setupDLEnableDisable2DTextures();
	return true;				
}

int DrawGLScene(){									
	scanKeys();
	int pen_delta[2];
	bool isTSCActive = get_pen_delta( &pen_delta[0], &pen_delta[1] );
	if( isTSCActive == false ){
		rotateX = 0.0;
		rotateY = 0.0;
	}
	else{
		clrscr();
		printfCoords(0, 14, "camera_mode: %d", game->scenario->camera_mode);
		printfCoords(0, 15, "xrot:%d-yrot:%d", xrot, yrot);
		
		rotateX = pen_delta[0];
		rotateY = pen_delta[1];
		if(yrot > 0){
			yrot--;
		}
		else{
			yrot++;
		}
		
		if(xrot > 0){
			xrot--;
		}
		else{
			xrot++;
		}
	}
	if (keysDown() & KEY_LEFT)
	{
		camMov-=2.8f;
	}
	if (keysDown() & KEY_RIGHT)
	{
		camMov+=2.8f;
	}


	/* //render example
	glReset(); //Clear The Screen And The Depth Buffer
	
	//Set initial view to look at
	gluPerspective(18, 256.0 / 192.0, 0.1, 40);

	//Camera perspective from there
	gluLookAt(	0.0, 0.0, 4.0,		//camera possition 
				0.0, 0.0, 0.0,		//look at
				0.0, 1.0, 0.0);		//up
	
	glMaterialShinnyness();
	
	GLfloat light_ambient[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, xrot, 1.0f };
    GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };

	//Draw each cube through a set of Open GL Display List calls 
	for (yloop=1;yloop<6;yloop++){
		for (xloop=0;xloop<yloop;xloop++){
			glBindTexture( 0, texturesSnakeGL[0]);
			// Reset The View
			glLoadIdentity();
			glTranslatef(1.4f+(float(xloop)*2.8f)-(float(yloop)*1.4f),((6.0f-float(yloop))*2.4f)-7.3f,-20.0f + camMov);
			
			glRotatef(45.0f-(2.0f*yloop)+xrot,1.0f,0.0f,0.0f);
			glRotatef(45.0f+yrot,0.0f,1.0f,0.0f);

			//not a real gl function and will likely change
			#define GX_LIGHT0 (1 << 0)
			glPolyFmt(GX_LIGHT0 | POLY_ALPHA(31) | POLY_CULL_NONE);
			
			glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
			glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
			glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
			glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			glMaterialf(GL_AMBIENT_AND_DIFFUSE, RGB15(31,31,31));	
			//Run the precompiled standard Open GL 1.1 Display List here
			glColor3fv(boxcol[yloop-1]);
			glCallList(box);
			glColor3fv(topcol[yloop-1]);
			glCallList(top);
		}
	}
	glFlush();
	*/

	//Snake GL exec
	{

		glReset(); //Clear The Screen And The Depth Buffer
	
		
		
		
		
		GLfloat light_ambient[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat light_specular[] = { 1.0f, 1.0f, xrot, 1.0f };
		GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };

		//Handle keypress
		game->on_key_pressed(keysDown());
		
		//Handle Display
		{
			//glViewport(width / 2, height / 2, 100, 100);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			//not a real gl function and will likely change
			#define GX_LIGHT0 (1 << 0)
			glPolyFmt(GX_LIGHT0 | POLY_ALPHA(31) | POLY_CULL_NONE);
			game->display();
			glFlush();
		}
	}
	return true;										// Keep Going
}