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
uint8_t num = 0;
int val = 0;

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
int temp = 0;

int length = 0;
int theta = 0;
int offset = 0.78539816;  //Radians

void setup()
{
  Serial.begin(9600);
  delay(1000);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setChannel(0x77);
  radio.setPALevel(RF24_PA_HIGH);
  radio.enableDynamicPayloads();
  radio.startListening();
}

void loop()
{
  if (radio.available()){
    radio.read(&data, sizeof(data));

    val = strtok(data, ",");
    num = atoi(val);
    val = strtok(NULL, ",");
    pots[0] = atoi(val);
    val = strtok(NULL, ",");
    pots[1] = atoi(val);
    val = strtok(NULL, ",");
    pots[3] = atoi(val);
    val = strtok(NULL, ",");
    pots[2] = atoi(val);
    val = strtok(NULL, ",");
    pots[4] = atoi(val);
    val = strtok(NULL, ",");
    pots[5] = atoi(val);
    val = strtok(NULL, ",");
    pots[6] = atoi(val);

    for (uint8_t i = 0; i < 8; i++){
      buttons[i] = (num >> i) & 1;
      if (i > 5){
        buttons[i] = !buttons[i];
      }
    }

    temp = buttons[7];
    buttons[7] = buttons[6];
    buttons[6] = temp;
    
    buttonTop = buttons[0];
    buttonLeft = buttons[1];
    buttonDown = buttons[2];
    buttonRight = buttons[3];
    bumperLeft = buttons[4];
    bumperRight = buttons[5];
    joyRightButton = buttons[6];
    joyLeftButton = buttons[7];

    leftY = pots[0];
    leftX = pots[1];
    rightX = pots[2];
    rightY = pots[3];
    potLeftVal = pots[4];
    potRightVal = pots[5];
    potVerticalVal = pots[6];
    
    for (int i = 0; i < 8; i++){
      Serial.print(buttons[i]);
      Serial.print(", ");
    }
    Serial.print("\t");
    for (int i = 0; i < 7; i++){
      Serial.print(pots[i]);
      Serial.print(", ");
    }
    Serial.print("\n");
  }
}