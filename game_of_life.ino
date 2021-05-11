
#include "myMATRIX.h"

//Define the matrix pins
#define RowA_Pin 2
#define RowB_Pin 3
#define RowC_Pin 4
#define RowD_Pin 5
#define OE_Pin 6
#define Red_Pin 7
#define Green_Pin 8
#define CLK_Pin 9
#define STB_Pin 10

//Define the world (pixels) dimensions
#define worldWidth 32
#define worldHeight 16

//World buffer
boolean world[worldWidth][worldHeight];

//Previous world buffer, used to determine if the previous world is as the new one. 
//If both world are the same, initializeWorld function is called to reset the world
//This way, the fun never ends...
boolean lastWorld[worldWidth][worldHeight];

int counter = 0;

void setup ()
{
  //Serial to debug options
  Serial.begin(115200);

  //Init the matrix
  myMatrix.Init(Red_Pin, Green_Pin, CLK_Pin, RowA_Pin, RowB_Pin, RowC_Pin, RowD_Pin, OE_Pin, STB_Pin);
  myMatrix.clearScreen();
  //myMatrix.setPixel(10, 10, red);
  initializeWorld();
  //initGlider();
}

void initializeWorld() {
  //Initialize the world on a random state
  randomSeed(analogRead(5));
  for (int i = 0; i < worldWidth; i++) {
    for (int j = 0; j < worldHeight; j++) {
      world[i][j] = random(0, 2) == 1;
    }
  }
}

//Function to test the glider
void initGlider() {
  int centerX = 15;
  int centerY = 8;

  world[centerX][centerY] = true;
  world[centerX + 1][centerY] = true;
  world[centerX + 2][centerY] = true;
  world[centerX + 2][centerY - 1] = true;
  world[centerX + 1][centerY - 2] = true;

}

//Get the neibhbors of a given cell
int retrieveNeighbors(int x, int y) {
  int count = 0;
  int nx, ny;
  for (int i = -1; i < 2; i++) {
    for (int j = -1; j < 2; j++) {
      if (i != 0 || j != 0) {
        nx = x + i;
        ny = y + j;

        if (nx < 0) {
          nx = worldWidth - 1;
        } else if(nx >= worldWidth) {
          nx = 0;
        }

        if (ny < 0) {
          ny = worldHeight - 1;
        } else if(ny >= worldHeight) {
          ny = 0;
        }

        if (world[nx][ny]) {
          count++;
        }
      }
    }
  }
  
  return count;
}

//Calculate the next state using the world buffers
void calculateNewState() {
  boolean next[worldWidth][worldHeight];
  for (int i = 0; i < worldWidth; i++) {
    for (int j = 0; j < worldHeight; j++) {
      next[i][j] = false;
    }
  }
  for (int r = 0; r < worldWidth; r++) { // for each row
    for (int c = 0; c < worldHeight; c++) { // and each column
      // count how many live neighbors this cell has
      int liveNeighbors = retrieveNeighbors(r, c);
      if (world[r][c] == 1 && liveNeighbors >= 2 && liveNeighbors <= 3) {
        next[r][c] = true;
      } else if (world[r][c] == 0 && liveNeighbors == 3) {
        next[r][c] = true;
      } else {
        next[r][c] = false;
      }
    }
  }

  boolean mismatch = false;
  for (int i = 0; i < worldWidth; i++) {
    for (int j = 0; j < worldHeight; j++) {
      if (!mismatch) {
        if (lastWorld[i][j] != next[i][j]) {
            mismatch = true;
        }
      }
    }
  }

  if (mismatch) {
    for (int i = 0; i < worldWidth; i++) {
      for (int j = 0; j < worldHeight; j++) {
        lastWorld[i][j] = world[i][j];
      }
    }
    for (int i = 0; i < worldWidth; i++) {
      for (int j = 0; j < worldHeight; j++) {
        world[i][j] = next[i][j];
      }
    }
  } else {
    initializeWorld();
  }
  
}

//Display the current world
void drawState() {
  for (int i = 0; i < worldWidth; i++) {
    for (int j = 0; j < worldHeight; j++) {
      boolean cell = world[i][j];
      myMatrix.setPixel(i, j, cell ? red : black);
    }
  }
}

//Main loop
void loop()
{
  drawState();
  delay(50);
  calculateNewState();

}
