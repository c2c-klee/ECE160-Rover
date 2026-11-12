void followLine() {
  pos = qtrArray.readLineWhite(sensorValuesCalibrated);
  combinedSensorValues = (sensorValuesCalibrated[0] + sensorValuesCalibrated[1] + sensorValuesCalibrated[2] + sensorValuesCalibrated[3] + sensorValuesCalibrated[4] + sensorValuesCalibrated[5] + sensorValuesCalibrated[6] + sensorValuesCalibrated[7]);
  error = (pos - 3500) * turnFix;

  Serial.print("ERROR ");
  Serial.println(error);

  Serial.print("POS ");
  Serial.println(pos);

  leftSpeed = constrain(pwmDutyCycle + error / 235, -pwmDutyCycle, pwmDutyCycle);
  rightSpeed = constrain(pwmDutyCycle - error / 235, -pwmDutyCycle, pwmDutyCycle);

  pwmGoLowCountLeft = 0.01 * leftSpeed * motorPwmMaxCount;
  pwmGoLowCountRight = 0.01 * rightSpeed * motorPwmMaxCount * calibratedMotorFac;

  if (leftSensorValues < 400 || rightSensorValues < 400) {
    leftSpeed = pwmDutyCycle;
    rightSpeed = pwmDutyCycle;
  }

  ledcWrite(0, pwmGoLowCountLeft);
  ledcWrite(2, pwmGoLowCountRight);

  //Serial.print(leftSensorValues);
  //Serial.print(" || ");
  //Serial.println(rightSensorValues);
}

void checkLED() {
  if (leftSensorValues < 375 || rightSensorValues < 375) {
    gpioExt.digitalWrite(GPB2, HIGH);
    gpioExt.digitalWrite(GPB3, HIGH);
  } else {
    gpioExt.digitalWrite(GPB3, LOW);
    gpioExt.digitalWrite(GPB2, LOW);
  }
}

void parseLineFollow() {
  /*
    if(combinedSensorValues < 2400 && k > 2){
        leftSensorValues = 1000;
        rightSensorValues = 1000;
        delay(1000);
        ledcWrite(0, 0);
        ledcWrite(2, 0);
        gpioExt.digitalWrite(GPB3, LOW);
        gpioExt.digitalWrite(GPB2, LOW);
    }
    else if(combinedSensorValues < 2400 && controllerEnabledPathFollow == 1){
        leftSensorValues = 1000;
        rightSensorValues = 1000;
        k++;
        gpioExt.digitalWrite(GPB3, LOW);
        gpioExt.digitalWrite(GPB2, LOW);
        followLine(); 
    }
    else if (controllerEnabledPathFollow == 1) {
        leftSensorValues = (sensorValuesCalibrated[0]);
        rightSensorValues = (sensorValuesCalibrated[7]);
        followLine(); 
        checkLED();
    }
    */
}
z