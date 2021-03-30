# EEPROM-Programmer

A general EEPROM programmer written for Arduino Mega.

## Hardware

_pass_

## Software

To start using the `EEPROMManager` class, you need the following code:

```cpp
void setup()
{
    // Setup serial monitor
    Serial.begin(DEFAULT_BAUD_RATE);

    // Creating instance of class EEPROMManager (stack alloc)
    EEPROMManager myEEPROM(41, 42, 22, 23, 24, 25, 26, 27, 28, 29,
        30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40);

    // init method (mandatory)
    myEEPROM.init();    
}
```

_under construction_
