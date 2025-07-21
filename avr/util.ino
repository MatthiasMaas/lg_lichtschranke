
void wait_for_release(uint16_t start_value) {
  uint16_t buttonValue;
  uint16_t result;
  while (1) {
    buttonValue = analogRead(buttonPin);
    result = (int)buttonValue - (int)start_value;

    if (abs(result) < 10) {
      delay(150);
      return;
    }
  }
}


// Interrupt Initialisierung für INT0 und INT1.
void interrupts_init(void) {
  // Konfiguration INT0 (an PD2) und INT1 (an PD3):
  // Setze "Any Change" Mode:
  // Für INT0: ISC01 = 0, ISC00 = 1
  // Für INT1: ISC11 = 0, ISC10 = 1
  EICRA &= ~((1 << ISC01) | (1 << ISC11));  // Clear ISC01, ISC11

  EICRA |= (1 << ISC00) | (1 << ISC10);  // Set ISC00, ISC10

  // activate INT0 und INT1.
  EIMSK |= (1 << INT0);
  EIMSK |= (1 << INT1);
}
