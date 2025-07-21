
/* Keys
1023  no key
 720  select
 479  left
 305  down
 128  up
   0  right */
#define KEY_SELECT 720
#define KEY_LEFT 480
#define KEY_RIGHT 0
#define KEY_DOWN 305
#define KEY_UP 128
#define KEY_OFFSET 20 // tolerance for volatage of key


// LCD 
#define LCD_x 16
#define LCD_y 2
#define LCD_ENTRY_SPACE "12"  // LCD_x - 4, must be a string




// app functions
void app_keys();
void app_edges();
void app_timers();
void app_duration();
void app_duration2();
void app_counter();
void app_frequency();


// utility functions
void wait_for_release(uint16_t start_value);
void interrupts_init(void);



