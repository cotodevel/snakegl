#if defined(_MSC_VER)
//disable _CRT_SECURE_NO_WARNINGS message to build this in VC++
#pragma warning(disable:4996)
#endif


#include "Scene.h"
#include "snake.h"
#include "game.h"

#ifdef ARM9
#include "Sphere008.h"
#include "ipcfifoTGDSUser.h"
#include "imagepcx.h"
#include "main.h"

//Textures
#include "apple.h"
#include "boxbitmap.h"
#include "brick.h"
#include "grass.h"
#include "menu.h"
#include "snakegfx.h"
#include "Texture_Cube.h"
#endif

//
// Scene implementation
//

//Global because TGDSVideo initialization happens much earlier than game context creation
bool TGDSProjectDual3DEnabled;

Scene::Scene(int argc, char *argv[])
{
	TWLPrintf("-- Creating scene\n");
	InitGL(argc, argv);
	load_resources(); // InitGL(); must be called before this

	// set up our directional overhead lights
	light0On = false;
	light1On = false;
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0Scene);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0Scene);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0Scene);
	glLightfv(GL_LIGHT0, GL_POSITION, position0Scene);
	
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1Scene);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1Scene);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular1Scene);
	glLightfv(GL_LIGHT1, GL_POSITION, position1Scene);
	
	fogMode = false;
	wireMode = false;		/// wireframe mode on / off
	flatShading = false;	/// flat shading on / off
    a = 0;
    m = 0.1;
	#ifdef ARM9
	close_camera_mode = false;
	#endif
}

void Scene::reset()
{
    barriers.clear();
	#ifdef WIN32
    camera_mode = 3;
    #endif
	snake.reset();
    change_food_pos();
    add_barrier();
}

void Scene::add_barrier()
{
    Point p = random_point();
    bool b = false;

    while (b || has_collision(p) != NONE)
    {
        p = random_point();
    }

    barriers.push_back(p);
}

void Scene::change_food_pos()
{
    Point p = random_point();

    while (has_collision(p) != NONE)
    {
        p = random_point();
    }

    food = p;
}

void Scene::draw_board()
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

void Scene::draw_food()
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
		drawSphere(3, 3, 3);
	glPopMatrix(
        #ifdef ARM9
        1
        #endif
    );
	disable_2D_texture();
}

void Scene::draw_barrier()
{
    for (size_t i = 0; i < barriers.size(); ++i)
    {
        Point p = barriers.at(i);
        draw_cube(0.5f, p, BARRIER_TEXTURE);
    }
}

void Scene::draw_objects()
{
    draw_board();
    draw_food();
    draw_barrier();
    snake.draw();
}

ObjectType Scene::has_collision(Point p)
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

