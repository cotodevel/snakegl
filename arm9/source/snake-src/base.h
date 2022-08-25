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
	#include "soil.h"
#else
    #include "videoTGDS.h"
	#include "VideoGL.h"
#endif

#include "keypadTGDS.h"

#include <deque>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>

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

#define FOOD_TEXTURE    0
#define BARRIER_TEXTURE 1
#define BRICK_TEXTURE   2
#define GROUND_TEXTURE  3
#define MENU_TEXTURE    4
#define SNAKE_TEXTURE   5
#define TEXTURE_COUNT   6

/* Key mappings. */
#define SNAKEGL_KEY_CAMERA      KEY_R  // space
#define SNAKEGL_KEY_PAUSE       KEY_Y // p
#define SNAKEGL_KEY_QUIT        KEY_L  // esc
#define SNAKEGL_KEY_SELECT      KEY_SELECT  // enter
#define SNAKEGL_KEY_START       KEY_START // s
#define SNAKEGL_KEY_STOP        KEY_X // q
#define SNAKEGL_KEY_RESET       KEY_A // r
#define SNAKEGL_KEY_UP          KEY_UP
#define SNAKEGL_KEY_DOWN        KEY_DOWN
#define SNAKEGL_KEY_RIGHT       KEY_RIGHT
#define SNAKEGL_KEY_LEFT        KEY_LEFT


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
    GLfloat eyeX, /* gluLookAt */
             eyeY,
             eyeZ,
             centerX,
             centerY,
             centerZ,
             upX,
             upY,
             upZ;
};

extern void setVSync(bool sync);
extern GLuint texturesSnakeGL[TEXTURE_COUNT+1];

#endif