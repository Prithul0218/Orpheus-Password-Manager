// Encrypts four passwords and saves them to EEPROM
// Reads them and decrypts again.
// void loop not used for now.


#include "Keyboard.h"
#include "AESLib.h"
#include <EEPROM.h>

uint8_t key[] = {1, 13, 2, 14, 4, 5, 0, 7, 8, 9, 5, 12, 15, 2, 0, 1};
char data[17];
String passwords[] = {"abcdef", "123456", "12345678", "2bOrNot2b"};

// All possible characters in a password, modified from https://wmich.edu/arts-sciences/technology-password-tips
String characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890~`!@#$%^&*()_-+={}[]:;""''<>,.?/|";

void setup() {
  Serial.begin(9600);
  while (!Serial);
  //  Serial.println(characters);


  Serial.println("encrypting data...");         // encrypt data and save in EEPROM
  for (byte i = 0; i < 4; i++) {
    sprintf(data, "%.15s", passwords[i].c_str());
    aes128_enc_single(key, data);

    for (int j = i * 16; j < (i * 16) + 16; j++) {
      EEPROM.put(j, data[j - i * 16]);
    }
  }

  memset(data, 0, sizeof(data));        // clear the array
 
  Serial.println("encrypted: ");        // read the encrypted data from the EEPROM
  for (byte i = 0; i < 4; i++) {
    for (int j = i * 16; j < i * 16 + 16; j++) {
      EEPROM.get(j, data[j - i * 16]);
      Serial.print(data[j - i * 16]);
    }
    Serial.println();
  }
  Serial.println();


  Serial.println("decrypting data...");     // read from EEPROM and decrypt to get all the passwords.
  Serial.println("decrypted: ");
  for (byte i = 0; i < 4; i++) {
    for (int j = i * 16; j < i * 16 + 16; j++) {
      EEPROM.get(j, data[j - i * 16]);
    }
    aes128_dec_single(key, data);
    Serial.print(data);
    Serial.println();
  }


  // initialize control over the keyboard:
  Keyboard.begin();

  while (1) {
    delay(100);
  }

  randomSeed(analogRead(0));


}

void loop() {
  delay(1000);
  byte password_index = 2;

  // Type passwords randomly so that anybody looking at the screen cannot guess the password.
  // *
  for (byte i = 0; i < passwords[password_index].length(); i++) {
    Keyboard.print(passwords[password_index][i]);
    delay(250);
    Keyboard.print(characters[random(84)]);
    delay(250);
  }
  for (byte i = 0; i < passwords[password_index].length(); i++) {
    Keyboard.write(KEY_BACKSPACE);
    delay(150);
    Keyboard.write(KEY_LEFT_ARROW);
    delay(150);
  }
  // * - Collected from an old code of mine.

  delay(500);
  Keyboard.write(KEY_END);
  Keyboard.write(KEY_RETURN);
  Keyboard.write(KEY_RETURN);
  Keyboard.print("Now at full speed!");
  Keyboard.write(KEY_RETURN);
  delay(1000);

  for (byte i = 0; i < passwords[password_index].length(); i++) {
    Keyboard.print(passwords[password_index][i]);
    Keyboard.print(characters[random(84)]);
  }
  for (byte i = 0; i < passwords[password_index].length(); i++) {
    Keyboard.write(KEY_BACKSPACE);
    Keyboard.write(KEY_LEFT_ARROW);
  }

  Keyboard.write(KEY_RETURN);

  while (1);



}