void Scene::change_camera_pos(){
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

void Scene::set_camera()
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
        Point snak = game->scenario.snake.head();
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

//VS2012 WIN32 GLUT defs
#ifdef WIN32
int width  = 640, height = 640;
#endif

GLuint texturesSnakeGL[TEXTURE_COUNT+1];

int widthScene;	/// the width of the window
int heightScene;	/// the height of the window

// light 0 colours
#ifdef WIN32
GLfloat ambient0Scene[4]	= {0.1f, 0.1f, 0.1f, 1.0f}; //WIN32
GLfloat diffuse0Scene[4]	= {0.4f, 0.4f, 0.4f, 1.01f}; //WIN32
GLfloat specular0Scene[4]	= {0.2f, 0.2f, 0.2f, 1.0f}; //WIN32
GLfloat position0Scene[4]	= {0.0f, -1.0f, 0.0f, 0.0f}; //WIN32
#endif
#ifdef ARM9
GLfloat ambient0Scene[]  = { 0.0f, 0.0f, 0.0f, 1.0f }; //NDS
GLfloat diffuse0Scene[]  = { 1.0f, 1.0f, 1.0f, 1.0f }; //NDS
GLfloat specular0Scene[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //NDS
GLfloat position0Scene[] = { 2.0f, 5.0f, 5.0f, 0.0f }; //NDS
#endif

// light 1 colours
GLfloat ambient1Scene[4]	= {0.1f, 0.1f, 0.1f, 1.0f};
GLfloat diffuse1Scene[4]	= {0.45f, 0.45f, 0.45f, 1.0f};
GLfloat specular1Scene[4]	= {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat position1Scene[4]	= {-2.0f, -5.0f, -5.0f, -1.0f};
GLfloat direction1Scene[4]	= {0.0f, 0.0f, -1.0f};

#ifdef ARM9
void render3DUpperScreen(){
	//Update camera for NintendoDS Upper 3D Screen:
	game->scenario.close_camera_mode = NDSDual3DCameraFlag;
}

void render3DBottomScreen(){
	//Update camera for NintendoDS Bottom 3D Screen
	game->scenario.close_camera_mode = !NDSDual3DCameraFlag;
}
#endif

/// Renders a single frame of the scene
void drawScene(){
	Scene * scene = &game->scenario;

	#ifdef ARM9
	//NDS: Dual 3D Render implementation. Must be called right before a new 3D scene is drawn
	if(TGDSProjectDual3DEnabled == true){
		TGDS_ProcessDual(render3DUpperScreen, render3DBottomScreen);
	}
	else{
		render3DUpperScreen(); //TGDS Single 3D Screen mode
	}
	#endif

	#ifdef WIN32
	// clear scene
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	#endif

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// draw element(s) in the scene + light source(s)
	#ifdef ARM9
		updateGXLights(); //Update GX 3D light scene!
		glColor3f(1.0, 1.0, 1.0); //clear last scene color/light vectors
	#endif

	//draw objects
	#ifdef WIN32
    int old_cam = scene->camera_mode;
    scene->camera_mode = 3;
	#endif
    scene->set_camera();
    game->calculateFPS();
    {
		char s [50];
		struct Point p;
    
		if (game->is_running){
			#ifdef WIN32
			scene->camera_mode = old_cam;
			#endif
			scene->set_camera();
			#ifdef ARM9
			if (1==1)
			#endif
			#ifdef WIN32
			if (game->clock2())
			#endif
			{
				if (scene->a > 360)
				{
					scene->a = 0;
				}

				scene->a += 5;

				if (scene->m > 0.1f)
				{
					scene->m = 0.0;
				}

				scene->m += 0.05;
			}

			scene->draw_objects();
			#ifdef WIN32
			glColor3f(0.0f, 0.0f, 0.0f);
			glRectf(0,0, 0.75f, -0.1f);
			scene->camera_mode = 3;
			#endif
			scene->set_camera();

			#ifdef ARM9
			if (1==1)
			#endif
			#ifdef WIN32
			if (game->clock())
			#endif
			{
				game->run();
			}

			if (game->is_game_over)
			{
				if (game->wait())
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

			sprintf(s, "SCORE: %d", game->score * 10);

			p.x = -1.0f;
			p.y = 0.5f;
			p.z = -7.0f;

			if (game->ate)
			{
				if (game->wait2())
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
			game->draw_menu();
			#endif
		}
		#ifdef WIN32
		scene->camera_mode = old_cam;
		#endif

	}

	//swap framebuffers
	#ifdef WIN32
	glutSwapBuffers();
	#endif

	#ifdef ARM9
    glFlush();
	handleARM9SVC();	/* Do not remove, handles TGDS services */
    IRQVBlankWait();
    #endif
}

//glutSolidSphere(radius, 16, 16);  -> NDS GX Replacement
void drawSphere(float r, int lats, int longs) {
	#ifdef _MSC_VER
	glCallList(DLSPHERE);
    #endif

	#ifdef ARM9
	// Execute the display list
    glCallListGX((u32*)&Sphere008); //comment out when running on NDSDisplayListUtils
	#endif
}


//gluDisk(qObj, 0.0, RADIUS, 16, 16); -> NDS GX Implementation
void drawCircle(GLfloat x, GLfloat y, GLfloat r, GLfloat BALL_RADIUS)
{
	#define SLICES_PER_CIRCLE ((int)16)
	float angle = 360.f / SLICES_PER_CIRCLE;
	float anglex = cos(angle);
	float angley = sin(angle);
	GLfloat lastX = 1;
	GLfloat lastY = 0;
	int c = 0; 
	glBegin(GL_TRIANGLE_STRIP);
	for (c = 1; c < SLICES_PER_CIRCLE; c++)
	{
		x = lastX * anglex - lastY * angley;
		y = lastX * angley + lastY * anglex;
		glVertex2f(x * BALL_RADIUS, y * BALL_RADIUS);
		lastX = x;
		lastY = y;
	}
	glEnd();
}


void drawCylinder(int numMajor, int numMinor, float height, float radius){
	double majorStep = height / numMajor;
	double minorStep = 2.0 * PI / numMinor;
	int i, j;

	for (i = 0; i < numMajor; ++i) {
		GLfloat z0 = 0.5 * height - i * majorStep;
		GLfloat z1 = z0 - majorStep;

		//glBegin(GL_TRIANGLE_STRIP);
		for (j = 0; j <= numMinor; ++j) {
			double a = j * minorStep;
			GLfloat x = radius * cos(a);
			GLfloat y = radius * sin(a);
			glNormal3f(x / radius, y / radius, 0.0);
			
			glTexCoord2f(j / (GLfloat) numMinor, i / (GLfloat) numMajor);
			glVertex3f(x, y, z0);

			glNormal3f(x / radius, y / radius, 0.0);
			glTexCoord2f(j / (GLfloat) numMinor, (i + 1) / (GLfloat) numMajor);
			glVertex3f(x, y, z1);
		}
		//glEnd();
	}
}

GLint DLEN2DTEX = -1;
GLint DLDIS2DTEX = -1;
GLint DLSOLIDCUBE05F = -1;
GLint DLSPHERE = -1;

void enable_2D_texture(){
	glCallList(DLEN2DTEX);
	#ifdef WIN32
	glEnable(GL_TEXTURE_2D); //win32 only
	#endif
}

void disable_2D_texture(){
	glCallList(DLDIS2DTEX);
	#ifdef WIN32
	glDisable(GL_TEXTURE_2D); //win32 only
	#endif
}

//hardcoded below function at 0.5f to speedup drawing
void glut2SolidCube05f(){
	//(snake body object)
	glColor3f(1.0f, 1.0f, 14.0f);
	glCallList(DLSOLIDCUBE05F);
}

void glut2SolidCube(GLdouble size){
    static GLfloat n[6][3] =
    {
        {-1.0, 0.0, -1.0},
        {0.0, 1.0, 0.0},
        {1.0, 0.0, 0.0},
        {0.0, -1.0, 0.0},
        {0.0, 0.0, 1.0},
        {0.0, 0.0, -1.0}
    };
    static GLint faces[6][4] =
    {
        {0, 1, 2, 3},
        {3, 2, 6, 7},
        {7, 6, 5, 4},
        {4, 5, 1, 0},
        {5, 6, 2, 1},
        {7, 4, 0, 3}
    };
    GLfloat v[8][3];
    GLint i;

    v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
    v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 2;
    v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
    v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2;
    v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
    v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2;

    for (i = 5; i >= 0; i--)
    {
		glBegin(GL_QUADS);
			//(snake head object)
			glColor3f(1.0f, 1.0f, 14.0f);
            glNormal3fv(&n[i][0]);
            glTexCoord2f(0, 0);
			glVertex3fv(&v[faces[i][0]][0]);
            glTexCoord2f(1, 0);
            glVertex3fv(&v[faces[i][1]][0]);
            glTexCoord2f(1, 1);
            glVertex3fv(&v[faces[i][2]][0]);
            glTexCoord2f(0, 1);
            glVertex3fv(&v[faces[i][3]][0]);
        glEnd();
    }
}

/// Sets up the OpenGL state machine environment
/// All hints, culling, fog, light models, depth testing, polygon model
/// and blending are set up here
#ifdef ARM9
#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__((optnone))
#endif
#endif
int InitGL(int argc, char *argv[]){
	TWLPrintf("-- Setting up OpenGL context\n");
#ifdef _MSC_VER
	// initialise glut
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
	glutCreateWindow("TGDS Project through OpenGL (GLUT)");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(ReSizeGLScene);
	glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboardSpecial);
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

#ifdef ARM9
	int TGDSOpenGLDisplayListWorkBufferSize = (256*1024);
	glInit(TGDSOpenGLDisplayListWorkBufferSize); //NDSDLUtils: Initializes a new videoGL context
	
	glClearColor(255,255,255);		// White Background
	glClearDepth(0x7FFF);		// Depth Buffer Setup
	glEnable(GL_ANTIALIAS|GL_TEXTURE_2D|GL_BLEND); // Enable Texture Mapping + light #0 enabled per scene

	glDisable(GL_LIGHT0|GL_LIGHT1|GL_LIGHT2|GL_LIGHT3);

	if(TGDSProjectDual3DEnabled == false){
		setTGDSARM9PrintfCallback((printfARM9LibUtils_fn)&TGDSDefaultPrintf2DConsole); //Redirect to default TGDS printf Console implementation
		menuShow();
	}
	REG_IE |= IRQ_VBLANK;
	glReset(); //Depend on GX stack to render scene
	glClearColor(0,35,195);		// blue green background colour

	/* TGDS 1.65 OpenGL 1.1 Initialization */
	ReSizeGLScene(255, 191);
	glMaterialShinnyness();

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
#endif

	glDisable(GL_CULL_FACE); 
	glCullFace (GL_NONE);

	setupTGDSProjectOpenGLDisplayLists();
	return 0;
}


#ifdef ARM9
#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__((optnone))
#endif
#endif
void setupTGDSProjectOpenGLDisplayLists(){
	// Generate Different Lists
	// 1: enable_2D_texture()
	DLEN2DTEX=(GLint)glGenLists(1);							
	glNewList(DLEN2DTEX,GL_COMPILE);						
	{
		GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f }; 
		GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f }; 
		GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; 
		GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f }; 

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	   
		glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient); 
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse); 
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); 
        glLightfv(GL_LIGHT0, GL_POSITION, light_position); 
		
		
		{
			#ifdef WIN32
			GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f }; //WIN32
			GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f }; //WIN32
			GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f }; //WIN32
			GLfloat high_shininess[] = { 0.0f }; //WIN32
			#endif
			#ifdef ARM9
			GLfloat mat_ambient[]    = { 60.0f, 60.0f, 60.0f, 60.0f }; //NDS
			GLfloat mat_diffuse[]    = { 1.0f, 1.0f, 1.0f, 1.0f }; //NDS
			GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f }; //NDS
			GLfloat high_shininess[] = { 128.0f }; //NDS
			#endif
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mat_ambient);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_diffuse); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mat_specular); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, high_shininess);
		}
	}
	
	glEndList();
	DLDIS2DTEX=glGenLists(1);

	// 2: disable_2D_texture()
	glNewList(DLDIS2DTEX,GL_COMPILE);
	{
		GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient); 
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse); 
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); 
        glLightfv(GL_LIGHT0, GL_POSITION, light_position); 

		
		{
			#ifdef ARM9
			GLfloat mat_ambient[]    = { 60.0f, 60.0f, 60.0f, 60.0f }; //NDS
			GLfloat mat_diffuse[]    = { 1.0f, 1.0f, 1.0f, 1.0f }; //NDS
			GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f }; //NDS
			GLfloat high_shininess[] = { 128.0f }; //NDS
			#endif

			#ifdef WIN32
			GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f }; //WIN32
			GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f }; //WIN32
			GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f }; //WIN32
			GLfloat high_shininess[] = { 100.0f }; //WIN32
			#endif
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mat_ambient); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_diffuse);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mat_specular);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, high_shininess);
		}
	}
	glEndList();
	DLSOLIDCUBE05F=glGenLists(1);

	// 3: draw solid cube: 0.5f()
	glNewList(DLSOLIDCUBE05F, GL_COMPILE);
	{
		float size = 0.5f;
		GLfloat n[6][3] =
		{
			{-1.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			{1.0f, 0.0f, 0.0f},
			{0.0f, -1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, -1.0f}
		};
		GLint faces[6][4] =
		{
			{0, 1, 2, 3},
			{3, 2, 6, 7},
			{7, 6, 5, 4},
			{4, 5, 1, 0},
			{5, 6, 2, 1},
			{7, 4, 0, 3}
		};
		GLfloat v[8][3];
		GLint i;

		v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
		v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 2;
		v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
		v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2;
		v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
		v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2;

		for (i = 5; i >= 0; i--)
		{
			glBegin(GL_QUADS);
				glNormal3fv(&n[i][0]);
				glTexCoord2f(0, 0);
				glVertex3fv(&v[faces[i][0]][0]);
				glTexCoord2f(1, 0);
				glVertex3fv(&v[faces[i][1]][0]);
				glTexCoord2f(1, 1);
				glVertex3fv(&v[faces[i][2]][0]);
				glTexCoord2f(0, 1);
				glVertex3fv(&v[faces[i][3]][0]);
			glEnd();
		}
	}

	glEndList();
	DLSPHERE=glGenLists(1);

	glNewList(DLSPHERE, GL_COMPILE);	// 4: draw sphere
	{
		#define TWOPI ((double)((3.141592f)*2))
		double r=0.6,faces=12;
		double PID2 = 6*5; //6*4 move tex to upper sphere end
		double ex=0,ey=0,ez=0,px=0,py=0,pz=0,cx=0,cy=0,cz=0;
   
		//internal
		int i,j;
		double theta1,theta2,theta3;
		if (r < 0)
			r = -r;
		if (faces < 0)
			faces = -faces;
		if (faces < 4 || r <= 0) {
			return;
		}
		for (j=0;j<faces/2;j++) {
			theta1 = j * TWOPI / faces - PID2;
			theta2 = (j + 1) * TWOPI / faces - PID2;

			glBegin(GL_TRIANGLE_STRIP);
			
			for (i=0;i<=faces;i++) {
				theta3 = i * TWOPI / faces;
				ex = cos(theta2) * cos(theta3);
				ey = sin(theta2);
				ez = cos(theta2) * sin(theta3);
				px = cx + r * ex;
				py = cy + r * ey;
				pz = cz + r * ez;
				
				glNormal3f(ex,ey,ez);
				
				glTexCoord2f(-i/(double)faces,2*(j+1)/(double)faces);
				glVertex3f(px,py,pz);
				
				ex = cos(theta1) * cos(theta3);
				ey = sin(theta1);
				ez = cos(theta1) * sin(theta3);
				px = cx + r * ex;
				py = cy + r * ey;
				pz = cz + r * ez;

				glNormal3f(ex,ey,ez);
				glTexCoord2f(-i/(double)faces,2*j/(double)faces);
				glVertex3f(px,py,pz);
			}
			glEnd();
		}
	}
	glEndList();
}

