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
#include "Texture_Cube.h"
#endif

#include "../src/base.h"

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
	#ifdef ARM9
	soundGameOverEmitted = false;
    BgMusicOff();
	BgMusic();
	#endif
    tick = 30;
    tick2 = 10;

    scenario->reset();
    is_running = true;
}

void Game::draw_menu(){
    enable_2D_texture();
		glPushMatrix();
		glBindTexture(
		#ifdef WIN32
			GL_TEXTURE_2D
		#endif
		#ifdef ARM9
		0
		#endif
		, texturesSnakeGL[MENU_TEXTURE]);
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

void Game::display(){
	#if defined(_MSC_VER)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    #endif
	#ifdef ARM9
	//NDS: Dual 3D Render implementation. Must be called right before a new 3D scene is drawn
	TGDS_ProcessDual(render3DUpperScreen, render3DBottomScreen);
	#endif
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    #ifdef ARM9
    updateGXLights(); //Update GX 3D light scene!
    #endif

	#ifdef WIN32
    int old_cam = scenario->camera_mode;
    scenario->camera_mode = 3;
	#endif
    scenario->set_camera();
    calculateFPS();
    Point p;
    char s [50];
    if (is_running){
		#ifdef WIN32
        scenario->camera_mode = old_cam;
        #endif
		scenario->set_camera();
		#ifdef ARM9
		if (1==1)
		#endif
		#ifdef WIN32
        if (clock2())
		#endif
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
		#ifdef WIN32
        glColor3f(0.0f, 0.0f, 0.0f);
        glRectf(0,0, 0.75f, -0.1f);
        scenario->camera_mode = 3;
		#endif
        scenario->set_camera();

		#ifdef ARM9
		if (1==1)
		#endif
		#ifdef WIN32
        if (clock())
		#endif
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
			#ifdef ARM9
			if(soundGameOverEmitted == false){
                gameoverSound();
                soundGameOverEmitted = true;
            }
			#endif
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
        #ifdef WIN32
		draw_menu();
		#endif
    }
	#ifdef WIN32
    scenario->camera_mode = old_cam;
	#endif
	
	glFlush();	
	#ifdef ARM9
    handleARM9SVC();	/* Do not remove, handles TGDS services */
    IRQVBlankWait();
    #endif
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
			#ifdef ARM9
			MunchFoodSound();
			#endif
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

//WIN32 & TGDS ARM9 Game boot code

int width  = 640, height = 640;
bool is_game_over = false, is_running = false;

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
Game* game = NULL;

#if defined(_MSC_VER)
void keyboard(unsigned char key, int x, int y)
{
    game->on_key_pressed((int)key);
    glutPostRedisplay();
}

void keyboardSpecial(int key, int x, int y)
{
    game->on_key_pressed((int)key);
    glutPostRedisplay();
}

/*
    http://www.dbfinteractive.com/forum/index.php?topic=5998.0
*/
void setVSync(bool sync)
{
#ifdef _WIN32
    typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALPROC)( int );
    PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;

    const char *extensions = (char*)glGetString( GL_EXTENSIONS );

    if(extensions && strstr( extensions, "WGL_EXT_swap_control" ) == 0 )
    {
        cout << "Can't enable vSync.\n";
        return;
    }
    else
    {
        wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

        if( wglSwapIntervalEXT )
        {
            wglSwapIntervalEXT(sync);
        }
        else
        {
            cout << "Can't enable vSync.\n";
        }
    }
#else
    glewGetProcAddress((const GLubyte*)"glXSwapIntervalEXT");
    PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = 0;

    GLboolean r = GL_FALSE;
    r = ((glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)glewGetProcAddress((const GLubyte*)"glXSwapIntervalEXT")) == NULL) || r;

    Display *dpy = glXGetCurrentDisplay();
    GLXDrawable drawable = glXGetCurrentDrawable();

    // TODO: not working.
    if (r && glXSwapIntervalEXT)
    {
        glXSwapIntervalEXT(dpy, drawable, (int)sync);
    }
    else
    {
        cout << "Can't enable vSync.\n";
    }
#endif
}
#endif

