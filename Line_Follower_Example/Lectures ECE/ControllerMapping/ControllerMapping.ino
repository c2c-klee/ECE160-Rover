#include <Bluepad32.h>
#include <Adafruit_MCP23X17.h>

// ---------- Pin and motor settings (each defined only once) ----------
#define GPB0 8
#define GPB1 9

#define nSLPLPin 8
#define nSLPRPin 9
#define rightMotorDIRPin 15
#define rightMotorPWMPin 13
#define leftMotorDIRPin 14
#define leftMotorPWMPin 12
#define motorFreq 500
#define motorPwmBits 8
#define FWD LOW    // DIR bit 0 = move in direction opposite the claw (forward)
#define REV HIGH   // DIR bit 1 = move in direction of the claw (reverse)

// ---------- Variables ----------
const int motorPwmMaxCount = pow(2, motorPwmBits);
int pwmGoLowCount;       // count at which the motor PWM pulse goes low
int pwmDutyCycle = 0;    // duty cycle as a percentage from 0-100
int spinDuty = 60;       // speed when spinning (0-100)
int driveDuty = 80;      // speed when going forward/reverse (0-100)

Adafruit_MCP23X17 gpioExt;

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// ---------- Controller connect / disconnect ----------
void onConnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.printf("Controller connected, index=%d\n", i);
      myControllers[i] = ctl;
      break;
    }
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.printf("Controller disconnected from index=%d\n", i);
      myControllers[i] = nullptr;
      break;
    }
  }
}

// ---------- Motor functions ----------
void drive(int leftDir, int leftDuty, int rightDir, int rightDuty) {
  // wake up both motors
  gpioExt.digitalWrite(nSLPLPin, HIGH);
  gpioExt.digitalWrite(nSLPRPin, HIGH);

  // set which way each motor spins
  digitalWrite(leftMotorDIRPin, leftDir);
  digitalWrite(rightMotorDIRPin, rightDir);

  // set the left motor speed
  pwmGoLowCount = 0.01 * leftDuty * motorPwmMaxCount;
  if (pwmGoLowCount >= motorPwmMaxCount) pwmGoLowCount = motorPwmMaxCount - 1;
  ledcWrite(leftMotorPWMPin, pwmGoLowCount);

  // set the right motor speed
  pwmGoLowCount = 0.01 * rightDuty * motorPwmMaxCount;
  if (pwmGoLowCount >= motorPwmMaxCount) pwmGoLowCount = motorPwmMaxCount - 1;
  ledcWrite(rightMotorPWMPin, pwmGoLowCount);
}

void stopMotors() {
  // speed 0
  ledcWrite(leftMotorPWMPin, 0);
  ledcWrite(rightMotorPWMPin, 0);

  // put both motors to sleep
  gpioExt.digitalWrite(nSLPLPin, LOW);
  gpioExt.digitalWrite(nSLPRPin, LOW);
}

// ---------- Button mapping ----------
void processGamepad(ControllerPtr ctl) {
  if (ctl->l1()) {
    // Spin Left
    drive(REV, spinDuty, FWD, spinDuty);
    Serial.println("Spinning left");
  }
  else if (ctl->r1()) {
    // Spin Right
    drive(FWD, spinDuty, REV, spinDuty);
    Serial.println("Spin Right");
  }
  else if (ctl->l2()) {
    // Go Backward
    drive(REV, driveDuty, REV, driveDuty);
    Serial.println("Going Backward");
  }
  else if (ctl->r2()) {
    // Go Forward
    drive(FWD, driveDuty, FWD, driveDuty);
    Serial.println("Going Forward");
  }
  else {
    // nothing pressed
    stopMotors();
  }
}

void processControllers() {
  for (auto myController : myControllers) {
    if (myController && myController->isConnected() && myController->hasData()) {
      if (myController->isGamepad()) {
        processGamepad(myController);
      } else {
        Serial.println("Unsupported controller");
      }
    }
  }
}

// ---------- Arduino setup ----------
void setup() {
  Serial.begin(115200);

  gpioExt.begin_I2C(0x20);  // 0x20 is the I2C address for the GPIO Extender
  gpioExt.pinMode(nSLPLPin, OUTPUT);
  gpioExt.pinMode(nSLPRPin, OUTPUT);
  gpioExt.digitalWrite(nSLPLPin, LOW);  // starts the robot with motors disabled (in sleep mode)
  gpioExt.digitalWrite(nSLPRPin, LOW);  // starts the robot with motors disabled (in sleep mode)

  pinMode(rightMotorDIRPin, OUTPUT);
  pinMode(leftMotorDIRPin, OUTPUT);
  pinMode(rightMotorPWMPin, OUTPUT);
  pinMode(leftMotorPWMPin, OUTPUT);
  ledcAttach(rightMotorPWMPin, motorFreq, motorPwmBits);
  ledcAttach(leftMotorPWMPin, motorFreq, motorPwmBits);

  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t* addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

  // Setup the Bluepad32 callbacks
  BP32.setup(&onConnectedController, &onDisconnectedController);

  // Uncomment only if you need to re-pair the controller from scratch
  // BP32.forgetBluetoothKeys();

  // Mouse / touchpad support is off
  BP32.enableVirtualDevice(false);
}

// ---------- Arduino loop (runs in CPU 1) ----------
void loop() {
  // fetches all the controllers' data
  bool dataUpdated = BP32.update();
  if (dataUpdated)
    processControllers();

  // yield so the watchdog doesn't trigger
  vTaskDelay(1);
}
