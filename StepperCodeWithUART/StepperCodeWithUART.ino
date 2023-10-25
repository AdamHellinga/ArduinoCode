#include <TMCStepper.h>         // TMCstepper - https://github.com/teemuatlut/TMCStepper
#include <SoftwareSerial.h>     // Software serial for the UART to TMC2209 - https://www.arduino.cc/en/Reference/softwareSerial
#include <Streaming.h>          // For serial debugging output - https://www.arduino.cc/reference/en/libraries/streaming/

#define EN_PIN           10     // Enable - PURPLE
#define DIR_PIN          2      // Direction - WHITE
#define STEP_PIN         4      // Step - ORANGE
#define SW_SCK           5      // Software Slave Clock (SCK) - BLUE
#define SW_TX            6      // SoftwareSerial receive pin - BROWN
#define SW_RX            7      // SoftwareSerial transmit pin - YELLOW
#define DRIVER_ADDRESS   0b00   // TMC2209 Driver address according to MS1 and MS2
#define R_SENSE 0.11f           // SilentStepStick series use 0.11 ...and so does my fysetc TMC2209 (?)

#define OPEN             3      //Button to open the window
#define CLOSE            8      //Button to clolse the window
#define CLOSELIMIT       12
#define OPENLIMIT        11

SoftwareSerial SoftSerial(SW_RX, SW_TX);                          // Be sure to connect RX to TX and TX to RX between both devices

TMC2209Stepper TMCdriver(&SoftSerial, R_SENSE, DRIVER_ADDRESS);   // Create TMC driver

int accel = 10000;
int deccel = 20000;
long maxSpeed = 500000;
long currentSpeed = 0;
unsigned long openClick = 0;
unsigned long closeClick = 0;
unsigned long lastOpenClick = 0;
unsigned long lastCloseClick = 0;
unsigned long noClick = 0;
unsigned long threshold = 250;
int speedChangeDelay;
bool dir = false;
bool first = true;
bool spinUp = true;
bool spinDown = false;
bool moving = false;
bool stop = false;
bool autoOpen = false;
bool autoClose = false;
int openWindow = 0;
int closeWindow = 0;
int openLimit = 0;
int closeLimit = 0;

long lastClicks[2][5] = {{0, 0, 0, 0, 0},{0, 0, 0, 0, 0}};

//== Setup ===============================================================================

void setup() {
  Serial.begin(9600);               // initialize hardware serial for debugging
  SoftSerial.begin(11520);           // initialize software serial for UART motor control
  TMCdriver.beginSerial(11520);      // Initialize UART
  
  pinMode(EN_PIN, OUTPUT);           // Set pinmodes
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(OPEN, INPUT_PULLUP);
  pinMode(CLOSE, INPUT_PULLUP);
  pinMode(OPENLIMIT, INPUT_PULLUP);
  pinMode(CLOSELIMIT, INPUT_PULLUP);
  digitalWrite(EN_PIN, LOW);         // Enable TMC2209 board  

  TMCdriver.begin();                                                                                                                                                                                                                                                                                                                            // UART: Init SW UART (if selected) with default 115200 baudrate
  TMCdriver.toff(5);                 // Enables driver in software
  TMCdriver.rms_current(1500);        // Set motor RMS current
  TMCdriver.microsteps(256);         // Set microsteps

  TMCdriver.en_spreadCycle(false);
  TMCdriver.pwm_autoscale(true);     // Needed for stealthChop
}

//== Loop =================================================================================

