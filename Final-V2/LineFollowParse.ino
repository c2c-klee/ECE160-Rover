/* LineFollowParse.ino
* Description: Line following algorithm for hurdles.
* Author: Kendrick Lee & Braylen Pridemore
* Last Revision: 11/15/2026 @ 7:00PM
* Hardware Used: (x1) Pololu Chassis Kit for TI-RSLK MAX, (x1) Custom Adapter Board for the ESP-32 from TI-RSLK MAX rover, (various) i2C Extension Devices for GPIO & PWM GPIO
* 
* Major Functions: followLine() - interpret line-follower data and calculate error, then propogate error to motor speed to control direction. checkLED() - separate function to enable or disable
*  LED blinking functionality, checks if a white line was crossed so that an LED can blink.
*/

void followLine() {

  linesCrossed = 0;          // counts lines crossed, only checked to see if first start line has been crossed in this instance
  pwmDutyCycle = 28;         // manually defined speed override
  firstLineCrossed = false;  // ensures that rover does not stop after first line

  // override to make sure motors drive forward.
  digitalWrite(leftMotorDIRPin, FWD);
  digitalWrite(rightMotorDIRPin, FWD);
  Serial.println("This should execute once.");

  // while(true) logic implemented so that a stray button press does not affect the event
  while (true) {
    // enable motors
    gpioExt.digitalWrite(nSLPLPin, HIGH);
    gpioExt.digitalWrite(nSLPRPin, HIGH);

    // read sensor, both for error and to check if the start line has been crossed (by summing all sensors and using that in an inequality)
    pos = qtrArray.readLineWhite(sensorValuesCalibrated);
    combinedSensorValues = (sensorValuesCalibrated[0] + sensorValuesCalibrated[1] + sensorValuesCalibrated[2] + sensorValuesCalibrated[3] + sensorValuesCalibrated[4] + sensorValuesCalibrated[5] + sensorValuesCalibrated[6] + sensorValuesCalibrated[7]);

    // check if start-line / finish-line have been crossed, and then execute code accordingly
    // Finish Line condition
    if (combinedSensorValues < 750 && linesCrossed >= 1) {
      gpioExt.digitalWrite(GPB3, LOW);
      gpioExt.digitalWrite(GPB2, LOW);
      break;

      // Old code from when Long Jump used this function for line following
    } else if (manualLineOverride == true && firstLineCrossed == true) {
      break;

      // Start line condition
    } else if (combinedSensorValues < 750) {
      // override sensor readings for the checkLED function so that LEDs do not blink
      leftSensorValues = 1000;
      rightSensorValues = 1000;
      gpioExt.digitalWrite(GPB3, LOW);
      gpioExt.digitalWrite(GPB2, LOW);

      // slight delay so that start line is not double checked
      delay(150);
      linesCrossed++;
      firstLineCrossed = true;

      // if not on a solid line, then check LEDs as normal.
    } else {
      checkLED();
    }

    // read left/right sensors to check if an LED should be blinked
    leftSensorValues = sensorValuesCalibrated[0];
    rightSensorValues = sensorValuesCalibrated[7];

    // math to map error / direction deviation data from sensor to motor speed
    error = (pos - 3500) * turnFix;

    // this factor of 160 is arbitrary, but determines the severity of how much it should turn when faced with a non-zero error.
    //   this factor also is heavily dependant on what the root "maximum" speed (pwmDutyCycle) is.
    leftSpeed = constrain(pwmDutyCycle + error / 160, -pwmDutyCycle, pwmDutyCycle);
    rightSpeed = constrain(pwmDutyCycle - error / 160, -pwmDutyCycle, pwmDutyCycle);
    pwmGoLowCountLeft = 0.01 * leftSpeed * motorPwmMaxCount;
    pwmGoLowCountRight = 0.01 * rightSpeed * motorPwmMaxCount * calibratedMotorFac;

    Serial.println(linesCrossed);

    // write speed data to motors.
    ledcWrite(0, pwmGoLowCountLeft);
    ledcWrite(2, pwmGoLowCountRight);
  }

  // if while loop is broken due to a certain condition (ie. finish line), wait 750ms and then stop robot.
  delay(750);
  ledcWrite(0, 0);
  ledcWrite(2, 0);
}


void checkLED() {
  // old code used for LongJump's blinking lights.
  if (manualLineOverride == true) {
    blinkLEDs();
  }

  // failsafe to ensure that lights do not blink if the start line is crossed.
  else if (firstLineCrossed == 1) {
    gpioExt.digitalWrite(GPB3, LOW);
    gpioExt.digitalWrite(GPB2, LOW);
  }
  
  // if during hurdles it encounters a "hurdle", or 2 spaced white lines, blink LEDs.
  if (leftSensorValues < 375 || rightSensorValues < 375) {
    rightSpeed = pwmDutyCycle;
    leftSpeed = pwmDutyCycle;
    gpioExt.digitalWrite(GPB2, HIGH);
    gpioExt.digitalWrite(GPB3, HIGH);
  } else {
    gpioExt.digitalWrite(GPB3, LOW);
    gpioExt.digitalWrite(GPB2, LOW);
  }
}