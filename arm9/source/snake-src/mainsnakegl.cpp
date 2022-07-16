//disable _CRT_SECURE_NO_WARNINGS message to build this in VC++
#pragma warning(disable:4996)

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "timerTGDS.h"
#include "keypadTGDS.h"
#include "biosTGDS.h"

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#include <iostream>
using namespace std;

/*
 *		This Code Was Created By Jeff Molofee 2000
 *		A HUGE Thanks To Fredric Echols For Cleaning Up
 *		And Optimizing The Base Code, Making It More Flexible!
 *		If You've Found This Code Useful, Please Let Me Know.
 *		Visit My Site At nehe.gamedev.net
 */

#ifdef WIN32
#include <windows.h>		// Header File For Windows
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glut.h>		// Header File For The Glaux Library
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#endif

#include "base.h"
#include "game.h"
#include "main.h"

int width  = 640,
    height = 640;

bool is_game_over = false,
     is_running   = false;

Game* game = NULL;

void keyboard(unsigned int key, int x, int y)
{
    bool pressed = game->on_key_pressed(key);
	#if defined(WIN32)
	glutPostRedisplay();
	#endif
}

void keyboardSpecial(int key, int x, int y)
{
    game->on_key_pressed((unsigned int)key);
    
	#if defined(WIN32)
	glutPostRedisplay();
	#endif
}

void display()
{
    /*glViewport(width / 4, height / 4, 200, 200);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    */

    game->display();

	#ifdef USE_BUFFERS
		glutSwapBuffers();
	#else
		glFlush();
	#endif

    handleARM9SVC();	/* Do not remove, handles TGDS services */
	IRQVBlankWait();
}

void resize(int w, int h)
{
    /*glViewport(0.0f, 0.0f, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();*/
    #if defined(WIN32)
		glutPostRedisplay();
	#endif
}

GLuint texturesSnakeGL[TEXTURE_COUNT];


int mainSnakeGL(int argc, char** argv){
    InitGL();

    // Init GL before call this. Otherwise don't work.
    setVSync(true);
	
	glClearColor(0.0f, 0.0f, 0.0f);
    float pos_light[4] = { 5.0f, 5.0f, 10.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, pos_light);
    glClearColor(1.0f, 1.0f, 1.0f);
	//glEnable(GL_LIGHT0); //todo: add per polygon attr, also lights are enabled per polygon, thus glEnable(GL_LIGHTING); / glDisable(GL_LIGHTING); is still unimplemented
	//glEnable(GL_CULL_FACE); //todo: add per polygon attr
	//glEnable(GL_DEPTH_TEST); //todo: add per polygon attr

    load_resources();

    #if defined(ARM9)
    startTimerCounter(tUnitsMilliseconds);
    #endif

    game = new Game();








	#if defined(WIN32)
	glutMainLoop();
	#endif
	
	#ifdef ARM9
	while(1==1){
		scanKeys();
		//Handle keypress
		keyboard(keysDown(), 0, 0);
		
		//Handle Display
		display();
	}
	#endif
	
    return 0;
}

/*
    http://www.dbfinteractive.com/forum/index.php?topic=5998.0
*/
void setVSync(bool sync)
{
#ifdef _WIN32
    typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALPROC)( int );
    PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;

    const char *extensions = (char*)glGetString( GL_EXTENSIONS );

    if(extensions && strstr( extensions, "WGL_EXT_swap_control" ) == 0 )
    {
        printf("Can't enable vSync.");
		return;
    }
    else
    {
        wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

        if( wglSwapIntervalEXT )
        {
            wglSwapIntervalEXT(sync);
        }
        else
        {
            printf("Can't enable vSync.");
        }
    }
#endif

#if defined(_MACOSX)
    glewGetProcAddress((const GLubyte*)"glXSwapIntervalEXT");
    PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = 0;

    GLboolean r = GL_FALSE;
    r = ((glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)glewGetProcAddress((const GLubyte*)"glXSwapIntervalEXT")) == NULL) || r;

    Display *dpy = glXGetCurrentDisplay();
    GLXDrawable drawable = glXGetCurrentDrawable();

    // TODO: not working.
    if (r && glXSwapIntervalEXT)
    {
        glXSwapIntervalEXT(dpy, drawable, (int)sync);
    }
    else
    {
        printf("Can't enable vSync.");
    }
#endif

}
