/* #include <Adafruit_MCP23X17.h>
#define GPA0 0
#define GPA1 1
#define GPA2 2
#define GPA3 3
#define GPA4 4
#define GPA5 5
//Adafruit_MCP23X17 gpioExt;
int gpioExtPins[6] = { GPA0, GPA1, GPA2, GPA3, GPA4, GPA5 };

/*
void setup() {
  Serial.begin(115200);
  gpioExt.begin_I2C(0x20);
  for (int i = 0; i < 6; i++) {
    gpioExt.pinMode(gpioExtPins[i], INPUT_PULLUP);
  }
  Serial.println("Reading Bumper Switches");
}

void SensorParseData() {
  for (int i = 5; i > -1; i--) {
    Serial.print(gpioExt.digitalRead(gpioExtPins[i]));
    Serial.print(" ");
  }
  Serial.print(gpioExt.readGPIOA(), BIN);
  Serial.println();
  delay(500);
}
//Change
*/