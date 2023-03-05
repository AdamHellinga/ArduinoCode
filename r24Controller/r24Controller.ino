//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <stdint.h>

#define ButU 6
#define ButD 3
#define ButL 5
#define ButR 4
#define BumpL 8
#define BumpR 9
#define joyButL 2
#define joyButR A7
#define LX A6
#define LY A5
#define RX A4
#define RY A3
#define potL A0
#define potR A1
#define potV A2

//create an RF24 object
RF24 radio(7, 10);  // CE, CSN

const byte address[6] = "node1";
char data[32] = "";
int temp = 0;
int buttonNum = 0;
int potNum = 0;

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

void setup()
{
  delay(1000);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setChannel(0x77);
  radio.setPALevel(RF24_PA_HIGH);
  radio.enableDynamicPayloads();
  pinMode(ButU, INPUT);
  pinMode(ButD, INPUT);
  pinMode(ButL, INPUT);
  pinMode(ButR, INPUT);
  pinMode(BumpL, INPUT);
  pinMode(BumpR, INPUT);
  pinMode(joyButL, INPUT);
  pinMode(joyButR, INPUT);
  pinMode(LX, INPUT);
  pinMode(LY, INPUT);
  pinMode(RX, INPUT);
  pinMode(RY, INPUT);
  pinMode(potL, INPUT);
  pinMode(potR, INPUT);
  pinMode(potV, INPUT);
}

void loop()
{
  buttonNum = 0;
  potNum = 0;
  temp = 0;

  buttonTop = digitalRead(ButU);
  buttonLeft = digitalRead(ButL);
  buttonDown = digitalRead(ButD);
  buttonRight = digitalRead(ButR);
  bumperLeft = digitalRead(BumpL);
  bumperRight = digitalRead(BumpR);
  joyLeftButton = digitalRead(joyButL);  
  joyRightButton = analogRead(joyButR);

  if (joyRightButton > 10){
    joyRightButton = 1;
  }
  else{
    joyRightButton = 0;
  }

  leftX = analogRead(LX);
  leftY = analogRead(LY);
  rightX = analogRead(RX);
  rightY = analogRead(RY);
  potLeftVal = analogRead(potL);
  potRightVal = analogRead(potR);
  potVerticalVal = analogRead(potV);

  buttons[0] = buttonTop;
  buttons[1] = buttonLeft;
  buttons[2] = buttonDown;
  buttons[3] = buttonRight;
  buttons[4] = bumperLeft;
  buttons[5] = bumperRight;
  buttons[6] = joyLeftButton;
  buttons[7] = joyRightButton;  

  pots[0] = leftX;
  pots[1] = leftY;
  pots[2] = rightX;
  pots[3] = rightY;
  pots[4] = potLeftVal;
  pots[5] = potRightVal;
  pots[6] = potVerticalVal;

  for (uint8_t i = 0; i < 8; i++){ 
      buttonNum = (buttons[i] << i) | (buttonNum & 0xFFFFFFFFFFFFFFFF);
  }
  
  sprintf(data, "%d,%d,%d,%d,%d,%d,%d,%d,%d", buttonNum, leftX, leftY, rightX, rightY, potLeftVal, potRightVal, potVerticalVal, buttons[4]);
  radio.write(&data, sizeof(data));
  delay(1);
}
