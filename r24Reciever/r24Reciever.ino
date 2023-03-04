//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include <Servo.h>
#define pwm 3
#define dir1 2
#define dir2 4

//create an RF24 object
RF24 radio(7, 8);  // CE, CSN

//Set Servo
Servo servo;

const byte address[6] = "node1";

char* val;
char output[60];
int LeftX = 0;
int LeftY = 0;
int RightX = 0;
int RightY = 0;
int pot1Val = 0;
int pot2Val = 0;
int BY = 0;
int BR = 0;
int pos = 0;
int spd = 10;
int count = 0;
int loops = 25;
int off = 0;

float value = 0.0;

char text[32] = {0};

void setup()
{
  delay(500);
  Serial.begin(9600);
  printf_begin();
  radio.begin();
  
  //set the address
  radio.openReadingPipe(0, address);
  radio.setChannel(0x77);
  radio.setPALevel(RF24_PA_MAX);
  radio.enableDynamicPayloads();
  
  //Set module as receiver
  radio.startListening();
  radio.printDetails();

 servo.attach(9);
}

void loop(){
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
      
      Serial.println("YAY!");
    }
}
