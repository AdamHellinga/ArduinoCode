#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <FastLED.h>
#include <stdio.h>
#include <stdlib.h>

#define DATA_PIN 3
#define MODEL PL9823
#define COLOR RGB
#define NUM_LEDS 216
#define BRIGHTNESS 20
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105


RF24 radio(7, 8);  // CE, CSN

//shapes
struct Shape {
  int rotations[4];
  int x;
  int y;
};

//struct
struct Ball
{
  int scored;
  int xPos;
  int yPos;
  int moveDir;
  int justBounced;
  double XPOS;
  double YPOS;
  double angle;
};

// Arcade vars
uint8_t game = 0;
bool gameSel = true;
bool gameWait = false;

//RF Var
const byte address[6] = "node1";

//Input string vars
char text[32] = { 0 };

// Parsing vars
uint8_t num = 0;
int val = 0;

//Controller Inputs
int LeftX = 0;
int LeftY = 0;
int RightX = 0;
int RightY = 0;
int pot1Val = 0;
int pot2Val = 0;
uint8_t BY = 0;
uint8_t BR = 0;
int buttons[8];
int pots[7];
int buttonTop = 0;
int buttonLeft = 0;
int buttonDown = 0;
int buttonRight = 0;
int bumperLeft = 0;
int bumperRight = 0;
int joyLeftButton = 0;
int joyRightButton = 0;
int leftX = 0;
int leftY = 0;
int rightX = 0;
int rightY = 0;
int potLeftVal = 0;
int potRightVal = 0;
int potVerticalVal = 0;

//Function frequencies
unsigned long move = 500;
unsigned long makeNewShape = 500;
unsigned long processCommands = 100;
unsigned long timeStamp1 = 0;
unsigned long timeStamp2 = 0;
unsigned long timeNow = 0;

// Snake vars
int countR = 0;
int countY = 0;
uint8_t snakeColor = 5;
int dirs[4] = {1, 1, -1, -1};
uint8_t dir = 0;
uint8_t north = 1;
uint8_t east = 2;
uint8_t south = 3;
uint8_t west = 4;
uint8_t snakeLength = 3;
uint8_t foodX = 0;
uint8_t foodY = 0;
uint8_t headX = 0;
uint8_t headY = 0;
bool lose = 0;
uint8_t snakeArray[100][2];
bool startSnake = false;

// Tetris vars
Shape stick;
Shape cube;
Shape table;
Shape el;
Shape mirrorEl;
Shape squiggle;
Shape mirrorSquiggle;
uint8_t curRot = 1;
uint8_t newShape = 1;
uint8_t temp = 0;
int tempInt = 0;
Shape tempShape;
int BRCount = 0;
int BYCount = 0;
uint8_t yCheck = 1;
int furthestY = 0;
int lowestX = 0;
int rowCount = 0;
int buttonWait = 0;
uint8_t endGame = 0;
int points = 0;
char score[10];
uint8_t yIndex = 0;
uint8_t xIndex = 0;

// Pong vars
uint8_t numPlayers = 1;
uint8_t up = 0;
uint8_t down = 0;
uint8_t left = 0;
uint8_t right = 0;
uint8_t ballX = 8;
uint8_t ballY = 5;
double BALLX = 8.0;
double BALLY = 5.0;
double stepVal = 1.0;
uint8_t ballYUP = 0;
uint8_t ballYDOWN = 0;
uint8_t ballXUP = 0;
uint8_t ballXDOWN = 0;
uint8_t YUPP = 0;
uint8_t YDOWNP = 0;
uint8_t XUPP = 0;
uint8_t XDOWNP = 0;
uint8_t ballXVelo = 0;
uint8_t ballYVelo = 0;
uint8_t ballAngle = 0;
uint8_t p1Y = 5;
uint8_t p2Y = 5;
uint8_t p1Dir = 0;
uint8_t p2Dir = 0;
uint8_t len = 1;
uint8_t p1Col = 0;
uint8_t p2Col = 0;
uint8_t colorIndex = 0;
long timeStamp = 0;
long ballStamp = 0;
int del = 200;
int del1 = 200;
uint8_t lastScored = 1;
uint8_t angleVariant = 60;
uint8_t start = 0;
uint8_t p1Score = 0;
uint8_t p2Score = 0;
uint8_t win = 0;
uint8_t botLeftX = 2;
uint8_t botLeftY = 4;
Ball ball;

CRGB leds[NUM_LEDS];

uint8_t ledMatrix[18][12] = {
  { 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 },
  { 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 },
  { 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24 },
  { 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47 },
  { 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48 },
  { 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71 },
  { 83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72 },
  { 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95 },
  { 107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96 },
  { 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119 },
  { 131, 130, 129, 128, 127, 126, 125, 124, 123, 122, 121, 120 },
  { 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143 },
  { 155, 154, 153, 152, 151, 150, 149, 148, 147, 146, 145, 144 },
  { 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167 },
  { 179, 178, 177, 176, 175, 174, 173, 172, 171, 170, 169, 168 },
  { 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191 },
  { 203, 202, 201, 200, 199, 198, 197, 196, 195, 194, 193, 192 },
  { 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215 }
};

