void SetClawPositionParse(ControllerPtr ctl) { 
  bool foundController = true;
    if (ctl->a()){
      pwmExt.setPWM(clawServoNum, ON_COUNT, angleStart);
    }
    else if (ctl->b()) {
      pwmExt.setPWM(clawServoNum, ON_COUNT, angleBall);
    }
    else {
    }
}

void ShotPutParseData(ControllerPtr ctl) {
  gpioExt.digitalWrite(nSLPRPin, HIGH);
  gpioExt.digitalWrite(nSLPLPin, HIGH);

  if (!spinning && spinTime <= 3 && ctl->y()) {
    pwmDutyCycle = (100);
    spinning = true;
    ledcWrite(rightMotorPWMPin, pwmDutyCycle);
    ledcWrite(leftMotorPWMPin, pwmDutyCycle);
    digitalWrite(leftMotorDIRPin, REV);
    digitalWrite(rightMotorDIRPin, FWD);

    delay(1000);

    spinTime++;
  }
  else {
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