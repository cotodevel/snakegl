
#ifdef WIN32
//disable _CRT_SECURE_NO_WARNINGS message to build this in VC++
#pragma warning(disable:4996)
#endif

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

struct Game game;

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
void initGame(struct Game * instGame, int argc, char *argv[]){
	initScene(&instGame->scenario, argc, argv);
    instGame->fps = 0;
    instGame->frameCount = 0;
    instGame->currentTime = 0,
    instGame->previousTime = 0;

    instGame->is_game_over = false;
    instGame->is_running = false;
    instGame->paused = false;

    instGame->level = 1;

    instGame->m = to_fps(instGame->fps, 30);
    instGame->m2 = to_fps(instGame->fps, 10);

    instGame->tick = 30;
    instGame->tick2 = 10;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void pauseGame(struct Game * instGame)
{
    instGame->paused = true;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void start(struct Game * instGame)
{
    instGame->paused = false;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void stop(struct Game * instGame)
{
    instGame->is_running = false;
    instGame->is_game_over = false;
    instGame->paused = false;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void reset(struct Game * instGame)
{
    instGame->m = to_fps(instGame->fps, 30);
    instGame->m2 = to_fps(instGame->fps, 10);

    instGame->score = 0;
    instGame->ate = false;
    instGame->is_game_over = false;
    instGame->paused = false;
	#ifdef ARM9
	soundGameOverEmitted = false;
    BgMusicOff();
	BgMusic("0:/stud.ima");
	#endif
    instGame->tick = 30;
    instGame->tick2 = 10;

	resetScene(&instGame->scenario);
    instGame->is_running = true;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void draw_menu(struct Game * instGame){
    struct Point p;
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
    
    p.x = -2.3f;
    p.y = 0.5f;
    p.z = -3.5f;
    draw_text("SELECT YOUR LEVEL", p, 0.3f, 0.0f, 1.0f);

    p.x = -1.0f;
    p.y = 0.5f;
    p.z = -2.0f;

    if (instGame->level == VIRGIN)
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

    if (instGame->level == JEDI)
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

    if (instGame->level == ASIAN)
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

    if (instGame->level == CHUCK_NORRIS)
    {
        p.x -= 0.3;
        draw_text("< CHUCK NORRIS >", p, 1.0f, 0.0f, 0.0f);
    }
    else
    {
        draw_text("CHUCK NORRIS", p, 0.0f, 0.0f, 0.0f);
    }

    if (waitGame(&game))
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
void run(struct Game * instGame)
{
	enum ObjectType o;
	struct Scene * sceneInst = &instGame->scenario;
    if (instGame->paused || instGame->is_game_over || !instGame->is_running) return;
    o = has_collision(sceneInst, head(&sceneInst->snake));
    instGame->ate = false;
    instGame->key_pressed = false;

    switch (o)
    {
        case NONE:
            moveDequeObject(&sceneInst->snake);
        break;
        case FOOD:
			#ifdef ARM9
			MunchFoodSound();
			#endif
            instGame->ate = true;
            instGame->score++;
            grow(&sceneInst->snake, true);
            moveDequeObject(&sceneInst->snake);
            change_food_pos(sceneInst);
            switch (instGame->level)
            {
                case VIRGIN:
                    if (size(&sceneInst->snake) % 6 == 0)
                    {
                        add_barrier(sceneInst);
                    }
                break;
                case JEDI:
                    if (size(&sceneInst->snake) % 4 == 0)
                    {
                        add_barrier(sceneInst);
                    }
                break;
                case ASIAN:{
                    if (size(&sceneInst->snake) % 4 == 0)
                    {
                        int v = random_range(1, 3);
                        int i = 0;
						for (i = 0; i < v; ++i)
                        {
                            add_barrier(sceneInst);
                        }
                    }
				}break;
                case CHUCK_NORRIS:{
                    int v = random_range(1, 5);
                    int i = 0;
					for (i = 0; i < v; ++i)
                    {
                        add_barrier(sceneInst);
                    }
				}break;
            }
        break;
        case BARRIER:
        case BOARD:
        case SNAKE:
            instGame->is_game_over = true;
        break;
        default:
        break;
    }
}


bool waitGame(struct Game * instGame)
{
    bool waitVal = instGame->m > 0 && instGame->m < to_fps(instGame->fps, 30);
    instGame->m++;
    if (instGame->m > to_fps(instGame->fps, 30)) instGame->m = -to_fps(instGame->fps, 30);
    return waitVal;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
bool wait2(struct Game * instGame)
{
    bool wait = instGame->m2 > 0 && instGame->m2 < to_fps(instGame->fps, 10);
    instGame->m2++;
    if (instGame->m2 > to_fps(instGame->fps, 10)) instGame->m2 = -to_fps(instGame->fps, 10);
    return wait;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void calculateFPS(struct Game * instGame)
{
    //  Increase frame count
    instGame->frameCount++;
	
	#if defined(WIN32)
    //  Get the number of milliseconds since glutInit called
    //  (or first call to glutGet(GLUT ELAPSED TIME)).
    instGame->currentTime = glutGet(GLUT_ELAPSED_TIME);
	#endif
	
	
	#if defined(ARM9)
    instGame->currentTime = getTimerCounter();
	#endif
	
	{
		//  Calculate time passed
		int timeInterval = instGame->currentTime - instGame->previousTime;
	
		#ifdef WIN32
		if(timeInterval > 1000)
		{
			//  calculate the number of frames per second
			instGame->fps = instGame->frameCount / (timeInterval / 1000.0f);
			//  Set time
			instGame->previousTime = instGame->currentTime;
			//  Reset frame count
			instGame->frameCount = 0;
		}
		#endif
		#ifdef ARM9
		if(timeInterval > 10)
		{
			//  calculate the number of frames per second
			instGame->fps = instGame->frameCount / (timeInterval / 10);
			//  Set time
			instGame->previousTime = instGame->currentTime;
			//  Reset frame count
			instGame->frameCount = 0;
		}
		#endif
	}
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
bool clockGame(struct Game * instGame)
{
	bool wait;
    // Speed up every time grows.
	instGame->tick += (instGame->level + (size(&instGame->scenario.snake) / 10));
    wait = instGame->tick < to_fps(instGame->fps, 30);
    if (instGame->tick > to_fps(instGame->fps, 30)) instGame->tick = 0;
    return !wait;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("Ofast")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
bool clock2(struct Game * instGame)
{
    bool wait = false;
	instGame->tick2++;
    wait = instGame->tick2 < to_fps(instGame->fps, 10);
    if (instGame->tick2 > to_fps(instGame->fps, 10)) instGame->tick2 = 0;
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
	struct Scene * scene = &game.scenario;
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
		
		case SNAKEGL_KEY_PAUSE:
            if (!game.is_running) return ;
            pauseGame(&game);
        break;
        case SNAKEGL_KEY_QUIT:{
            exit(0);
		}
        break;
        
		case SNAKEGL_KEY_STOP:
            if (!game.is_running) return ;
            stop(&game);
        break;
		
		case SNAKEGL_KEY_START:
            if (!game.paused) return ;
            start(&game);
        break;
        
        case SNAKEGL_KEY_RESET:
            if (!game.is_running) return ;
            reset(&game);
        break;
		
		#endif

		case SNAKEGL_KEY_CAMERA:
            if (!game.is_running) return ;
            change_camera_pos(scene);
        break;
        case SNAKEGL_KEY_SELECT:
            reset(&game);
        break;
        case SNAKEGL_KEY_LEFT:
            if (!game.is_running || game.key_pressed) return ;
            set_directionSnake(&scene->snake, LEFT);
            game.key_pressed = true;
        break;
        case SNAKEGL_KEY_UP:
            if (!game.is_running && !game.is_game_over)
            {
                game.level--;
                if (game.level < 1) game.level = 4;
            }
            if (!game.is_running || game.key_pressed) return ;
            set_directionSnake(&scene->snake, UP);
            game.key_pressed = true;
        break;
        case SNAKEGL_KEY_RIGHT:{
			if (!game.is_running || game.key_pressed) return ;
            set_directionSnake(&scene->snake, RIGHT);
            game.key_pressed = true;
		}
		break;
        case SNAKEGL_KEY_DOWN:
            if (!game.is_running && !game.is_game_over)
            {
                game.level++;
                if (game.level > 4) game.level = 1;
            }
            if (!game.is_running || game.key_pressed) return ;
            set_directionSnake(&scene->snake, DOWN);
            game.key_pressed = true;
        break;
	}
	
	#if defined(_MSC_VER)
	glutPostRedisplay();
	#endif
}