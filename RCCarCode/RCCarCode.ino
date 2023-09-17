//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <FastLED.h>
#include <printf.h>
#include <Servo.h>
#include <stdio.h>
#include <stdlib.h>

//Pin defines
#define BATTERY A3
#define PWM 3
#define MC1 5
#define MC2 6
#define SERVO 9

//Control defines
#define DEADZONE 4
#define MAXFORWARD 120
#define MAXBACKWARD 60
#define NORMALFORWARD 100
#define NORMALBACKWARD 80
#define IDLE 90

//Steering defines
#define OFFSET 20
#define STRAIGHT 90
#define TUNING 10

//LED defines
#define LEDS 4
#define MODEL PL9823
#define COLOR RGB
#define NUM_LEDS 6
#define BRIGHTNESS 255

//Battery Stats
#define DEADBATTERY 10.50

//create an RF24 object
RF24 radio(7, 8);  // CE, CSN

//Set Servo
Servo servo;
Servo drive;

//RF Vars
const byte address[6] = "node1";
char data[32] = "";
uint8_t num = 0;
int val = 0;

//I/O Vars
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

//Stat vars
double batt = 100.0;
int turningDir = 0;
int pos = 0;
int spd = 10;
int count = 0;
int loops = 25;
int off = 0;
float value = 0.0;

//Driving vars
int topSpeed = 0;
int bottomSpeed = 0;

//LEDS
CRGB leds[NUM_LEDS];

//Timing vars
int signalCount = 0;
int delayTime = 75;

//Battery proctection var
bool battTooLow = false;

char text[32] = {0};

void setup()
{
  Serial.begin(9600);
  delay(500);

  printf_begin();
  radio.begin();
  
  //set the address
  radio.openReadingPipe(0, address);
  radio.setChannel(0x77);
  radio.setPALevel(RF24_PA_MAX);
  radio.enableDynamicPayloads();

  radio.startListening();
  radio.printDetails();

  //LEDs
  FastLED.addLeds<MODEL, LEDS, COLOR>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  //Actuators
  servo.attach(SERVO);
  drive.attach(PWM);
  drive.write(IDLE);
  pinMode(BATTERY, INPUT);
  servo.write(STRAIGHT);
  leds[0] = CRGB::White;
  leds[1] = CRGB::White;
  leds[2] = CRGB::Maroon;
  leds[3] = CRGB::Maroon;
  leds[4] = CRGB::Maroon;
  leds[5] = CRGB::Maroon;

  //Driving
  topSpeed = MAXFORWARD;
  bottomSpeed = MAXBACKWARD;
}

void loop(){
  batt = analogRead(BATTERY);
  batt = batt * (5.0/1023.0) * 2.4898419864559819413092550790068;
  if (batt <= DEADBATTERY){
    battTooLow = true;
  }
  if (!battTooLow){
    FastLED.show();
    leds[0] = CRGB::White;
    leds[1] = CRGB::White;
    leds[2] = CRGB::Maroon;
    leds[3] = CRGB::Maroon;
    leds[4] = CRGB::Maroon;
    leds[5] = CRGB::Maroon;
      //Read the data if available in buffer
    if (radio.available())
    {
      radio.read(&text, sizeof(text));
      val = strtok(text, ",");
      num = atoi(val);
      val = strtok(NULL, ",");
      leftX = atoi(val);
      val = strtok(NULL, ",");
      leftY = atoi(val);
      val = strtok(NULL, ",");
      rightY = atoi(val);
      val = strtok(NULL, ",");
      rightX = atoi(val);
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

      bumperLeft = buttons[4];
      bumperRight = buttons[5]; 

      //Steering
      rightX = map(rightX, 0, 100, -OFFSET, OFFSET);
      //For tuning the steering
      potLeftVal = map(potLeftVal, 0, 100, -TUNING, TUNING);

      //Write the steering value
      turningDir = rightX + STRAIGHT + potLeftVal;
      servo.write(turningDir);

      //Sets which direction
      if (turningDir < (STRAIGHT - 5)){
        signalCount++;
        if (signalCount <= delayTime){
          leds[2] = CRGB::Orange;
          leds[3] = CRGB::Orange;
        }
        if ((delayTime < signalCount) && (signalCount < (delayTime*2))){
          leds[2] = CRGB::Maroon;
          leds[3] = CRGB::Maroon;
        }
        if (signalCount > (delayTime*2)){
          signalCount = 0;
        }
      }
      if (turningDir > (STRAIGHT + 5)){
        signalCount++;
        if (signalCount <= delayTime){
          leds[4] = CRGB::Orange;
          leds[5] = CRGB::Orange;
        }
        if ((delayTime < signalCount) && (signalCount < (delayTime*2))){
          leds[4] = CRGB::Maroon;
          leds[5] = CRGB::Maroon;
        }
        if (signalCount > (delayTime*2)){
          signalCount = 0;
        }
      }
      if (abs(turningDir - STRAIGHT) < 5){
        signalCount = 0;
        leds[2] = CRGB::Maroon;
        leds[3] = CRGB::Maroon;
        leds[4] = CRGB::Maroon;
        leds[5] = CRGB::Maroon;
      }

      value = map(leftY, 0, 100, topSpeed, bottomSpeed);

      //Adjust the value for the deadzone
      if (value > 90){
        value = value + DEADZONE;
      }
      if (value < 90){
        value = value - DEADZONE;
      }
      drive.write(value);
      sprintf(text, "");
      off = 0;
    }
    else{
      if (off > 50){
        value = 0;
        servo.write(STRAIGHT);
        drive.write(IDLE);
      }
      off++;
    }
  }
  else{
    servo.write(STRAIGHT);
    drive.write(IDLE);
    signalCount++;
    if (signalCount <= delayTime){
      leds[0] = CRGB::Red;
      leds[1] = CRGB::Red;
      leds[2] = CRGB::Red;
      leds[3] = CRGB::Red;
      leds[4] = CRGB::Red;
      leds[5] = CRGB::Red;
    }
    if ((delayTime < signalCount) && (signalCount < (delayTime*2))){
      leds[0] = CRGB::Black;
      leds[1] = CRGB::Black;
      leds[2] = CRGB::Black;
      leds[3] = CRGB::Black;
      leds[4] = CRGB::Black;
      leds[5] = CRGB::Black;
    }
    if (signalCount > (delayTime*2)){
      signalCount = 0;
    }
    FastLED.show();
  }
}
