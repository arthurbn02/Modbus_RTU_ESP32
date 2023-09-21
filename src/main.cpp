/*
  ModbusRTU ESP8266/ESP32
  Simple slave example

  (c)2019 Alexander Emelianov (a.m.emelianov@gmail.com)
  https://github.com/emelianov/modbus-esp8266

  modified 13 May 2020
  by brainelectronics

  This code is licensed under the BSD New License. See LICENSE.txt for more info.
*/

#include "my_modbus.h"

void setup() {
  Serial.begin(9600);
  MB_Slave_Setup();
}

void loop() {
  MB_Slave_Run();
}

