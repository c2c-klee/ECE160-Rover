#include <QTRSensors.h>                                          // Line Following Sensor
#define nSensors 8                                               //Define number of sensors in array
#define ctrlEven 5                                               //Define Pin number for control signal for even numbered sensors
#define ctrlOdd 25                                               //Define Pin number for control signal for odd numbered sensors
const uint8_t sensorPins[] = { 26, 27, 4, 16, 17, 18, 19, 23 };  // ESP32 Pins for line sensor on robot
QTRSensors qtrArray;
// Create a sensor array object of class QTRSensors
uint16_t sensorValues[nSensors];            // Store raw sensor values in an array
uint16_t sensorValuesCalibrated[nSensors];  //Store calibrated sensor values in an array
void setup() {
  Serial.begin(115200);
  qtrArray.setTypeRC();
  // Ours is an RC-type (digital) array
  qtrArray.setSensorPins(sensorPins, nSensors);  // use the name of the array without the []
  qtrArray.setEmitterPins(ctrlOdd, ctrlEven);
  Serial.println("Starting Calibration");
  for (int i = 0; i < 400; i++) {
    qtrArray.calibrate();
  }
  // This is the calibration loop that will store min/max values
  // Takes about 10 seconds to complete. while calibrating, move the
  // sensor array over full range of color that it will see.
  Serial.println("Calibration Completed");
}
void loop() {
  qtrArray.read(sensorValues);
  // Read the raw sensor data
  qtrArray.readCalibrated(sensorValuesCalibrated);  // Read and calibrate the sensor data
  for (int i = 0; i < 8; i++) {
    Serial.print(sensorValues[i]);
    Serial.print(" ");
  }
  Serial.print(" || ");
  for (int i = 0; i < 8; i++) {
    //Print the raw data array
    //Separate the raw and calibrated data
    // Print the calibrated data array
    Serial.print(sensorValuesCalibrated[i]);
    Serial.print(" ");
  }
  Serial.println();
  delay(1000);
}
