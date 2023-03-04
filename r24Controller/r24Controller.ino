//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>

#define ButU 6
#define ButD 3
#define ButL 5
#define ButR 4
//#define BumpL 10
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
RF24 radio(7, 8);  // CE, CSN

const byte address[6] = "node1";
char num[32] = "";
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
bool test = true;

void setup()
{
  Serial.begin(9600);
  printf_begin();
  delay(1000);
  radio.begin();

  //erial.println(test);
  
  radio.openWritingPipe(address);

  //set the address
  radio.setChannel(0x77);
  radio.setPALevel(RF24_PA_LOW);
  radio.enableDynamicPayloads();
  //radio.printDetails();
  //Serial.println("Transmitter ok");
  
  pinMode(ButU, INPUT);
  pinMode(ButD, INPUT);
  pinMode(ButL, INPUT);
  pinMode(ButR, INPUT);
 // pinMode(BumpL, INPUT);
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
  //Send message to receiver
  buttonTop = digitalRead(ButU);
  buttonLeft = digitalRead(ButL);
  buttonDown = digitalRead(ButD);
  buttonRight = digitalRead(ButR);
  //bumperLeft = digitalRead(BumpL);
  bumperRight = digitalRead(BumpR);
  joyLeftButton = digitalRead(joyButL);
  joyRightButton = analogRead(joyButR);
  leftX = analogRead(LX);
  leftY = analogRead(LY);
  rightX = analogRead(RX);
  rightY = analogRead(RY);
  potLeftVal = analogRead(potL);
  potRightVal = analogRead(potR);
  potVerticalVal = analogRead(potV);

  sprintf(num, "%d,%d,%d,%d,%d", buttonTop, buttonLeft, buttonDown, buttonRight, bumperRight);
  //Serial.println(num);
  
  radio.write(&num, sizeof(num));
  delay(5);
}