void loop() {
  openWindow  = !(digitalRead(OPEN));
  closeWindow = !(digitalRead(CLOSE));
  openLimit   = !(digitalRead(OPENLIMIT));
  closeLimit  = !(digitalRead(CLOSELIMIT));

  if ((lastClicks[0][1] == 1) && (lastClicks[0][2] == 3) && (lastClicks[0][3] == 1)){
    if (abs(openClick - lastOpenClick) <= threshold){
      for (int i = 0; i < 5; i++){
        lastClicks[0][i] = 0;
      }
      if (!autoClose){
        autoOpen = !autoOpen;
      }
    }
  }

  if ((lastClicks[0][1] == 2) && (lastClicks[0][2] == 3) && (lastClicks[0][3] == 2)){
    if (abs(closeClick - lastCloseClick) <= threshold){
      for (int i = 0; i < 5; i++){
        lastClicks[0][i] = 0;
      }
      if (!autoOpen){
        autoClose = !autoClose;
      }
    }
  }

  if (moving){
    spinUp = false;
    if (openLimit || closeLimit){
      stop = true;
    }
  }
  else{
    spinUp = true;
  }

  if (openWindow || autoOpen){
    if (openWindow && (lastClicks[0][0] != 1)){
      lastOpenClick = openClick;
      openClick = millis();
      for (int i = 0; i < 2; i++){
        for (int j = 4; j >= 0; j--){
          lastClicks[i][j+1] = lastClicks[i][j];
        }
      }
      lastClicks[0][0] = 1;
    }
    if (!openLimit){
      moving = true;
      TMCdriver.shaft(!dir); // SET DIRECTION
      if (spinUp){
        for (long i = 0; i <= maxSpeed; i = i + accel){             // Speed up to maxSpeed
          openWindow = !(digitalRead(OPEN));
          openLimit = !(digitalRead(OPENLIMIT));
          closeLimit = !(digitalRead(CLOSELIMIT));
          if ((openWindow || autoOpen) && (!openLimit)){
            TMCdriver.VACTUAL(i);                                   // Set motor speed
            //Serial << TMCdriver.VACTUAL() << endl;
            delay(50);
            currentSpeed = i;
          }
          else{
            i = maxSpeed;
          }
        }
        spinUp = false;
      }
    }
  }

  if (closeWindow || autoClose){
    if (closeWindow && (lastClicks[0][0] != 2)){
      lastCloseClick = closeClick;
      closeClick = millis();
      for (int i = 0; i < 2; i++){
        for (int j = 4; j >= 0; j--){
          lastClicks[i][j+1] = lastClicks[i][j];
        }
      }
      lastClicks[0][0] = 2;
    }
    if (!closeLimit){
      moving = true;
      TMCdriver.shaft(dir); // SET DIRECTION
      if (spinUp){
        for (long i = 0; i <= maxSpeed; i = i + accel){             // Speed up to maxSpeed
          closeWindow = !(digitalRead(CLOSE));
          openLimit = !(digitalRead(OPENLIMIT));
          closeLimit = !(digitalRead(CLOSELIMIT));
          if ((closeWindow || autoClose) && (!closeLimit)){
            TMCdriver.VACTUAL(i);                                     // Set motor speed
            //Serial << TMCdriver.VACTUAL() << endl;
            delay(50);
            currentSpeed = i;
          }
          else{
            i = maxSpeed;
          }
        }
        spinUp = false;
      }
    }
  } 

  if (!(closeLimit || openLimit)){
    if (!(closeWindow || openWindow || autoOpen || autoClose)){
      noClick = millis();
      if (lastClicks[0][0] != 3){
        for (int i = 0; i < 2; i++){
          for (int j = 4; j >= 0; j--){
            lastClicks[i][j+1] = lastClicks[i][j];
          }
        }
        lastClicks[0][0] = 3;
      }

      if (moving){
        spinDown = true;
      }
      if (spinDown){
        for (long i = currentSpeed; i >=0; i = i - deccel){              // Decrease speed to zero
          openLimit = !(digitalRead(OPENLIMIT));
          closeLimit = !(digitalRead(CLOSELIMIT));
          if (!(closeLimit || openLimit)){
            TMCdriver.VACTUAL(i);
            //Serial << TMCdriver.VACTUAL() << endl;
            delay(50);
          }
          else{
            i = 0;
          }
        }
        TMCdriver.VACTUAL(0);
        currentSpeed = 0;
        spinDown = false;
      }
      moving = false;
    }
    else{
      if (autoClose || autoOpen){
        noClick = millis();
        if (lastClicks[0][0] != 3){
          for (int i = 0; i < 2; i++){
            for (int j = 4; j >= 0; j--){
              lastClicks[i][j+1] = lastClicks[i][j];
            }
          }
          lastClicks[0][0] = 3;
        }
      }
    }
  }
  else{
    if (!(closeWindow || openWindow || autoOpen || autoClose)){
      noClick = millis();
      if (lastClicks[0][0] != 3){
        for (int i = 0; i < 2; i++){
          for (int j = 4; j >= 0; j--){
            lastClicks[i][j+1] = lastClicks[i][j];
          }
        }
        lastClicks[0][0] = 3;
      }
    }
    if (stop){
      TMCdriver.VACTUAL(0);
      moving = false;
      spinDown = false;
      stop = false;
      currentSpeed = 0;
      if (closeLimit){
        autoClose = false;
      }
      if (openLimit){
        autoOpen = false;
      }
    }
  }
  Serial.print(autoOpen);
  Serial.print(autoClose);
  Serial.print("\n");
}