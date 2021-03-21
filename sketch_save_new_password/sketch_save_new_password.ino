#include "AESLib.h"
#include <EEPROM.h>


uint8_t key[] = {1, 13, 2, 14, 4, 5, 0, 7, 8, 9, 5, 12, 15, 2, 0, 1};
char data[17];

void setup() {
  Serial.begin(9600);
  while (!Serial);

}

void loop() {
  Serial.println("Type a new password.");
  while (Serial.available() == 0);
  String newPassword = Serial.readString();

  Serial.println("Select a slot for saving (1-62)");
  while (Serial.available() == 0);
  int slot = Serial.parseInt();


  if (slot > 0 && slot < 63) {
    Serial.print("Saving to slot ");
    Serial.println(slot);
    write_password(newPassword, slot);
    read_password(slot);
    Serial.print("Saved as: ");
    Serial.println(data);
    Serial.println();
    Serial.println();
    Serial.println("Add more? (Y)");
    while (Serial.available() == 0);
    if (Serial.readString() != 'Y' || Serial.readString() != 'y') {
      while (1);
    }
  } else {
    Serial.println("Try again.");
  }

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
