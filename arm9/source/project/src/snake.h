#ifndef SNAK_H
#define SNAK_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#ifdef ARM9
#include <math.h>
#include "TimerTGDS.h"
#include "VideoGL.h"
#include "SoundTGDS.h"
#include "biosTGDS.h"
#include "main.h"
#include "ndsDisplayListUtils.h"
#endif

#if defined(_MSC_VER)
#include "..\..\..\..\..\ndsdisplaylistutils\ndsDisplayListUtils\ndsDisplayListUtils.h"
#endif

#if defined(_MSC_VER) || defined(WIN32)
#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include "SOIL.h"
#include "TGDSTypes.h"

#ifdef __cplusplus
using namespace std;
#endif

// https://github.com/AdrienHerubel/imgui/blob/master/lib/glew/glew.c
#if defined(_MSC_VER)
    #define glewGetProcAddress(name) wglGetProcAddress((LPCSTR)name)
#elif defined(__APPLE__)
    #define glewGetProcAddress(name) NSGLGetProcAddress(name)
#elif defined(__sgi) || defined(__sun)
    #define glewGetProcAddress(name) dlGetProcAddress(name)
#else /* __linux */
    #define glewGetProcAddress(name) (*glXGetProcAddressARB)(name)
#endif

#endif

#ifdef __cplusplus
#include <ctime>
#include <cstdio>
#include <deque>
#include <iostream>
#include <vector>
#endif
#ifdef _MSC_VER
#include "SOIL.h"
#include "GL\glut.h"
#endif

#define SIGN(x) (x < 0 ? (-1) : 1)
#define PI 3.14159

////////////////////////////////////////////////Game specifics////////////////////////////////////////////////

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


#ifdef __cplusplus

class Snake
{
private:
    std::deque<Point> points;
    int direction;
public:
    Snake();
    void  move();
    void  set_direction(int d);
    bool  has_collision(Point p);
    void  draw();
    void  grow(bool back = false);
    Point head();
    Point tail();
    void  reset();
    int   size();
};

#endif

extern int random_range(int min, int max);
extern float random_pos();
extern struct Point random_point();
extern void draw_cube(float size, struct Point p, int res_id);

#ifdef __cplusplus
extern void draw_text(std::string s, struct Point p, float r, float g, float b);
#endif

#endif
