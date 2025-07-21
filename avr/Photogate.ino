#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdarg.h>  // Für va_list, va_start, va_end
#include <stdio.h>   // Für vsnprintf
#include <LiquidCrystal.h>
#include <inttypes.h>

#include "common.h"

// menues
typedef enum { MENU,
               APP } entry_type;

typedef struct menu_entry {
  char name[17];
  entry_type type;
  void* reference;  // reference to app-funktion or menu
  size_t len;       // length of menu (=array of menu_entries)
} menu_entry;


// Debug-Menu
menu_entry edges = { "Flanken", APP, &app_edges, 0 };
menu_entry keys = { "Tasten", APP, &app_keys, 0 };
menu_entry inputs = { "Input-Test", APP, &app_inputs, 0 };
menu_entry menu_debug[] = { edges, keys, inputs };

// Main-Menu
menu_entry timers = { "Timer", APP, &app_timers, 0 };
menu_entry duration = { "Laufzeit", APP, &app_duration, 0 };
menu_entry duration2 = { "Laufzeit2", APP, &app_duration2, 0 };
menu_entry counter = { "Zaehler", APP, &app_counter, 0 };
menu_entry frequency = { "Frequenz", APP, &app_frequency, 0 };
menu_entry debug = { "Debug", MENU, menu_debug, sizeof(menu_debug) / sizeof(*menu_debug) };
menu_entry menu_main[] = { timers, duration, duration2, counter, frequency, debug };


// Init LCD&Button-Shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
const int buttonPin = A0;


// function pointer to the interrupt handler
// the ISRs only start these handlers
void (*isrHandler0)(void) = 0;
void (*isrHandler1)(void) = 0;


void setup() {
  // set INT0 and INT1
  interrupts_init();
  sei();
  // UART
  Serial.begin(9600);
  // LCD initialisieren
  lcd.begin(LCD_x, LCD_y);
  // Interrupts initialisieren
  sei();
}




void loop() {
  menu_handler(menu_main, sizeof(menu_main) / sizeof(*menu_main));
  // app_debug_keys();
}




uint8_t menu_handler(menu_entry menu[], size_t len_of_menu) {
  int8_t entry_num = 0;  // set to first entry on start
  char buf[17];
  uint16_t buttonValue;

  goto first_run;  //at first no button is pressed. print out menu anyway

  while (1) {
    // look, if right or left has been pressed
    buttonValue = analogRead(buttonPin);
    wait_for_release(buttonValue);

    // LEFT pressed
    if ((buttonValue > KEY_LEFT - KEY_OFFSET) && (buttonValue < KEY_LEFT + KEY_OFFSET)) {
      entry_num = entry_num - 1;
      entry_num = entry_num < 0 ? len_of_menu - 1 : entry_num;  // cycle?
      for (uint8_t i = 0; i < LCD_x; i++) {   // cheap transiton
        delay(60);
        lcd.scrollDisplayRight();
      }
    }
    // RIGHT
    else if (buttonValue < KEY_RIGHT + KEY_OFFSET) {
      entry_num = entry_num + 1;
      entry_num = entry_num > len_of_menu - 1 ? 0 : entry_num;  // cycle?
      for (uint8_t i = 0; i < LCD_x; i++) {   // cheap transiton 
        delay(60);
        lcd.scrollDisplayLeft();
      }
    }
    // SELECT pressed
    else if ((buttonValue > KEY_SELECT - KEY_OFFSET) && (buttonValue < KEY_SELECT + KEY_OFFSET)) {
      if (menu[entry_num].type == APP) {
        ((void (*)())menu[entry_num].reference)();
      } else if (menu[entry_num].type == MENU) {
        menu_handler(menu[entry_num].reference,
                     menu[entry_num].len);
      }
    } else continue;

first_run:
    // interesting key has been pressed: print out entry}
    lcd.clear();
    lcd.setCursor(0, 0);
    char* str = menu[entry_num].name;
    snprintf(buf, 17, "< %" LCD_ENTRY_SPACE "s >", str);
    lcd.print(buf);
  }
}




// Interrupt Service Routine for INT0 (PD2)
ISR(INT0_vect) {
  // call only, if set to a function
  if (isrHandler0) {
    isrHandler0();
  }
}

// Interrupt Service Routine for INT0 (PD2)
ISR(INT1_vect) {
  // call only, if set to a function
  if (isrHandler1) {
    isrHandler1();
  }
}