uint8_t displayMatrix[18][12];
const int colLen = 8;
uint64_t colors[colLen] = { CRGB::Black, CRGB::Turquoise, CRGB::Blue, CRGB::DarkOrange, CRGB::Yellow, CRGB::Green, CRGB::Purple, CRGB::Red };
uint64_t moreColors[colLen] = {CRGB::Black, CRGB::Seashell, CRGB::Honeydew, CRGB::PeachPuff, CRGB::Thistle, CRGB::CornflowerBlue, CRGB::Fuchsia, CRGB::IndianRed};

Shape getShape(int num) {
  switch (num) {
    case 1:
      return stick;
      break;
    case 2:
      return cube;
      break;
    case 3:
      return table;
      break;
    case 4:
      return el;
      break;
    case 5:
      return mirrorEl;
      break;
    case 6:
      return squiggle;
      break;
    case 7:
      return mirrorSquiggle;
      break;
  }
}

void printWord(char* displayThis, int X, int Y, uint64_t col) {
  char string[50];
  int len = 0;
  uint32_t letter;
  int furthestX = 0;
  int yIndex = 0;
  uint64_t tempCol = 0;
  int go = 1;
  sprintf(string, displayThis);
  len = strlen(string);

  for (int i = 0; i < len; i++) {
    if ((i % 2) && (gameSel)){
      tempCol = CRGB::Gold;
    }
    else{
      tempCol = col;
    }

    switch (string[i]) {
      case 'A':
        letter = 0x0229CA5;
        break;
      case 'B':
        letter = 0x0749D27;
        break;
      case 'C':
        letter = 0x0E0842E;
        break;
      case 'D':
        letter = 0x074A527;
        break;
      case 'E':
        letter = 0x0308C23;
        break;
      case 'F':
        letter = 0x0F09C21;
        break;
      case 'G':
        letter = 0x0E0B52E;
        break;
      case 'H':
        letter = 0x094BD29;
        break;
      case 'I':
        letter = 0x0108421;
        break;
      case 'J':
        letter = 0x0842126;
        break;
      case 'K':
        letter = 0x0528CA5;
        break;
      case 'L':
        letter = 0x010842F;
        break;
      case 'M':
        letter = 0x11DD631;
        break;
      case 'N':
        letter = 0x095B529;
        break;
      case 'O':
        letter = 0x02294A2;
        break;
      case 'P':
        letter = 0x0749C21;
        break;
      case 'Q':
        letter = 0x064A5AE;
        break;
      case 'R':
        letter = 0x0328CA5;
        break;
      case 'S':
        letter = 0x0608883;
        break;
      case 'T':
        letter = 0x0710842;
        break;
      case 'U':
        letter = 0x094A52F;
        break;
      case 'V':
        letter = 0x05294A2;
        break;
      case 'W':
        letter = 0x118C6AA;
        break;
      case 'X':
        letter = 0x05288A5;
        break;
      case 'Y':
        letter = 0x0529C42;
        break;
      case 'Z':
        letter = 0x0F4104F;
        break;
      case '1':
        letter = 0x0218842;
        break;
      case '2':
        letter = 0x0721827;
        break;
      case '3':
        letter = 0x0720887;
        break;
      case '4':
        letter = 0x0529C84;
        break;
      case '5':
        letter = 0x0709C87;
        break;
      case '6':
        letter = 0x0709CA7;
        break;
      case '7':
        letter = 0x0720821;
        break;
      case '8':
        letter = 0x0729CA7;
        break;
      case '9':
        letter = 0x0729C87;
        break;
      case '0':
        letter = 0x07294A7;
        break;
      case '!':
        letter = 0x0108401;
        break;
      case ' ':
        letter = 0x0000800;
        tempCol = CRGB::Black;
        break;
      case '~':
        X = 1;
        furthestX = X;
        Y = Y - 7;
        go = 0;
        break;
    }
    if (go){
      for (int j = 0; j < 25; j++) {
        if (j % 5 == 0) {
          yIndex++;
        }
        if ((letter >> j) & 1) {
          if ((game == 2) && (!gameSel)){
            leds[ledMatrix[17 - ((yIndex - 1) + Y)][(j % 5) + X]] = tempCol;
          }
          else{
            leds[ledMatrix[(j % 5) + X][(yIndex - 1) + Y]] = tempCol;
          }
          if (((j % 5) + X) >= furthestX) {
            furthestX = (j % 5) + X;
          }
        }
      }
      yIndex = 0;
      if (gameSel){
        X = furthestX + 1;
      }
      else{
        X = furthestX + 2;
      }
    }
    else{
      go = 1;
    }
  }
  FastLED.show();
}

