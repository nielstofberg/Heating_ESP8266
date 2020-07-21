#include <Arduino.h>
#include "src/pins_arduino.h"
#include "src/app_gpio.h"

void app_gpio_setPin(String pin, int stat)
{
    unsigned int p = 0;
    if (pin == "D0")
        p = D0;
    else if (pin == "D1")
        p = D1;
    else if (pin == "D2")
        p = D2;
    else if (pin == "D3")
        p = D3;
    else if (pin == "D4")
        p = D4;
    else if (pin == "D5")
        p = D5;
    else if (pin == "D6")
        p = D6;
    else if (pin == "D7")
        p = D7;
    else if (pin == "D8")
        p = D8;
    digitalWrite(p, stat);
}

int app_gpio_getPin(String pin)
{
        unsigned int p = 0;
    if (pin == "D0")
        p = D0;
    else if (pin == "D1")
        p = D1;
    else if (pin == "D2")
        p = D2;
    else if (pin == "D3")
        p = D3;
    else if (pin == "D4")
        p = D4;
    else if (pin == "D5")
        p = D5;
    else if (pin == "D6")
        p = D6;
    else if (pin == "D7")
        p = D7;
    else if (pin == "D8")
        p = D8;
    return digitalRead(p);
}