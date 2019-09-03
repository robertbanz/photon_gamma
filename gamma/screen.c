
void MonoCursorOff(void) {
  outp(0x3b4, 14);
  outp(0x3b5, 2);
  outp(0x3b4, 15);
  outp(0x3b5, 81);
}
