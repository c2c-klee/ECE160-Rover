// Final Codebase.

#include <Adafruit_MCP23X17.h>
#include <Bluepad32.h>
#include <QTRSensors.h>                                          // Line Following Sensor
#include <Adafruit_PWMServoDriver.h>
#define pca9685PwmBits 12
#define minPW 500   // min pulse width in microseconds to make servo at 0 degrees
#define maxPW 2500  // max pulse width in microseconds to make servo at 180
#define SERVO_FREQ 50   // Analog servos run at ~50 Hz updates
#define clawServoNum 0  // this is servo number 0 controller on the PCA9685
#define ON_COUNT 0

#define nSensors 8                                               //Define number of sensors in array
#define ctrlEven 5                                               //Define Pin number for control signal for even numbered sensors
#define ctrlOdd 25                                               //Define Pin number for control signal for odd numbered sensors

#define nSLPLPin 8
#define nSLPRPin 9
#define rightMotorDIRPin 15
#define rightMotorPWMPin 13
#define leftMotorDIRPin 14
#define leftMotorPWMPin 12
#define motorFreq 500
#define motorPwmBits 8
#define FWD LOW
#define REV HIGH

#define GPA0 0
#define GPA1 1
#define GPA2 2
#define GPA3 3
#define GPA4 4
#define GPA5 5
#define GPB2 10
#define GPB3 11

int bumperValue = 0;
int k = 0;

int pos;
int error;
int turnFix = 1;

int leftSpeed;
int rightSpeed;
int angleStart;
int angleBall;

  // the PWM signal will go high at a count value of 0
const int pwm0DegCount = 0.000001 * minPW * SERVO_FREQ * pow(2, pca9685PwmBits);
const int pwm180DegCount = 0.000001 * maxPW * SERVO_FREQ * pow(2, pca9685PwmBits);
const int pwm90DegCount = (pwm0DegCount + pwm180DegCount) / 2;
int angleCount;

Adafruit_PWMServoDriver pwmExt = Adafruit_PWMServoDriver(0x40);

const int motorPwmMaxCount = pow(2, motorPwmBits);
int pwmGoLowCountLeft;
int pwmGoLowCountRight;
int pwmDutyCycle = 0;

int leftDirection;
int rightDirection;

float calibratedMotorFac = 0.9682;

bool bumperPressed = false;

Adafruit_MCP23X17 gpioExt;

int gpioExtPins[6] = { GPA0, GPA1, GPA2, GPA3, GPA4, GPA5 };

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

const uint8_t sensorPins[] = {26, 27, 4, 16, 17, 18, 19, 23};  // ESP32 Pins for line sensor on robot
QTRSensors qtrArray;

// Create a sensor array object of class QTRSensors
uint16_t sensorValues[nSensors];            // Store raw sensor values in an array
uint16_t sensorValuesCalibrated[nSensors];  //Store calibrated sensor values in an array

int leftSensorValues = 0;
int rightSensorValues = 0;
int combinedSensorValues = 0;

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
/*
void disableControllers() {
    pwmDutyCycle = 25;
    gpioExt.digitalWrite(nSLPLPin, HIGH);
    gpioExt.digitalWrite(nSLPRPin, HIGH);
    digitalWrite(rightMotorDIRPin, REV);
    digitalWrite(leftMotorDIRPin, REV);
    pwmGoLowCount = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    ledcWrite(2, pwmGoLowCount);
    ledcWrite(0, pwmGoLowCount);
    Serial.println(pwmDutyCycle);

    delay(2000);
    gpioExt.digitalWrite(nSLPLPin, LOW);
    gpioExt.digitalWrite(nSLPRPin, LOW);
    setServoPosition(10);
    delay(1000);
    loop();
}
*/


void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                //LeftparseControllerData(myController);
                //RightparseControllerData(myController);
                Serial.println("Controller Testing");
            } else if (myController->isMouse()) {
                //processMouse(myController);
            } else if (myController->isKeyboard()) {
               //processKeyboard(myController);
            } else if (myController->isBalanceBoard()) {
                //processBalanceBoard(myController);
            } else {
                Serial.println("Unsupported controller");
            }
        }
    }
}




