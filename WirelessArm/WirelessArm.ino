//Include Libraries
#include <FABRIK2D.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <math.h>

//#include <printf.h>
#include <Servo.h>
# define GRIPPER 3
# define WRIST 5
# define ELBOW 6
# define BASE 9
# define ROTATE 10 

//create an RF24 object
RF24 radio(7, 8);  // CE, CSN

//Set Servos
Servo Gripper;
Servo Wrist; 
Servo Elbow; 
Servo Base;
Servo Rotate; 

//Arm Lenghts
int lengths[] = {182,180,100};
Fabrik2D fabrik2D(4, lengths);

//Arm Angles
int angles[] = {0,0,0};

//RF Var
const byte address[6] = "node1";

//Input string vars
char text[32] = {0};
char* val;

//Controller Inputs
uint8_t num = 0;
int buttons[8];
int pots[7];
int potLeftVal = 0;
int potRightVal = 0;
int potVerticalVal = 0;
int LeftX = 0;
int LeftY = 0;
int RightX = 0;
int RightY = 0;
int pot1Val = 0;
int pot2Val = 0;
int BY = 0;
int BR = 0;

//Rotation Vars
int Roff = 91;
int GRoff = 93;
int Rsp = 20;

//Speed Vars
int wristPos = 110;
int elbowPos = 170;
int basePos = 35;
int wristLoops = 0;
int elbowLoops = 0;
int baseLoops = 0;
int wristSpeed = 0;
int elbowSpeed = 0;
int baseSpeed = 1;
int wS = 0;
int eS = 0;
int bS = 0;
int spd = 10;
int newElbowPos;
int newBasePos;
int newWristPos;
int yOffset = 0;
int del = 10;

//No connection vars
int noCon = 0;

void setup()
{
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

  Gripper.attach(GRIPPER);
  Elbow.attach(ELBOW);
  Wrist.attach(WRIST);
  Base.attach(BASE);  
  Rotate.attach(ROTATE);
  
  //Start Rotate in not moving position
  Rotate.write(Roff);

  //Move Robot to Home
  
  Base.write(basePos);
  Elbow.write(elbowPos);
  Wrist.write(wristPos);
}

void loop(){
    //Read the data if available in buffer
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

      BR = !buttons[4];
      BY = !buttons[5]; 

      LeftY = map(LeftY, 0, 100, 0, 180);
      RightY = map(RightY, 0, 100, 150, 30);
      LeftX = map(LeftX, 0, 100, 0, 180);
      RightX = map(RightX, 0, 100, 0, 180);

      if (LeftY > 140){
        basePos -= 1;
      }
      else if (LeftY < 40){
        basePos += 1;
      }

      if (buttons[0]){
        wristPos -= 1;
      }
      if (buttons[2]){
        wristPos += 1;
      }
      if (BY){
        if (RightY > 140){
          yOffset += 1;
        }
        else if (RightY < 40){
          yOffset -= 1;
        }
        if (RightY > 140){
          wristPos += 1;
        }
        else if (RightY < 40){
          wristPos -= 1;
        }
      }
      
      yOffset = max(-80, (min(yOffset, 80)));
      basePos = max(0, (min(basePos, 180)));
      wristPos = max(0, (min(wristPos, 180)));

      if (((180 - basePos) - yOffset) < 10){
        yOffset = 170 - basePos;
      }
      elbowPos = (180 - basePos) - yOffset;

      elbowPos = max(10, (min(elbowPos, 170)));

      Rsp = map(potRightVal, 0, 100, 2, 18);
      if (LeftX > 140){
        Rotate.write(Roff - Rsp);
      }
      else if (LeftX < 40){
        Rotate.write(Roff + Rsp);
      }
      else{
        Rotate.write(Roff);
      }

      Elbow.write(elbowPos);
      Wrist.write(wristPos);
      Base.write(basePos);
      Gripper.write(GRoff);

      if (buttons[1]){
        Gripper.write(GRoff - Rsp);
      }
      else if (buttons[3]){
        Gripper.write(GRoff + Rsp);
      }
      else{
        Gripper.write(GRoff);
      }  
      
      noCon = 0;
      del = map(potLeftVal, 0, 100, 40, 8);
      delay(del);
    }
    else{
      noCon++;
      if (noCon >= 10000){
        Rotate.write(Roff);
        noCon = 0;
      } 
    }
}
