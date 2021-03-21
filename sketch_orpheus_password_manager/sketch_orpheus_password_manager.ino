// Encrypts four passwords and saves them to EEPROM
// Reads them and decrypts again.
// void loop not used for now.


#include "Keyboard.h"
#include "AESLib.h"
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <rotary.h>                 // rotary handler

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

#define PINB 4
#define PINA 5
#define PUSHB 6

// Initialize the Rotary object
// Rotary(Encoder Pin 1, Encoder Pin 2, Button Pin) Attach center to ground
Rotary r = Rotary(PINA, PINB, PUSHB);        // there is no must for using interrupt pins !!

int columnsLCD = 20;
char* main_menu[] = {"Use a Password", "Generate a Password"};

char* MenuLine[] = {"Google", "repl.it", "Twitter", "Instagram", "Instructables", "Pinterest", "Facebook"};

int MenuItems = 7;
int CursorLine = 0;

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 10000;  //the value is a number of milliseconds


uint8_t key[] = {1, 13, 2, 14, 4, 5, 0, 7, 8, 9, 5, 12, 15, 2, 0, 1};
char data[17];

// All possible characters in a password, modified from https://wmich.edu/arts-sciences/technology-password-tips
String characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890~`!@#$%^&*()_-+={}[]:;""''<>,.?/|";

void setup() {
  digitalWrite(PINA, HIGH);     // enable pull-ups
  digitalWrite(PINB, HIGH);
  digitalWrite(PUSHB, HIGH);
  lcd.init();
  lcd.backlight();

  Serial.begin(9600);
  while (!Serial);

  // initialize control over the keyboard:
  Keyboard.begin();

  //while (1);

  randomSeed(analogRead(0));
  print_menu();

}
bool in_main_menu = true;
bool main_menu_option = 0;

void loop() {
  volatile unsigned char result = r.process();
  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  if (currentMillis - startMillis >= period)  //test whether the period has elapsed
  {
    LCDfadeOut();      //set LCD to sleep...
  } //End if currenMillis...

  if (result) {
    init_backlight();  //wake up LCD...
    result == DIR_CCW ? CursorLine = CursorLine - 1 : CursorLine = CursorLine + 1;
    Serial.println(result);
    if (CursorLine < 0) {
      CursorLine = MenuItems - 1; // roll over to last item
    } else if (CursorLine > MenuItems - 1) {
      CursorLine = 0;                 // roll over to first item
    }
    print_menu();
  } //End if result

  if (r.buttonPressedReleased(25)) {
    init_backlight();     //wake up LCD...
    lcd.setCursor(0, 0);  //(col, row)
    lcd.print("You selected:");
    lcd.setCursor(0, 1);  //(col, row)
    selection();
    print_menu();
  } //endif buttonPressedReleased
}


//---------------FUNCTIONS----------------

void print_menu()
{
  lcd.clear();
  lcd.setCursor(0, 1);     //(col, row)
  lcd.print("  Choose Password:  ");
  lcd.setCursor(0, 2);    //2nd row
  lcd.print("<"); lcd.setCursor(columnsLCD - 1, 2); lcd.print(">");
  lcd.setCursor((20 - strlen(MenuLine[CursorLine])) / 2, 2);
  lcd.print(MenuLine[CursorLine]);
}

void selection()
{
  switch (CursorLine) {
    case 0:
      lcd.print("Option 1    ");
      //set a flag or do something....
      break;
    case 1:
      lcd.print("Option 2    ");
      //set a flag or do something....
      break;
    case 2:
      lcd.print("Option 3    ");
      //set a flag or do something....
      break;
    case 3:
      lcd.print("Option 4    ");
      //set a flag or do something....
      break;
    case 4:
      lcd.print("Option 5    ");
      //set a flag or do something....
      break;
    case 5:
      lcd.print("Option 6    ");
      //set a flag or do something....
      break;
    case 6:
      lcd.print("Option 7    ");
      //set a flag or do something....
      break;
    default:
      break;
  } //end switch

  delay(2000);
  CursorLine = 0;     // reset to start position
} //End selection

void LCDfadeOut() {
  //  lcd.noBacklight();
  //  lcd.clear();
} //End LCDfadeOut

void init_backlight() {
  lcd.backlight();
} //End init_backlight

void typePassword(String password) {
  // Type passwords randomly so that anybody looking at the screen cannot guess the password.
  // -------------------------------------------------------------------
  for (byte i = 0; i < password.length(); i++) {
    Keyboard.print(password[i]);
    Keyboard.print(characters[random(84)]);
  }
  for (byte i = 0; i < password.length(); i++) {
    Keyboard.write(KEY_BACKSPACE);
    Keyboard.write(KEY_LEFT_ARROW);
  }
  //-------------Collected from an old code of mine---------------------

  Keyboard.write(KEY_RETURN);
}

void write_password(String password, byte slot) {   // Writes password to EEPROM with encryption
  sprintf(data, "%.15s", password.c_str());
  aes128_enc_single(key, data);

  for (int j = slot * 16; j < (slot * 16) + 16; j++) {
    EEPROM.put(j, data[j - slot * 16]);
  }
  memset(data, 0, sizeof(data));        // clear the array
}

void read_raw(byte slot) {                        // Reads encrypted raw password from EEPROM
  for (int j = slot * 16; j < slot * 16 + 16; j++) {
    EEPROM.get(j, data[j - slot * 16]);
  }
}

void read_password(byte slot) {                   // Reads password and decrypts
  for (int j = slot * 16; j < slot * 16 + 16; j++) {
    EEPROM.get(j, data[j - slot * 16]);
  }
  aes128_dec_single(key, data);
}
