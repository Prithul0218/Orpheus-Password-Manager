#include "Keyboard.h"

String passwords[] = {"abcdef", "123456", "12345678"};

// All possible characters in a password, modified from https://wmich.edu/arts-sciences/technology-password-tips
String characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890~`!@#$%^&*()_-+={}[]:;""''<>,.?/|";

void setup() {
  // initialize control over the keyboard:
  Keyboard.begin();

  //  Serial.begin(9600);
  //  while (!Serial);
  //  Serial.println(characters);
  //  while (1);

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
    delay(350);
    Keyboard.write(KEY_LEFT_ARROW);
    delay(350);
  }
  // * - Collected from an old code of mine.

  Keyboard.write(KEY_RETURN);

  while (1);

}
