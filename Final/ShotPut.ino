void readEncoders() {
  encoderRdir = digitalRead(encoderRAPin);
  encoderLdir = digitalRead(encoderLAPin);
  if (encoderRdir == 1) {
    encoderRcount--;
  } else {
    encoderRcount++;
  } if (encoderLdir == 1) {
    encoderLcount--;
  } else {
    encoderLcount++;
  }
  encoderCount = encoderRcount + encoderLcount;
}


void shotPut() {
  pwmDutyCycle = 100;
  gpioExt.digitalWrite(nSLPRPin, HIGH);
  gpioExt.digitalWrite(nSLPLPin, HIGH);
  
  while(true){
    readEncoders();
    if(encoderCount > 4182){
      break;
    }

    pwmGoLowCount = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    ledcWrite(0, pwmGoLowCount);
    ledcWrite(2, pwmGoLowCount);
    digitalWrite(leftMotorDIRPin, REV);
    digitalWrite(rightMotorDIRPin, FWD);
  }
  
  angleCount = map(angleStart, 0, 180, pwm0DegCount, pwm180DegCount);
  pwmExt.setPWM(clawServoNum, ON_COUNT, angleCount);
  ledcWrite(0, 0);
  ledcWrite(2, 0);

}

