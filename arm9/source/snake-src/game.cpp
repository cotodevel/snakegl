//disable _CRT_SECURE_NO_WARNINGS message to build this in VC++
#pragma warning(disable:4996)

#include "game.h"
#include "timerTGDS.h"

float to_fps(float fps, int value)
{
    if (fps < 1)
        fps = 60;
    return value * fps / 60;
}

Game::Game()
{
    fps = 0;
    frameCount = 0;
    currentTime = 0,
    previousTime = 0;

    is_game_over = false;
    is_running = false;
    paused = false;

    level = 1;

    m = to_fps(fps, 30);
    m2 = to_fps(fps, 10);

    tick = 30;
    tick2 = 10;

    scenario = new Scenario();
}

Game::~Game()
{
    delete scenario;
}

void Game::pause()
{
    paused = true;
}

void Game::start()
{
    paused = false;
}

void Game::stop()
{
    is_running = false;
    is_game_over = false;
    paused = false;
}

void Game::reset()
{
    m = to_fps(fps, 30);
    m2 = to_fps(fps, 10);

    score = 0;
    ate = false;
    is_game_over = false;
    paused = false;

    tick = 30;
    tick2 = 10;

    scenario->reset();
    is_running = true;
}

void Game::draw_menu()
{
    enable_2D_texture();
    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, texturesSnakeGL[MENU_TEXTURE]);
        glBegin(GL_QUADS);
            glNormal3f(0.0, 1.0, 0.0);
            glTexCoord2f(0, 1);
            glVertex3f(-8, 0.0f,  8);
            glTexCoord2f(1, 1);
            glVertex3f( 8, 0.0f,  8);
            glTexCoord2f(1, 0);
            glVertex3f( 8, 0.0f, -8);
            glTexCoord2f(0, 0);
            glVertex3f(-8, 0.0f, -8);
        glEnd();
    glPopMatrix(1);
    disable_2D_texture();

    Point p;

    p.x = -2.3f;
    p.y = 0.5f;
    p.z = -3.5f;
    draw_text("SELECT YOUR LEVEL", p, 0.3f, 0.0f, 1.0f);

    p.x = -1.0f;
    p.y = 0.5f;
    p.z = -2.0f;

    if (level == VIRGIN)
    {
        p.x -= 0.3;
        draw_text("< VIRGIN >", p, 1.0f, 0.0f, 0.0f);
    }
    else
    {
        draw_text("VIRGIN", p, 0.0f, 0.0f, 0.0f);
    }

    p.x = -0.7f;
    p.y = 0.5f;
    p.z = -1.0f;

    if (level == JEDI)
    {
        p.x -= 0.3;
        draw_text("< JEDI >", p, 1.0f, 0.0f, 0.0f);
    }
    else
    {
        draw_text("JEDI", p, 0.0f, 0.0f, 0.0f);
    }

    p.x = -0.85f;
    p.y = 0.5f;
    p.z = 0.0f;

    if (level == ASIAN)
    {
        p.x -= 0.3;
        draw_text("< ASIAN >", p, 1.0f, 0.0f, 0.0f);
    }
    else
    {
        draw_text("ASIAN", p, 0.0f, 0.0f, 0.0f);
    }

    p.x = -1.7f;
    p.y = 0.5f;
    p.z = 1.0f;

    if (level == CHUCK_NORRIS)
    {
        p.x -= 0.3;
        draw_text("< CHUCK NORRIS >", p, 1.0f, 0.0f, 0.0f);
    }
    else
    {
        draw_text("CHUCK NORRIS", p, 0.0f, 0.0f, 0.0f);
    }

    if (wait())
    {
        p.x = -2.6f;
        p.y = 0.5f;
        p.z = 3.0f;
        draw_text("PRESS ENTER TO START", p, 0.0f, .0f, 0.0f);
    }
}

void Game::display()
{
    int old_cam = scenario->camera_mode;

    scenario->camera_mode = 3;
    scenario->set_camera();

    calculateFPS();
    Point p;

    char s [50];

#ifdef DEBUG
    sprintf(s, "FPS: %.2f", fps);

    p.x = -7.0f;
    p.y = 0.5f;
    p.z = 7.0f;
    draw_text(s, p, 0.0f, 0.0f, 0.0f);
#endif

    if (is_running)
    {

        scenario->camera_mode = old_cam;
        scenario->set_camera();

        if (clock2())
        {
            if (scenario->a > 360)
            {
                scenario->a = 0;
            }

            scenario->a += 5;

            if (scenario->m > 0.1f)
            {
                scenario->m = 0.0;
            }

            scenario->m += 0.05;
        }

        scenario->draw_objects();
        scenario->camera_mode = 3;
        scenario->set_camera();

        if (clock())
        {
            run();
        }

        if (is_game_over)
        {
            if (wait())
            {
                p.x = -1.25f;
                p.y = 0.5f;
                p.z = 0.15f;
                draw_text("GAME OVER", p, 0.0f, 0.0f, 0.0f);
            }
        }

        sprintf(s, "SCORE: %d", score * 10);

        p.x = -1.0f;
        p.y = 0.5f;
        p.z = -7.0f;

        if (ate)
        {
            if (wait2())
            {
                draw_text(s, p, 0.0f, 0.0f, 0.0f);
            }
        }
        else
        {
            draw_text(s, p, 0.0f, 0.0f, 0.0f);
        }
    }
    else
    {
        //draw_menu();
    }

    scenario->camera_mode = old_cam;
}

