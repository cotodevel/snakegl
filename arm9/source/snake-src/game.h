#ifndef GAME_H
#define GAME_H

#include "scenario.h"
#include <ctime>
#include <cstdio>

class Game
{
private:
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
public:
    Scenario* scenario;
    int       currentTime;
	Game();
    ~Game();
    void display();
    void pause();
    void start();
    void stop();
    void reset();
    bool on_key_pressed(unsigned int key);
};


extern float to_fps(float fps, int value);
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern Game* game;

#ifdef __cplusplus
}
#endif
