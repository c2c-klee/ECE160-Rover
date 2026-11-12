void longJump(ControllerPtr ctl) {
  qtrArray.readCalibrated(sensorValuesCalibrated);
  combinedSensorValues = (sensorValuesCalibrated[0] + sensorValuesCalibrated[1] + sensorValuesCalibrated[2] + sensorValuesCalibrated[3] + sensorValuesCalibrated[4] + sensorValuesCalibrated[5] + sensorValuesCalibrated[6] + sensorValuesCalibrated[7]);
  digitalWrite(leftMotorDIRPin, FWD);
  digitalWrite(rightMotorDIRPin, FWD);
  pwmGoLowCount = 0.01 * 18 * motorPwmMaxCount;
  Serial.print(combinedSensorValues);
  Serial.print(" || ");
  Serial.println(linesCrossed);


  if (combinedSensorValues < 2400) {
    linesCrossed++;
    firstLineCrossed = true;
  }

  if (firstLineCrossed == true) {
    gpioExt.digitalWrite(GPB2, LOW);
    gpioExt.digitalWrite(GPB3, LOW);
  }

  if (firstLineCrossed == false) {
    gpioExt.digitalWrite(GPB2, HIGH);
    gpioExt.digitalWrite(GPB3, HIGH);
    delay(200);
    gpioExt.digitalWrite(GPB2, LOW);
    gpioExt.digitalWrite(GPB3, LOW);
    delay(200);
  }

  if (linesCrossed < 3) {
    ledcWrite(0, pwmGoLowCount);
    ledcWrite(2, pwmGoLowCount);
  }

  else if (linesCrossed >= 3) {
    delay(500);
    ledcWrite(0, 0);
    ledcWrite(2, 0);
    gpioExt.digitalWrite(GPB2, HIGH);
    gpioExt.digitalWrite(GPB3, HIGH);
  }
}