void movePaddle(int player, int Ypos, int col, int dir) {
  int x = 0;
  if (player == 1) {
    x = 0;
  }
  else {
    x = 17;
  }

  if (dir) {
    leds[ledMatrix[x][Ypos - 2]] = CRGB::Black;
  }
  else {
    leds[ledMatrix[x][Ypos + 2]] = CRGB::Black;
  }

  leds[ledMatrix[x][Ypos + 1]] = colors[col];
  leds[ledMatrix[x][Ypos]] = colors[col];
  leds[ledMatrix[x][Ypos - 1]] = colors[col];
}

Ball moveBall(Ball tempBall) {
  double Y = sin(tempBall.angle);
  double X = cos(tempBall.angle);

  int dirX = (int)X;
  int dirY = (int)Y;

  X = X * (stepVal);
  Y = Y * (stepVal);

  if ((tempBall.XPOS + X) > 17.0) {
    tempBall.scored = 1;
  }
  if ((tempBall.XPOS + X) < 0) {
    tempBall.scored = 2;
  }
  if (tempBall.scored == 0) {
    leds[ledMatrix[(int)tempBall.XPOS][(int)tempBall.YPOS]] = CRGB::Black;
    leds[ledMatrix[(int)(tempBall.XPOS + X)][(int)(tempBall.YPOS + Y)]] = CRGB::Gold;
    tempBall.xPos = min(17, max(0, tempBall.xPos + dirX));
    tempBall.yPos = min(10, max(1, tempBall.yPos + dirY));
    tempBall.XPOS = min(17.0, max(0.0, tempBall.XPOS + X));
    tempBall.YPOS = min(10.0, max(1.75, tempBall.YPOS + Y));
  }
  else {
    leds[ledMatrix[(int)tempBall.XPOS][(int)tempBall.YPOS]] = CRGB::Black;
  }

  return tempBall;
}

int checkScore(int p1Sc, int p2Sc, int p1C, int p2C) {
  if ((p1Sc == 10) || (p2Sc == 10)) {
    if (p1Sc == 10) {
      return 1;
    }
    else {
      return 2;
    }
  }
  else {
    for (int i = 0; i < p1Score; i++) {
      leds[ledMatrix[i + 4][0]] = colors[p1C];
    }
    for (int i = 0; i < p2Score; i++) {
      leds[ledMatrix[i + 4][11]] = colors[p2C];
    }
    return 0;
  }
}

void newFood(){
  foodX = random(18);
  foodY = random(12);
  while (displayMatrix[foodX][foodY] == snakeColor){
    foodX = random(18);
    foodY = random(12);
  }
  displayMatrix[foodX][foodY] = 7;
}

void newSnake(){
  headX = 7;
  headY = 7;
  dir = east;
  snakeLength = 3;
  displayMatrix[headX][headY] = snakeColor;
  displayMatrix[headX-1][headY] = snakeColor;
  displayMatrix[headX-2][headY] = snakeColor;
  snakeArray[0][0] = headX;
  snakeArray[0][1] = headY;
  snakeArray[1][0] = headX-1;
  snakeArray[1][1] = headY;
  snakeArray[2][0] = headX-2;
  snakeArray[2][1] = headY;
}

void clearMatrix(){
  for (int i = 0; i < 18; i++) {
    for (int j = 0; j < 12; j++) {
      displayMatrix[i][j] = 0;
    }
  }
}

void printMatrix(){
  for (int i = 0; i < 18; i++) {
    for (int j = 0; j < 12; j++) {
      leds[ledMatrix[i][j]] = colors[displayMatrix[i][j]];
    }
  }
  FastLED.show();
}

void addToSnake(){
  for (int i = snakeLength - 1; i >= 0; i--){
    snakeArray[i+1][0] = snakeArray[i][0];
    snakeArray[i+1][1] = snakeArray[i][1];
  }
  snakeArray[0][0] = foodX; 
  snakeArray[0][1] = foodY;
  headX = snakeArray[0][0];
  headY = snakeArray[0][1];
}

void moveSnake(){
  if ((dir % 2) == 0){ // east / west
    headX = headX + dirs[dir-1];
  }
  else{                // north / south
    headY = headY + dirs[dir-1];
  }

  if ((headX < 0) || (headX > 17) || (headY < 0) || (headY > 11) || ((displayMatrix[headX][headY] != 0) && (displayMatrix[headX][headY] != 7))){
    lose = 1;
  }

  for (int i = snakeLength; i > 0; i--){
    snakeArray[i][0] = snakeArray[i-1][0];
    snakeArray[i][1] = snakeArray[i-1][1];
  }

  snakeArray[0][0] = headX;
  snakeArray[0][1] = headY;
}

void reset(){
  clearMatrix();
  newSnake();
  newFood();
}

void countDown(){
  printWord("2", 10, 3, CRGB::White);
  delay(1000);
  printWord("2", 10, 3, CRGB::Black);
  FastLED.show();
  printWord("1", 10, 3, CRGB::White);
  delay(1000);
  printWord("1", 10, 3, CRGB::Black);
  FastLED.show();
}

