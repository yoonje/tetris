#include <iostream>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <termios.h>
#include <cmath>

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include "Matrix_e.h"

using namespace std;

char getch();
/**************************************************************/
/**************** Linux System Functions **********************/
/**************************************************************/

pthread_t mainThread;
pthread_mutex_t con_mutex = PTHREAD_MUTEX_INITIALIZER;
char saved_key = 0;
int tty_raw(int fd);	/* put terminal into a raw mode */
int tty_reset(int fd);	/* restore terminal's mode */


void sigalrm_handler(int signo) {
  alarm(1);
  saved_key = 's';
}

void registerAlarm() {
  struct sigaction act, oact;
  act.sa_handler = sigalrm_handler;
  sigemptyset(&act.sa_mask);
#ifdef SA_INTERRUPT
  act.sa_flags = SA_INTERRUPT;
#else
  act.sa_flags = 0;
#endif
  if (sigaction(SIGALRM, &act, &oact) < 0) {
    cerr << "sigaction error" << endl;
    exit(1);
  }
  alarm(1);
}

/**************************************************************/
/******************** Tetris Main Loop ************************/
/**************************************************************/

int arrayBlk[] = { 0, 0, 0, 0,
		   1, 1, 1, 1,
		   0, 0, 0, 0,
		   0, 0, 0, 0, -1 };

int iScreenDy = 15;
int iScreenDx = 10;
int iScreenDw = 4;

int* createArrayScreen(int dy, int dx, int dw) {
  int y, x;
  int *array = new int[(dy + dw)*(dx + 2 * dw)];

  for (int y = 0; y < (dy + dw)*(dx + 2 * dw); y++)
    array[y] = 0;

  for (y = 0; y < dy + dw; y++)
    for (x = 0; x < dw; x++)
      array[y * (dx + 2 * dw) + x] = 1;

  for (y = 0; y < dy + dw; y++)
    for (x = dw + dx; x < 2 * dw + dx; x++)
      array[y * (dx + 2 * dw) + x] = 1;

  for (y = dy; y < dy + dw; y++)
    for (x = 0; x < 2 * dw + dx; x++)
      array[y * (dx + 2 * dw) + x] = 1;

  return array;
}

void drawMatrix(Matrix *m) {
  int dy = m->get_dy();
  int dx = m->get_dx();
  int **array = m->get_array();
  for (int y = 0; y < dy - iScreenDw + 1; y++) {
    for (int x = iScreenDw - 1; x < dx - iScreenDw + 1; x++) {
      if (array[y][x] == 0) cout << "□ ";
      else if (array[y][x] == 1) cout << "■ ";
      else cout << array[y][x];//"X ";
    }
    cout << endl;
  }
}

/* Read 1 character - echo defines echo mode */
char getch() {
  char ch;
  int n;
  while (1) {
    pthread_mutex_lock(&con_mutex);
    tty_raw(0);
    n = read(0, &ch, 1);
    tty_reset(0);
    pthread_mutex_unlock(&con_mutex);
    if (n > 0)
      break;
    else if (n < 0) {
      if (errno == EINTR) {
	if (saved_key != 0) {
	  ch = saved_key;
	  saved_key = 0;
	  break;
	}
      }
    }
  }
  return ch;
}

/**************************************************************/
/**************** Tetris Blocks Definitions *******************/
/**************************************************************/
#define MAX_BLK_TYPES 7
#define MAX_BLK_DEGREES 4

int T0D0[] = { 1, 1, 1, 1, -1 };
int T0D1[] = { 1, 1, 1, 1, -1 };
int T0D2[] = { 1, 1, 1, 1, -1 };
int T0D3[] = { 1, 1, 1, 1, -1 };

int T1D0[] = { 0, 1, 0, 1, 1, 1, 0, 0, 0, -1 };
int T1D1[] = { 0, 1, 0, 0, 1, 1, 0, 1, 0, -1 };
int T1D2[] = { 0, 0, 0, 1, 1, 1, 0, 1, 0, -1 };
int T1D3[] = { 0, 1, 0, 1, 1, 0, 0, 1, 0, -1 };

int T2D0[] = { 1, 0, 0, 1, 1, 1, 0, 0, 0, -1 };
int T2D1[] = { 0, 1, 1, 0, 1, 0, 0, 1, 0, -1 };
int T2D2[] = { 0, 0, 0, 1, 1, 1, 0, 0, 1, -1 };
int T2D3[] = { 0, 1, 0, 0, 1, 0, 1, 1, 0, -1 };

