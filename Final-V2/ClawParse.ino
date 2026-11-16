/* ClawParse.ino
* Description: Functions for opening and closing the claw.
* Author: Kendrick Lee & Braylen Pridemore
* Last Revision: 11/15/2026 @ 7:00PM
* Hardware Used: (x1) Pololu Chassis Kit for TI-RSLK MAX, (x1) Custom Adapter Board for the ESP-32 from TI-RSLK MAX rover, (various) i2C Extension Devices for GPIO & PWM GPIO
* 
* Major Functions: openClaw() - Open the claw based on a set angle. closeClaw() - Close the claw based on a set angle.
*/


void openClaw() {
  // map actual angle provided between 0-180. We did not use constrain since we knew our values were between 0-180.
  angleCount = map(angleStart, 0, 180, pwm0DegCount, pwm180DegCount);
  
  // write angle data to the servo motor.
  pwmExt.setPWM(clawServoNum, ON_COUNT, angleCount);
}

void closeClaw() {
  // map actual angle provided between 0-180. We did not use constrain since we knew our values were between 0-180.
  angleCount = map(angleBall, 0, 180, pwm0DegCount, pwm180DegCount);

  // write angle data to the servo motor.
  pwmExt.setPWM(clawServoNum, ON_COUNT, angleCount);
}
