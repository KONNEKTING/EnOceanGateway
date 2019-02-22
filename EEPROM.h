#ifndef EEPROM_h
#define EEPROM_h

#include <Wire.h>

byte readMemory(int index) {//Example for 24AA02E64 eeprom chip
    // when using external storage, implement READ command here
   // return EEPROM.read(index);
  byte rdata = 0xFF;
  Wire.beginTransmission(0x50);
  Wire.write((int)(index >> 8));   // MSB
  Wire.write((int)(index & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(0x50,1);
  if (Wire.available()) rdata = Wire.read();
  return rdata;
}

void writeMemory(int index, byte val) {//Example for 24AA02E64 eeprom chip
    // when using external storage, implement WRITE command here
   // return EEPROM.write(index, val);
  Wire.beginTransmission(0x50);
  Wire.write((int)(index >> 8));   // MSB
  Wire.write((int)(index & 0xFF)); // LSB
  Wire.write(val);
  Wire.endTransmission();
  delay(5);//is it needed?!
}

void updateMemory(int index, byte val) {
    // when using external storage, implement UPDATE command here
    //return EEPROM.update(index, val);
      if (readMemory(index) != val) {
        writeMemory(index, val);
    }
}

void commitMemory() {
    // EEPROM needs no commit, so this function is intentionally left blank 
}

#endif