int T3D0[] = { 0, 0, 1, 1, 1, 1, 0, 0, 0, -1 };
int T3D1[] = { 0, 1, 0, 0, 1, 0, 0, 1, 1, -1 };
int T3D2[] = { 0, 0, 0, 1, 1, 1, 1, 0, 0, -1 };
int T3D3[] = { 1, 1, 0, 0, 1, 0, 0, 1, 0, -1 };

int T4D0[] = { 0, 1, 0, 1, 1, 0, 1, 0, 0, -1 };
int T4D1[] = { 1, 1, 0, 0, 1, 1, 0, 0, 0, -1 };
int T4D2[] = { 0, 1, 0, 1, 1, 0, 1, 0, 0, -1 };
int T4D3[] = { 1, 1, 0, 0, 1, 1, 0, 0, 0, -1 };

int T5D0[] = { 0, 1, 0, 0, 1, 1, 0, 0, 1, -1 };
int T5D1[] = { 0, 0, 0, 0, 1, 1, 1, 1, 0, -1 };
int T5D2[] = { 0, 1, 0, 0, 1, 1, 0, 0, 1, -1 };
int T5D3[] = { 0, 0, 0, 0, 1, 1, 1, 1, 0, -1 };

int T6D0[] = { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, -1 };
int T6D1[] = { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1 };
int T6D2[] = { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, -1 };
int T6D3[] = { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1 };

int *setOfBlockArrays[] = {
  T0D0, T0D1, T0D2, T0D3,
  T1D0, T1D1, T1D2, T1D3,
  T2D0, T2D1, T2D2, T2D3,
  T3D0, T3D1, T3D2, T3D3,
  T4D0, T4D1, T4D2, T4D3,
  T5D0, T5D1, T5D2, T5D3,
  T6D0, T6D1, T6D2, T6D3,
};

#define HOMEWORK_ANSWER 1
#ifdef HOMEWORK_ANSWER
Matrix ***setOfBlockObjects;
int find_matrix_size(int *array) {
  int len, size;
  
  for (len = 0; array[len] != -1; len++);

  for (size = 0; ; size++) {
    if (size * size > len) break;
  }

  return (size-1);
}

void initBlockObjects(int *set_of_arrays[]) {
  int size;
  setOfBlockObjects = new Matrix**[MAX_BLK_TYPES];
  for (int t = 0; t < MAX_BLK_TYPES; t++) {
    setOfBlockObjects[t] = new Matrix*[MAX_BLK_DEGREES];
  }

  for (int t = 0; t < MAX_BLK_TYPES; t++) {
    for (int d = 0; d < MAX_BLK_DEGREES; d++) {
      size = find_matrix_size(set_of_arrays[t*MAX_BLK_DEGREES+d]);
      //cout << size << endl;
      setOfBlockObjects[t][d] = new Matrix(set_of_arrays[t*MAX_BLK_DEGREES+d], size, size);
    }
  }
}

Matrix *deleteFullLines(Matrix *screen, Matrix *blk, int top,
			int dy, int dx, int dw) {
  Matrix *line, *zero, *temp;
  if (blk == NULL) // called right after the game starts. 
    return screen; // no lines to be deleted
  int cy, y, nDeleted = 0, nScanned = blk->get_dy();
  if (top + blk->get_dy() - 1 >= dy)
    nScanned -= (top + blk->get_dy() - dy);
  zero = new Matrix(1, dx - 2*dw);
  for (y = nScanned - 1; y >= 0; y--) {
    cy = top + y + nDeleted;
    line = screen->clip(cy, 0, cy + 1, screen->get_dx());
    if (line->sum() == screen->get_dx()) {
      temp = screen->clip(0, 0, cy, screen->get_dx());
      screen->paste(temp, 1, 0);
      screen->paste(zero, 0, dw);
      nDeleted++;
      delete temp;
    }
    delete line;
  }
  delete zero;
  return screen;
}
#endif

