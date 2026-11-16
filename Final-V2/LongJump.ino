/* LongJump.ino
* Description: New function that better adapts the pre-exsting algo for line following for this event
* Author: Kendrick Lee & Braylen Pridemore
* Last Revision: 11/15/2026 @ 7:00PM
* Hardware Used: (x1) Pololu Chassis Kit for TI-RSLK MAX, (x1) Custom Adapter Board for the ESP-32 from TI-RSLK MAX rover, (various) i2C Extension Devices for GPIO & PWM GPIO
* 
* Major Functions: blinkLEDs() - blinkLEDs non-invasively to the rest of the loop via time reading. longJump() - line following and logic for the "jump".
*/

// define variables for time elapsed
unsigned long prevTime = 0;
int blinkTime = 500;
bool ledState = false;

void blinkLEDs() {
  // read current time
  unsigned long currTime = millis();

  // if enough time (defined by blinkTime) has elapsed between readings, then turn on the right LED, while keeping left off. If not enough time
  // has elapsed, then vice versa.
  if (currTime - prevTime >= blinkTime) {
    prevTime = currTime;   // set time offset
    ledState = !ledState;  // change LED state to differ between left or right LED to be on
    if (ledState) {
      gpioExt.digitalWrite(GPB3, HIGH);
      gpioExt.digitalWrite(GPB2, LOW);
    } else {
      gpioExt.digitalWrite(GPB3, LOW);
      gpioExt.digitalWrite(GPB2, HIGH);
    }
  }
}

void longJump() {
  linesCrossed = 0;  // used more in this event, but count how many lines crossed, only read to see if the "jump line" has been crossed.

  // manual override for speed
  pwmDutyCycle = 30;

  // override direction
  digitalWrite(leftMotorDIRPin, FWD);
  digitalWrite(rightMotorDIRPin, FWD);
  Serial.println("This should execute once.");

  while (true) {

    // override motor enables
    gpioExt.digitalWrite(nSLPLPin, HIGH);
    gpioExt.digitalWrite(nSLPRPin, HIGH);

    // read sensor, both for error and to check if the start line has been crossed (by summing all sensors and using that in an inequality)
    pos = qtrArray.readLineWhite(sensorValuesCalibrated);
    combinedSensorValues = (sensorValuesCalibrated[0] + sensorValuesCalibrated[1] + sensorValuesCalibrated[2] + sensorValuesCalibrated[3] + sensorValuesCalibrated[4] + sensorValuesCalibrated[5] + sensorValuesCalibrated[6] + sensorValuesCalibrated[7]);

    // if the first "jump" line has been crossed, then turn off LEDs and stop line following
    if (combinedSensorValues < 750) {
      gpioExt.digitalWrite(GPB3, LOW);
      gpioExt.digitalWrite(GPB2, LOW);
      break;
    }

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

    // if still line following, it should not be jumping, so blinkLEDs.
    blinkLEDs();
  }

  // if jumping, turn lights off initially, and then set speed arbitrarily to 60%
  pwmDutyCycle = 60;
  pwmGoLowCount = 0.01 * pwmDutyCycle * motorPwmMaxCount;
  ledcWrite(0, pwmGoLowCount);
  ledcWrite(2, pwmGoLowCount);

  // Arbitrarily set to 850ms due to time constraints. If more time provided, we would read lines and count to make it more accurate and a closed-loop system.
  delay(850);

  // After delay, TURN OFF motors and then turn LEDS on.
  ledcWrite(0, 0);
  ledcWrite(2, 0);
  gpioExt.digitalWrite(GPB3, HIGH);
  gpioExt.digitalWrite(GPB2, HIGH);
}
