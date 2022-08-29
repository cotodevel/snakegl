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
	#include "../opengl/SOIL.h"
#else
	#include "videoTGDS.h"
	#include "VideoGL.h"
    #include "keypadTGDS.h"
#endif


#include <deque>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>

#ifdef WIN32
#include "../opengl/SOIL.h"
#endif
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

#ifdef WIN32
#define GROUND_TEXTURE  0
#define FOOD_TEXTURE    1
#define BARRIER_TEXTURE 2
#define SNAKE_TEXTURE   3
#define MENU_TEXTURE    4
#define BRICK_TEXTURE   5
#endif

#ifdef ARM9
#define FOOD_TEXTURE    0
#define BARRIER_TEXTURE 1
#define BRICK_TEXTURE   2
#define GROUND_TEXTURE  3
#define MENU_TEXTURE    4
#define SNAKE_TEXTURE   5
#endif

#define TEXTURE_COUNT   6

/* Key mappings. */
#ifdef WIN32
#define SNAKEGL_KEY_CAMERA      32  // space
#define SNAKEGL_KEY_PAUSE       112 // p
#define SNAKEGL_KEY_QUIT        27  // esc
#define SNAKEGL_KEY_SELECT      13  // enter
#define SNAKEGL_KEY_START       115 // s
#define SNAKEGL_KEY_STOP        113 // q
#define SNAKEGL_KEY_RESET       114 // r
#define SNAKEGL_KEY_UP          GLUT_KEY_UP
#define SNAKEGL_KEY_DOWN        GLUT_KEY_DOWN
#define SNAKEGL_KEY_RIGHT       GLUT_KEY_RIGHT
#define SNAKEGL_KEY_LEFT        GLUT_KEY_LEFT
#endif

#ifdef ARM9
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
#endif


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

#ifdef ARM9
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
#endif

#ifdef WIN32
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
#endif

#ifdef __cplusplus
extern "C"{
#endif

extern void setVSync(bool sync);
extern GLuint texturesSnakeGL[TEXTURE_COUNT+1];

#ifdef __cplusplus
}
#endif

#endif