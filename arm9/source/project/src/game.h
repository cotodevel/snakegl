#ifndef GAME_H
#define GAME_H

#ifdef WIN32
#include "TGDSTypes.h"
#endif

#include "Scene.h"

#ifdef __cplusplus

class Game
{
public:
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
    bool      wait();
    bool      wait2();
    bool      clock();
    bool      clock2();
    void      calculateFPS();
    void      run();
    void      draw_menu();
    Scene	  scenario; /// the scene we render
    int       currentTime;
	Game(int argc, char *argv[]);
    ~Game();
    void display();
    void pause();
    void start();
    void stop();
    void reset();
};

extern Game* game;

#endif

#endif

extern float to_fps(float fps, int value);

extern GLvoid ReSizeGLScene(GLsizei widthIn, GLsizei heightIn);

#if defined(_MSC_VER)
extern void keyboard(unsigned char key, int x, int y);
extern void keyboardSpecial(int key, int x, int y);
#endif

extern void keyboardInput(unsigned int key, int x, int y);	/// handles keyboard input
