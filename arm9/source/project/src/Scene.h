#ifndef SCENE_H
#define SCENE_H

#include "snake.h"

struct Camera{
	GLfloat eyeX, /* gluLookAt */
             eyeY,
             eyeZ,
             centerX,
             centerY,
             centerZ,
             upX,
             upY,
             upZ,
             /* gluPerspective  */
             fovy,
             aspect,
             zNear,
             zFar;

};


#ifdef __cplusplus

class Scene
{
public:
	bool fogMode;
	bool light0On;
	bool light1On;
	bool wireMode;	/// wireframe mode on / off
	bool flatShading;	/// flat shading on / off
	bool fullScreen;

	//game specific
    Point food;
    std::vector<struct Point> barriers;
    Camera camera;
    void draw_food();
    void draw_barrier();
    void draw_background();
	#ifdef WIN32
    int camera_mode;
    #endif
	#ifdef ARM9
	bool close_camera_mode;
	#endif
	float m;
    float a;
    Snake snake;
    Scene(int argc, char *argv[]);
    void add_barrier();
    void change_food_pos();
    void draw_board();
    void draw_objects();
    void change_camera_pos();
    void set_camera();
    void reset();
    ObjectType has_collision(Point p);
};
#endif

#endif

#ifdef __cplusplus
extern "C"{
#endif

// light 0 colours
extern GLfloat light_ambient0Scene[4];
extern GLfloat light_diffuse0Scene[4];
extern GLfloat light_specular0Scene[4];
extern GLfloat light_position0Scene[4];

// light 1 colours
extern GLfloat light_ambient1Scene[4];
extern GLfloat light_diffuse1Scene[4];
extern GLfloat light_specular1Scene[4];
extern GLfloat light_position1Scene[4];

extern int widthScene;	/// the width of the window
extern int heightScene;	/// the height of the window

extern void animateScene(int type);	/// animates the scene (GLUT)
extern void drawScene(void);	/// Renders a single frame of the scene
extern int TWLPrintf(const char *fmt, ...);

extern GLint DLEN2DTEX;
extern GLint DLDIS2DTEX;
extern GLint DLSOLIDCUBE05F;
extern void enable_2D_texture();
extern void disable_2D_texture();
extern void glut2SolidCube05f();
extern int InitGL(int argc, char *argv[]); /// initialises OpenGL
extern void setupTGDSProjectOpenGLDisplayLists();

extern void render3DUpperScreen();
extern void render3DBottomScreen();
extern int startTGDSProject(int argc, char *argv[]);
extern void TGDSAPPExit(u32 fn_address);
#ifdef WIN32
extern GLuint texturesSnakeGL[TEXTURE_COUNT+1];
#endif
extern void load_resources();
extern void unload_resources();
extern void setVSync(bool sync);
extern int width, height;
extern bool NDSDual3DCameraFlag;
extern bool TGDSProjectDual3DEnabled;

#if defined(ARM9)
extern void enableDual3DTGDS();
extern void disableDual3DTGDS();
extern bool getDual3DTGDSStatus();
#endif

#ifdef __cplusplus
}
#endif