void showScore(){
  sprintf(score, "%d", snakeLength);
  printWord(score, 5, 3, CRGB::White);
}

void initPong(){
  FastLED.clear();
  clearMatrix();

  for (int i = 0; i < 18; i++) {
    leds[ledMatrix[i][0]] = CRGB::White;
    leds[ledMatrix[i][11]] = CRGB::White;
  }

  ball.scored = 0;
  ball.xPos = ballX;
  ball.yPos = ballY;
  ball.XPOS = BALLX;
  ball.YPOS = BALLY;
  ball.moveDir = 0;
  ball.justBounced = 0;

  start = random(2);
  if (start) {
    ball.angle = 0;
  }
  else {
    ball.angle = 180;
  }

  p1Col = random(colLen);
  p2Col = random(colLen);
  if (p1Col == p2Col) {
    while (p1Col == p2Col) {
      p2Col = random(colLen);
    }
  }
}

void initSnake(){
  headX = 7;
  headY = 7;
  clearMatrix();
  for (int i = 0; i < 18; i++){
    for (int j = 0; j < 12; j++){
      displayMatrix[i][j] = 0;
    }
  }
}

void initTetris(){
  clearMatrix();
  stick.rotations[0] = 0x000F;
  stick.rotations[1] = 0x1111;
  stick.rotations[2] = 0x000F;
  stick.rotations[3] = 0x1111;
  stick.x = 1;
  stick.y = 1;
  cube.rotations[0] = 0x0033;
  cube.rotations[1] = 0x0033;
  cube.rotations[2] = 0x0033;
  cube.rotations[3] = 0x0033;
  cube.x = 1;
  cube.y = 1;
  table.rotations[0] = 0x0027;
  table.rotations[1] = 0x0232;
  table.rotations[2] = 0x0072;
  table.rotations[3] = 0x0131;
  table.x = 1;
  table.y = 1;
  el.rotations[0] = 0x0017;
  el.rotations[1] = 0x0223;
  el.rotations[2] = 0x0074;
  el.rotations[3] = 0x0311;
  el.x = 1;
  el.y = 1;
  mirrorEl.rotations[0] = 0x0047;
  mirrorEl.rotations[1] = 0x0322;
  mirrorEl.rotations[2] = 0x0071;
  mirrorEl.rotations[3] = 0x0113;
  mirrorEl.x = 1;
  mirrorEl.y = 1;
  squiggle.rotations[0] = 0x0063;
  squiggle.rotations[1] = 0x0132;
  squiggle.rotations[2] = 0x0063;
  squiggle.rotations[3] = 0x0132;
  squiggle.x = 1;
  squiggle.y = 1;
  mirrorSquiggle.rotations[0] = 0x0036;
  mirrorSquiggle.rotations[1] = 0x0231;
  mirrorSquiggle.rotations[2] = 0x0036;
  mirrorSquiggle.rotations[3] = 0x0231;
  mirrorSquiggle.x = 1;
  mirrorSquiggle.y = 1;

  FastLED.clear();
  FastLED.show();
}

