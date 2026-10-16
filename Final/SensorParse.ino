
int SensorParseData() {
  for (int i = 5; i > -1; i--) {
    Serial.print(gpioExt.digitalRead(gpioExtPins[i]));
    Serial.print(" ");
  }
  Serial.print(gpioExt.readGPIOA(), BIN);
  Serial.println();
  delay(500);
  }
}