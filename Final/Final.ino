// Final Codebase.

#include <Adafruit_MCP23X17.h>
#include <Bluepad32.h>

#include <Adafruit_PWMServoDriver.h>
#define pca9685PwmBits 12
#define minPW 500   // min pulse width in microseconds to make servo at 0 degrees
#define maxPW 2500  // max pulse width in microseconds to make servo at 180
#define SERVO_FREQ 50   // Analog servos run at ~50 Hz updates
#define clawServoNum 0  // this is servo number 0 controller on the PCA9685
#define ON_COUNT 0


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

int bumperValue = 0;

  // the PWM signal will go high at a count value of 0
const int pwm0DegCount = 0.000001 * minPW * SERVO_FREQ * pow(2, pca9685PwmBits);
const int pwm180DegCount = 0.000001 * maxPW * SERVO_FREQ * pow(2, pca9685PwmBits);
const int pwm90DegCount = (pwm0DegCount + pwm180DegCount) / 2;
int angleCount;

Adafruit_PWMServoDriver pwmExt = Adafruit_PWMServoDriver(0x40);

const int motorPwmMaxCount = pow(2, motorPwmBits);
int pwmGoLowCount;
int pwmDutyCycle = 0;

int leftDirection;
int rightDirection;

float calibratedMotorFac = 0.9682;

bool bumperPressed = false;

Adafruit_MCP23X17 gpioExt;

int gpioExtPins[6] = { GPA0, GPA1, GPA2, GPA3, GPA4, GPA5 };

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

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



void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                LeftparseControllerData(myController);
                RightparseControllerData(myController);
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

    pwmExt.begin();
    pwmExt.setOscillatorFrequency(27000000);
    pwmExt.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
    pwmExt.setPWM(clawServoNum, ON_COUNT, pwm90DegCount);

    setServoPosition(120);

    BP32.setup(&onConnectedController, &onDisconnectedController);
    Serial.println("Connected, Controller");

    BP32.forgetBluetoothKeys();

    BP32.enableVirtualDevice(false);
}


void loop() {
  // put your main code here, to run repeatedly:
    SensorParseData();
    bool dataUpdated = BP32.update();
    if (dataUpdated)
        Serial.println("Controller Data Updated");
        processControllers();
  delay(150);
}
