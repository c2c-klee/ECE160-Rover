// Final Codebase.

#include <Adafruit_MCP23X17.h>
#include <Bluepad32.h>

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
int pwmDutyCycle = 0;

int leftDirection;
int rightDirection;

float calibratedMotorFac = 0.9682;

Adafruit_MCP23X17 gpioExt;

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
