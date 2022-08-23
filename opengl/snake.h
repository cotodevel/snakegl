#ifndef SNAK_H
#define SNAK_H
#include "base.h"


class Snake
{
private:
    deque<Point> points;
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


extern int random_range(int min, int max);
extern float random_pos();
extern Point random_point();
extern void load_image(const char* filename);
extern void enable_2D_texture();
extern void disable_2D_texture();
extern void draw_cube(float size, Point p, int res_id);
extern void load_resources();
extern void unload_resources();
extern void draw_text(string s, Point p, float r, float g, float b);

extern void glut2SolidCube(GLdouble size);
	extern void glut2SolidCube05f();


//Display Lists for accelerating repeated OpenGL calls
extern GLint DLEN2DTEX;
extern GLint DLDIS2DTEX;
extern GLint DLSOLIDCUBE05F;
extern GLint DLDRAWSPHERE;

extern void setupDLEnableDisable2DTextures();

#endif
