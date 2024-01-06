#include "snake.h"
#include "Scene.h"

#ifdef ARM9
#include "debugNocash.h"
#include "game.h"
#include "timerTGDS.h"
#include "Sphere008.h"
#endif

//
// Snake implementation
//
Snake::Snake()
{
}

void Snake::reset()
{
    points.clear();
    int d = (rand() % 4) + 1;
    set_direction(d);

    Point p;
    p.x = 0.0f;
    p.y = GROUND_DIFF;
    p.z = 0.0f;

    points.push_front(p);

    grow();
}

void Snake::move()
{
    points.pop_back();
    grow();
}

void Snake::set_direction(int d)
{
    if ((d == DOWN && direction == UP) ||
        (direction == DOWN && d == UP) ||
        (d == LEFT && direction == RIGHT) ||
        (direction == LEFT && d == RIGHT))
    {
        return;
    }

    direction = d;
}

void Snake::draw()
{
    // TODO: Draw cylindric snake.
    // It's more hard.
    // Use glut2Cylinder. 
    Point h = points[0];

	glColor3f(1.0, 1.0, 0.6);
    glPushMatrix();
        glTranslatef(h.x, h.y, h.z);
	    glut2SolidCube05f();
	glPopMatrix(
		#ifdef ARM9
		1
		#endif
	);
    enable_2D_texture();
	glBindTexture(
		#ifdef WIN32
		GL_TEXTURE_2D, texturesSnakeGL[SNAKE_TEXTURE]
		#endif
		#ifdef ARM9
		0, textureSizePixelCoords[SNAKE_TEXTURE].textureIndex
		#endif
	);
	
	for (size_t i = 1; i < points.size(); ++i)
    {
        Point p = points.at(i);

		glPushMatrix();
            glTranslatef(p.x, p.y, p.z);
		    glut2SolidCube05f();
		glPopMatrix(
			#ifdef ARM9
			1
			#endif
		);
    }

    disable_2D_texture();
}

Point Snake::head()
{
    return points[0];
}

Point Snake::tail()
{
    return points[points.size() - 1];
}

void Snake::grow(bool back)
{
    Point p;
    p.x = points[0].x;
    p.y = points[0].y;
    p.z = points[0].z;

    switch (direction)
    {
        case DOWN:
            p.z += 0.5f;
        break;
        case UP:
            p.z -= 0.5f;
        break;
        case LEFT:
            p.x -= 0.5f;
        break;
        case RIGHT:
            p.x += 0.5f;
        break;
    }

    if (back)
    {
        points.push_back(p);
    }
    else
    {
        points.push_front(p);
    }
}

bool Snake::has_collision(Point p)
{
    // Skip head. It's the same point.
    for (size_t i = 1; i < points.size(); ++i)
    {
        Point b = points.at(i);

        if (p.x == b.x && p.z == b.z)
        {
            return true;
        }
    }

    return false;
}

int Snake::size()
{
    return points.size();
}

//////////////////////////////////////////////////////////////////////////////////////////

int random_range(int min, int max)
{
    return (rand() % (max + min)) + min;
}

float random_pos()
{
    return (rand() % 20 / 2.0f) - 5.0f;
}

Point random_point()
{
    Point p;
    p.x = random_pos();
    p.y = GROUND_DIFF;
    p.z = random_pos();

    return p;
}

void draw_cube(float size, Point p, int res_id)
{
    enable_2D_texture();
	glPushMatrix();
		glBindTexture(
			#ifdef WIN32
			    GL_TEXTURE_2D, texturesSnakeGL[res_id]
			#endif
            #ifdef ARM9
			    0, textureSizePixelCoords[res_id].textureIndex
			#endif
		);
		glTranslatef(p.x, p.y, p.z);
		glut2SolidCubeSlow(size);
	glPopMatrix(
		#ifdef ARM9
		1
		#endif
	);
	disable_2D_texture();
}

//Todo DS: use WoopsiSDK instead
void draw_text(std::string s, Point p, float r, float g, float b)
{
    glDisable(GL_LIGHTING);

	#ifdef WIN32
    int len, i;
    glColor3f(r, g, b);
    glRasterPos3f(p.x, p.y,p.z);
    len = (int) s.length();

    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
    }
	#endif
    
	#ifdef ARM9
    char debugBuf[64];
    //sprintf(debugBuf, "currentTime:%d", game->currentTime);
    sprintf(debugBuf, "%s--timer:%d", s.c_str(), getTimerCounter());
    nocashMessage(debugBuf);
    #endif
	
	glEnable(GL_LIGHTING);
}