void tetris(){
  if ((millis() - timeStamp1) > makeNewShape) {
    timeStamp1 = millis();
    if (newShape) {
      for (int i = 0; i < 18; i++){
        rowCount = 0;
        for (int j = 0; j < 12; j++){
          if (displayMatrix[i][j] != 0){
            rowCount++;
          }
        }
        if (rowCount == 12){
          points++;
          for (int k = i; k > 0; k--){
            for (int l = 0; l < 12; l++){
              displayMatrix[k][l] = displayMatrix[k-1][l];
            }
          }
          i = 0;
        }
      }
      newShape = 0;
      curRot = random(4);
      tempShape = getShape(random(8));
      temp = random(1, 8);
      lowestX = 0;
      tempShape.x = 0;
      tempShape.y = 4;

      for (uint8_t j = 0; j < 16; j++) {
        if (j % 4 == 0) {
          yIndex++;
        }
        if ((tempShape.rotations[curRot] >> j) & 1) {
          if (displayMatrix[(j % 4) + tempShape.x][(yIndex - 1) + tempShape.y] != 0) {
            endGame = 1;
          }
        }
      }
    }

    if (!endGame){
      for (uint8_t j = 0; j < 16; j++) {
        if (j % 4 == 0) {
          yIndex++;
        }
        if ((tempShape.rotations[curRot] >> j) & 1) {
          displayMatrix[(j % 4) + tempShape.x][(yIndex - 1) + tempShape.y] = 0;
        }
      }
      yIndex = 0;

      for (uint8_t j = 0; j < 16; j++) {
        if (j % 4 == 0) {
          yIndex++;
        }
        if ((tempShape.rotations[curRot] >> j) & 1) {
          if (displayMatrix[(j % 4) + tempShape.x + 2][(yIndex - 1) + tempShape.y] != 0) {
            newShape = 1;
          }
        }
      }
      yIndex = 0;

      if ((lowestX + 1) == 18) {
        newShape = 1;
      } else {
        tempShape.x = max(0, min(17, tempShape.x + 1));
      }

      for (uint8_t j = 0; j < 16; j++) {
        if (j % 4 == 0) {
          yIndex++;
        }
        if ((tempShape.rotations[curRot] >> j) & 1) {
          displayMatrix[(j % 4) + tempShape.x][(yIndex - 1) + tempShape.y] = temp;
          if (((j % 4) + tempShape.x) >= lowestX){
            lowestX = (j % 4) + tempShape.x;
          }
        }
      }
      yIndex = 0;
    }
  }

  if ((!newShape) && (!endGame)){
    if ((millis() - timeStamp2) > processCommands) {
      timeStamp2 = millis();
      
      for (uint8_t j = 0; j < 16; j++) {
        if (j % 4 == 0) {
          yIndex++;
        }
        if ((tempShape.rotations[curRot] >> j) & 1) {
          displayMatrix[(j % 4) + tempShape.x][(yIndex - 1) + tempShape.y] = 0;
        }
      }
      yIndex = 0;

      if (LeftX > 70) {
        yCheck = 1;
        for (uint8_t j = 0; j < 16; j++) {
          if (j % 4 == 0) {
            yIndex++;
          }
          if ((tempShape.rotations[curRot] >> j) & 1) {
            if(displayMatrix[(j % 4) + tempShape.x][(yIndex - 1) + tempShape.y-1] != 0){
              yCheck = 0;
            }
          }
        }
        yIndex = 0;
        if (yCheck){
          tempShape.y = max(0, min(11, tempShape.y - 1));
        }
      } 
      else if (LeftX < 40) {
        furthestY = 0;
        yCheck = 1;
        for (uint8_t j = 0; j < 16; j++) {
          if (j % 4 == 0) {
            yIndex++;
          }
          if ((tempShape.rotations[curRot] >> j) & 1) {
            if (((yIndex - 1) + tempShape.y) >= furthestY){
              furthestY = ((yIndex - 1) + tempShape.y);
            }
            if(displayMatrix[(j % 4) + tempShape.x][(yIndex - 1) + tempShape.y + 1] != 0){
              yCheck = 0;
            }
          }
        }
        yIndex = 0;
        if (yCheck){
          tempShape.y = max(0, min(11 - (furthestY - tempShape.y), tempShape.y + 1));
        }
      }
      
      for (uint8_t j = 0; j < 16; j++) {
        if (j % 4 == 0) {
          yIndex++;
        }
        if ((tempShape.rotations[curRot] >> j) & 1) {
          if (displayMatrix[(j % 4) + tempShape.x + 1][(yIndex - 1) + tempShape.y] != 0) {
            newShape = 1;
          }
        }
      }

      yIndex = 0;

      if (BR) {
        BRCount++;
        if (BRCount > buttonWait){
          BRCount = 0;
          if ((curRot + 1) == 4) {
            curRot = 0;
          } else {
            curRot++;
          }
        }
      }
      if (BY) {
        BYCount++;
        if (BYCount > buttonWait){
          BYCount = 0;
          if ((curRot - 1) == -1) {
            curRot = 3;
          } else {
            curRot--;
          }
        }
      }

      if (RightY > 60){
        makeNewShape = 50;
      }
      else{
        makeNewShape = 500;
      }

      for (uint8_t j = 0; j < 16; j++) {
        if (j % 4 == 0) {
          yIndex++;
        }
        if ((tempShape.rotations[curRot] >> j) & 1) {
          displayMatrix[(j % 4) + tempShape.x][(yIndex - 1) + tempShape.y] = temp;
        }
      }
      yIndex = 0;
    }
  }

  if (!endGame){
    for (int i = 0; i < 18; i++) {
      for (int j = 0; j < 12; j++) {
        leds[ledMatrix[i][j]] = colors[displayMatrix[i][j]];
      }
    }
  }
  else{
    FastLED.clear();
    FastLED.show();
    for (int i = 0; i < 18; i++){
      for (int j = 0; j < 12; j++){
        displayMatrix[i][j] = 0;
      }
    }
    sprintf(score, "%d", points);
    printWord(score, 1, 6, CRGB::LightSeaGreen);
    delay(3000);
    for (int i = 0; i < 18; i++){
      for (int j = 0; j < 12; j++){
        displayMatrix[i][j] = 0;
      }
    }
    endGame = 0;
    points = 0;
  }
  FastLED.show();
}

