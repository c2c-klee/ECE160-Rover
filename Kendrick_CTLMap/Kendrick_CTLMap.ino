// Version 1 - Finalized at X:XX AM/PM on 10/13/2026, by Kendrick Lee
// See GitHub for VC corrections.
// Includes basic drivetrain control.

#include <Adafruit_MCP23X17.h>
#include <Bluepad32.h>

#define GBP0 8
#define GBP1 9

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

const int motorPwmMaxCount = pow(2, motorPwmBits);
int pwmGoLowCount;
int pwmDutyCycle = 0;

// Define i2C Multiplex
Adafruit_MCP23X17 gpioExt;

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// This function is used to cycle through the 4 available controller slots
//  and populate them sequentially. Rewritten by Kendrick Lee from BluePad Library.
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


// Separate function for parsing controller data, remove conditionals from
//  main loop function.
// Note that the controller uses xBox Mappings.
void parseControllerData(ControllerPtr ctl) {
  /*
  if(ctl->a()){
    // eventually used to add open claw functionality
  }
  if(ctl->b()){
    // eventually used to add close claw functionality
  }
  */
  Serial.print(ctl->axisY());
  Serial.println(" Y-Axis");
  if(ctl->axisY() >= 10){    
    // Full scale map in positive Y
    pwmDutyCycle = (-1*map(ctl->axisY(),0, -512, 0, 100));
    gpioExt.digitalWrite(nSLPLPin, HIGH);
    gpioExt.digitalWrite(nSLPRPin, HIGH);
    digitalWrite(rightMotorDIRPin, FWD);
    digitalWrite(leftMotorDIRPin, FWD);
    pwmGoLowCount = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    ledcWrite(0, pwmGoLowCount);
    ledcWrite(2, pwmGoLowCount);
    Serial.println(pwmDutyCycle);
  }

  else if(ctl->axisY() <= -10) {
    
    // Full scale map in negative Y
    pwmDutyCycle = (-1*map(ctl->axisY(), 0, 512, 0, 100));
    gpioExt.digitalWrite(nSLPLPin, HIGH);
    gpioExt.digitalWrite(nSLPRPin, HIGH);
    digitalWrite(rightMotorDIRPin, REV);
    digitalWrite(leftMotorDIRPin, REV);
    pwmGoLowCount = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    ledcWrite(0, pwmGoLowCount);
    ledcWrite(2, pwmGoLowCount);
    Serial.println(pwmDutyCycle);
  }
  else if(ctl->throttle() >= 10) {
    pwmDutyCycle = (map(ctl->throttle(), 0, 1023, 0, 100));
    gpioExt.digitalWrite(nSLPLPin, HIGH);
    gpioExt.digitalWrite(nSLPRPin, HIGH);
    digitalWrite(rightMotorDIRPin, FWD);
    digitalWrite(leftMotorDIRPin, REV);
    pwmGoLowCount = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    ledcWrite(0, pwmGoLowCount);
    ledcWrite(2, pwmGoLowCount);
    Serial.println(pwmDutyCycle);
  }
  else if(ctl->brake() >= 10) {
    pwmDutyCycle = (map(ctl->brake(), 0, 1023, 0, 100));
    gpioExt.digitalWrite(nSLPLPin, HIGH);
    gpioExt.digitalWrite(nSLPRPin, HIGH);
    digitalWrite(rightMotorDIRPin, REV);
    digitalWrite(leftMotorDIRPin, FWD);
    pwmGoLowCount = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    ledcWrite(0, pwmGoLowCount);
    ledcWrite(2, pwmGoLowCount);
    Serial.println(pwmDutyCycle);
  }

  else {
    ledcWrite(0, 0);
    ledcWrite(2, 0);
    gpioExt.digitalWrite(nSLPLPin, LOW);
    gpioExt.digitalWrite(nSLPRPin, LOW);
  }

}

void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                parseControllerData(myController);
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

  BP32.setup(&onConnectedController, &onDisconnectedController);

  BP32.forgetBluetoothKeys();

  BP32.enableVirtualDevice(false);
}

void loop() {

    bool dataUpdated = BP32.update();
    if (dataUpdated)
        processControllers();
  delay(150);
}
