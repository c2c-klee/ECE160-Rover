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

/*Set up for Claw command function*/
const int pwm0DegCount = 0.000001 * minPW * SERVO_FREQ * pow(2, pca9685PwmBits);
const int pwm180DegCount = 0.000001 * maxPW * SERVO_FREQ * pow(2, pca9685PwmBits);
const int pwm90DegCount = (pwm0DegCount + pwm180DegCount) / 2;
int angle = angle;
int angleCount;
//Start at angle of 90 degrees to not stress servo
//variable to store count at which PWM will go low
//Create PWM extender object at I2C address 0x40
Adafruit_PWMServoDriver pwmExt = Adafruit_PWMServoDriver(0x40);
void setup() {
  Serial.begin(57600);
  Serial.println("Enter the claw servo angle in degrees from 0-180:");
  // commands to initialize the PCA9685 PWM extender
  pwmExt.begin();
  pwmExt.setOscillatorFrequency(27000000);
  pwmExt.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  pwmExt.setPWM(clawServoNum, ON_COUNT, pwm90DegCount);
}

/*Below is the set up for the bumper function*/
Adafruit_MCP23X17 gpioExt;
int gpioExtPins[6] = { GPA0, GPA1, GPA2, GPA3, GPA4, GPA5 };
void setup() {
  Serial.begin(115200);
  gpioExt.begin_I2C(0x20);
  for (int i = 0; i < 6; i++) {
    gpioExt.pinMode(gpioExtPins[i], INPUT_PULLUP);
  }
  Serial.println("Reading Bumper Switches");
}

int SensorParseData();
  if (SensorParseData == 1);
    return 1;
  } else {
    return 0;
  }
}
void loop() {
  // put your main code here, to run repeatedly:

}
