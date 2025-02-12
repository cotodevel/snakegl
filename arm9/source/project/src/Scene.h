#ifndef __SCENE_H
#define __SCENE_H

#ifdef _MSC_VER
#include <GL\GLUT.h>
#include "TGDSTypes.h"
#include "..\..\..\..\..\toolchaingenericds\src\common\libutils\arm9\source\ndsDisplayListUtils.h"
#endif

#ifdef ARM9
#include <math.h>
#include "timerTGDS.h"
#include "biosTGDS.h"
#include "videoGL.h"
#include "soundTGDS.h"
#include "main.h"
#include "ipcfifoTGDSUser.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#if defined(_WIN32) || defined(WIN32)
    #include <windows.h>
    #include <GL/gl.h>
    #include <GL/glut.h>
	#include "SOIL.h"
#endif

#if defined(_MSC_VER) || defined(WIN32)
#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include "SOIL.h"



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

#define SIGN(x) (x < 0 ? (-1) : 1)
#define PI 3.14159

//Deque implementation in C
#define MAX_DEQUE_OBJECTS 600
#define INVALID_DEQUE_ENTRY ((int)-1)
#define TYPEDEF_NULL ((int)-1)
#define TYPEDEF_STRUCT_POINT ((int)0)

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
#define SNAKE_TEXTURE   4
#define MENU_TEXTURE    5
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

//GCC assumes whatever value instead proper u16 value here, override keypad bits to fix it
#define KEY_A      ((u16)(1 << 0))
#define KEY_B      ((u16)(1 << 1))
#define KEY_SELECT ((u16)(1 << 2))
#define KEY_START  ((u16)(1 << 3))
#define KEY_RIGHT  ((u16)(1 << 4))
#define KEY_LEFT   ((u16)(1 << 5))
#define KEY_UP     ((u16)(1 << 6))
#define KEY_DOWN   ((u16)(1 << 7))
#define KEY_R      ((u16)(1 << 8))
#define KEY_L      ((u16)(1 << 9))

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
	int index; //deque: if -1 non-allocated, otherwise index number, until MAX
};


struct DequeObject
{ 
	//deque impl.
	struct Point points[MAX_DEQUE_OBJECTS];
	int front, rear;
    int direction;
	//
};


struct Camera{
	GLfloat eyeX, /* gluLookAt */
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


struct Scene
{
	bool fogMode;
	bool light0On;
	bool light1On;
	bool wireMode;	/// wireframe mode on / off
	bool flatShading;	/// flat shading on / off
	bool fullScreen;

	//game specific
    struct Point food;
    
	//barrier
	struct DequeObject barriers;
	
    struct Camera camera;
    #ifdef WIN32
    int camera_mode;
    #endif
	#ifdef ARM9
	bool close_camera_mode;
	#endif
	float m;
    float a;

