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