void snake(){
  move = map(potLeftVal, 0, 100, 0, 500); 

  if (BR) {
    countR++;
    if (countR > 4) {
      if ((snakeColor + 1) > (colLen - 2)) {
        snakeColor = 1;
      }
      else {
        snakeColor++;
      }
      countR = 0;
    }
  }
  else {
    countR = 0;
  }

  if ((millis() - timeStamp1) > move) {
    timeStamp1 = millis();
    if (LeftX > 80){
      if (dir != east){
        dir = west;
      }    
    }
    else if (LeftX < 20){
      if (dir != west){
        dir = east;
      }
    }
    else if (LeftY > 80){
      if (dir != north){
        dir = south;
      }    
    }
    else if (LeftY < 20){
      if (dir != south){
        dir = north;
      }
    }

    if ((headX == foodX) && (headY == foodY)){
      snakeLength++;
      addToSnake();
      newFood();
    }
    else{
      moveSnake();
    }
  }

  if (!lose){
    clearMatrix();

    for (int i = 0; i < snakeLength; i++){
      displayMatrix[snakeArray[i][0]][snakeArray[i][1]] = snakeColor;
    }
    displayMatrix[foodX][foodY] = 7;

    printMatrix();
  }
  else{
    lose = 0;
    dir = east;
    if (!startSnake){
      clearMatrix();
      showScore();
      delay(3000);
    }
    reset();
    printMatrix();
    countDown();
  }
  startSnake = false;
  FastLED.show();
}

