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
#define BRIGHTNESS 10

RF24 radio(7, 8);  // CE, CSN

//RF Var
const byte address[6] = "node1";

//Input string vars
char text[32] = { 0 };
char* val;

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
uint8_t snake[100][2];

// Timing vars
unsigned long move = 500;
unsigned long timeStamp1 = 0;

void newFood(){
  foodX = random(18);
  foodY = random(12);
  displayMatrix[foodX][foodY] = CRGB::Red;
}

void newSnake(){
  headX = 7;
  headY = 7;
  dir = east;
  displayMatrix[headX][headY] = CRGB::Green;
  displayMatrix[headX-1][headY] = CRGB::Green;
  displayMatrix[headX-2][headY] = CRGB::Green;
  snake[0] = [headX, headY];
  snake[1] = [headX-1, headY];
  snake[2] = [headX-2, headY];
}

void addToSnake(){
  for (int i = snakeLength; i >= 0; i--){
    snake[i+1] = snake[i];
  }
  snake[0] = [foodX, foodY];
  headX = snake[0][0];
  headY = snake[0][1];
}

void moveSnake(){
  if ((dir % 2) == 0){ // east / west
    headX = headX + dirs[dir-1];
  }
  else{                // north / south
    headY = headY + dirs[dir-1];
  }

  for (int i = snakeLength; i > 0; i--){
    snake[i] = snake[i-1];
  }

  snake[0][0] = headX;
  snake[0][1] = headY;
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

  for (int i = 0; i < 18; i++){
    for (int j = 0; j < 12; j++){
      displayMatrix[i][j] = 0;
    }
  }

  newFood();
  newSnake();

  for (int i = 0; i < 18; i++) {
    for (int j = 0; j < 12; j++) {
      leds[ledMatrix[i][j]] = colors[displayMatrix[i][j]];
    }
  }

  FastLED.clear();
  FastLED.show();
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

  if (LeftX > 60){
    dir = east;    
  }
  if (LeftX < 40){
    dir = west;
  }
  if (LeftY > 60){
    dir = north;
  }
  if (LeftY < 40){
    dir = south;
  }

  if ((millis() - timeStamp1) > move) {
    timeStamp1 = millis();
    if ((headX == foodX) && (headY == foodY)){
      snakeLength++;
      addToSnake();
      newFood();
    }
    else{
      moveSnake();
    }
  }

  for (int i = 0; i < 18; i++) {
    for (int j = 0; j < 12; j++) {
      displayMatrix[i][j] = 0;
    }
  }

  for (int i = 0; i < snakeLength; i++){
    displayMatrix[snake[i][0]][snake[i][1]] = CRGB::Green;
  }

  displayMatrix[foodX, foodY] = CRGB::Red;

  for (int i = 0; i < 18; i++) {
    for (int j = 0; j < 12; j++) {
      leds[ledMatrix[i][j]] = colors[displayMatrix[i][j]];
    }
  }
  FastLED.show();
}
