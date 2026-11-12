// TEAM HERCULES - FINAL CODEBASE, REVISION v30.01

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

// BUMPER
int bumperValue = 0;
int k = 0;
int linesCrossed = 0;
bool firstLineCrossed = false;


Adafruit_MCP23X17 gpioExt;
int gpioExtPins[6] = { GPA0, GPA1, GPA2, GPA3, GPA4, GPA5 };

// DEFINE ANGLES FOR CLAWS
int angleStart = 8;
int angleBall = 135;

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

// CALIBRATED MOTOR SPEED COEFF.
float calibratedMotorFac = 0.9682;

// INIT CONTROLLER
ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// BLUEPAD INIT
void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.print("Controller is connected, index: ");
      Serial.println(i);

      ControllerProperties properties = ctl->getProperties();
      Serial.print("Controller Model: ");
      Serial.print(ctl->getModelName().c_str());

      myControllers[i] = ctl;
      foundEmptySlot = true;
      break;
    }
  }
}

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


void processControllers() {
  for (auto myController : myControllers) {
    if (myController && myController->isConnected() && myController->hasData()) {
      if (myController->isGamepad()) {
        parseControllers(myController);
        Serial.println("Controller Testing");
      } else {
        Serial.println("Unsupported controller");
      }
    }
  }
}

void setup() {
    Serial.begin(112500);
    Serial.println("SYSTEM INIT");

    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys();
    BP32.enableVirtualDevice(false);

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
    gpioExt.digitalWrite(GPB2, HIGH);
    gpioExt.digitalWrite(GPB3, HIGH);
    delay(500);
    gpioExt.digitalWrite(GPB2, LOW);
    gpioExt.digitalWrite(GPB3, LOW);
    delay(500);
}


void loop() {
    bool dataUpdated = BP32.update();
    //parseLineFollow();
    if (dataUpdated) {
      processControllers();
    }
}