void pong(){
  up = 0;
  down = 0;
  left = 0;
  right = 0;

  if (potVerticalVal <= 50){
    numPlayers = 1;
  }
  else{
    numPlayers = 2;
  }

  del = map(potLeftVal, 0, 100, 0, 300);
  del1 = map(potRightVal, 0, 100, 0, 300);

  if (BR) {
    countR++;
    if (countR > 8) {
      if ((p1Col + 1) > (colLen - 1)) {
        p1Col = 1;
      }
      else {
        p1Col++;
      }
      countR = 0;
      if (p1Col == p2Col) {
        if ((p1Col + 1) > (colLen - 1)) {
          p1Col = 1;
        }
        else {
          p1Col++;
        }
      }
    }
  }
  else {
    countR = 0;
  }
  if (BY) {
    countY++;
    if (countY > 8) {
      if ((p2Col + 1) > (colLen - 1)) {
        p2Col = 1;
      }
      else {
        p2Col++;
      }
      countY = 0;
      if (p2Col == p1Col) {
        if ((p2Col + 1) > (colLen - 1)) {
          p2Col = 1;
        }
        else {
          p2Col++;
        }
      }
    }
  }
  else {
    countY = 0;
  }

  if ((millis() - timeStamp) > del) {
    timeStamp = millis();
    if (LeftY < 40) {
      p1Y = min(9, max(2, (p1Y + 1)));
      p1Dir = 1;
    }
    if (LeftY > 80) {
      p1Y = min(9, max(2, (p1Y - 1)));
      p1Dir = 0;
    }
    if (numPlayers == 2){
      if (RightY < 40) {
        p2Y = min(9, max(2, (p2Y + 1)));
        p2Dir = 1;
      }
      if (RightY > 80) {
        p2Y = min(9, max(2, (p2Y - 1)));
        p2Dir = 0;
      }
    }
    else{
      if ((int)ball.YPOS > p2Y){
        p2Y = min(9, max(2, (p2Y + 1)));
        p2Dir = 1;
      }
      else if ((int)ball.YPOS < p2Y){
        p2Y = min(9, max(2, (p2Y - 1)));
        p2Dir = 0;
      }
    }
    movePaddle(1, p1Y, p1Col, p1Dir);
    movePaddle(2, p2Y, p2Col, p2Dir);
  }

  if  ((millis() - ballStamp) > del1) {
    ballStamp = millis();
    if (ball.scored != 0) {
      if (ball.scored == 1) {
        ball.angle = 0;
        p1Score++;
      }
      else if (ball.scored == 2) {
        ball.angle = 180 * DEG_TO_RAD;
        p2Score++;
      }
      ball.scored = 0;
      ball.xPos = ballX;
      ball.yPos = ballY;
      ball.XPOS = BALLX;
      ball.YPOS = BALLY;

      win = checkScore(p1Score, p2Score, p1Col, p2Col);

      if (win == 0){  
        leds[ledMatrix[0][p1Y + 1]] = CRGB::Black;
        leds[ledMatrix[0][p1Y]] = CRGB::Black;
        leds[ledMatrix[0][p1Y - 1]] = CRGB::Black;
        leds[ledMatrix[17][p2Y + 1]] = CRGB::Black;
        leds[ledMatrix[17][p2Y]] = CRGB::Black;
        leds[ledMatrix[17][p2Y - 1]] = CRGB::Black;

        p1Y = 5;
        p2Y = 5;

        leds[ledMatrix[0][p1Y + 1]] = colors[p1Col];
        leds[ledMatrix[0][p1Y]] = colors[p1Col];
        leds[ledMatrix[0][p1Y - 1]] = colors[p1Col];
        leds[ledMatrix[17][p2Y + 1]] = colors[p2Col];
        leds[ledMatrix[17][p2Y]] = colors[p2Col];
        leds[ledMatrix[17][p2Y - 1]] = colors[p2Col];

        printWord("2", 8, 4, CRGB::White);
        delay(1000);
        FastLED.show();
        printWord("2", 8, 4, CRGB::Black);
        printWord("1", 8, 4, CRGB::White);
        delay(1000);
        FastLED.show();
        printWord("1", 8, 4, CRGB::Black);
      }
    }
    ball = moveBall(ball);

    if ((int)ball.XPOS == 1) {
      if ((int)ball.YPOS == (p1Y)) {
        ball.angle = (0.0 + random(-(angleVariant / 2.0), (angleVariant / 2.0))) * DEG_TO_RAD;
        if (ball.angle < 0.0) {
          ball.angle = 360 * DEG_TO_RAD - ((ball.angle) * (-1.0));
        }
      }
      else if ((int)ball.YPOS == (p1Y + 1)) {
        ball.angle = (0.0 + random(0.0, angleVariant)) * DEG_TO_RAD;
        if (ball.angle < 0.0) {
          ball.angle = 360 * DEG_TO_RAD - ((ball.angle) * (-1.0));
        }
      }
      else if ((int)ball.YPOS == (p1Y - 1)) {
        ball.angle = (0.0 + random(-angleVariant, 0.0)) * DEG_TO_RAD;
        if (ball.angle < 0.0) {
          ball.angle = 360 * DEG_TO_RAD - ((ball.angle) * (-1.0));
        }
      }
    }
    if ((int)ball.XPOS == 16) {
      if ((int)ball.YPOS == (p2Y)) {
        ball.angle = (180.0 + random(-(angleVariant / 2.0), (angleVariant / 2.0))) * DEG_TO_RAD ;
        if (ball.angle < 0.0) {
          ball.angle = 360 * DEG_TO_RAD - ((ball.angle) * (-1.0));
        }
      }
      else if ((int)ball.YPOS == (p2Y + 1)) {
        ball.angle = (180.0 + random(-angleVariant, 0)) * DEG_TO_RAD ;
        if (ball.angle < 0.0) {
          ball.angle = 360 * DEG_TO_RAD - ((ball.angle) * (-1.0));
        }
      }
      else if ((int)ball.YPOS == (p2Y - 1)) {
        ball.angle = (180.0 + random(0, angleVariant)) * DEG_TO_RAD ;
        if (ball.angle < 0.0) {
          ball.angle = 360 * DEG_TO_RAD - ((ball.angle) * (-1.0));
        }
      }
    }

    if (((ball.angle * RAD_TO_DEG) <= 270.0) && ((ball.angle * RAD_TO_DEG) >= 90.0)) {
      ball.moveDir = 0;
    }
    else {
      ball.moveDir = 1;
    }

    if (((int)ball.YPOS == 1) && (ball.justBounced == 0)) {
      if ((ball.angle != 0.0) || (ball.angle != (180.0 * DEG_TO_RAD))) {
        if (ball.moveDir) {
          if (ball.angle > (180.0 * DEG_TO_RAD)) {
            ball.angle = ball.angle - (180.0 * DEG_TO_RAD);
          }
          ball.angle = ((90.0 * (double)DEG_TO_RAD) - (ball.angle - (90.0 * (double)DEG_TO_RAD)));
        }
        else {
          if (ball.angle > (90.0 * DEG_TO_RAD)) {
            ball.angle = ball.angle - (180.0 * DEG_TO_RAD);
          }
          ball.angle = ((90.0 * (double)DEG_TO_RAD) + ((90.0 * (double)DEG_TO_RAD) - ball.angle));
        }
        ball.justBounced = 1;
      }
    }
    if (((int)ball.YPOS == 10) && (ball.justBounced == 0)) {
      if ((ball.angle != 0.0) || (ball.angle != (180.0 * DEG_TO_RAD))) {
        if (ball.moveDir) {
          if (ball.angle < (180 * DEG_TO_RAD)) {
            ball.angle = ball.angle + (180.0 * DEG_TO_RAD);
          }
          ball.angle = ((270.0 * (double)DEG_TO_RAD) + ((270.0 * (double)DEG_TO_RAD) - ball.angle));
        }
        else {
          if (ball.angle < (270.0 * DEG_TO_RAD)) {
            ball.angle = ball.angle + (180.0 * DEG_TO_RAD);
          }
          ball.angle = ((270.0 * (double)DEG_TO_RAD) - (ball.angle - (270.0 * (double)DEG_TO_RAD)));
        }
        ball.justBounced = 1;
      }
    }
    if (((int)ball.YPOS != 10) && ((int)ball.YPOS != 1)) {
      ball.justBounced = 0;
    }
  }

  win = checkScore(p1Score, p2Score, p1Col, p2Col);

  if (win == 1) {
    FastLED.clear();
    printWord("P1~WINS", 1, 7, CRGB::White);
    FastLED.show();
    delay(5000);
    FastLED.clear();
    FastLED.show();
  }
  else if (win == 2) {
    FastLED.clear();
    printWord("P2~WINS", 1, 7, CRGB::White);
    FastLED.show();
    delay(5000);
    FastLED.clear();
    FastLED.show();
  }
  if (win != 0) {
    win = 0;
    p1Score = 0;
    p2Score = 0;

    if (random(2)) {
      ball.angle = 0;
    }
    else {
      ball.angle = 180;
    }

    p1Y = 5;
    p2Y = 5;

    FastLED.clear();

    leds[ledMatrix[0][p1Y + 1]] = colors[p1Col];
    leds[ledMatrix[0][p1Y]] = colors[p1Col];
    leds[ledMatrix[0][p1Y - 1]] = colors[p1Col];
    leds[ledMatrix[17][p2Y + 1]] = colors[p2Col];
    leds[ledMatrix[17][p2Y]] = colors[p2Col];
    leds[ledMatrix[17][p2Y - 1]] = colors[p2Col];

    for (int i = 0; i < 18; i++) {
      leds[ledMatrix[i][0]] = CRGB::White;
      leds[ledMatrix[i][11]] = CRGB::White;
    }

    printWord("2", 8, 3, CRGB::White);
    delay(1000);
    printWord("2", 8, 3, CRGB::Black);
    FastLED.show();
    printWord("1", 8, 3, CRGB::White);
    delay(1000);
    printWord("1", 8, 3, CRGB::Black);
    FastLED.show();
  }
  FastLED.show();
}