void Game::run()
{
    if (paused || is_game_over || !is_running) return;

    ObjectType o = scenario->has_collision(scenario->snake.head());
    ate = false;
    key_pressed = false;

    switch (o)
    {
        case NONE:
            scenario->snake.move();
        break;
        case FOOD:
            ate = true;
            score++;
            scenario->snake.grow(true);
            scenario->snake.move();
            scenario->change_food_pos();
            switch (level)
            {
                case VIRGIN:
                    if (scenario->snake.size() % 6 == 0)
                    {
                        scenario->add_barrier();
                    }
                break;
                case JEDI:
                    if (scenario->snake.size() % 4 == 0)
                    {
                        scenario->add_barrier();
                    }
                break;
                case ASIAN:
                    if (scenario->snake.size() % 4 == 0)
                    {
                        int v = random_range(1, 3);
                        for (int i = 0; i < v; ++i)
                        {
                            scenario->add_barrier();
                        }
                    }
                break;
                case CHUCK_NORRIS:
                    int v = random_range(1, 5);
                    for (int i = 0; i < v; ++i)
                    {
                        scenario->add_barrier();
                    }
                break;
            }
        break;
        case BARRIER:
        case BOARD:
        case SNAKE:
            is_game_over = true;
        break;
        default:
        break;
    }
}

bool Game::on_key_pressed(unsigned int key)
{
    switch (key)
    {
        case SNAKEGL_KEY_CAMERA:
            if (!is_running) return false;
            scenario->change_camera_pos();
        break;
        case SNAKEGL_KEY_PAUSE:
            if (!is_running) return false;
            pause();
        break;
        case SNAKEGL_KEY_QUIT:{
            //exit(0);
		}
        break;
        case SNAKEGL_KEY_SELECT:
            reset();
        break;
        case SNAKEGL_KEY_START:
            if (!paused) return false;
            start();
        break;
        case SNAKEGL_KEY_STOP:
            if (!is_running) return false;
            stop();
        break;
        case SNAKEGL_KEY_RESET:
            if (!is_running) return false;
            reset();
        break;
        case SNAKEGL_KEY_LEFT:
            if (!is_running || key_pressed) return false;
            scenario->snake.set_direction(LEFT);
            key_pressed = true;
        break;
        case SNAKEGL_KEY_UP:
            if (!is_running && !is_game_over)
            {
                level--;
                if (level < 1) level = 4;
            }
            if (!is_running || key_pressed) return false;
            scenario->snake.set_direction(UP);
            key_pressed = true;
        break;
        case SNAKEGL_KEY_RIGHT:
            if (!is_running || key_pressed) return false;
            scenario->snake.set_direction(RIGHT);
            key_pressed = true;
        break;
        case SNAKEGL_KEY_DOWN:
            if (!is_running && !is_game_over)
            {
                level++;
                if (level > 4) level = 1;
            }
            if (!is_running || key_pressed) return false;
            scenario->snake.set_direction(DOWN);
            key_pressed = true;
        break;
        default:{
            //printf("key = %d", key);
        }
        break;
    }
	return true;
}

bool Game::wait()
{
    bool wait = m > 0 && m < to_fps(fps, 30);
    m++;
    if (m > to_fps(fps, 30)) m = -to_fps(fps, 30);
    return wait;
}

bool Game::wait2()
{
    bool wait = m2 > 0 && m2 < to_fps(fps, 10);
    m2++;
    if (m2 > to_fps(fps, 10)) m2 = -to_fps(fps, 10);
    return wait;
}

void Game::calculateFPS(void)
{
    //  Increase frame count
    frameCount++;
	
	#if defined(WIN32)
    //  Get the number of milliseconds since glutInit called
    //  (or first call to glutGet(GLUT ELAPSED TIME)).
    currentTime = glutGet(GLUT_ELAPSED_TIME);
	#endif
	
	
	#if defined(ARM9)
    currentTime = getTimerCounter();
	#endif
	
    //  Calculate time passed
    int timeInterval = currentTime - previousTime;

    if(timeInterval > 1000)
    {
        //  calculate the number of frames per second
        fps = frameCount / (timeInterval / 1000.0f);
        //  Set time
        previousTime = currentTime;
        //  Reset frame count
        frameCount = 0;
    }
}

bool Game::clock()
{
    // Speed up every time grows.
    tick += (level + (scenario->snake.size() / 10));
    bool wait = tick < to_fps(fps, 30);
    if (tick > to_fps(fps, 30)) tick = 0;
    return !wait;
}

bool Game::clock2()
{
    tick2++;
    bool wait = tick2 < to_fps(fps, 10);
    if (tick2 > to_fps(fps, 10)) tick2 = 0;
    return !wait;
}