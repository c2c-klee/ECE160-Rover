void SetClawPositionParse(ControllerPtr ctl) {
  bool foundController = true;
  if (ctl->x()) {
    angleCount = map(angleStart, 0, angleBall, pwm0DegCount, pwm180DegCount);
    pwmExt.setPWM(clawServoNum, ON_COUNT, angleCount);
  } else if (ctl->b()) {
    angleCount = map(angleBall, 0, angleBall, pwm0DegCount, pwm180DegCount);
    pwmExt.setPWM(clawServoNum, ON_COUNT, angleCount);
  } else {
  }
}

void triggerRightEncoder() {
  encoderRdir = digitalRead(encoderRAPin);
  if (encoderRdir == 1) {
    encoderRcount--;
  } else {
    encoderRcount++;
  }
  /* The wheel direction can be determined by the second encoder pin */
}
void triggerLeftEncoder() {
  /* The wheel direction can be determined by the second encoder pin */
  encoderLdir = digitalRead(encoderLAPin);
  if (encoderLdir == 1) {
    encoderLcount--;
  } else {
    encoderLcount++;
  }
}

bool spinning = false;
int spinTime = 0;

void ShotPutParseData(ControllerPtr ctl) {
  triggerRightEncoder();
  triggerLeftEncoder();
  encoderCount = encoderRcount + encoderLcount;

  gpioExt.digitalWrite(nSLPRPin, HIGH);
  gpioExt.digitalWrite(nSLPLPin, HIGH);
    spinning = true;
    pwmGoLowCount = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    ledcWrite(0, pwmGoLowCount);
    ledcWrite(2, pwmGoLowCount);
    digitalWrite(leftMotorDIRPin, REV);
    digitalWrite(rightMotorDIRPin, FWD);
    Serial.print(encoderCount);
    if (encoderCount >= 10000){
      angleCount = map(angleStart, 0, 180, pwm0DegCount, pwm180DegCount);
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