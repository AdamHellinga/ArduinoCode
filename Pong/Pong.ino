#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <FastLED.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define DATA_PIN 3
#define MODEL PL9823
#define COLOR RGB
#define NUM_LEDS 216
#define BRIGHTNESS 50
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105


RF24 radio(7, 8);  // CE, CSN

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

//RF Var
const byte address[6] = "node1";

//Input string vars
char text[32] = {0};
char* val;

//Controller Inputs
int LeftX = 0;
int LeftY = 0;
int RightX = 0;
int RightY = 0;
int pot1Val = 0;
int pot2Val = 0;
int vertVal = 0;
uint8_t num = 0;
int buttons[8];
int numPlayers = 1;
int BR = 0;
int BY = 0;

//directions
int up = 0;
int down = 0;
int left = 0;
int right = 0;

//Coordinates
int ballX = 8;
int ballY = 5;
double BALLX = 8.0;
double BALLY = 5.0;
double stepVal = 1.0;
double temp = 0.0;
int ballYUP = 0;
int ballYDOWN = 0;
int ballXUP = 0;
int ballXDOWN = 0;
int YUPP = 0;
int YDOWNP = 0;
int XUPP = 0;
int XDOWNP = 0;
int ballXVelo = 0;
int ballYVelo = 0;
int ballAngle = 0;
int p1Y = 5;
int p2Y = 5;
int p1Dir = 0;
int p2Dir = 0;
int len = 1;

//other vars
int p1Col = 0;
int p2Col = 0;
int colorIndex = 0;
int countR = 0;
int countY = 0;
long timeStamp = 0;
long ballStamp = 0;
int del = 200;
int del1 = 200;
int lastScored = 1;
int angleVariant = 60;
int start = 0;

//scoring
int p1Score = 0;
int p2Score = 0;
int win = 0;
int botLeftX = 2;
int botLeftY = 4;

CRGB leds[NUM_LEDS];

int ledMatrix[18][12] = {
  { 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0},
  { 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23},
  { 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24},
  { 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47},
  { 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48},
  { 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71},
  { 83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72},
  { 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95},
  {107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96},
  {108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119},
  {131, 130, 129, 128, 127, 126, 125, 124, 123, 122, 121, 120},
  {132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143},
  {155, 154, 153, 152, 151, 150, 149, 148, 147, 146, 145, 144},
  {156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167},
  {179, 178, 177, 176, 175, 174, 173, 172, 171, 170, 169, 168},
  {180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191},
  {203, 202, 201, 200, 199, 198, 197, 196, 195, 194, 193, 192},
  {204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215}
};

const int colLen = 8;
uint64_t colors[colLen] = {CRGB::Red, CRGB::Pink, CRGB::Yellow, CRGB::Green, CRGB::Blue, CRGB::Gold, CRGB::Maroon, CRGB::Turquoise};

Ball ball;

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
    //    leds[ledMatrix[(int)ceil((tempBall.XPOS + X))][(int)ceil((tempBall.YPOS + Y))]] = CRGB::Gold;
    //    leds[ledMatrix[(int)ceil((tempBall.XPOS + X))][(int)floor((tempBall.YPOS + Y))]] = CRGB::Gold;
    //    leds[ledMatrix[(int)floor((tempBall.XPOS + X))][(int)ceil((tempBall.YPOS + Y))]] = CRGB::Gold;
    //    leds[ledMatrix[(int)floor((tempBall.XPOS + X))][(int)floor((tempBall.YPOS + Y))]] = CRGB::Gold;
    //    leds[ledMatrix[(int)ceil((tempBall.XPOS))][(int)ceil(tempBall.YPOS)]] = CRGB::Black;
    //    leds[ledMatrix[(int)ceil((tempBall.XPOS))][(int)floor(tempBall.YPOS)]] = CRGB::Black;
    //    leds[ledMatrix[(int)floor((tempBall.XPOS))][(int)ceil(tempBall.YPOS)]] = CRGB::Black;
    //    leds[ledMatrix[(int)floor((tempBall.XPOS))][(int)floor(tempBall.YPOS)]] = CRGB::Black;
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

void printWord(char* displayThis, int X, int Y, uint64_t col) {
  char string[50];
  int len = 0;
  uint32_t letter;
  int furthestX = 0;
  int yIndex = 0;
  int tempCol = 0;
  int go = 1;
  sprintf(string, displayThis);
  len = strlen(string);

  for (int i = 0; i < len; i++) {
    tempCol = col;
    switch (string[i]) {
      case 'A':
        letter = 0x064BD29;
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
        letter = 0x0F0BC2F;
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
        letter = 0x0928CA9;
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
        letter = 0x064A536;
        break;
      case 'P':
        letter = 0x0749C21;
        break;
      case 'Q':
        letter = 0x064A5AE;
        break;
      case 'R':
        letter = 0x0749CA9;
        break;
      case 'S':
        letter = 0x0E09907;
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
        letter = 0x07292A7;
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
          leds[ledMatrix[(j % 5) + X][(yIndex - 1) + Y]] = tempCol;
          if (((j % 5) + X) >= furthestX) {
            furthestX = (j % 5) + X;
          }
        }
      }
      yIndex = 0;
      X = furthestX + 2;
    }
    else{
      go = 1;
    }
  }
  FastLED.show();
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

  randomSeed(millis());
}

void loop() {
  // put your main code here, to run repeatedly:
  //Read the data if available in buffer
  up = 0;
  down = 0;
  left = 0;
  right = 0;

  if (radio.available())
  {
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
    pot1Val = atoi(val);
    val = strtok(NULL, ",");
    pot2Val = atoi(val);
    val = strtok(NULL, ",");
    vertVal = atoi(val);

    for (uint8_t i = 0; i < 8; i++){
      buttons[i] = (num >> i) & 1;
      if (i > 5){
        buttons[i] = !buttons[i];
      }
    }

    if (vertVal <= 50){
      numPlayers = 1;
    }
    else{
      numPlayers = 2;
    }

    BR = !buttons[4];
    BY = !buttons[5]; 

    del = map(pot1Val, 0, 100, 0, 300);
    del1 = map(pot2Val, 0, 100, 0, 300);

    if (!BR) {
      countR++;
      if (countR > 8) {
        if ((p1Col + 1) > (colLen - 1)) {
          p1Col = 0;
        }
        else {
          p1Col++;
        }
        countR = 0;
        if (p1Col == p2Col) {
          if ((p1Col + 1) > (colLen - 1)) {
            p1Col = 0;
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
    if (!BY) {
      countY++;
      if (countY > 8) {
        if ((p2Col + 1) > (colLen - 1)) {
          p2Col = 0;
        }
        else {
          p2Col++;
        }
        countY = 0;
        if (p2Col == p1Col) {
          if ((p2Col + 1) > (colLen - 1)) {
            p2Col = 0;
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
}
