/* ParseControllers.ino
* Description: File for processing controller data, minus manual control.
* Author: Kendrick Lee & Braylen Pridemore
* Last Revision: 11/15/2026 @ 7:00PM
* Hardware Used: (x1) Pololu Chassis Kit for TI-RSLK MAX, (x1) Custom Adapter Board for the ESP-32 from TI-RSLK MAX rover, (various) i2C Extension Devices for GPIO & PWM GPIO
* 
* Major Functions: parseControllers(), takes the controller as an input, and then reads the controller data to see if a certain button is pressed. If a button is pressed,
*  execute that corresponding function.
*/


// NOTE: mappings are a little weird and are usually based off of an xBox controller.
void parseControllers(ControllerPtr ctl) {
  // Triggers open / close bumpers
  if (ctl->l1()) {
    openClaw();
  } else if (ctl->r1()) {
    closeClaw();

    // Shotput
  } else if (ctl->y()) {
    shotPut();

    // Hurdles
  } else if (ctl->a()) {
    followLine();

    // Long Jump event
  } else if (ctl->x()) {
    longJump();

    // Manual Mode
  } else if (ctl->thumbL()) {
    manualMode = true;

    // Reset Encoder Count by adding an offset, so that shotput can be run as many times as it wants to
  } else if (ctl->b()) {
    currentEncoderCount = encoderCount + 4000;
  }
}