	//snake
    struct DequeObject snake;
};

struct Game
{
	bool      is_game_over;
    bool      is_running;
    bool      paused;
    bool      ate;
    bool      key_pressed;
    int       m, m2;
    int       frameCount;
    int       previousTime;
    int       score;
    int       tick;
    int       tick2;
    int       level;
    float     fps;
    struct Scene scenario; /// the scene we render
    int       currentTime;
};

#endif

#ifdef __cplusplus
extern "C"{
#endif

extern void initScene(struct Scene * sceneInst, int argc, char *argv[]);
extern void resetScene(struct Scene * sceneInst);
extern void add_barrier(struct Scene * sceneInst);
extern void change_food_pos(struct Scene * sceneInst);
extern void draw_board(struct Scene * sceneInst);
extern void draw_food(struct Scene * sceneInst);
extern void draw_barrier(struct Scene * sceneInst);
extern void draw_objects(struct Scene * sceneInst);
extern enum ObjectType has_collision(struct Scene * sceneInst, struct Point p);
extern void change_camera_pos(struct Scene * sceneInst);
extern void set_camera(struct Scene * sceneInst);

// light 0 colours
extern GLfloat light_ambient0Scene[4];
extern GLfloat light_diffuse0Scene[4];
extern GLfloat light_specular0Scene[4];
extern GLfloat light_position0Scene[4];

// light 1 colours
extern GLfloat light_ambient1Scene[4];
extern GLfloat light_diffuse1Scene[4];
extern GLfloat light_specular1Scene[4];
extern GLfloat light_position1Scene[4];

extern int widthScene;	/// the width of the window
extern int heightScene;	/// the height of the window

extern void animateScene(int type);	/// animates the scene (GLUT)
extern void drawScene(void);	/// Renders a single frame of the scene
extern int TWLPrintf(const char *fmt, ...);

extern GLint DLEN2DTEX;
extern GLint DLDIS2DTEX;
extern GLint DLSOLIDCUBE05F;
extern void enable_2D_texture();
extern void disable_2D_texture();
extern void glut2SolidCube05f();
extern int InitGL(int argc, char *argv[]); /// initialises OpenGL
extern void setupTGDSProjectOpenGLDisplayLists();

extern void render3DUpperScreen();
extern void render3DBottomScreen();
extern int startTGDSProject(int argc, char *argv[]);
extern void TGDSAPPExit(u32 fn_address);
#ifdef WIN32
extern GLuint texturesSnakeGL[TEXTURE_COUNT+1];
#endif
extern void load_resources();
extern void unload_resources();
extern void setVSync(bool sync);
extern int width, height;
extern bool NDSDual3DCameraFlag;
extern bool TGDSProjectDual3DEnabled;

#if defined(ARM9)
extern void enableDual3DTGDS();
extern void disableDual3DTGDS();
extern bool getDual3DTGDSStatus();
#endif

extern void SnakeInit(struct DequeObject * snakeInst);
extern void  grow(struct DequeObject * snakeInst, bool back);
extern void  resetSnake(struct DequeObject * snakeInst);
extern void  moveDequeObject(struct DequeObject * snakeInst);
extern void  set_directionSnake(struct DequeObject * snakeInst, int d);
extern bool  has_collisionSnake(struct DequeObject * snakeInst, struct Point p);
extern void  draw(struct DequeObject * snakeInst);
extern struct Point head(struct DequeObject * snakeInst);
extern struct Point tail(struct DequeObject * snakeInst);
extern int   size(struct DequeObject * snakeInst);


extern int random_range(int min, int max);
extern float random_pos();
extern struct Point random_point();
extern void draw_cube(float size, struct Point p, int res_id);
extern void draw_text(char * s, struct Point p, float r, float g, float b);

//Deque implementation in C
extern void pushFront(struct Point *, struct Point, int *pfront, int *prear);
extern void pushBack(struct Point *, struct Point, int *pfront, int *prear);
extern struct Point popFront(struct Point *arr, int *pfront, int *prear);
extern struct Point popBack(struct Point *arr, int *pfront, int *prear);
extern void display(struct Point *arr);
extern int count(struct Point *arr);
extern void clearDequeObject(struct DequeObject *arr);

extern struct Game game;
extern void initGame(struct Game * instGame, int argc, char *argv[]);
extern void pauseGame(struct Game * instGame);
extern void start(struct Game * instGame);
extern void stop(struct Game * instGame);
extern void reset(struct Game * instGame);
extern void      draw_menu(struct Game * instGame);
extern void      run(struct Game * instGame);
extern bool      waitGame(struct Game * instGame);
extern bool      wait2(struct Game * instGame);
extern void      calculateFPS(struct Game * instGame);
extern bool      clockGame(struct Game * instGame);
extern bool      clock2(struct Game * instGame);

extern float to_fps(float fps, int value);
extern GLvoid ReSizeGLScene(GLsizei widthIn, GLsizei heightIn);

#if defined(_MSC_VER)
extern void keyboard(unsigned char key, int x, int y);
extern void keyboardSpecial(int key, int x, int y);
#endif

extern void keyboardInput(unsigned int key, int x, int y);	/// handles keyboard input
extern void drawSphereCustom(float r, int lats, int longs);

#ifdef __cplusplus
}
#endif
