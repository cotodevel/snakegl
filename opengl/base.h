#ifndef _BASE_H
#define _BASE_H

#if defined(__APPLE__) || defined(MACOSX)
    #include <OpenGL/gl.h>
    #include <OpenGL/glx.h>
    #include <GLUT/glut.h>
#elif defined(_WIN32) || defined(WIN32)
    #include <windows.h>
    #include <GL/gl.h>
    #include <GL/glut.h>
    //#include <GL/glext.h>
#else
    #include <GL/gl.h>
    #include <GL/glx.h>
    #include <GL/glut.h>
#endif

//#include "glut_shapes.h"

#include <deque>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "soil.h"

using namespace std;

// https://github.com/AdrienHerubel/imgui/blob/master/lib/glew/glew.c
#if defined(_WIN32)
    #define glewGetProcAddress(name) wglGetProcAddress((LPCSTR)name)
#elif defined(__APPLE__)
    #define glewGetProcAddress(name) NSGLGetProcAddress(name)
#elif defined(__sgi) || defined(__sun)
    #define glewGetProcAddress(name) dlGetProcAddress(name)
#else /* __linux */
    #define glewGetProcAddress(name) (*glXGetProcAddressARB)(name)
#endif

// Objects can navigate from -5.0f to 5.0f.
#define BOARD_SIZE      5
// Y axis difference. Ground to Objects.
#define GROUND_DIFF     0.25f

#define GROUND_TEXTURE  0
#define FOOD_TEXTURE    1
#define BARRIER_TEXTURE 2
#define SNAKE_TEXTURE   3
#define MENU_TEXTURE    4
#define BRICK_TEXTURE   5

#define TEXTURE_COUNT   6

/* Key mappings. */
#define KEY_CAMERA      32  // space
#define KEY_PAUSE       112 // p
#define KEY_QUIT        27  // esc
#define KEY_SELECT      13  // enter
#define KEY_START       115 // s
#define KEY_STOP        113 // q
#define KEY_RESET       114 // r
#define KEY_UP          GLUT_KEY_UP
#define KEY_DOWN        GLUT_KEY_DOWN
#define KEY_RIGHT       GLUT_KEY_RIGHT
#define KEY_LEFT        GLUT_KEY_LEFT


enum Direction
{
    DOWN  = 1,
    UP    = 2,
    LEFT  = 3,
    RIGHT = 4,
};

enum ObjectType
{
    NONE    = 0,
    SNAKE   = 1,
    FOOD    = 2,
    BARRIER = 3,
    BOARD   = 4,
};

enum Level
{
    VIRGIN       = 1,
    JEDI         = 2,
    ASIAN        = 3,
    CHUCK_NORRIS = 4,
};

struct Point
{
    float x, y, z;
};

struct Camera
{
    GLdouble eyeX, /* gluLookAt */
             eyeY,
             eyeZ,
             centerX,
             centerY,
             centerZ,
             upX,
             upY,
             upZ,
             /* gluPerspective  */
             fovy,
             aspect,
             zNear,
             zFar;
};

extern void setVSync(bool sync);
extern GLuint textures[TEXTURE_COUNT];

#endif