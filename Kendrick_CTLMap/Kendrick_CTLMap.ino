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

  if(ctl->axisY() >= 10){
    gpioExt.digitalWrite(nSLPLPin, HIGH);
    gpioExt.digitalWrite(nSLPRPin, HIGH);
    
    // Full scale map in positive Y
    pwmDutyCycle = map(ctl->axisY(), 0, 512, 0, 100);
    
    digitalWrite(rightMotorDIRPin, FWD);
    digitalWrite(leftMotorDIRPin, FWD);
    pwmGoLowCount = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    ledcWrite(0, pwmGoLowCount);
    ledcWrite(1, pwmGoLowCount);
  }

  else if(ctl->axisY() <= -10) {
    gpioExt.digitalWrite(nSLPLPin, HIGH);
    gpioExt.digitalWrite(nSLPRPin, HIGH);
    
    // Full scale map in negative Y
    pwmDutyCycle = map(ctl->axisY(), -512, 0, 0, 100);
    
    digitalWrite(rightMotorDIRPin, REV);
    digitalWrite(leftMotorDIRPin, REV);
    pwmGoLowCount = 0.01 * pwmDutyCycle * motorPwmMaxCount;
    ledcWrite(0, pwmGoLowCount);
    ledcWrite(1, pwmGoLowCount);
  }

}

void processControllers() {
  for (auto myController : myControllers) {
    parseControllerData(myController);
  }
}

void setup() {
  Serial.begin(112500);
  Serial.println("Debug Comms Open");

  // start GPIO interface
  gpioExt.begin_I2C(0x20);
  gpioExt.pinMode(nSLPLPin, OUTPUT);
  gpioExt.pinMode(nSLPRPin, OUTPUT);

  gpioExt.digitalWrite(nSLPLPin, LOW);  //starts the robot with motors disabled (in sleep mode)
  gpioExt.digitalWrite(nSLPRPin, LOW);  //starts the robot with motors disabled (in sleep mode)

  pinMode(rightMotorDIRPin, OUTPUT);
  pinMode(leftMotorDIRPin, OUTPUT);
  pinMode(rightMotorPWMPin, OUTPUT);
  pinMode(leftMotorPWMPin, OUTPUT);
  ledcSetup(0, motorFreq, motorPwmBits); 
  ledcAttachPin(rightMotorPWMPin, 0); 
  ledcSetup(1, motorFreq, motorPwmBits); 
  ledcAttachPin(leftMotorPWMPin, 1); 

  BP32.setup(&onConnectedController, &onDisconnectedController);

  BP32.forgetBluetoothKeys();

  BP32.enableVirtualDevice(false);
}

void loop() {
  bool dataUpdated = BP32.update();
    if (dataUpdated){
      processControllers();
    }
  delay(150);
}
