/*Here is the definitions for the set up data of bumper function*/
#include <Adafruit_MCP23X17.h>
#define GPA0 0
#define GPA1 1
#define GPA2 2
#define GPA3 3
#define GPA4 4
#define GPA5 5

/*Definitions for claw set up*/
#include <Adafruit_PWMServoDriver.h>
#define pca9685PwmBits 12
#define minPW 500   // min pulse width in microseconds to make servo at 0 degrees
#define maxPW 2500  // max pulse width in microseconds to make servo at 180
#define SERVO_FREQ 50   // Analog servos run at ~50 Hz updates
#define clawServoNum 0  // this is servo number 0 controller on the PCA9685
#define ON_COUNT 0

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
void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
