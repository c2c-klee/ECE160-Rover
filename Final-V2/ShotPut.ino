/* ShotPut.ino
* Description: File for the entirety of the shotput event.
* Author: Kendrick Lee & Braylen Pridemore
* Last Revision: 11/15/2026 @ 7:00PM
* Hardware Used: (x1) Pololu Chassis Kit for TI-RSLK MAX, (x1) Custom Adapter Board for the ESP-32 from TI-RSLK MAX rover, (various) i2C Extension Devices for GPIO & PWM GPIO
* 
* Major Functions: readEncoders() - read the instantaneous hall-effect encoders and add or subtract to the total encoder revolutions count depending on direction.
*   shotPut() - spin until enough revolutions have passed, and then open claw.
*/

void readEncoders() {
  // read hall-effect encoders
  encoderRdir = digitalRead(encoderRAPin);
  encoderLdir = digitalRead(encoderLAPin);
  
  // if spinning in a positive direction, reduce total revolutions in the right encoder and add total revolutions in left motor
  if (encoderRdir == 1) {
    encoderRcount--;
  } else {
    encoderRcount++;
  }
  // if spinning in a negative direction, reduce total revolutions in the left encoder and add total revolutions in right motor
  if (encoderLdir == 1) {
    encoderLcount--;
  } else {
    encoderLcount++;
  }
  encoderCount = encoderRcount + encoderLcount;
}


void shotPut() {
  // set speed to the maximum.
  pwmDutyCycle = 100;
  
  // override to enable motors.
  gpioExt.digitalWrite(nSLPRPin, HIGH);
  gpioExt.digitalWrite(nSLPLPin, HIGH);
  
  // reset encoderCount, this should be deprecated as readEncoders overrides this.
  encoderCount = 0;

  while (true) {
    // find encoder data.
    readEncoders();
    Serial.println(encoderCount);
    
    // if enough revolutons have been passed past the threshold (defined in ParseControllers.ino), break out of loop
    if (encoderCount > currentEncoderCount) {
      break;
    }
    
    // spin within this loop
    pwmGoLowCount = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    ledcWrite(0, pwmGoLowCount);
    ledcWrite(2, pwmGoLowCount);
    digitalWrite(leftMotorDIRPin, REV);
    digitalWrite(rightMotorDIRPin, FWD);
  }

  // if broken out of loop, open claw, then come to a sudden stop.
  angleCount = map(angleStart, 0, 180, pwm0DegCount, pwm180DegCount);
  pwmExt.setPWM(clawServoNum, ON_COUNT, angleCount);
  ledcWrite(0, 0);
  ledcWrite(2, 0);
}
