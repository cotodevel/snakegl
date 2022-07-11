//disable _CRT_SECURE_NO_WARNINGS message to build this in VC++
#pragma warning(disable:4996)

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>

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

#include <windows.h>		// Header File For Windows
#include <stdio.h>			// Header File For Standard Input/Output
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glut.h>		// Header File For The Glaux Library
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#ifndef BASE_H
    #define BASE_H
    #include "base.h"
#endif

#include "game.h"

int width  = 640,
    height = 640;

bool is_game_over = false,
     is_running   = false;

Game* game = NULL;

void keyboard(unsigned char key, int x, int y)
{
    game->on_key_pressed((int)key);
    glutPostRedisplay();
}

void keyboardSpecial(int key, int x, int y)
{
    game->on_key_pressed((int)key);
    glutPostRedisplay();
}

void init()
{
    // Init GL before call this. Otherwise don't work.
    setVSync(true);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    float pos_light[4] = { 5.0f, 5.0f, 10.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, pos_light);
    glEnable(GL_LIGHT0);

#ifdef DEBUG
    glDisable(GL_CULL_FACE);
#else
    glEnable(GL_CULL_FACE);
#endif

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
#ifdef USE_BUFFERS
    glEnable(GL_LIGHTING);
#else
    glDisable(GL_LIGHTING);
#endif
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    load_resources();

    game = new Game();
}

void display()
{
    /*glViewport(width / 4, height / 4, 200, 200);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();*/

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    game->display();

#ifdef USE_BUFFERS
    glutSwapBuffers();
#else
    glFlush();
#endif
}

void resize(int w, int h)
{
    /*glViewport(0.0f, 0.0f, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();*/
    glutPostRedisplay();
}

GLuint textures[TEXTURE_COUNT];


int main(int argc, char** argv)
{
    glutInit(&argc, argv);

#ifdef USE_BUFFERS
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
#else
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
#endif

    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("SnakeGL");
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboardSpecial);
    init();
    glutMainLoop();

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
        cout << "Can't enable vSync.\n";
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
            cout << "Can't enable vSync.\n";
        }
    }
#else
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
        cout << "Can't enable vSync.\n";
    }
#endif
}
