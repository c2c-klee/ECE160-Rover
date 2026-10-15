#include <Adafruit_PWMServoDriver.h>
#define pca9685PwmBits 12
#define minPW 500   // min pulse width in microseconds to make servo at 0 degrees
#define maxPW 2500  // max pulse width in microseconds to make servo at 180
#define SERVO_FREQ 50   // Analog servos run at ~50 Hz updates
#define clawServoNum 0  // this is servo number 0 controller on the PCA9685
#define ON_COUNT 0
  // the PWM signal will go high at a count value of 0
  const int pwm0DegCount = 0.000001 * minPW * SERVO_FREQ * pow(2, pca9685PwmBits);
const int pwm180DegCount = 0.000001 * maxPW * SERVO_FREQ * pow(2, pca9685PwmBits);
const int pwm90DegCount = (pwm0DegCount + pwm180DegCount) / 2;
int angle = 90;
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
void loop() {
  if (Serial.available() > 1) {
    angle = Serial.parseInt();
    Serial.print("The angle is: ");
    Serial.println(angle);
  }
  if ((angle >= 0) && (angle <= 180)) {
    angleCount = map(angle, 0, 180, pwm0DegCount, pwm180DegCount);
    pwmExt.setPWM(clawServoNum, ON_COUNT, angleCount);
  } else {
    Serial.println("You entered an invalid angle: setting it to 90 degrees");
    pwmExt.setPWM(clawServoNum, ON_COUNT, pwm90DegCount);
  }
}
 /// Max amount to shut claw is 148
 /// Map and COnstrain could be added 