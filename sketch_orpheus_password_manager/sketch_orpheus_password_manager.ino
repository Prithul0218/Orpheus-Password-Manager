// Encrypts four passwords and saves them to EEPROM
// Reads them and decrypts again.
// void loop not used for now.


#include "Keyboard.h"
#include "AESLib.h"
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <rotary.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define PINB 4
#define PINA 5
#define PUSHB 6


Rotary r = Rotary(PINA, PINB, PUSHB);

const byte columnsLCD = 20;

char* MenuLine[] = {"Google", "repl.it", "Twitter", "Instagram", "Instructables", "Pinterest", "Generate Password"};

int MenuItems = 7;
int CursorLine = 0;
bool locked = true;

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 1 * 60 * 1000; // Timeout for LCD and lock - 1 minute


uint8_t key[] = {1, 13, 2, 14, 4, 5, 0, 7, 8, 9, 5, 12, 15, 2, 0, 1}; // key for AES encryption
char data[17];

// All possible characters in a password, modified from https://wmich.edu/arts-sciences/technology-password-tips
String characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890~`!@#$%^&*()_-+={}[]:;""''<>,.?/|";

void setup() {
  digitalWrite(PINA, HIGH);     // enable pull-ups
  digitalWrite(PINB, HIGH);
  digitalWrite(PUSHB, HIGH);
  lcd.init();
  lcd.backlight();

  //  Serial.begin(9600);
  //  while (!Serial);
  Serial1.begin(115200);

  // initialize control over the keyboard:
  Keyboard.begin();

  if (locked)
    lock_screen();

  randomSeed(analogRead(0));
  print_menu();

}
bool in_main_menu = true;
bool main_menu_option = 0;

void loop() {
  //  if (locked) {
  //    lock_screen();
  //  }
  volatile unsigned char result = r.process();
  currentMillis = millis();  //get the current "time"
  if (currentMillis - startMillis >= period)  //test whether the period has elapsed
  {
    locked = true;
    lcd_fadeout();      // lock device and turn off LCD
  }

  if (result) {
    init_backlight();  //wake up LCD...
    result == DIR_CCW ? CursorLine = CursorLine - 1 : CursorLine = CursorLine + 1;

    if (CursorLine < 0) {
      CursorLine = MenuItems - 1; // roll over to last item
    } else if (CursorLine > MenuItems - 1) {
      CursorLine = 0;  // roll over to first item
    }
    print_menu();
  }

  if (r.buttonPressedReleased(25)) {
    lcd.clear();
    init_backlight();     //wake up LCD...
    lcd.setCursor(0, 0);  //(col, row)
    lcd.print("Press enter to use");
    lcd.setCursor(0, 1);  //(col, row)
    lcd.print("the password for:");
    lcd.setCursor((20 - strlen(MenuLine[CursorLine])) / 2, 3);
    lcd.print(MenuLine[CursorLine]);
    if (CursorLine == 6) {
      lcd.clear();
      lcd.setCursor(0, 1);  //(col, row)
      lcd.print("Your new Password is");
      String new_password = generate_password();
      lcd.setCursor(0, 2);  //(col, row)
      lcd.print(new_password);
      sprintf(data, "%.16s", new_password.c_str());
      type_password();
      Serial1.println();
      Serial1.println();
      Serial1.println(new_password);
      Serial1.println();CYa%V4Amm)f^Cqrk
      
      delay(2000);


      CursorLine = 0;     // reset to start position
    } else {
      selection();
    }
    print_menu();
  } //endif buttonPressedReleased
}


//---------------FUNCTIONS----------------

void print_menu()
{
  lcd.clear();
  lcd.setCursor(0, 1);     //(col, row)
  lcd.print(" Choose a Password: ");
  lcd.setCursor(0, 2);    //2nd row
  lcd.print("<"); lcd.setCursor(columnsLCD - 1, 2); lcd.print(">");
  lcd.setCursor((20 - strlen(MenuLine[CursorLine])) / 2, 2);
  lcd.print(MenuLine[CursorLine]);
}

void selection() {
  while (1) {
    if (r.buttonPressedReleased(25)) {
      lcd.clear();
      init_backlight();     //wake up LCD...
      lcd.setCursor(8, 1);  //(col, row)
      lcd.print("Done!");
      read_password(CursorLine);
      type_password();
      break;
    }
  }
  delay(1000);
  CursorLine = 0;     // reset to start position
}
void lcd_fadeout() {
  lcd.noBacklight();
  lcd.clear();
}

void init_backlight() {
  lcd.backlight();
  startMillis = millis();  //initial start time
}


void lock_screen() {

  while (locked) {
    lcd.setCursor(1, 1);
    lcd.print("Type Password or");
    lcd.setCursor(1, 2);
    lcd.print("verify with phone.");
    if (Serial1.available() > 0) {
      String received_password = Serial.readString();
      locked = false;
    }
  }
}



void type_password() {
  for (byte i = 0; i < 16; i++) {
    Keyboard.print(data[i]);
  }

  memset(data, 0, sizeof(data));        // clear the array
  Keyboard.write(KEY_END);
  Keyboard.write(KEY_RETURN);
}

void write_password(String password, byte slot) {   // Writes password to EEPROM with encryption
  sprintf(data, "%.16s", password.c_str());
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

String generate_password() {
  String generated_password = "";
  for (byte i = 0; i < 16; i++)
    generated_password += characters[random(84)];
  return generated_password;
}
