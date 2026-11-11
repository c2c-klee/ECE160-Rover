<<<<<<< Updated upstream

//just a thought should we add a lineup function so it lines up to the line same way each time? like detects too
/* If "Y" pressed then enter 'Shotput' mode and grips claw and spins 3 times waits 10 milliseconds and releases and stops after 3.5 spins
 */
  // the PWM signal will go high at a count value of 0
=======
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
    angleCount = map(angleBall, 0, 180, pwm0DegCount, pwm180DegCount);
    pwmExt.setPWM(clawServoNum, ON_COUNT, angleCount);
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

>>>>>>> Stashed changes
