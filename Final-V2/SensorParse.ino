/* SensorParse.ino
* Description: Deprecated file used to read bump sensors. Not used.
* Author: Kendrick Lee & Braylen Pridemore
* Last Revision: 11/15/2026 @ 7:00PM
* Hardware Used: (x1) Pololu Chassis Kit for TI-RSLK MAX, (x1) Custom Adapter Board for the ESP-32 from TI-RSLK MAX rover, (various) i2C Extension Devices for GPIO & PWM GPIO
* 
* Major Functions: SensorParseData() - DEPRECATED, read bump sensors and report data.
*/

void SensorParseData() {
  // Cycle through each limit switch and read its corresponding state
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
