/* Final-V2.ino
* GITHUB REPO: https://github.com/c2c-klee/ECE160-Rover
* Description: The main file for our robot. Includes all pre-definition statements, variable definitions, top-level functions (loop/setup), 
*   and controller parsing code via Bluepad.
* Author: Kendrick Lee & Braylen Pridemore
* Last Revision: 11/15/2026 @ 7:00PM
* Hardware Used: (x1) Pololu Chassis Kit for TI-RSLK MAX, (x1) Custom Adapter Board for the ESP-32 from TI-RSLK MAX rover, (various) i2C Extension Devices for GPIO & PWM GPIO
* 
* Major Functions: setup() - Init. Code, onConnectedController() - update Bluepad API when new controller connected,
*   onDisconnectedController() - update Bluepad AI when existing controller disconnected, processControllers() - run controller-based 
*   functionality whenever controller data updated, loop() - when controller data updated, run processControllers().
*/

#include <Adafruit_MCP23X17.h>
#include <Bluepad32.h>
#include <QTRSensors.h>  // Line Following Sensor
#include <Adafruit_PWMServoDriver.h>

// SERVO DEFS
#define pca9685PwmBits 12
#define minPW 500       // min pulse width in microseconds to make servo at 0 degrees
#define maxPW 2500      // max pulse width in microseconds to make servo at 180
#define SERVO_FREQ 50   // Analog servos run at ~50 Hz updates
#define clawServoNum 0  // this is servo number 0 controller on the PCA9685
#define ON_COUNT 0

// LINE FOLLOWER
#define nSensors 8  //Define number of sensors in array
#define ctrlEven 5  //Define Pin number for control signal for even numbered sensors
#define ctrlOdd 25  //Define Pin number for control signal for odd numbered sensors

// MOTOR / DRIVETRAIN CONTROL
#define nSLPLPin 8
#define nSLPRPin 9
#define rightMotorDIRPin 15
#define leftMotorDIRPin 14
#define rightMotorPWMPin 13
#define leftMotorPWMPin 12
#define motorFreq 500
#define motorPwmBits 8
#define FWD LOW
#define REV HIGH

// BUMP & EXTERNAL GPIO
#define GPA0 0
#define GPA1 1
#define GPA2 2
#define GPA3 3
#define GPA4 4
#define GPA5 5
#define GPB2 10
#define GPB3 11

#define leftPWMCh 2
#define rightPWMCh 0

// MOTOR ENCODERS
#define encoderLAPin 32
#define encoderRAPin 35
#define encoderLBPin 34
#define encoderRBPin 33

#define leftMotorPWMCH 2
#define rightMotorPWMCH 0

// END PREDEFS

// ENCODERS
float positionL, positionR;
bool encoderRdir, encoderLdir;
long encoderRcount = 0;
long encoderLcount = 0;
int encoderCount = encoderRcount + encoderLcount;  //Combines Left and Right Encoder Values
int currentEncoderCount = 4000;

// BUMPER
int bumperValue = 0;
int linesCrossed = 0;
bool firstLineCrossed = false;


Adafruit_MCP23X17 gpioExt;
int gpioExtPins[6] = { GPA0, GPA1, GPA2, GPA3, GPA4, GPA5 };

// DEFINE ANGLES FOR CLAWS
int angleStart = 8;
int angleBall = 150;

// SERVO CALCULATIONS FOR PWM
const int pwm0DegCount = 0.000001 * minPW * SERVO_FREQ * pow(2, pca9685PwmBits);
const int pwm180DegCount = 0.000001 * maxPW * SERVO_FREQ * pow(2, pca9685PwmBits);
const int pwm90DegCount = (pwm0DegCount + pwm180DegCount) / 2;
int angleCount;

// DEFINE i2C SERVO DRIVER
Adafruit_PWMServoDriver pwmExt = Adafruit_PWMServoDriver(0x40);

// LINE FOLLOWERS
int fullLinesCrossed = 0;
int pos;
int error;
int turnFix = 1;
int leftSpeed;
int rightSpeed;
int controllerEnabledPathFollow = 0;
const uint8_t sensorPins[] = { 26, 27, 4, 16, 17, 18, 19, 23 };  // ESP32 Pins for line sensor on robot
QTRSensors qtrArray;
uint16_t sensorValues[nSensors];            // Store raw sensor values in an array
uint16_t sensorValuesCalibrated[nSensors];  //Store calibrated sensor values in an array
int leftSensorValues = 0;
int rightSensorValues = 0;
int combinedSensorValues = 0;

// MOTORS
const int motorPwmMaxCount = pow(2, motorPwmBits);
int pwmGoLowCountLeft;
int pwmGoLowCountRight;
int pwmGoLowCount;
int pwmDutyCycle = 0;
int pwmDutyCycleLeft = 0;
int pwmDutyCycleRight = 0;


