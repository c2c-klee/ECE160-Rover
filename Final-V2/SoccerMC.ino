/* SoccerMC.ino
* Description: Manual Control for soccer, based on "tank"-style controls.
* Author: Kendrick Lee & Braylen Pridemore
* Last Revision: 11/15/2026 @ 7:00PM
* Hardware Used: (x1) Pololu Chassis Kit for TI-RSLK MAX, (x1) Custom Adapter Board for the ESP-32 from TI-RSLK MAX rover, (various) i2C Extension Devices for GPIO & PWM GPIO
* 
* Major Functions: soccerParseL() - read left controller joystick data and write corresponding motor speed / direction on left side, 
*  soccerParseR() - read right controller joystick data and write corresponding motor speed / direction on right side
*/

void soccerParseL(ControllerPtr ctl) {
  
  // if joystick is in the "FWD" direction
  if (ctl->axisY() >= 10) {
    // if forward, will always be positive, so map as a percentage of how "forward" the stick is
    pwmDutyCycleLeft = (map(ctl->axisY(), 0, 512, 0, 100));
    
    // enable motor, tell driver to drive motor fwd
    gpioExt.digitalWrite(nSLPLPin, HIGH);
    digitalWrite(leftMotorDIRPin, FWD);
    
    // write data to motor
    pwmGoLowCountLeft = 0.01 * pwmDutyCycleLeft * motorPwmMaxCount;
    ledcWrite(0, pwmGoLowCountLeft);

  // if joystick is in the "REV" direction
  } else if (ctl->axisY() <= -10) {
    // if backward, will always be negative, so map as a percentage of how "backward" the stick is
    pwmDutyCycleLeft = (map(ctl->axisY(), 0, -512, 0, 100));
    
    // enable motor, tell driver to drive motor rev
    gpioExt.digitalWrite(nSLPLPin, HIGH);
    digitalWrite(leftMotorDIRPin, REV);

    // write data to motor
    pwmGoLowCountLeft = 0.01 * pwmDutyCycleLeft * motorPwmMaxCount;
    ledcWrite(0, pwmGoLowCountLeft);
  
  // if joystick is "centered" between a certain threshold, stop the motor to prevent drift
  } else{
      ledcWrite(0, 0);
    }
  
  // check if right thumb-switch is pressed, and if so, exit manual control.
  if (ctl->thumbR()) {
    manualMode = false;
  }
}

void soccerParseR(ControllerPtr ctl) {
  
  // if joystick is in the "FWD" direction
  // same code as above, but mapped for RIGHT.

  if (ctl->axisRY() >= 10) {
    pwmDutyCycleRight = (map(ctl->axisRY(), 0, 512, 0, 100));
    gpioExt.digitalWrite(nSLPRPin, HIGH);
    digitalWrite(rightMotorDIRPin, FWD);
    pwmGoLowCountRight = 0.01 * pwmDutyCycleRight * motorPwmMaxCount;
    ledcWrite(2, pwmGoLowCountRight);
  } else if (ctl->axisRY() <= -10) {
    pwmDutyCycleRight = (map(ctl->axisRY(), 0, -512, 0, 100));
    gpioExt.digitalWrite(nSLPRPin, HIGH);
    digitalWrite(rightMotorDIRPin, REV);

    pwmGoLowCountRight = 0.01 * pwmDutyCycleRight * motorPwmMaxCount;
    ledcWrite(2, pwmGoLowCountRight);
  
  // if joystick is "centered" between a certain threshold, stop the motor to prevent drift
  } else {
      ledcWrite(2, 0);
    }
}