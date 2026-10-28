#include <Adafruit_MCP23X17.h>
#define GPB0 8
#define GPB1 9
//This is what nSLPL is connected to
//This is what nSLPR is connected to
//Associate the GPB0 and GPB1 pins with the nSLP signal for the motors
#define nSLPLPin GPB0
#define nSLPRPin GPB1
#define rightMotorDIRPin 15
#define rightMotorPWMPin 13
#define leftMotorDIRPin 14
#define leftMotorPWMPin 12
#define motorFreq 500
#define motorPwmBits 8
#define FWD LOW
#define REV HIGH
// set motor PWM frequency to 100 Hz
// use 8 bit counter for motor PWM control
// Set DIR bit to 0 to move the robot in direction opposite claw (forward)
// Set DIR bit to 1 to move the robot in direction of claw (reverse)
const int motorPwmMaxCount = pow(2, motorPwmBits);
int pwmGoLowCount;
int pwmDutyCycle = 0;
//Count at which the motor PWM pulse goes low
//Duty cycle as percentage from 0-100
Adafruit_MCP23X17 gpioExt;
void setup() {
  Serial.begin(57600);
  Serial.println("Enter the motor duty cycle as a percentage from 0-100:");
  gpioExt.begin_I2C(0x20);
  gpioExt.pinMode(nSLPLPin, OUTPUT);
  gpioExt.pinMode(nSLPRPin, OUTPUT);
  //0x20 is the I2C address for the GPIO Extender
  gpioExt.digitalWrite(nSLPLPin, LOW);  //starts the robot with motors disabled (in sleep mode)
  gpioExt.digitalWrite(nSLPRPin, LOW);  //starts the robot with motors disabled (in sleep mode)
  pinMode(rightMotorDIRPin, OUTPUT);
  pinMode(leftMotorDIRPin, OUTPUT);
  pinMode(rightMotorPWMPin, OUTPUT);
  pinMode(leftMotorPWMPin, OUTPUT);
  ledcAttach(rightMotorPWMPin, motorFreq, motorPwmBits);
  ledcAttach(leftMotorPWMPin, motorFreq, motorPwmBits);
}
void loop() {
  if (Serial.available() >= 1) {
    pwmDutyCycle = Serial.parseInt();
    Serial.print("The duty cycle is: ");
    Serial.println(pwmDutyCycle);
  }
  if ((pwmDutyCycle >= 0) && (pwmDutyCycle <= 100)) {
    gpioExt.digitalWrite(nSLPLPin, HIGH);
    gpioExt.digitalWrite(nSLPRPin, HIGH);
    digitalWrite(rightMotorDIRPin, REV);
    digitalWrite(leftMotorDIRPin, REV);
    pwmGoLowCount = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    ledcWrite(rightMotorPWMPin, pwmGoLowCount);
    ledcWrite(leftMotorPWMPin, pwmGoLowCount);
  } else {
    Serial.println("You entered an invalid Duty Cycle: setting it to 0");
    pwmDutyCycle = 0;
    gpioExt.digitalWrite(nSLPLPin, LOW);  //disables the motor (in sleep mode)
    gpioExt.digitalWrite(nSLPRPin, LOW);  //disables the motor (in sleep mode)
  }
}