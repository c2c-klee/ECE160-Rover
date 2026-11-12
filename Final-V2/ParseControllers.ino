
void parseControllers(ControllerPtr ctl) {
  // Buttons First
  if (ctl->l1()) {
    openClaw();
  } else if (ctl->r1()) {
    closeClaw();
  } else if (ctl->y()) {
    shotPut();
  } else if (ctl->a()) {
    
  }
}