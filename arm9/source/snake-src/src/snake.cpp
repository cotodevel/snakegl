#include "snake.h"
#ifdef ARM9
#include "debugNocash.h"
#include "game.h"
#include "timerTGDS.h"
#include "Sphere008.h"
#endif

//
// Snake implementation
//

GLuint texturesSnakeGL[TEXTURE_COUNT+1];

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
	#ifdef WIN32
    glPopMatrix();
	#endif
	#ifdef ARM9
	glPopMatrix(1);
    #endif
    enable_2D_texture();
	#ifdef WIN32
    glBindTexture(GL_TEXTURE_2D, texturesSnakeGL[SNAKE_TEXTURE]);
	#endif
	#ifdef ARM9
	glBindTexture(0, texturesSnakeGL[SNAKE_TEXTURE]);
	#endif
    for (size_t i = 1; i < points.size(); ++i)
    {
        Point p = points.at(i);

        glPushMatrix();
            glTranslatef(p.x, p.y, p.z);
            glut2SolidCube05f();
		#ifdef WIN32
        glPopMatrix();
		#endif
		#ifdef ARM9
		glPopMatrix(1);
		#endif
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

void load_image(const char* filename)
{
    #ifdef WIN32
	int width, height;
    unsigned char* image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);
	#endif
}

GLint DLEN2DTEX = -1;
GLint DLDIS2DTEX = -1;
GLint DLSOLIDCUBE05F = -1;
GLint DLSPHERE = -1;


#ifdef ARM9
#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__((optnone))
#endif
#endif
void setupDLEnableDisable2DTextures(){
	DLEN2DTEX=glGenLists(4);									// Generate Different Lists
	glNewList(DLEN2DTEX,GL_COMPILE);							// 1: enable_2D_texture()
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

        GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
        GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat high_shininess[] = { 0.0f };

        glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mat_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mat_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, high_shininess);
	}
	glEndList();
	DLDIS2DTEX=DLEN2DTEX+1;										
	glNewList(DLDIS2DTEX,GL_COMPILE);							// 2: disable_2D_texture()
	{
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

        GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
        GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat high_shininess[] = { 100.0f };

        glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mat_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mat_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, high_shininess);
	}
	glEndList();


	DLSOLIDCUBE05F=DLDIS2DTEX+1;										
	glNewList(DLSOLIDCUBE05F,GL_COMPILE);							// 3: draw solid cube: 0.5f()
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

	
	DLSPHERE=DLSOLIDCUBE05F+1;										
	glNewList(DLSPHERE,GL_COMPILE);							// 4: draw sphere
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

void draw_cube(float size, Point p, int res_id)
{
    enable_2D_texture();

    glPushMatrix();
		#ifdef ARM9
        glBindTexture(0, texturesSnakeGL[res_id]);
		#endif
		#ifdef WIN32
		glBindTexture(GL_TEXTURE_2D, texturesSnakeGL[res_id]);
		#endif
		glTranslatef(p.x, p.y, p.z);
        glut2SolidCube(size);
	#ifdef ARM9
    glPopMatrix(1);
	#endif
	#ifdef WIN32
    glPopMatrix();
	#endif
    disable_2D_texture();
}

//This will be replaced by a NintendoDS display list sphere later because ARM9 cpu sucks for building more than 3 spheres realtime
void drawSphere(){
    #ifdef WIN32
	glCallList(DLSPHERE);
    #endif
	#ifdef ARM9
    // Execute the display list
    glCallListGX((u32*)&Sphere008);
	#endif
}

void load_resources()
{
	#ifdef WIN32
    glGenTextures(TEXTURE_COUNT, texturesSnakeGL);

    glBindTexture(GL_TEXTURE_2D, texturesSnakeGL[GROUND_TEXTURE]);
    load_image("../src/resources/grass.png");

    glBindTexture(GL_TEXTURE_2D, texturesSnakeGL[FOOD_TEXTURE]);
    load_image("../src/resources/apple.png");

    glBindTexture(GL_TEXTURE_2D, texturesSnakeGL[BARRIER_TEXTURE]);
    load_image("../src/resources/box.png");

    glBindTexture(GL_TEXTURE_2D, texturesSnakeGL[SNAKE_TEXTURE]);
    load_image("../src/resources/snake.png");

    glBindTexture(GL_TEXTURE_2D, texturesSnakeGL[MENU_TEXTURE]);
    load_image("../src/resources/menu.png");

    glBindTexture(GL_TEXTURE_2D, texturesSnakeGL[BRICK_TEXTURE]);
    load_image("../src/resources/brick.png");
	#endif
}

void unload_resources()
{
    #ifdef WIN32
	glDeleteTextures(TEXTURE_COUNT, texturesSnakeGL);
	#endif
}

//Todo DS: use WoopsiSDK instead
void draw_text(string s, Point p, float r, float g, float b)
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

//hardcoded below function at 0.5f to speedup drawing
void glut2SolidCube05f(){
	#ifdef ARM9
    //Apply GX polygon properties (lights) before rendering vertices (snake body object)
	#define GX_LIGHT0 (1 << 0)
	glPolyFmt(GX_LIGHT0 | POLY_ALPHA(31) | POLY_CULL_NONE);
	#endif
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
			#ifdef ARM9
        	//Apply GX polygon properties (lights) before rendering vertices (snake head object)
			#define GX_LIGHT0 (1 << 0)
			glPolyFmt(GX_LIGHT0 | POLY_ALPHA(31) | POLY_CULL_NONE);
			#endif
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

