#include <Arduino.h>

// Samsung with remote ARH-465 or remote ARH-1362

bool decodeSamsung(byte *bytes, int byteCount)
{  
  // If this looks like a Samsung code...
  if (bytes[0] == 0x02
      && (byteCount == 14 && bytes[1] == 0x92)
      && bytes[2] == 0x0F) 
  {
    Serial.println(F("Looks like a short 14 bytes Samsung protocol"));
    
    Serial.println(F("POWER ON"));

    // Operating mode
    switch (bytes[12] & 0xF0) {
      case 0x00:
        Serial.println(F("MODE AUTO"));
        break;
      case 0x10:
        Serial.println(F("MODE COOL"));
        break;
      case 0x20:
        Serial.println(F("MODE DRY"));
        break;
      case 0x30:
        Serial.println(F("MODE FAN"));
        break;
      case 0x40:
        Serial.println(F("MODE HEAT"));
        break;
    }

    // Temperature
    Serial.print(F("Temperature: "));
    Serial.println((bytes[11] >> 4) + 16);

    // Fan speed
    switch (bytes[12] & 0x0F) {
      case 0x01:
        Serial.println(F("FAN: AUTO"));
        break;
      case 0x05:
        Serial.println(F("FAN: 1 (low)"));
        break;
      case 0x09:
        Serial.println(F("FAN: 2 (medium)"));
        break;
      case 0x0B:
        Serial.println(F("FAN: 3 (high)"));
        break;
      case 0x0F:
        Serial.println(F("FAN: 4"));
        break;
    }

    // Airflow mode
    Serial.print(F("Airflow: "));
    switch (bytes[9] & 0xF0) {
      case 0xA0:
        Serial.println(F("ON"));
        break;
      case 0xF0:
        Serial.println(F("OFF"));
        break;
    }

    // Turbo mode
    Serial.print(F("Turbo mode: "));
    switch (bytes[10] & 0x0F) {
      case 0x07:
        Serial.println(F("ON"));
        break;
      case 0x01:
        Serial.println(F("OFF"));
        break;
    }

    // Check if the checksum matches
    byte originalChecksum = bytes[8];
    byte checksum = 0x00;

    // Calculate the byte 8 checksum
    // Count the number of ONE bits
    bytes[9] &= 0b11111110;
    for (uint8_t j=9; j<13; j++) {
      uint8_t samsungByte = bytes[j];
      for (uint8_t i=0; i<8; i++) {
        if ( (samsungByte & 0x01) == 0x01 ) {
          checksum++;
        }
        samsungByte >>= 1;
      }
    }

    checksum = 28 - checksum;
    checksum <<= 4;
    checksum |= 0x02;

    Serial.print(F("Checksum '0x"));
    Serial.print(checksum, HEX);

    if ( originalChecksum == checksum ) {
      Serial.println(F("' matches"));
    } else {
      Serial.print(F("' does not match 0x"));
      Serial.println(originalChecksum, HEX);
    }
    return true;
  }


  if (bytes[0] == 0x02
      && ((byteCount == 21 && bytes[1] == 0xB2) || (byteCount == 21 && bytes[1] == 0x92))
      && bytes[2] == 0x0F) 
  {
    Serial.println(F("Looks like a 21 bytes long Samsung protocol"));

    // Power mode
    if (byteCount == 21 && bytes[1] == 0xB2)
    {
      Serial.println(F("POWER OFF"));
    }
    else
      Serial.println(F("POWER ON"));

    // Operating mode | fan speed auto
    switch (bytes[19] & 0xF0) {
      case 0x00:
        Serial.println(F("MODE AUTO"));
        break;
      case 0x10:
        Serial.println(F("MODE COOL"));
        break;
      case 0x20:
        Serial.println(F("MODE DRY"));
        break;
      case 0x40:
        Serial.println(F("MODE HEAT"));
        break;
    }

    // Temperature
    Serial.print(F("Temperature: "));
    Serial.println((bytes[18] >> 4) + 16);

    // Fan speed
    switch (bytes[19] & 0x0F) {
      case 0x0D:
	  case 0x01:
        Serial.println(F("FAN: AUTO"));
        break;
      case 0x05:
        Serial.println(F("FAN: 1 (low)"));
        break;
      case 0x09:
        Serial.println(F("FAN: 2 (medium)"));
        break;
      case 0x0B:
        Serial.println(F("FAN: 3 (high)"));
        break;
    }

    // calculate the checksum
    uint8_t checksum = 0;
    byte originalChecksum = bytes[15];
  
    // Calculate the byte 15 checksum
    // Count the number of ONE bits on message uint8_ts 15-20
    for (uint8_t j=16; j<20; j++) {
      uint8_t Samsungbyte = bytes[j];
      for (uint8_t i=0; i<8; i++) {
        if ( (Samsungbyte & 0x01) == 0x01 ) {
          checksum++;
        }
        Samsungbyte >>= 1;
      }
    }

    // Transform the number of ONE bits to the actual checksum
    checksum = 28 - checksum;
    checksum <<= 4;
    checksum |= (byteCount == 21 && bytes[1] == 0xB2) ? 0x22 : 0x02;

    // incredible hack if power off and temp = 20 and mode heat or cool or dry
    if (byteCount == 21 && bytes[1] == 0xB2 && bytes[18] == 0x40 && ((bytes[19] & 0xF0) == 0x40 || (bytes[19] & 0xF0) == 0x20 || (bytes[19] & 0xF0) == 0x10))
      checksum = 0x02;     
	
    Serial.print(F("Checksum '0x"));
    Serial.print(checksum, HEX);

    if ( originalChecksum == checksum ) {
      Serial.println(F("' matches"));
    } else {
      Serial.print(F("' does not match 0x"));
      Serial.println(originalChecksum, HEX);
    }
    return true;
  }
  
  return false;
}
