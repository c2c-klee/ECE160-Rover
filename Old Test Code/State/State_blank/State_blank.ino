void loop() {
  if (PS4.isConnected()) {
    squarePressed = PS4.Square();
    circlePressed = PS4.Circle();
  }
  Serial.print("The Current State is: ");
  Serial.print(currentState);
Serial.print(“ || The SquareCircle buttons are: "); Serial.print(squarePressed); Serial.println(circlePressed);
switch (currentState) {
    case MANUAL_STATE:
      if (squarePressed) {
        currentState = AUTO_STATE;
      } else {
        currentState = MANUAL_STATE;
      }
      break;
    case AUTO_STATE:
      if (circlePressed) {
        currentState = MANUAL_STATE;
      } else {
        currentState = AUTO_STATE;
      }
      break;
}
delay(500);
}