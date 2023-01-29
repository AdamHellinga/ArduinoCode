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
#define BRIGHTNESS 50
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105


RF24 radio(7, 8);  // CE, CSN

//shapes

struct Shape
{
  int rotations[4];
  int x;
  int y;
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
uint8_t BY = 0;
uint8_t BR = 0;
int curRot = 1;
int newShape = 1;

//Function frequencies
unsigned long makeNewShape = 500;
unsigned long moveDown = 250;
unsigned long timeStamp1 = 0;
unsigned long timeStamp2 = 0;
unsigned long timeNow = 0;

//Temp variables
uint8_t temp = 0;
int tempInt = 0;
Shape tempShape;
int BRCount = 0;
int BYCount = 0;

//Position variables
uint8_t yIndex = 0;
uint8_t xIndex = 0;

CRGB leds[NUM_LEDS];

uint8_t ledMatrix[18][12] = {
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

//Creating shapes
uint8_t displayMatrix[18][12];

const int colLen = 8;
uint32_t colors[colLen] = {CRGB::Black, CRGB::Turquoise, CRGB::Blue, CRGB::DarkOrange, CRGB::Yellow, CRGB::Green, CRGB::Purple, CRGB::Red};

Shape stick;
Shape cube;
Shape table;
Shape el;
Shape mirrorEl;
Shape squiggle;
Shape mirrorSquiggle;

Shape getShape(int num){
  switch(num){
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

  randomSeed(millis());

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

void loop() {
  // put your main code here, to run repeatedly:
  //Read the data if available in buffer

  if (radio.available())
  {
    radio.read(&text, sizeof(text));
    val = strtok(text, ",");
    LeftX = atoi(val);
    val = strtok(NULL, ",");
    LeftY = atoi(val);
    val = strtok(NULL, ",");
    RightX = atoi(val);
    val = strtok(NULL, ",");
    RightY = atoi(val);
    val = strtok(NULL, ",");
    pot1Val = atoi(val);
    val = strtok(NULL, ",");
    pot2Val = atoi(val);
    val = strtok(NULL, ",");
    BY = atoi(val);
    val = strtok(NULL, ",");
    BR = atoi(val);
  }

  if ((millis() - timeStamp1) > makeNewShape)  {
    timeStamp1 = millis();
    if (newShape){
      newShape = 0;
      curRot = random(4);
      tempShape = getShape(random(7)); 
      temp = random(1, 8);
      tempShape.x = 2;
      tempShape.y = 4;
    }
    for (uint8_t j = 0; j < 16; j++) {
      if (j % 4 == 0) {
        yIndex++;
      }
      if ((tempShape.rotations[curRot] >> j) & 1) {
        displayMatrix[(j % 4) + tempShape.x][(yIndex - 1) + tempShape.y] = 0;
      }
      else{
        displayMatrix[(j % 4) + tempShape.x][(yIndex - 1) + tempShape.y] = 0;
      }
    }
    yIndex = 0;

    if ((tempShape.x + 1) == 19){
      newShape = 1;
    }

    else{
      tempShape.x++;
    }

    if ((tempShape.x % 4) == 0){
      if ((curRot + 1) == 4){
        curRot = 0;
      }
      else{
        curRot++;
      }
    }

    for (uint8_t j = 0; j < 16; j++) {
      if (j % 4 == 0) {
        yIndex++;
      }
      if ((tempShape.rotations[curRot] >> j) & 1) {
        displayMatrix[(j % 4) + tempShape.x][(yIndex - 1) + tempShape.y] = temp;
      }
      else{
        displayMatrix[(j % 4) + tempShape.x][(yIndex - 1) + tempShape.y] = 0;
      }
    }
    yIndex = 0;
  }

  for (int i = 0; i < 18; i++) {
    for (int j = 0; j < 12; j++) {
      leds[ledMatrix[i][j]] = colors[displayMatrix[i][j]];
    }
  }
  
  FastLED.show();
  FastLED.clear();
}
