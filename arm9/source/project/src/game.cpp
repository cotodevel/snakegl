#ifdef _MSC_VER
#include <windows.h>
#endif

#ifdef WIN32
//disable _CRT_SECURE_NO_WARNINGS message to build this in VC++
#pragma warning(disable:4996)
#endif

#include "game.h"

#ifdef ARM9
#include "main.h"
#include "timerTGDS.h"
#include "ipcfifoTGDSUser.h"
#include "biosTGDS.h"
#include "imagepcx.h"

//Textures
#include "apple.h"
#include "boxbitmap.h"
#include "brick.h"
#include "grass.h"
#include "menu.h"
#include "snakegfx.h"
#endif

#include "Scene.h"

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
Game* game = NULL;

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
float to_fps(float fps, int value)
{
    if (fps < 1)
        fps = 60;
    return value * fps / 60;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
Game::Game(int argc, char *argv[]):
	scenario(argc, argv)
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
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
Game::~Game()
{
    
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void Game::pause()
{
    paused = true;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void Game::start()
{
    paused = false;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void Game::stop()
{
    is_running = false;
    is_game_over = false;
    paused = false;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void Game::reset()
{
    m = to_fps(fps, 30);
    m2 = to_fps(fps, 10);

    score = 0;
    ate = false;
    is_game_over = false;
    paused = false;
	#ifdef ARM9
	soundGameOverEmitted = false;
    BgMusicOff();
	BgMusic("0:/stud.ima");
	#endif
    tick = 30;
    tick2 = 10;

	game->scenario.reset();
    is_running = true;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void Game::draw_menu(){
    enable_2D_texture();
		glPushMatrix();
		glBindTexture(
		#ifdef WIN32
			GL_TEXTURE_2D, texturesSnakeGL[MENU_TEXTURE]
		#endif
		#ifdef ARM9
		    0,  textureSizePixelCoords[MENU_TEXTURE].textureIndex
		#endif
		);
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
		glPopMatrix(
            #ifdef ARM9
            1
            #endif
        );
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

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void Game::run()
{
	Scene * scene = &game->scenario;
    if (paused || is_game_over || !is_running) return;

    ObjectType o = scene->has_collision(head(&scene->snake));
    ate = false;
    key_pressed = false;

    switch (o)
    {
        case NONE:
            move(&scene->snake);
        break;
        case FOOD:
			#ifdef ARM9
			MunchFoodSound();
			#endif
            ate = true;
            score++;
            grow(&scene->snake, true);
            move(&scene->snake);
            scene->change_food_pos();
            switch (level)
            {
                case VIRGIN:
                    if (size(&scene->snake) % 6 == 0)
                    {
                        scene->add_barrier();
                    }
                break;
                case JEDI:
                    if (size(&scene->snake) % 4 == 0)
                    {
                        scene->add_barrier();
                    }
                break;
                case ASIAN:
                    if (size(&scene->snake) % 4 == 0)
                    {
                        int v = random_range(1, 3);
                        for (int i = 0; i < v; ++i)
                        {
                            scene->add_barrier();
                        }
                    }
                break;
                case CHUCK_NORRIS:
                    int v = random_range(1, 5);
                    for (int i = 0; i < v; ++i)
                    {
                        scene->add_barrier();
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

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
bool Game::wait()
{
    bool wait = m > 0 && m < to_fps(fps, 30);
    m++;
    if (m > to_fps(fps, 30)) m = -to_fps(fps, 30);
    return wait;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
bool Game::wait2()
{
    bool wait = m2 > 0 && m2 < to_fps(fps, 10);
    m2++;
    if (m2 > to_fps(fps, 10)) m2 = -to_fps(fps, 10);
    return wait;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
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
	
	#ifdef WIN32
    if(timeInterval > 1000)
    {
        //  calculate the number of frames per second
        fps = frameCount / (timeInterval / 1000.0f);
        //  Set time
        previousTime = currentTime;
        //  Reset frame count
        frameCount = 0;
    }
	#endif
	#ifdef ARM9
	if(timeInterval > 10)
    {
        //  calculate the number of frames per second
        fps = frameCount / (timeInterval / 10);
        //  Set time
        previousTime = currentTime;
        //  Reset frame count
        frameCount = 0;
    }
	#endif
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
bool Game::clock()
{
    // Speed up every time grows.
	tick += (level + (size(&game->scenario.snake) / 10));
    bool wait = tick < to_fps(fps, 30);
    if (tick > to_fps(fps, 30)) tick = 0;
    return !wait;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
bool Game::clock2()
{
    tick2++;
    bool wait = tick2 < to_fps(fps, 10);
    if (tick2 > to_fps(fps, 10)) tick2 = 0;
    return !wait;
}

#ifdef _MSC_VER
void keyboard(unsigned char key, int x, int y)
{
    keyboardInput((int)key, x, y);
    glutPostRedisplay();
}

void keyboardSpecial(int key, int x, int y)
{
    keyboardInput((int)key, x, y);
    glutPostRedisplay();
}
#endif

/// Handles keyboard input for normal keys
#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void keyboardInput(unsigned int key, int x, int y)
{
	Scene * scene = &game->scenario;
	switch(key) {
		#ifdef _MSC_VER
		case '1':{	// toggles light 0 on / off
			scene->light0On = !scene->light0On;
			if (scene->light0On){
				glEnable(GL_LIGHTING);
				glEnable(GL_LIGHT0);
			}
			else {
				glDisable(GL_LIGHTING);
				glDisable(GL_LIGHT0);
			}
		}break;

		case '2':{	// toggles light 1 on / off
			scene->light1On = !scene->light1On;
			if (scene->light1On){
				glEnable(GL_LIGHTING);
				glEnable(GL_LIGHT1);
			}
			else {
				glDisable(GL_LIGHTING);
				glDisable(GL_LIGHT1);
			}
		}break;
		
		case '3':{	// toggles Fullscreen (WIN32 VS2012, GLUT)
			scene->fullScreen = !scene->fullScreen;
			if (scene->fullScreen){
				int detectedW = glutGet(GLUT_SCREEN_WIDTH);
				int detectedH = glutGet(GLUT_SCREEN_HEIGHT);
				glutReshapeWindow(detectedW, detectedH);
				glutFullScreen();
			}
			else {
				glutReshapeWindow(width, height);
			}
		}break;
		#endif

		case SNAKEGL_KEY_CAMERA:
            if (!game->is_running) return ;
            scene->change_camera_pos();
        break;
        case SNAKEGL_KEY_PAUSE:
            if (!game->is_running) return ;
            game->pause();
        break;
        case SNAKEGL_KEY_QUIT:{
            exit(0);
		}
        break;
        case SNAKEGL_KEY_SELECT:
            game->reset();
        break;
        case SNAKEGL_KEY_START:
            if (!game->paused) return ;
            game->start();
        break;
        case SNAKEGL_KEY_STOP:
            if (!game->is_running) return ;
            game->stop();
        break;
        case SNAKEGL_KEY_RESET:
            if (!game->is_running) return ;
            game->reset();
        break;
        case SNAKEGL_KEY_LEFT:
            if (!game->is_running || game->key_pressed) return ;
            set_directionSnake(&scene->snake, LEFT);
            game->key_pressed = true;
        break;
        case SNAKEGL_KEY_UP:
            if (!game->is_running && !game->is_game_over)
            {
                game->level--;
                if (game->level < 1) game->level = 4;
            }
            if (!game->is_running || game->key_pressed) return ;
            set_directionSnake(&scene->snake, UP);
            game->key_pressed = true;
        break;
        case SNAKEGL_KEY_RIGHT:{
			if (!game->is_running || game->key_pressed) return ;
            set_directionSnake(&scene->snake, RIGHT);
            game->key_pressed = true;
		}
		break;
        case SNAKEGL_KEY_DOWN:
            if (!game->is_running && !game->is_game_over)
            {
                game->level++;
                if (game->level > 4) game->level = 1;
            }
            if (!game->is_running || game->key_pressed) return ;
            set_directionSnake(&scene->snake, DOWN);
            game->key_pressed = true;
        break;
	}
	
	#if defined(_MSC_VER)
	glutPostRedisplay();
	#endif
}