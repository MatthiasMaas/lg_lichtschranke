// Memory for the times recorded
// the return-values of micros are stored in in?_data. after each write,
// in?_count is incremented
#define MAX_DATA 10

uint32_t in0_rise_data[MAX_DATA];
uint8_t in0_rise_count = 0;

uint32_t in0_fall_data[MAX_DATA];
uint8_t in0_fall_count = 0;

uint32_t in1_rise_data[MAX_DATA];
uint8_t in1_rise_count = 0;

uint32_t in1_fall_data[MAX_DATA];
uint8_t in1_fall_count = 0;

// time retrieving function



// initialize with zeros
void init_data() {
  for (uint8_t i = 0; i < 10; i++) {
    in0_rise_data[i] = 0;
    in1_rise_data[i] = 0;
    in0_fall_data[i] = 0;
    in1_fall_data[i] = 0;
  }
}







// *** print adc values of key-presses ***
//
//
void app_keys() {
  while (1) {
    delay(10);
    lcd.clear();
    lcd.setCursor(0, 1);
    int buttonValue = analogRead(buttonPin);
    char buff[16];
    sprintf(buff, "%d", buttonValue);
    lcd.print(buff);
  }
}







// *** edge detector ***
//
//
void app_edges() {
  isrHandler0 = isr_edge_0;
  isrHandler1 = isr_edge_1;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("int0 ");
  lcd.setCursor(0, 1);
  lcd.print("int1 ");
  while (1)
    ;
}

// Interrupt Service Routine für INT0 (PD2)
void isr_edge_0() {
  // Lesepin PD2:
  lcd.setCursor(5, 0);
  if (PIND & (1 << PD2)) {
    lcd.print("L -> H");
  } else {
    lcd.print("H -> L");
  }
}

// Interrupt Service Routine für INT1 (PD3)
void isr_edge_1() {
  // Lesepin PD3:
  lcd.setCursor(5, 1);
  if (PIND & (1 << PD3)) {
    lcd.print("L -> H");
  } else {
    lcd.print("H -> L");
  }
}


// *** input test ***
//
//
void app_inputs() {
  pinMode(2, INPUT);  // in0
  pinMode(3, INPUT);  // in1
  lcd.clear();

  int val0;
  int val1;
  char buf[2];

  while (1) {
    val0 = digitalRead(2);
    val1 = digitalRead(3);

    lcd.setCursor(0, 0);
    lcd.print("in0 ");
    snprintf(buf, 2, "%d", val0);
    lcd.print(buf);

    lcd.setCursor(0, 1);
    lcd.print("in1 ");
    snprintf(buf, 2, "%d", val1);
    lcd.print(buf);
  }
}









// *** timers ***
//
//
void app_timers() {
  isrHandler0 = isr_timers_0;
  isrHandler1 = isr_timers_1;

  uint16_t buttonValue;
  char buf[30];
  uint32_t diff;     //delta t
  int16_t page = 0;  // which page (0 .. 9) to display
  // uint8_t last_page = 0;

  init_data();

  lcd.clear();


  while (1) {
    buttonValue = analogRead(buttonPin);
    wait_for_release(buttonValue);


    // LEFT pressed: page -> page-1
    if ((buttonValue > KEY_LEFT - KEY_OFFSET) && (buttonValue < KEY_LEFT + KEY_OFFSET)) {
      page = page - 1;
      page = page < 0 ? page + 1 : page;  // if page below zero -> leave it
    }
    // RIGHT pressed: page -> page+1
    else if (buttonValue < KEY_RIGHT + KEY_OFFSET) {
      page = page + 1;

      if ((page > in0_rise_count) && (page > in1_rise_count))  // behind aviable values
        page = page - 1;
    }

    // print corresponding page
    //
    // startpage
    if (page == 0) {
      lcd.setCursor(0, 0);
      snprintf(buf, 17, "Eingang 1: %2d   >", in0_rise_count);
      lcd.print(buf);
      lcd.setCursor(0, 1);
      snprintf(buf, 17, "Eingang 2: %2d", in1_rise_count);
      lcd.print(buf);
    } else {

      // page == 1 -> fist value, index = 1 - 1 = 0
      uint8_t i = page - 1;

      // check for input0 / input1, if data is aviable
      if (i < in0_rise_count) {
        // time in ms
        diff = (in0_rise_data[i] - in0_fall_data[i]) / 1000;
        lcd.setCursor(0, 0);
        snprintf(buf, 30, "%d:  %d ms             ", page, diff);
        lcd.print(buf);
      } else {
        // if no data: blank line
        lcd.setCursor(0, 0);
        snprintf(buf, 30, "                     ");
        lcd.print(buf);
      }

      if (i < in1_rise_count) {
        // time in ms
        diff = (in1_rise_data[i] - in1_fall_data[i]) / 1000;
        lcd.setCursor(0, 1);
        snprintf(buf, 30, "%d:  %d ms             ", page, diff);
        lcd.print(buf);
      } else {
        // if no data: blank line
        lcd.setCursor(0, 1);
        snprintf(buf, 30, "                     ");
        lcd.print(buf);
      }
    }
  }
}


