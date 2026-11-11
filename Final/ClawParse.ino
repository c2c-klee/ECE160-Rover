void openClaw() {
    angleCount = map(angleStart, 0, 180, pwm0DegCount, pwm180DegCount);
    pwmExt.setPWM(clawServoNum, ON_COUNT, angleCount);
}

void closeClaw() {
  angleCount = map(angleBall, 0, 180, pwm0DegCount, pwm180DegCount);
  pwmExt.setPWM(clawServoNum, ON_COUNT, angleCount);
}

