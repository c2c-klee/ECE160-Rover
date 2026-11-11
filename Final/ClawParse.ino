<<<<<<< Updated upstream
void SetClawPositionParse(ControllerPtr ctl) { 
  bool foundController = true;
  if (ctl->x()){ 
      angleCount = map(angleStart, 0, 180, pwm0DegCount, pwm180DegCount);
      pwmExt.setPWM(clawServoNum, ON_COUNT, angleCount);
    }
  else if (ctl->b()) {
      angleCount = map(angleBall, 0, 180, pwm0DegCount, pwm180DegCount);
      pwmExt.setPWM(clawServoNum, ON_COUNT, angleCount);
    }
  else {
    }
}


bool spinning = false;
int spinTime = 0;
void ShotPutParseData(ControllerPtr ctl) {
  gpioExt.digitalWrite(nSLPRPin, HIGH);
  gpioExt.digitalWrite(nSLPLPin, HIGH);

  if (!spinning && ctl->y()) {
    pwmDutyCycle = 100;
    pwmGoLowCount = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    spinning = true;
    ledcWrite(0, pwmGoLowCount);
    ledcWrite(2, pwmGoLowCount);
    digitalWrite(leftMotorDIRPin, REV);
    digitalWrite(rightMotorDIRPin, FWD);

    delay(3000);

    angleCount = map(angleStart, 0, 148, pwm0DegCount, pwm180DegCount);
    pwmExt.setPWM(clawServoNum, ON_COUNT, angleCount);

    ledcWrite(0, 0);
    ledcWrite(2, 0);
  }
  else {
    spinning = false;
  }
}

/*
void setServoPosition(int angle) {
  angle = constrain(angle, 0, 148);
  if ((angle >= 0) && (angle <= 148)) {
    angleCount = map(angle, 0, 148, pwm0DegCount, pwm180DegCount);
    pwmExt.setPWM(clawServoNum, ON_COUNT, angleCount);
  } else {
    Serial.println("You entered an invalid angle: setting it to 90 degrees");
    pwmExt.setPWM(clawServoNum, ON_COUNT, pwm90DegCount);
  }
  delay(50);
}
*/
=======
void openClaw() {
    angleCount = map(angleStart, 0, 180, pwm0DegCount, pwm180DegCount);
    pwmExt.setPWM(clawServoNum, ON_COUNT, angleCount);
}

void closeClaw() {
  angleCount = map(angleBall, 0, 180, pwm0DegCount, pwm180DegCount);
  pwmExt.setPWM(clawServoNum, ON_COUNT, angleCount);
}

>>>>>>> Stashed changes
