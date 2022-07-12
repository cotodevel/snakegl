#include "scenario.h"


//
// Scenario implementation
//

Scenario::Scenario()
{
    a = 0;
    m = 0.1;
}

void Scenario::reset()
{
    barriers.clear();
    camera_mode = 3;
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
void Scenario::draw_axis()
{
    //glLineWidth(1.0f);
    glNormal3f(0.0, 1.0, 0.0);
    // eixo X - Red
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, 0.0f);
    glEnd();
    // eixo Y - Green
    glColor3f(0.0f, 0.8f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);
    glEnd();
    // eixo Z - Blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);
    glEnd();
}

void Scenario::draw_board()
{
    enable_2D_texture();
    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, textures[GROUND_TEXTURE]);
        glBegin(GL_POLYGON);
            //glColor3f(0.0f, 1.0f, 0.0f);
            glNormal3f(0.0, 1.0, 0.0);
            glTexCoord2f(0, 0);
            glVertex3f(-BOARD_SIZE, 0.0f,  BOARD_SIZE);
            glTexCoord2f(1, 0);
            glVertex3f( BOARD_SIZE, 0.0f,  BOARD_SIZE);
            glTexCoord2f(1, 1);
            glVertex3f( BOARD_SIZE, 0.0f, -BOARD_SIZE);
            glTexCoord2f(0, 1);
            glVertex3f(-BOARD_SIZE, 0.0f, -BOARD_SIZE);
        glEnd();

        Point p;
        float size = -BOARD_SIZE - 0.1f;
        // Draw bordes. TODO: It's better use a rectangle.
        while (size < BOARD_SIZE + 0.1f)
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

    glPopMatrix();
    disable_2D_texture();
}

void Scenario::draw_food()
{
    Point p = food;
    //draw_sphere(0.25f, p, FOOD_TEXTURE);

    // "leaf"
	glPushMatrix();
        glTranslatef(p.x + 0.05, p.y + m + 0.25, p.z);
        glRotatef(a, 0.0, 1.0, 0.0);

        glColor3f(0.2f, 0.4f, 0.0f);
         glBegin(GL_TRIANGLE_STRIP);
            glVertex3f(0, 0, 0.1);
            glVertex3f(0, 0, 0.2);
            glVertex3f(0.1, -0.01, 0.3);

            glVertex3f(0.1, -0.01, 0.4);
            glVertex3f(0.15, -0.02, 0.4);
            glVertex3f(0.15, -0.02, 0.4);
            
        glEnd();
    glPopMatrix();

    enable_2D_texture();

    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, textures[FOOD_TEXTURE]);
        glTranslatef(p.x, p.y + m, p.z);
        glRotatef(a, 0.0, 1.0, 0.0);
        drawSphere(0.25f, 50.0f, 50.0f); //glut2SolidSphere(0.25f, 100.0f, 100.0f); 
    glPopMatrix();

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
#ifdef DEBUG
    draw_axis();
#endif
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

void Scenario::change_camera_pos()
{
    camera_mode += 1;

    if (camera_mode > 3)
    {
        camera_mode = 0;
    }
}

void Scenario::set_camera()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    camera.upX    = 0.0f;
    camera.upY    = 1.0f;
    camera.upZ    = 0.0f;
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
}