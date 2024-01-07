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
	clear(this);
    int d = (rand() % 4) + 1;
    set_direction(d);

    struct Point p;
    p.x = 0.0f;
    p.y = GROUND_DIFF;
    p.z = 0.0f;

	pushFront((struct Point*)&points, p, &front, &rear);
    grow();
}

void Snake::move()
{
	popBack((struct Point*)&points, &front, &rear);
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
	
	for (size_t i = 1; i < count((struct Point *)&points); ++i)
    {
        Point p = points[i];

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
    return points[count((struct Point *)&points) - 1];
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
		pushBack((struct Point*)&points, p, &front, &rear);
    }
    else
    {
		pushFront((struct Point*)&points, p, &front, &rear);
    }
}

bool Snake::has_collision(Point p)
{
    // Skip head. It's the same point.
    for (size_t i = 1; i < count((struct Point *)&points); ++i)
    {
        Point b = points[i];

        if (p.x == b.x && p.z == b.z)
        {
            return true;
        }
    }

    return false;
}

int Snake::size()
{
    return count((struct Point *)&points);
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

/*
int main() {
  int arr[MAX];
  int front, rear, i, n;

  front = rear = -1;
  for (i = 0; i < MAX; i++)
    arr[i] = 0;

  pushRear(arr, 5, &front, &rear);
  pushFront(arr, 12, &front, &rear);
  pushRear(arr, 11, &front, &rear);
  pushFront(arr, 5, &front, &rear);
  pushRear(arr, 6, &front, &rear);
  pushFront(arr, 8, &front, &rear);

  printf("\nElements in a deque: ");
  display(arr);

  i = delFront(arr, &front, &rear);
  printf("\nremoved item: %d", i);

  printf("\nElements in a deque after deletion: ");
  display(arr);

  pushRear(arr, 16, &front, &rear);
  pushRear(arr, 7, &front, &rear);

  printf("\nElements in a deque after addition: ");
  display(arr);

  i = delRear(arr, &front, &rear);
  printf("\nremoved item: %d", i);

  printf("\nElements in a deque after deletion: ");
  display(arr);

  n = count(arr);
  printf("\nTotal number of elements in deque: %d", n);
}
*/

//Deque implementation in C
void pushFront(struct Point *arr, struct Point item, int *pfront, int *prear) {
  int i, k, c;

  if (*pfront == 0 && *prear == MAX - 1) {
    return;
  }

  if (*pfront == -1) {
    *pfront = *prear = 0;
    arr[*pfront] = item;
    return;
  }

  if (*prear != MAX - 1) {
    c = count(arr);
    k = *prear + 1;
    for (i = 1; i <= c; i++) {
      arr[k] = arr[k - 1];
      k--;
    }
    arr[k] = item;
    *pfront = k;
    (*prear)++;
  } else {
    (*pfront)--;
    arr[*pfront] = item;
  }
}

void pushBack(struct Point *arr, struct Point item, int *pfront, int *prear) {
  int i, k;

  if (*pfront == 0 && *prear == MAX - 1) {
    return;
  }

  if (*pfront == -1) {
    *prear = *pfront = 0;
    arr[*prear] = item;
    return;
  }

  if (*prear == MAX - 1) {
    k = *pfront - 1;
    for (i = *pfront - 1; i < *prear; i++) {
      k = i;
      if (k == MAX - 1)
		  arr[k].index = INVALID_DEQUE_ENTRY;
      else
        arr[k] = arr[i + 1];
    }
    (*prear)--;
    (*pfront)--;
  }
  (*prear)++;
  arr[*prear] = item;
}

//if ret == INVALID_DEQUE_ENTRY, failed to delete front record
struct Point popFront(struct Point *arr, int *pfront, int *prear) {
  struct Point item;

  if (*pfront == -1) {
	  item.index = INVALID_DEQUE_ENTRY;
	 return item;
  }

  item = arr[*pfront];
  arr[*pfront].index = INVALID_DEQUE_ENTRY;

  if (*pfront == *prear)
    *pfront = *prear = -1;
  else
    (*pfront)++;

  return item;
}

//if ret == INVALID_DEQUE_ENTRY, failed to delete rear record
struct Point popBack(struct Point *arr, int *pfront, int *prear) {
  struct Point item;

  if (*pfront == -1) {
     item.index = INVALID_DEQUE_ENTRY;
	 return item;
  }

  item = arr[*prear];
  arr[*prear].index = INVALID_DEQUE_ENTRY;
  (*prear)--;
  if (*prear == -1)
    *pfront = -1;
  return item;
}

int count(struct Point *arr) {
  int c = 0, i;

  for (i = 0; i < MAX; i++) {
	  if (arr[i].index != INVALID_DEQUE_ENTRY)
      c++;
  }
  return c;
}

void clear(class Snake * inst){
	int c = 0, i;
	memset((u8*)&inst->points, 0, sizeof(inst->points));
	inst->front = inst->rear = -1;
	for (i = 0; i < MAX; i++) {
		struct Point * ref = &inst->points[i];
		ref->index = INVALID_DEQUE_ENTRY;
		ref->x = 0;
		ref->y = 0;
		ref->z = 0;
	}
}