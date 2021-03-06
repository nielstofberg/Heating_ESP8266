#ifndef _lib_eeprom_rw_anything_h_
#define _lib_eeprom_rw_anything_h_

#include <EEPROM.h>
#include <Arduino.h>  // for type definitions

template <class T> int lib_eeprom_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++) {
           EEPROM.write(ee++, *p++);
    }
    EEPROM.commit();
    Serial.print("Bytes saved to EEPROM: ");
    Serial.println(i);
    return i;
}

template <class T> int lib_eeprom_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
}

#endif //_lib_eeprom_rw_anything_h_