void followLine(){
    pos = qtrArray.readLineWhite(sensorValuesCalibrated);
    combinedSensorValues = (sensorValuesCalibrated[0]+sensorValuesCalibrated[1]+sensorValuesCalibrated[2]+sensorValuesCalibrated[3]+sensorValuesCalibrated[4]+sensorValuesCalibrated[5]+sensorValuesCalibrated[6]+sensorValuesCalibrated[7]);
    error = (pos - 3500) * turnFix;

    Serial.print("ERROR ");
    Serial.println(error);

    Serial.print("POS ");
    Serial.println(pos);

    leftSpeed = constrain(pwmDutyCycle + error / 185, -pwmDutyCycle, pwmDutyCycle);
    rightSpeed = constrain(pwmDutyCycle - error / 185, -pwmDutyCycle, pwmDutyCycle);

    pwmGoLowCountLeft = 0.01 * leftSpeed * motorPwmMaxCount;
    pwmGoLowCountRight = 0.01 * rightSpeed * motorPwmMaxCount * calibratedMotorFac;
    
    if(leftSensorValues < 400 || rightSensorValues < 400) {
        leftSpeed = pwmDutyCycle;
        rightSpeed = pwmDutyCycle;
    }
    
    ledcWrite(0, pwmGoLowCountLeft);
    ledcWrite(2, pwmGoLowCountRight);

    //Serial.print(leftSensorValues);
    //Serial.print(" || ");
    //Serial.println(rightSensorValues);

}

void checkLED(){
    if (leftSensorValues < 375 || rightSensorValues < 375){
        gpioExt.digitalWrite(GPB2, HIGH);
        gpioExt.digitalWrite(GPB3, HIGH);
    }
    else {
        gpioExt.digitalWrite(GPB3, LOW);
        gpioExt.digitalWrite(GPB2, LOW);
    }

}

void setup() {
    Serial.begin(112500);
    Serial.println("Debug Comms Open");
    // put your setup code here, to run once:


     // start GPIO interface
    gpioExt.begin_I2C(0x20);
    gpioExt.pinMode(nSLPLPin, OUTPUT);
    gpioExt.pinMode(nSLPRPin, OUTPUT);
    gpioExt.digitalWrite(nSLPLPin, HIGH);  //starts the robot with motors disabled (in sleep mode)
    gpioExt.digitalWrite(nSLPRPin, HIGH);  //starts the robot with motors disabled (in sleep mode)
    pinMode(rightMotorDIRPin, OUTPUT);
    pinMode(leftMotorDIRPin, OUTPUT);
    pinMode(rightMotorPWMPin, OUTPUT);
    pinMode(leftMotorPWMPin, OUTPUT);
    ledcSetup(0, motorFreq, motorPwmBits); 
    ledcAttachPin(rightMotorPWMPin, 0); 
    ledcSetup(2, motorFreq, motorPwmBits); 
    ledcAttachPin(leftMotorPWMPin, 2); 

    for (int i = 0; i < 6; i++) {
        gpioExt.pinMode(gpioExtPins[i], INPUT_PULLUP);
    }

    gpioExt.pinMode(GPB2, OUTPUT);
    gpioExt.pinMode(GPB3, OUTPUT);

    pwmExt.begin();
    pwmExt.setOscillatorFrequency(27000000);
    pwmExt.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
    pwmExt.setPWM(clawServoNum, ON_COUNT, pwm90DegCount);

    //setServoPosition(120);
    gpioExt.digitalWrite(nSLPLPin, LOW);
    gpioExt.digitalWrite(nSLPRPin, LOW);

    BP32.setup(&onConnectedController, &onDisconnectedController);
    Serial.println("Connected, Controller");

    BP32.forgetBluetoothKeys();

    BP32.enableVirtualDevice(false);

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
    
    // Blink LEDs after successful Calib
    Serial.println("Calibration Completed");
    gpioExt.digitalWrite(GPB2, HIGH);
    gpioExt.digitalWrite(GPB3, HIGH);
    delay(500);
    gpioExt.digitalWrite(GPB2, LOW);
    gpioExt.digitalWrite(GPB3, LOW);
    delay(500);

    pwmDutyCycle = (20);
    digitalWrite(leftMotorDIRPin, FWD);
    digitalWrite(rightMotorDIRPin, FWD);
    
}



void loop() {
  // put your main code here, to run repeatedly:
    //SensorParseData();
    //gpioExt.digitalWrite(GPB2, HIGH);
    gpioExt.digitalWrite(nSLPLPin, HIGH);
    gpioExt.digitalWrite(nSLPRPin, HIGH);
    delay(100);
    if(combinedSensorValues < 2400 && k > 12){
        leftSensorValues = 1000;
        rightSensorValues = 1000;
        delay(1000);
        ledcWrite(0, 0);
        ledcWrite(2, 0);
        gpioExt.digitalWrite(GPB3, LOW);
        gpioExt.digitalWrite(GPB2, LOW);
    }
    else if(combinedSensorValues < 2400){
        leftSensorValues = 1000;
        rightSensorValues = 1000;
        k++;
        gpioExt.digitalWrite(GPB3, LOW);
        gpioExt.digitalWrite(GPB2, LOW);
        followLine(); 
    }
    else{
        leftSensorValues = (sensorValuesCalibrated[0]);
        rightSensorValues = (sensorValuesCalibrated[7]);
        followLine(); 
        checkLED();
    }
}