// CALIBRATED MOTOR SPEED COEFF.
float calibratedMotorFac = 0.9682;

// INIT CONTROLLER
ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// Long Jump
bool manualLineOverride;
int jumpSpeed;

bool manualMode = false;

// BLUEPAD INIT
void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false; // take priority, do not allow simultaneous connections
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.print("Controller is connected, index: ");
      Serial.println(i);
      // print debug info to the serial monitor
      ControllerProperties properties = ctl->getProperties();
      Serial.print("Controller Model: ");
      Serial.print(ctl->getModelName().c_str());

      myControllers[i] = ctl;
      foundEmptySlot = true;
      break;
    }
  }
}

// DEBIND CONTROLLER FROM ARRAY IF DISCONNECTED
void onDisconnectedController(ControllerPtr ctl) {
  bool foundController = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
      myControllers[i] = nullptr;
      foundController = true;
      break;
    }
  }
  if (!foundController) {
    Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
  }
}

// Propogate controller data to individual functions
void processControllers() {
  for (auto myController : myControllers) {
    if (myController && myController->isConnected() && myController->hasData()) {
      if (myController->isGamepad()) {
        // Separate function to determine what mode to enter (or what event)
        parseControllers(myController);
        // If manual mode is true, execute soccer functionality ONLY.
        if (manualMode == true) {
          soccerParseL(myController);
          soccerParseR(myController);
        }
      } else {
        Serial.println("Unsupported controller");
      }
    }
  }
}

void setup() {
  Serial.begin(112500);
  Serial.println("SYSTEM INIT");

  // Bluepad INIT first
  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(false);

  // Encoder Setup
  pinMode(encoderLAPin, INPUT_PULLDOWN);
  pinMode(encoderRAPin, INPUT_PULLDOWN);
  pinMode(encoderLBPin, INPUT_PULLDOWN);
  pinMode(encoderRBPin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(encoderRBPin), readEncoders, RISING);
  attachInterrupt(digitalPinToInterrupt(encoderLBPin), readEncoders, RISING);

  // i2C GPIO Ext
  gpioExt.begin_I2C(0x20);
  gpioExt.pinMode(nSLPLPin, OUTPUT);
  gpioExt.pinMode(nSLPRPin, OUTPUT);
  for (int i = 0; i < 6; i++) {
    gpioExt.pinMode(gpioExtPins[i], INPUT_PULLUP);
  }

  // LED PINMODES
  gpioExt.pinMode(GPB2, OUTPUT);
  gpioExt.pinMode(GPB3, OUTPUT);

  // MOTORS
  pinMode(rightMotorDIRPin, OUTPUT);
  pinMode(leftMotorDIRPin, OUTPUT);
  pinMode(rightMotorPWMPin, OUTPUT);
  pinMode(leftMotorPWMPin, OUTPUT);

  // PWM MOTOR SETUP
  ledcSetup(rightMotorPWMCH, motorFreq, motorPwmBits);
  ledcAttachPin(rightMotorPWMPin, rightMotorPWMCH);
  ledcSetup(leftMotorPWMCH, motorFreq, motorPwmBits);
  ledcAttachPin(leftMotorPWMPin, leftMotorPWMCH);

  // INIT CLAW
  pwmExt.begin();
  pwmExt.setOscillatorFrequency(27000000);
  pwmExt.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  pwmExt.setPWM(clawServoNum, ON_COUNT, pwm90DegCount);
  angleCount = map(90, 0, 180, pwm0DegCount, pwm180DegCount);
  pwmExt.setPWM(clawServoNum, ON_COUNT, angleCount);

  // INIT MOTORS OFF
  gpioExt.digitalWrite(nSLPLPin, LOW);
  gpioExt.digitalWrite(nSLPRPin, LOW);
  digitalWrite(leftMotorDIRPin, FWD);
  digitalWrite(rightMotorDIRPin, FWD);


  // INIT SENSOR ARRAY
  qtrArray.setTypeRC();
  qtrArray.setSensorPins(sensorPins, nSensors);  // use the name of the array without the []
  qtrArray.setEmitterPins(ctrlOdd, ctrlEven);

  // CALIBRATE SENSORS
  Serial.println("Starting Calibration");
  for (int i = 0; i < 400; i++) {
    qtrArray.calibrate();
  }
  Serial.println("Calibration Completed");
  
  // Blink LEDs afer calibration complete.
  gpioExt.digitalWrite(GPB2, HIGH);
  gpioExt.digitalWrite(GPB3, HIGH);
  delay(500);
  gpioExt.digitalWrite(GPB2, LOW);
  gpioExt.digitalWrite(GPB3, LOW);
  delay(500);
}


void loop() {
  // check if CTL data was updated. If so, then execute function to process controller data.
  bool dataUpdated = BP32.update();
  //parseLineFollow();
  if (dataUpdated) {
    processControllers();
  }
}
