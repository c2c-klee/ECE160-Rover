
void setServoPosition(int angle) {
  angle = constran(angle, 0, 148);
  puleCount = map(angle, 0, 180, minPulseCount, maxPulseCount);
  ledcWrite(servoPin, pulseCount);
  delay(50);
}

int ClawParseData() {
  if (Serial.available() > 1) {
    angle = Serial.parseInt();
    Serial.print("The angle is: ");
    Serial.println(angle);
  }
  if ((angle >= 0) && (angle <= 148)) {
    angleCount = map(angle, 0, 148, pwm0DegCount, pwm180DegCount);
    pwmExt.setPWM(clawServoNum, ON_COUNT, angleCount);
  } else {
    Serial.println("You entered an invalid angle: setting it to 90 degrees");
    pwmExt.setPWM(clawServoNum, ON_COUNT, pwm90DegCount);
  }
}
 /// Max amount to shut claw is 148
 /// Map and COnstrain could be added 