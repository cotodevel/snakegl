#include "Scene.h"

#ifdef ARM9
#include "debugNocash.h"
#include "timerTGDS.h"
#include "Sphere008.h"
#endif

//
// Snake implementation
//
void SnakeInit(struct DequeObject * snakeInst)
{
}

void resetSnake(struct DequeObject * snakeInst)
{
	struct Point p;
    int d = (rand() % 4) + 1;
	SnakeInit(snakeInst);
	clear(snakeInst);
    set_directionSnake(snakeInst, d);

    p.x = 0.0f;
    p.y = GROUND_DIFF;
    p.z = 0.0f;

	pushFront((struct Point*)&snakeInst->points, p, &snakeInst->front, &snakeInst->rear);
    grow(snakeInst, false); //default: bool back = false
}

void move(struct DequeObject * snakeInst)
{
	popBack((struct Point*)&snakeInst->points, &snakeInst->front, &snakeInst->rear);
    grow(snakeInst, false); //default: bool back = false
}

void set_directionSnake(struct DequeObject * snakeInst, int d)
{
    if ((d == DOWN && snakeInst->direction == UP) ||
        (snakeInst->direction == DOWN && d == UP) ||
        (d == LEFT && snakeInst->direction == RIGHT) ||
        (snakeInst->direction == LEFT && d == RIGHT))
    {
        return;
    }

    snakeInst->direction = d;
}

bool has_collisionSnake(struct DequeObject * snakeInst, struct Point p)
{
	size_t i = 0;
    // Skip head. It's the same point.
    for (i = 1; i < count((struct Point *)&snakeInst->points); ++i)
    {
        struct Point b = snakeInst->points[i];

        if (p.x == b.x && p.z == b.z)
        {
            return true;
        }
    }

    return false;
}

void draw(struct DequeObject * snakeInst)
{
    // TODO: Draw cylindric snake.
    // It's more hard.
    // Use glut2Cylinder. 
    struct Point h = snakeInst->points[0];

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
	
	{
		size_t i = 0;
		for (i = 1; i < count((struct Point *)&snakeInst->points); ++i)
		{
			struct Point p = snakeInst->points[i];

			glPushMatrix();
				glTranslatef(p.x, p.y, p.z);
				glut2SolidCube05f();
			glPopMatrix(
				#ifdef ARM9
				1
				#endif
			);
		}
	}

    disable_2D_texture();
}

struct Point head(struct DequeObject * snakeInst)
{
    return snakeInst->points[0];
}

struct Point tail(struct DequeObject * snakeInst)
{
    return snakeInst->points[count((struct Point *)&snakeInst->points) - 1];
}

void grow(struct DequeObject * snakeInst, bool back) //default: bool back = false
{
    struct Point p;
    p.x = snakeInst->points[0].x;
    p.y = snakeInst->points[0].y;
    p.z = snakeInst->points[0].z;

    switch (snakeInst->direction)
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
		pushBack((struct Point*)&snakeInst->points, p, &snakeInst->front, &snakeInst->rear);
    }
    else
    {
		pushFront((struct Point*)&snakeInst->points, p, &snakeInst->front, &snakeInst->rear);
    }
}

int size(struct DequeObject * snakeInst)
{
    return count((struct Point *)&snakeInst->points);
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

struct Point random_point()
{
    struct Point p;
    p.x = random_pos();
    p.y = GROUND_DIFF;
    p.z = random_pos();

    return p;
}

void draw_cube(float size, struct Point p, int res_id)
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
void draw_text(char * s, struct Point p, float r, float g, float b)
{
    int len, i;
    glDisable(GL_LIGHTING);

	#ifdef WIN32
    glColor3f(r, g, b);
    glRasterPos3f(p.x, p.y,p.z);
    len = (int) strlen(s) + 1;

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

  if (*pfront == 0 && *prear == MAX_DEQUE_OBJECTS - 1) {
    return;
  }

  if (*pfront == -1) {
    *pfront = *prear = 0;
    arr[*pfront] = item;
    return;
  }

  if (*prear != MAX_DEQUE_OBJECTS - 1) {
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

  if (*pfront == 0 && *prear == MAX_DEQUE_OBJECTS - 1) {
    return;
  }

  if (*pfront == -1) {
    *prear = *pfront = 0;
    arr[*prear] = item;
    return;
  }

  if (*prear == MAX_DEQUE_OBJECTS - 1) {
    k = *pfront - 1;
    for (i = *pfront - 1; i < *prear; i++) {
      k = i;
      if (k == MAX_DEQUE_OBJECTS - 1)
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

  for (i = 0; i < MAX_DEQUE_OBJECTS; i++) {
	  if (arr[i].index != INVALID_DEQUE_ENTRY)
      c++;
  }
  return c;
}

void clear(struct DequeObject * inst){
	int c = 0, i;
	memset((u8*)&inst->points, 0, sizeof(inst->points));
	inst->front = inst->rear = -1;
	for (i = 0; i < MAX_DEQUE_OBJECTS; i++) {
		struct Point * ref = &inst->points[i];
		ref->index = INVALID_DEQUE_ENTRY;
		ref->x = 0;
		ref->y = 0;
		ref->z = 0;
	}
}