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
#define BRIGHTNESS 30

RF24 radio(7, 8);  // CE, CSN

//RF Var
const byte address[6] = "node1";

//Input string vars
char text[32] = { 0 };
char* val;

//Controller Inputs
uint8_t num = 0;
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

// Matrix for displaying the game
uint8_t displayMatrix[18][12];

const int colLen = 8;
uint64_t colors[colLen] = { CRGB::Black, CRGB::Turquoise, CRGB::Blue, CRGB::DarkOrange, CRGB::Yellow, CRGB::Green, CRGB::Purple, CRGB::Red };
uint64_t moreColors[colLen] = {CRGB::Black, CRGB::Seashell, CRGB::Honeydew, CRGB::PeachPuff, CRGB::Thistle, CRGB::CornflowerBlue, CRGB::Fuchsia, CRGB::IndianRed};

// Game vars
int countR = 0;
int countY = 0;
int snakeColor = 5;
int dirs[4] = {1, 1, -1, -1};
uint8_t dir = 0;
uint8_t north = 1;
uint8_t east = 2;
uint8_t south = 3;
uint8_t west = 4;
int snakeLength = 3;
uint8_t foodX = 0;
uint8_t foodY = 0;
uint8_t headX = 0;
uint8_t headY = 0;
bool lose = 0;
uint8_t snakeArray[100][2];

// Timing vars
unsigned long move = 500;
unsigned long timeStamp1 = 0;

// Display Val
char scoreString[10];

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
  sprintf(scoreString, "%d", snakeLength);
  printWord(scoreString, 5, 3, CRGB::White);
}

void setup() {
  // put your setup code here, to run once:
  delay(500);
  Serial.begin(9600);
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

  for (int i = 0; i < 18; i++){
    for (int j = 0; j < 12; j++){
      displayMatrix[i][j] = 0;
    }
  }
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

    move = map(potLeftVal, 0, 100, 0, 500); 
  }

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
    clearMatrix();
    showScore();
    delay(3000);
    reset();
    printMatrix();
    countDown();
  }
  FastLED.show();
}
