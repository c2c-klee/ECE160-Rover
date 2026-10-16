int xAxis;



void parseControllerData(ControllerPtr ctl) {
  /*
  if(ctl->a()){
    // eventually used to add open claw functionality
  }
  if(ctl->b()){
    // eventually used to add close claw functionality
  }
  */
  Serial.print(ctl->axisY());
  Serial.println(" Y-Axis");
  if(ctl->axisY() >= 10){    
    // Full scale map in positive Y
    pwmDutyCycle = (-1*map(ctl->axisY(),0, -512, 0, 100));
    xAxis = (map(ctl->axisX(), -512, 512, 0, 100));
    leftDirection = (map(xAxis/50, 0, 2, 0, 100));
    rightDirection = (map(50/(xAxis+1), 0, 50, 0, 100));

    Serial.println(xAxis);


    gpioExt.digitalWrite(nSLPLPin, HIGH);
    gpioExt.digitalWrite(nSLPRPin, HIGH);
    digitalWrite(rightMotorDIRPin, FWD);
    digitalWrite(leftMotorDIRPin, FWD);
    pwmGoLowCountLeft = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    pwmGoLowCountRight = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    ledcWrite(0, pwmGoLowCountRight);
    ledcWrite(2, pwmGoLowCountLeft);
    Serial.println(pwmDutyCycle);
    Serial.println(xAxis);

  }

  else if(ctl->axisY() <= -10) {
    // Full scale map in negative Y
    pwmDutyCycle = (-1*map(ctl->axisY(), 0, 512, 0, 100));

    xAxis = (map(ctl->axisX(), -512, 512, 0, 100));
    leftDirection = (map(xAxis/50, 0, 2, 0, 100));
    rightDirection = (map(50/(xAxis+1), 0, 50, 0, 100));

    Serial.println(xAxis);


    gpioExt.digitalWrite(nSLPLPin, HIGH);
    gpioExt.digitalWrite(nSLPRPin, HIGH);
    digitalWrite(rightMotorDIRPin, REV);
    digitalWrite(leftMotorDIRPin, REV);
    pwmGoLowCountLeft = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    pwmGoLowCountRight = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    ledcWrite(0, pwmGoLowCountRight);
    ledcWrite(2, pwmGoLowCountLeft);
    //Serial.println(pwmDutyCycle);
  }

  else if(ctl->axisRX() >= 10){    
    // Full scale map in positive Y
    pwmDutyCycle = (-1*map(ctl->axisRX(),0, -512, 0, 100));
    xAxis = (map(ctl->axisX(), -512, 512, 0, 100));
    leftDirection = (map(xAxis/50, 0, 2, 0, 100));
    rightDirection = (map(50/(xAxis+1), 0, 50, 0, 100));

    Serial.println(xAxis);


    gpioExt.digitalWrite(nSLPLPin, HIGH);
    gpioExt.digitalWrite(nSLPRPin, LOW);
    digitalWrite(rightMotorDIRPin, FWD);
    digitalWrite(leftMotorDIRPin, FWD);
    pwmGoLowCountLeft = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    pwmGoLowCountRight = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    ledcWrite(0, pwmGoLowCountRight);
    ledcWrite(2, pwmGoLowCountLeft);
    Serial.println(pwmDutyCycle);
    Serial.println(xAxis);

  }

  else if(ctl->axisRX() <= -10) {
    // Full scale map in negative Y
    pwmDutyCycle = (-1*map(ctl->axisRX(), 0, 512, 0, 100));

    xAxis = (map(ctl->axisX(), -512, 512, 0, 100));
    leftDirection = (map(xAxis/50, 0, 2, 0, 100));
    rightDirection = (map(50/(xAxis+1), 0, 50, 0, 100));

    Serial.println(xAxis);

    gpioExt.digitalWrite(nSLPLPin, LOW);
    gpioExt.digitalWrite(nSLPRPin, HIGH);
    digitalWrite(rightMotorDIRPin, FWD);
    digitalWrite(leftMotorDIRPin, FWD);
    pwmGoLowCountLeft = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    pwmGoLowCountRight = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    ledcWrite(0, pwmGoLowCountRight);
    ledcWrite(2, pwmGoLowCountLeft);
    //Serial.println(pwmDutyCycle);
  }

  else if(ctl->throttle() >= 10) {
    pwmDutyCycle = (map(ctl->throttle(), 0, 1023, 0, 100));
    gpioExt.digitalWrite(nSLPLPin, HIGH);
    gpioExt.digitalWrite(nSLPRPin, HIGH);
    digitalWrite(rightMotorDIRPin, FWD);
    digitalWrite(leftMotorDIRPin, REV);
    pwmGoLowCountCenter = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    ledcWrite(0, pwmGoLowCountCenter);
    ledcWrite(2, pwmGoLowCountCenter);
    //Serial.println(pwmDutyCycle);
  }
  else if(ctl->brake() >= 10) {
    pwmDutyCycle = (map(ctl->brake(), 0, 1023, 0, 100));
    gpioExt.digitalWrite(nSLPLPin, HIGH);
    gpioExt.digitalWrite(nSLPRPin, HIGH);
    digitalWrite(rightMotorDIRPin, REV);
    digitalWrite(leftMotorDIRPin, FWD);
    pwmGoLowCountCenter = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    ledcWrite(0, pwmGoLowCountCenter);
    ledcWrite(2, pwmGoLowCountCenter);
    //Serial.println(pwmDutyCycle);
  }

  else {
    ledcWrite(0, 0);
    ledcWrite(2, 0);
    gpioExt.digitalWrite(nSLPLPin, LOW);
    gpioExt.digitalWrite(nSLPRPin, LOW);
  }

}

void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                parseControllerData(myController);
            } else if (myController->isMouse()) {
                //processMouse(myController);
            } else if (myController->isKeyboard()) {
               //processKeyboard(myController);
            } else if (myController->isBalanceBoard()) {
                //processBalanceBoard(myController);
            } else {
                Serial.println("Unsupported controller");
            }
        }
    }
}
