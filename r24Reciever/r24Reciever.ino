//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//create an RF24 object
RF24 radio(7, 8);  // CE, CSN

const byte address[6] = "node1";
char data[32] = "";
unsigned long num = 0;
unsigned long temp = 0;

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
  Serial.begin(9600);
  //printf_begin();
  delay(1000);
  radio.begin();

  //set the address
  radio.openReadingPipe(0, address);
  radio.setChannel(0x77);
  radio.setPALevel(RF24_PA_MAX);
  radio.enableDynamicPayloads();
  
  //Set module as receiver
  radio.startListening();
  //radio.printDetails();
}

void loop()
{
  if (radio.available()){
    radio.read(&data, sizeof(data));
    //sscanf(data, "%l", &num);

    Serial.println(data);

    for (uint8_t i = 0; i < 8; i++){
      buttons[i] = (num >> i) & 1;
    }
    for (int i = 1; i < 8; i++){
      pots[i] = (num >> (i*8)) & 255;
    }
    
    buttonTop = buttons[0];
    buttonLeft = buttons[1];
    buttonDown = buttons[2];
    buttonRight = buttons[3];
    bumperLeft = buttons[4];
    bumperRight = buttons[5];
    joyLeftButton = buttons[6];
    joyRightButton = buttons[7];

    leftX = pots[0];
    leftY = pots[1];
    rightX = pots[2];
    rightY = pots[3];
    potLeftVal = pots[4];
    potRightVal = pots[5];
    potVerticalVal = pots[6];
    
    // for (int i = 0; i < 8; i++){
    //   Serial.print(buttons[i]);
    //   Serial.print(", ");
    // }
    // Serial.print("\t\t");
    // for (int i = 0; i < 7; i++){
    //   Serial.print( pots[i]);
    //   Serial.print(", ");
    // }
    // Serial.print("\n");
  }
}