// Interrupt Service Routine for INT0 (PD2)
void isr_timers_0() {
  // save current time and increment counter
  if (in0_rise_count >= MAX_DATA) return;

  // rising Edge
  if (PIND & (1 << PD2)) {
    in0_rise_data[in0_rise_count] = micros();
    in0_rise_count = in0_rise_count + 1;
  } else {
    in0_fall_data[in0_fall_count] = micros();
    in0_fall_count = in0_fall_count + 1;
  }
}

// Interrupt Service Routine for INT1 (PD3)
void isr_timers_1() {
  if (in1_rise_count >= MAX_DATA) return;

  // save current time and increment counter
  if (PIND & (1 << PD3)) {
    in1_rise_data[in1_rise_count] = micros();
    in1_rise_count = in1_rise_count + 1;
  } else {
    in1_fall_data[in1_fall_count] = micros();
    in1_fall_count = in1_fall_count + 1;
  }
}





// *** duration ***
//
//
void app_duration() {
  isrHandler0 = isr_duration_0;
  isrHandler1 = isr_duration_1;

  uint16_t buttonValue;
  char buf[30];
  uint32_t diff;     //delta t
  int16_t page = 0;  // which page (0 .. 9) to display
  // uint8_t last_page = 0;
  uint8_t counts;  // how many duartion have finished?

  init_data();

  lcd.clear();


  while (1) {
    buttonValue = analogRead(buttonPin);
    wait_for_release(buttonValue);

    // the lesser value has finished
    counts = in0_fall_count >= in1_fall_count ? in1_fall_count : in0_fall_count;

    // LEFT pressed: page -> page-1
    if ((buttonValue > KEY_LEFT - KEY_OFFSET) && (buttonValue < KEY_LEFT + KEY_OFFSET)) {
      page = page - 1;
      page = page < 0 ? page + 1 : page;  // if page below zero -> leave it
    }
    // RIGHT pressed: page -> page+1
    else if (buttonValue < KEY_RIGHT + KEY_OFFSET) {
      page = page + 1;

      if ((page > in0_fall_count) && (page > in1_fall_count))  // behind aviable values
        page = page - 1;
    }

    // startpage
    if (page == 0) {
      lcd.setCursor(0, 0);
      snprintf(buf, 17, "Zeiten: %2d   >", counts);
      lcd.print(buf);
      lcd.setCursor(0, 1);  //clear, if previous page
      lcd.print("                     ");
    } else {
      // page == 1 -> fist value, index = 1 - 1 = 0
      uint8_t i = page - 1;

      // time in ms
      diff = abs((int32_t)in0_fall_data[i] - (int32_t)in1_fall_data[i]) / 1000;
      lcd.setCursor(0, 0);
      snprintf(buf, 30, "%d:  %d ms             ", page, diff);
      lcd.print(buf);
    }
  }
}


// Interrupt Service Routine for INT0 (PD2)
void isr_duration_0() {

  // save current time and increment counter
  if (in0_fall_count >= MAX_DATA) return;

  // only record falling Edge
  if (PIND & (1 << PD2)) {  // rise
  } else {                  // fall
    in0_fall_data[in0_fall_count] = micros();
    in0_fall_count = in0_fall_count + 1;
  }
}

// Interrupt Service Routine for INT1 (PD3)
void isr_duration_1() {
  if (in1_fall_count >= MAX_DATA) return;

  // save current time and increment counter
  if (PIND & (1 << PD3)) {
  } else {
    in1_fall_data[in1_fall_count] = micros();
    in1_fall_count = in1_fall_count + 1;
  }
}




//
//
// duration2
// *** duration ***
//
//
void app_duration2() {
  isrHandler0 = isr_duration_0_2;
  isrHandler1 = isr_duration_1_2;

  uint16_t buttonValue;
  char buf[30];
  uint32_t diff;  //delta t

  init_data();

  lcd.clear();


  while (1) {
    delay(100);  // update not so often needed

    // print measurement only if data aviable
    // the first line is a status line
    // the second shows the result of the last duration
    if (in0_fall_count + in1_fall_count == 2) {
      // time in ms
      diff = abs((int32_t)in0_fall_data[0] - (int32_t)in1_fall_data[0]);
      lcd.setCursor(0, 1);
      snprintf(buf, 30, "%d ms ", diff);
      lcd.print(buf);

      // reset to no data aviable
      in0_fall_count = 0;
      in1_fall_count = 0;
    } else {
      // print status of the inputs
      lcd.setCursor(0, 0);
      snprintf(buf, 30, "Status: %d %d ", in0_fall_count, in1_fall_count);
      lcd.print(buf);  
    }
  }
}



// Interrupt Service Routine for INT0 (PD2)
void isr_duration_0_2() {

  // only record falling Edge
  if (PIND & (1 << PD2)) {  // rise
  } else {                  // fall
    in0_fall_data[0] = millis();
    in0_fall_count = 1;  // set True, means: there is a measurement recorded
  }
}

// Interrupt Service Routine for INT1 (PD3)
void isr_duration_1_2() {
  if (in1_fall_count >= MAX_DATA) return;

  // save current time and increment counter
  if (PIND & (1 << PD3)) {
  } else {
    in1_fall_data[0] = millis();
    in1_fall_count = 1;  // set True, means: there is a measurement recorded
  }
}





//
//
//
void app_counter() {
  return;
}

void app_frequency() {
  return;
}
