#ifndef GAME_H
#define GAME_H

#ifdef WIN32
#include "TGDSTypes.h"
#endif
#include "Scene.h"

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

extern struct Game game;
extern void initGame(struct Game * instGame, int argc, char *argv[]);
extern void pause(struct Game * instGame);
extern void start(struct Game * instGame);
extern void stop(struct Game * instGame);
extern void reset(struct Game * instGame);
extern void      draw_menu(struct Game * instGame);
extern void      run(struct Game * instGame);
extern bool      wait(struct Game * instGame);
extern bool      wait2(struct Game * instGame);
extern void      calculateFPS(struct Game * instGame);
extern bool      clock(struct Game * instGame);
extern bool      clock2(struct Game * instGame);

#endif

extern float to_fps(float fps, int value);

extern GLvoid ReSizeGLScene(GLsizei widthIn, GLsizei heightIn);

#if defined(_MSC_VER)
extern void keyboard(unsigned char key, int x, int y);
extern void keyboardSpecial(int key, int x, int y);
#endif

extern void keyboardInput(unsigned int key, int x, int y);	/// handles keyboard input
