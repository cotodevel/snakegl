#ifndef SCEN_H
#define SCEN_H

#include "base.h"

#include "snake.h"

class Scenario
{
private:
    Point food;
    vector<Point> barriers;
    Camera camera;
    void draw_food();
    void draw_barrier();
    void draw_background();
public:
	#ifdef WIN32
    int camera_mode;
    #endif
	#ifdef ARM9
	bool close_camera_mode;
	#endif
	float m;
    float a;
    Snake snake;
    Scenario();
    void add_barrier();
    void change_food_pos();
    void draw_board();
    void draw_objects();
    void change_camera_pos();
    void set_camera();
    void reset();
    ObjectType has_collision(Point p);
};

#endif