#ifdef ARM9
#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__((optnone))
#endif
#endif
GLvoid ReSizeGLScene(GLsizei widthIn, GLsizei heightIn)		// resizes the window (GLUT & TGDS GL)
{
	#ifdef WIN32 //todo: does this work on NDS?
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

int startTGDSProject(int argc, char *argv[])
{
	time_t time1 = time(NULL);
	TWLPrintf("-- Program starting: %d\n", (unsigned int)time1);
	srand(time1);
	
	// register our call-back functions
	TWLPrintf("-- Registering callbacks\n");
	
	game = new Game(argc, argv);

	// 'fake' keys being pressed to enable the state to
	// setup lighting and fog
	keyboardInput((unsigned char)'L', 0, 0);
	keyboardInput((unsigned char)'0', 0, 0);
	keyboardInput((unsigned char)'1', 0, 0);
	keyboardInput((unsigned char)'2', 0, 0);
	keyboardInput((unsigned char)'F', 0, 0);

	// start the timer and enter the mail GLUT loop
#ifdef _MSC_VER
	glutTimerFunc(50, animateScene, 0);
	glutMainLoop();
#endif

#if defined(ARM9)
	startTimerCounter(tUnitsMilliseconds, 1);
    glMaterialShinnyness();
	glReset(); //Depend on GX stack to render scene
	while(1==1){
		//Handle Input & game logic
		scanKeys();
		if(keysDown()&KEY_TOUCH){
			scanKeys();
			while(keysHeld() & KEY_TOUCH){
				scanKeys();
			}
			NDSDual3DCameraFlag = !NDSDual3DCameraFlag;
			menuShow();
		}
		keyboardInput((int)keysHeld(), 0, 0);
		
		//sound (ARM7)
		
		//Render
		drawScene();
	}
#endif
	return 0;
}

/*
    http://www.dbfinteractive.com/forum/index.php?topic=5998.0
*/
void setVSync(bool sync)
{
#ifdef _MSC_VER
    typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALPROC)( int );
    PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;

    const char *extensions = (char*)glGetString( GL_EXTENSIONS );

    if(extensions && strstr( extensions, "WGL_EXT_swap_control" ) == 0 )
    {
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

        }
    }
#elif !defined(ARM9)
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

#if defined(ARM9)
//false to enable TGDS Single 3D Screen + Console; true to enable TGDS Dual 3D Screen mode
void enableDual3DTGDS(){
	TGDSProjectDual3DEnabled = true; 
}

void disableDual3DTGDS(){
	TGDSProjectDual3DEnabled = false;
}

bool getDual3DTGDSStatus(){
	return TGDSProjectDual3DEnabled;
}
#endif