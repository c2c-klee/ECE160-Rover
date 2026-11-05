
int xAxis;

void LeftparseControllerData(ControllerPtr ctl) {

  Serial.print(ctl->axisY());
  Serial.println(" Y-Axis");
  if(ctl->axisY() >= 10){    
    // Full scale map in positive Y
    pwmDutyCycle = (map(ctl->axisY(),0, 512, 0, 100));
    gpioExt.digitalWrite(nSLPLPin, HIGH);
    digitalWrite(leftMotorDIRPin, FWD);
    pwmGoLowCount = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    //ledcWrite(2, pwmGoLowCount);
    Serial.println(pwmDutyCycle);
  }

  else if(ctl->axisY() <= -10) {
    
    // Full scale map in negative Y
    pwmDutyCycle = (map(ctl->axisY(), 0, -512, 0, 100));
    gpioExt.digitalWrite(nSLPLPin, HIGH);
    digitalWrite(leftMotorDIRPin, REV);
    pwmGoLowCount = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    //ledcWrite(2, pwmGoLowCount);
    Serial.println(pwmDutyCycle);
  }
  else if(ctl->a()) {
    controllerEnabledPathFollow = 1;
  }
  else {
    //ledcWrite(2, 0);
    //gpioExt.digitalWrite(nSLPLPin, LOW);
  }

}

void RightparseControllerData(ControllerPtr ctl) {
  if(ctl->axisRY() >= 10){    
    // Full scale map in positive Y
    pwmDutyCycle = (map(ctl->axisRY(),0, 512, 0, 100));
    gpioExt.digitalWrite(nSLPRPin, HIGH);
    digitalWrite(rightMotorDIRPin, FWD);
    pwmGoLowCount = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    //ledcWrite(0, pwmGoLowCount);
    Serial.println("test");
  }
  else if(ctl->axisRY() <= -10) {
    // Full scale map in negative Y
    pwmDutyCycle = (map(ctl->axisRY(), 0, -512, 0, 100));
    gpioExt.digitalWrite(nSLPRPin, HIGH);
    digitalWrite(rightMotorDIRPin, REV);
    pwmGoLowCount = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    //ledcWrite(0, pwmGoLowCount);
    Serial.println(pwmDutyCycle);
  }
  else {
    //ledcWrite(0, 0);
    //gpioExt.digitalWrite(nSLPRPin, LOW);
  }
} 
