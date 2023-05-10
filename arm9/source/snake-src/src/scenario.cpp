#include "scenario.h"
#ifdef ARM9
#include "main.h"
#endif
#include "snake.h"
#include "game.h"

//
// Scenario implementation
//

Scenario::Scenario()
{
    a = 0;
    m = 0.1;
	#ifdef ARM9
	close_camera_mode = false;
	#endif
}

void Scenario::reset()
{
    barriers.clear();
	#ifdef WIN32
    camera_mode = 3;
    #endif
	snake.reset();
    change_food_pos();
    add_barrier();
}

void Scenario::add_barrier()
{
    Point p = random_point();
    bool b = false;

    while (b || has_collision(p) != NONE)
    {
        p = random_point();
    }

    barriers.push_back(p);
}

void Scenario::change_food_pos()
{
    Point p = random_point();

    while (has_collision(p) != NONE)
    {
        p = random_point();
    }

    food = p;
}

void Scenario::draw_board()
{
    enable_2D_texture();
	glPushMatrix();
	
		glBindTexture(
        #ifdef WIN32
            GL_TEXTURE_2D,
        #endif
        #ifdef ARM9
        0,
        #endif
         texturesSnakeGL[GROUND_TEXTURE]);
        glBegin(GL_QUADS);
            glNormal3i(0.0, 1.0, 0.0);
            glTexCoord2f(0, 0);
            glVertex3f(-BOARD_SIZE, 0,  BOARD_SIZE);
            glTexCoord2f(1, 0);
            glVertex3f( BOARD_SIZE, 0,  BOARD_SIZE);
            glTexCoord2f(1, 1);
            glVertex3f( BOARD_SIZE, 0, -BOARD_SIZE);
            glTexCoord2f(0, 1);
            glVertex3f(-BOARD_SIZE, 0, -BOARD_SIZE);
        glEnd();
		
        Point p;
        double size = -BOARD_SIZE - 0.1;
        // Draw bordes. TODO: It's better use a rectangle.
        while (size < BOARD_SIZE + 0.1)
        {
            p.x = size;
            p.y = 0.125f;
            p.z = BOARD_SIZE + 0.125;
            draw_cube(0.25f, p, BRICK_TEXTURE);

            p.x = -BOARD_SIZE - 0.125;
            p.y = 0.125f;
            p.z = size;
            draw_cube(0.25f, p, BRICK_TEXTURE);

            p.x = BOARD_SIZE + 0.125;
            p.y = 0.125f;
            p.z = -size;
            draw_cube(0.25f, p, BRICK_TEXTURE);

            p.x = -size;
            p.y = 0.125f;
            p.z = -BOARD_SIZE - 0.125;
            draw_cube(0.25f, p, BRICK_TEXTURE);

            size += 0.25f;
        }
	glPopMatrix(
        #ifdef ARM9
        1
        #endif
    );
	disable_2D_texture();
}

void Scenario::draw_food()
{
    Point p = food;
    //draw_sphere(0.25f, p, FOOD_TEXTURE);

    // "leaf"
	glPushMatrix();
        glTranslatef(p.x, p.y + m + 0.45, p.z);
        glRotatef(a, 0.0, 1.0, 0.0);
		//(leaf object)
        glColor3f(0.0f, 200.4f, 0.0f);
         glBegin(GL_TRIANGLE_STRIP);
            glVertex3f(0, 0, 0.3);
            glVertex3f(0, 0, 0.4);
            glVertex3f(0.1, -0.01, 0.5);

            glVertex3f(0.1, -0.01, 1.0);
            glVertex3f(0.15, -0.02, 1.0);
            glVertex3f(0.15, -0.02, 1.0);
            
        glEnd();
	glPopMatrix(
	#ifdef ARM9
		1
	#endif
	);
	
	enable_2D_texture();
	
    glPushMatrix();
		glBindTexture(
            #ifdef WIN32
                GL_TEXTURE_2D,
            #endif
            #ifdef ARM9
                0, 
            #endif    
            texturesSnakeGL[FOOD_TEXTURE]
        );
		glTranslatef(p.x, p.y + m, p.z);
        glRotatef(a, 0.0, 1.0, 0.0);
		//(fruit object)
		glColor3f(1.0f, 1.0f, 14.0f);
		drawSphere();
	glPopMatrix(
        #ifdef ARM9
        1
        #endif
    );
	disable_2D_texture();
}

void Scenario::draw_barrier()
{
    for (size_t i = 0; i < barriers.size(); ++i)
    {
        Point p = barriers.at(i);
        draw_cube(0.5f, p, BARRIER_TEXTURE);
    }
}

void Scenario::draw_objects()
{
    draw_board();
    draw_food();
    draw_barrier();
    snake.draw();
}