void ResizeGLScene(GLsizei widthIn, GLsizei heightIn)
{
	#ifdef WIN32
    glutPostRedisplay();
	#endif

	#ifdef ARM9
	if (heightIn==0)										// Prevent A Divide By Zero By
	{
		heightIn=1;										// Making Height Equal One
	}

	glViewport(0,0,widthIn,heightIn);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)widthIn/(GLfloat)heightIn,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
	#endif
}

void DrawGLScene(){									
	#ifdef ARM9
	scanKeys();
	if(keysDown()&KEY_TOUCH){
		scanKeys();
		while(keysHeld() & KEY_TOUCH){
			scanKeys();
		}
		NDSDual3DCameraFlag = !NDSDual3DCameraFlag;
		menuShow();
	}
	{
		game->on_key_pressed(keysDown());
	}
	#endif

	game->display();
}

void InitGL(){
	#ifdef ARM9
	/* OpenGL 1.1 Dynamic Display List */
	int TGDSOpenGLDisplayListWorkBufferSize = (256*1024);
	glInit(TGDSOpenGLDisplayListWorkBufferSize); //NDSDLUtils: Initializes a new videoGL context	
	glClearColor(255,255,255);		// White Background
	glClearDepth(0x7FFF);		// Depth Buffer Setup
	glEnable(GL_ANTIALIAS|GL_TEXTURE_2D|GL_BLEND|GL_LIGHT0); // Enable Texture Mapping + light #0 enabled per scene
	
	//#1: Load a texture and map each one to a texture slot
	u32 arrayOfTextures[7];
	arrayOfTextures[0] = (u32)&apple; //0: apple.bmp  
	arrayOfTextures[1] = (u32)&boxbitmap; //1: boxbitmap.bmp  
	arrayOfTextures[2] = (u32)&brick; //2: brick.bmp  
	arrayOfTextures[3] = (u32)&grass; //3: grass.bmp
	arrayOfTextures[4] = (u32)&menu; //4: menu.bmp
	arrayOfTextures[5] = (u32)&snakegfx; //5: snakegfx.bmp
	arrayOfTextures[6] = (u32)&Texture_Cube; //6: Texture_Cube.bmp
	int texturesInSlot = LoadLotsOfGLTextures((u32*)&arrayOfTextures, (int*)&texturesSnakeGL, 7); //Implements both glBindTexture and glTexImage2D 
	int i = 0;
	for(i = 0; i < texturesInSlot; i++){
		printf("Texture loaded: %d:textID[%d] Size: %d", i, texturesSnakeGL[i], getTextureBaseFromTextureSlot(activeTexture));
	}
	setupDLEnableDisable2DTextures();
	#endif
	
	#if defined(_MSC_VER)
	setVSync(true);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    float pos_light[4] = { 5.0f, 5.0f, 10.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, pos_light);
    glEnable(GL_LIGHT0);
	glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	#endif
}

int startSnakeGL(int argc, char *argv[])
{	
	#if defined(_MSC_VER)
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("SnakeGL");
    glutDisplayFunc(DrawGLScene);
    glutIdleFunc(DrawGLScene);
    glutReshapeFunc(ResizeGLScene);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboardSpecial);
	#endif

	InitGL();
    #if defined(ARM9)
	ResizeGLScene(255, 191);
	#endif

	load_resources(); // InitGL(); must be called before this
	setupDLEnableDisable2DTextures();
    game = new Game();

	#if defined(_MSC_VER)
	glutMainLoop();
	#endif
    
	#if defined(ARM9)
	startTimerCounter(tUnitsMilliseconds, 1);
    glMaterialShinnyness();
	glReset(); //Depend on GX stack to render scene
	while (1){
		DrawGLScene();
	}
	#endif
	return 0;
}