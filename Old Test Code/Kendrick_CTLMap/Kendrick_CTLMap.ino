// Version 1 - Finalized at X:XX AM/PM on 10/13/2026, by Kendrick Lee
// See GitHub for VC corrections.
// Includes basic drivetrain control.

#include <Adafruit_MCP23X17.h>
#include <Bluepad32.h>

#define GBP0 8
#define GBP1 9

#define nSLPLPin 8
#define nSLPRPin 9
#define rightMotorDIRPin 15
#define rightMotorPWMPin 13
#define leftMotorDIRPin 14
#define leftMotorPWMPin 12
#define motorFreq 500
#define motorPwmBits 8
#define FWD LOW
#define REV HIGH

const int motorPwmMaxCount = pow(2, motorPwmBits);
int pwmGoLowCountLeft;
int pwmGoLowCountRight;
int pwmGoLowCountCenter;
int pwmDutyCycle = 0;

int leftDirection;
int rightDirection;
int xAxis;

// Define i2C Multiplex
Adafruit_MCP23X17 gpioExt;

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void setup() {
  Serial.begin(112500);
  Serial.println("Debug Comms Open");

  // start GPIO interface
  gpioExt.begin_I2C(0x20);
  gpioExt.pinMode(nSLPLPin, OUTPUT);
  gpioExt.pinMode(nSLPRPin, OUTPUT);

  gpioExt.digitalWrite(nSLPLPin, HIGH);  //starts the robot with motors disabled (in sleep mode)
  gpioExt.digitalWrite(nSLPRPin, HIGH);  //starts the robot with motors disabled (in sleep mode)

  pinMode(rightMotorDIRPin, OUTPUT);
  pinMode(leftMotorDIRPin, OUTPUT);
  pinMode(rightMotorPWMPin, OUTPUT);
  pinMode(leftMotorPWMPin, OUTPUT);
  ledcSetup(0, motorFreq, motorPwmBits); 
  ledcAttachPin(rightMotorPWMPin, 0); 
  ledcSetup(2, motorFreq, motorPwmBits); 
  ledcAttachPin(leftMotorPWMPin, 2); 

  BP32.setup(&onConnectedController, &onDisconnectedController);

  BP32.forgetBluetoothKeys();

  BP32.enableVirtualDevice(false);
}

void loop() {
    bool dataUpdated = BP32.update();
    if (dataUpdated)
        processControllers();
  delay(150);
}