ObjectType Scenario::has_collision(Point p)
{
    if (p.x >  5.0f ||
        p.x < -5.0f ||
        p.z >  5.0f ||
        p.z < -5.0f)
    {
        return BOARD;
    }

    if (p.x == food.x && p.z == food.z)
    {
        return FOOD;
    }

    for (size_t i = 0; i < barriers.size(); ++i)
    {
        Point b = barriers.at(i);

        if (p.x == b.x && p.z == b.z)
        {
            return BARRIER;
        }
    }

    if (snake.has_collision(p))
    {
        return SNAKE;
    }

    return NONE;
}

void Scenario::change_camera_pos(){
    #ifdef WIN32
	camera_mode += 1;
    if (camera_mode > 3)
    {
        camera_mode = 0;
    }
	#endif
	#ifdef ARM9
	close_camera_mode = !close_camera_mode;
	#endif
}

void Scenario::set_camera()
{
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	camera.upX    = 0.0f;
    camera.upY    = 1.0f;
    camera.upZ    = 0.0f;
	#ifdef WIN32
    camera.aspect = 1;

    if (camera_mode == 0)
    {
        camera.eyeX    = 0.0f;
        camera.eyeY    = 10.0f;
        camera.eyeZ    = 15.0f;
        camera.centerX = 0.0f;
        camera.centerY = 0.0f;
        camera.centerZ = 0.0f;
        camera.fovy    = 45;
        camera.zNear   = 0.1f;
        camera.zFar    = 50;
    }
    else if (camera_mode == 1)
    {
        camera.eyeX    = 0.0f;
        camera.eyeY    = 1.0f;
        camera.eyeZ    = 20.0f;
        camera.centerX = 0.0f;
        camera.centerY = 0.0f;
        camera.centerZ = 0.0f;
        camera.fovy    = 45;
        camera.zNear   = 0.1f;
        camera.zFar    = 50;
    }
    else if (camera_mode == 2)
    {
        #ifdef DEBUG
            camera.eyeX    = 0.0f;
            camera.eyeY    = -10.0f;
            camera.eyeZ    = 15.0f;
            camera.centerX = 0.0f;
            camera.centerY = 0.0f;
            camera.centerZ = 0.0f;
            camera.fovy    = 45;
            camera.zNear   = 0.1f;
            camera.zFar    = 50;
        #else
            camera.eyeX    = -2.0f;
            camera.eyeY    = 5.0f;
            camera.eyeZ    = 20.0f;
            camera.centerX = 0.0f;
            camera.centerY = 0.0f;
            camera.centerZ = 0.0f;
            camera.fovy    = 45;
            camera.zNear   = 1.0f;
            camera.zFar    = 50;
        #endif
    }
    else if (camera_mode == 3)
    {
        camera.eyeX    = 0.0f;
        camera.eyeY    = 45.0f;
        camera.eyeZ    = 1.0f;
        camera.centerX = 0.0f;
        camera.centerY = 0.0f;
        camera.centerZ = 0.0f;
        camera.fovy    = 20;
        camera.zNear   = 1.0f;
        camera.zFar    = 100;
    }

    gluPerspective(camera.fovy, camera.aspect, camera.zNear, camera.zFar);
    gluLookAt(camera.eyeX, camera.eyeY, camera.eyeZ, camera.centerX, camera.centerY, camera.centerZ, camera.upX, camera.upY, camera.upZ);
	glMatrixMode(GL_MODELVIEW);
	#endif
	
	#ifdef ARM9
	if (close_camera_mode == true){
        Point snak = game->scenario->snake.head();
        camera.eyeX    = snak.x;
        camera.eyeY    = 1.0f;
        camera.eyeZ    = 5.0f + snak.z;
        camera.centerX = 0.0f;
        camera.centerY = 45.0f;
        camera.centerZ = -90.0f;
        camera.upX = 0.0f;
        camera.upY = 0.0f;
        camera.upZ = 45.0f;
    }
    else {
        camera.eyeX    = 0.0f;
        camera.eyeY    = 0.0f;
        camera.eyeZ    = 16.0f;
        camera.centerX = 0.0f;
        camera.centerY = 0.0f;
        camera.centerZ = 0.0f;
        camera.upX = 1.0f;
        camera.upY = 1.0f;
        camera.upZ = 1.0f;
    }
    
    //Set initial view to look at
    gluPerspective(20, 256.0 / 192.0, 0.1, 140);
    gluLookAt(camera.eyeX, camera.eyeY, camera.eyeZ,		//camera possition / eye
                camera.centerX, camera.centerY, camera.centerZ,		//look at / center 
                camera.upX, camera.upY, camera.upZ);		//up X,Y,Z

	if (close_camera_mode != true){
		glRotateX(90.0f);
	    glRotateY(45.0f);
	}
	glMatrixMode(GL_MODELVIEW);
	#endif
}