int main(int argc, char *argv[]) {
  bool newBlockNeeded = false;
  int top = 0;
  int left = iScreenDw + iScreenDx/2 - iScreenDw/2;
  int* arrayScreen = createArrayScreen(iScreenDy, iScreenDx, iScreenDw);
  char key;

  //registerAlarm(); // enable a one-second timer

  Matrix *iScreen = new Matrix(arrayScreen, iScreenDy + iScreenDw, iScreenDx + 2 * iScreenDw);
#ifdef HOMEWORK_ANSWER
  initBlockObjects(setOfBlockArrays);
  srand((unsigned int) time(NULL));
  int t = rand() % MAX_BLK_TYPES;
  int d = 0;
  Matrix *currBlk = setOfBlockObjects[t][d];
#else
  Matrix *currBlk = new Matrix(arrayBlk, 4, 4);
#endif
  Matrix *tempBlk = iScreen->clip(top, left, top + currBlk->get_dy(), left + currBlk->get_dx());
  Matrix *tempBlk2 = tempBlk->add(currBlk);
  Matrix *oScreen = new Matrix(iScreen);

  oScreen->paste(tempBlk2, top, left);
  drawMatrix(oScreen); cout << endl;

  while ((key = getch()) != 'q') {
    switch (key) {
    case 'a': left--; break; // move left
    case 'd': left++; break; // move right
    case 's': top++; break; // move down
    case 'w':
#ifdef HOMEWORK_ANSWER
      d = (d + 1) % MAX_BLK_DEGREES;
      currBlk = setOfBlockObjects[t][d];
#endif
      break; // rotate the block clockwise
    case ' ':
#ifdef HOMEWORK_ANSWER
      do {
	top++;
	delete tempBlk;
	tempBlk = iScreen->clip(top, left, top + currBlk->get_dy(), left + currBlk->get_dx());
	delete tempBlk2;
	tempBlk2 = tempBlk->add(currBlk);
      } while (!tempBlk2->anyGreaterThan(1));
#endif
      break; // drop the block
    default: cout << "unknown key!" << endl;
    }
    delete tempBlk;
    tempBlk = iScreen->clip(top, left, top + currBlk->get_dy(), left + currBlk->get_dx());
    delete tempBlk2;
    tempBlk2 = tempBlk->add(currBlk);
    if (tempBlk2->anyGreaterThan(1)) {
      switch (key) {
      case 'a': left++; break; // undo: move right
      case 'd': left--; break; // undo: move left
      case 's': top--; newBlockNeeded = true; break; // undo: move up
      case 'w':
#ifdef HOMEWORK_ANSWER
	d = (d + MAX_BLK_DEGREES - 1) % MAX_BLK_DEGREES;
	currBlk = setOfBlockObjects[t][d];
#endif
	break; // undo: rotate the block counter-clockwise
      case ' ':
#ifdef HOMEWORK_ANSWER
	top--; newBlockNeeded = true;
#endif
	break; // undo: move up
      }
      delete tempBlk;
      tempBlk = iScreen->clip(top, left, top + currBlk->get_dy(), left + currBlk->get_dx());
      delete tempBlk2;
      tempBlk2 = tempBlk->add(currBlk);
    }
    //delete oScreen;    oScreen = new Matrix(iScreen);
    oScreen->paste(iScreen, 0, 0);
    oScreen->paste(tempBlk2, top, left);
    drawMatrix(oScreen); cout << endl;
    if (newBlockNeeded) {
      //delete iScreen;      iScreen = new Matrix(oScreen);
#ifdef HOMEWORK_ANSWER
      oScreen = deleteFullLines(oScreen, currBlk, top, iScreenDy, iScreenDx, iScreenDw);
#endif
      iScreen->paste(oScreen, 0, 0);
      top = 0; left = iScreenDw + iScreenDx/2 - iScreenDw/2;
      newBlockNeeded = false;
#ifdef HOMEWORK_ANSWER
      t = rand() % MAX_BLK_TYPES;
      d = 0;
      currBlk = setOfBlockObjects[t][d];
      // DO NOT DELETE currblk!! setOfBlockObjects[t][d] pointed to by currblk should not be freed until the game is over.
#else
      delete currBlk;
      currBlk = new Matrix(arrayBlk, 4, 4);
#endif
      delete tempBlk;
      tempBlk = iScreen->clip(top, left, top + currBlk->get_dy(), left + currBlk->get_dx());
      delete tempBlk2;
      tempBlk2 = tempBlk->add(currBlk);

      if (tempBlk2->anyGreaterThan(1)) {
	cout << "Game Over!" << endl;
	exit(0);
      }
      //delete oScreen;      oScreen = new Matrix(iScreen);
      oScreen->paste(iScreen, 0, 0);
      oScreen->paste(tempBlk2, top, left);
      drawMatrix(oScreen); cout << endl;
    }
  }

  exit(0);
}
