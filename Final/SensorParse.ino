void SensorParseData() {
  for (int i = 5; i > -1; i--) {
    Serial.print(gpioExt.digitalRead(gpioExtPins[i]));
    Serial.print(" ");
    if (gpioExt.digitalRead(gpioExtPins[i]) == 0) {
      //disableControllers();
    } else {
      // do nothing
    }
  }
  Serial.print(gpioExt.readGPIOA(), BIN);
  Serial.println();
}
