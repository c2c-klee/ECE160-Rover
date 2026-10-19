// Final Codebase.

#include <Adafruit_MCP23X17.h>
#include <Bluepad32.h>

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

int leftDirection;
int rightDirection;

float calibratedMotorFac = 0.9682;

Adafruit_MCP23X17 gpioExt;

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
  // put your setup code here, to run once:
  BP32.setup(&onConnectedController, &onDisconnectedController);
  Serial.println("Connected, Controller");

  BP32.forgetBluetoothKeys();

  BP32.enableVirtualDevice(false);
}


void loop() {
  // put your main code here, to run repeatedly:
    bool dataUpdated = BP32.update();
    if (dataUpdated)
        Serial.println("Controller Data Updated");
        processControllers();
  delay(150);
}
