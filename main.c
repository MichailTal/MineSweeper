#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "include/raylib.h"
#include "include/raymath.h"


#define COLS 10
#define ROWS 10

const int screenWidth = 400;
const int screenHeight = 400;

const int cellWidth = screenWidth / COLS;
const int cellHeight = screenWidth / ROWS;

typedef struct Cell {
  int i;
  int j;
  bool containsMine;
  bool revealed;
  int nearbyMines;
  bool flagged;

} Cell;

Cell grid[COLS][ROWS];

Texture2D flagSprite;

void CellDraw(Cell);
bool IndexIsValid(int, int);
void CellReveal(int, int);
void CellFlag(int, int);
int CellCountMines(int, int);
void GridInit(void);
void GridFloodClearFrom(int , int);

int main() {
  srand(time(0));

  InitWindow(screenWidth, screenHeight, "MineSweeper");

  flagSprite = LoadTexture("resources/flag.png");

  GridInit();

  while (!WindowShouldClose()) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      Vector2 mPos = GetMousePosition();
      int indexI = mPos.x / cellWidth;
      int indexJ = mPos.y / cellHeight;

      if (IndexIsValid(indexI, indexJ)) {
        CellReveal(indexI, indexJ);
      }
    }
    else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
      Vector2 mPos = GetMousePosition();
      int indexI = mPos.x / cellWidth;
      int indexJ = mPos.y / cellHeight;

      if (IndexIsValid(indexI, indexJ)) {
        CellFlag(indexI, indexJ);
      }
    }

    BeginDrawing();

    ClearBackground(RAYWHITE);

    for (int i = 0; i < COLS; i++) {
      for (int j = 0; j < ROWS; j++) {
        CellDraw(grid[i][j]);
      }
    }

    EndDrawing();
  }

  CloseWindow();

  return 0;
}

void GridInit(void) {
	
  for (int i = 0; i < COLS; i++) {
    for (int j = 0; j < ROWS; j++) {
      grid[i][j] = (Cell){
          .i = i,
          .j = j,
          .containsMine = false,
          .revealed = false,
          .nearbyMines = -1,
          .flagged = false,

      };
    }
  }

  int minesToPlace = (int)(ROWS * COLS * 0.1f);
  while (minesToPlace > 0) {
    int i = rand() % COLS;
    int j = rand() % ROWS;

    if (!grid[i][j].containsMine) {
      grid[i][j].containsMine = true;
      minesToPlace--;
    }
  }

  for (int i = 0; i < COLS; i++) {
    for (int j = 0; j < ROWS; j++) {
      if (!grid[i][j].containsMine) {
        grid[i][j].nearbyMines = CellCountMines(i, j);
      }
    }
  }
}

void CellDraw(Cell cell) {
  if (cell.revealed) {
    if (cell.containsMine) {
      DrawRectangle(cell.i * cellWidth, cell.j * cellHeight, cellWidth,
                    cellHeight, RED);
    } else {
      DrawRectangle(cell.i * cellWidth, cell.j * cellHeight, cellWidth,
                    cellHeight, LIGHTGRAY);
      if (cell.nearbyMines > 0) {
        DrawText(TextFormat("%d", cell.nearbyMines), cell.i * cellWidth + 12,
                 cell.j * cellHeight + 4, cellHeight - 10, DARKGRAY);
      }
    }
  }
  else if (cell.flagged)
  {
    Rectangle source = {0, 0, flagSprite.width, flagSprite.height};
    Rectangle dest = {cell.i * cellWidth, cell.j * cellHeight, cellWidth, cellHeight};
    Vector2 origin = {0, 0};

    DrawTexturePro(flagSprite, source, dest, origin ,0.0f, Fade(WHITE, 1.0f));
  }

  DrawRectangleLines(cell.i * cellWidth, cell.j * cellHeight, cellWidth,
                     cellHeight, BLACK);
}

bool IndexIsValid(int i, int j) {
  return i >= 0 && i < COLS && j >= 0 && j < ROWS;
}

int CellCountMines(int i, int j) {
  int count = 0;
  for (int iOff = -1; iOff <= 1; iOff++) {
    for (int jOff = -1; jOff <= 1; jOff++) {
      if (iOff == 0 && jOff == 0) {
        continue;
      }

      if (!IndexIsValid(i + iOff, j + jOff)) {
        continue;
      }

      if (grid[i + iOff][j + jOff].containsMine) {
        count++;
      }
    }
  }

  return count;
}

void CellReveal(int i, int j) {

  if (grid[i][j].flagged)
  {
    return;
  }  

  grid[i][j].revealed = true;

  if (grid[i][j].containsMine) {
    // lose!
  } else {
    // play sound
	
	if (grid[i][j].nearbyMines == 0) {

		GridFloodClearFrom(i ,j);
	}
  }
}

void CellFlag(int i, int j)
{
  if (grid[i][j].revealed)
  {
    return;
  }  
  grid[i][j].flagged = !grid[i][j].flagged;
}


void GridFloodClearFrom(int i, int j)
{
	
    for (int iOff = -1; iOff <= 1; iOff++) {
		for (int jOff = -1; jOff <= 1; jOff++) {
		  if (iOff == 0 && jOff == 0) {
			continue;
		  }

		  if (!IndexIsValid(i + iOff, j + jOff)) {
			continue;
		  }

		  if (!grid[i + iOff][j + jOff].revealed) {
			  CellReveal(i + iOff, j + jOff);
		  }
		}
	}
}