int gameSelect(){
  while((!buttons[6]) && (!buttons[7])){
    if (radio.available()) {
      radio.read(&text, sizeof(text));
      val = strtok(text, ",");
      num = atoi(val);
      val = strtok(NULL, ",");
      LeftX = atoi(val);
      val = strtok(NULL, ",");
      LeftY = atoi(val);
      val = strtok(NULL, ",");
      RightY = atoi(val);
      val = strtok(NULL, ",");
      RightX = atoi(val);
      val = strtok(NULL, ",");
      potLeftVal = atoi(val);
      val = strtok(NULL, ",");
      potRightVal = atoi(val);
      val = strtok(NULL, ",");
      potVerticalVal = atoi(val);

      for (uint8_t i = 0; i < 8; i++){
        buttons[i] = (num >> i) & 1;
        if (i > 5){
          buttons[i] = !buttons[i];
        }
      }

      BR = buttons[4];
      BY = buttons[5];
    }

    if (game == 0){
      game = 1;
    }

    if (buttons[1] && (!gameWait)){
      game = max(1, min(3, game+1));
      gameWait = true;
    }
    if (buttons[3] && (!gameWait)){
      game = max(1, min(3, game-1));
      gameWait = true;
    }
    if (!buttons[3] && !buttons[1]){
      gameWait = false;
    }

    FastLED.clear();
    switch(game){
      case 1:
        printWord("PONG", 1, 3, CRGB::DarkRed);
        break;
      case 2:
        printWord("TETRIS", 1, 3, CRGB::LightBlue);
        break;
      case 3:
        printWord("SNAKE", 1, 3, CRGB::Green);
        break;
    
    delay(10);
    }
  }
  switch(game){
    case 1:
      initPong();
      break;
    case 2:
      initTetris();
      break;
    case 3:
      startSnake = true;
      initSnake();
      break;
  }
  FastLED.setBrightness(BRIGHTNESS);
  return game;
}

void setup() {
  // put your setup code here, to run once:
  delay(500);
  //Serial.begin(9600);
  radio.begin();

  //set the address
  radio.openReadingPipe(0, address);
  radio.setChannel(0x77);
  radio.setPALevel(RF24_PA_MAX);
  radio.enableDynamicPayloads();

  //Set module as receiver
  radio.startListening();

  FastLED.addLeds<MODEL, DATA_PIN, COLOR>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  randomSeed(analogRead(0));
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  if (radio.available()) {
    radio.read(&text, sizeof(text));
    val = strtok(text, ",");
    num = atoi(val);
    val = strtok(NULL, ",");
    LeftX = atoi(val);
    val = strtok(NULL, ",");
    LeftY = atoi(val);
    val = strtok(NULL, ",");
    RightY = atoi(val);
    val = strtok(NULL, ",");
    RightX = atoi(val);
    val = strtok(NULL, ",");
    potLeftVal = atoi(val);
    val = strtok(NULL, ",");
    potRightVal = atoi(val);
    val = strtok(NULL, ",");
    potVerticalVal = atoi(val);

    for (uint8_t i = 0; i < 8; i++){
      buttons[i] = (num >> i) & 1;
      if (i > 5){
        buttons[i] = !buttons[i];
      }
    }

    BR = buttons[4];
    BY = buttons[5];
  }

  if ((BY && BR && buttons[0]) || (game == 0)){
    FastLED.setBrightness(60);
    gameSel = true;
    game = gameSelect();
  }
  gameSel = false;

  switch(game){
    case 1:
      pong();
      break;
    case 2:
      tetris();
      break;
    case 3:
      snake();
      break;
  }
}
