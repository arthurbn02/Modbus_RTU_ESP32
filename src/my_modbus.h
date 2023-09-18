#ifndef _MYMODBUS_
#define _MYMODBUS_

#include <Arduino.h>
#include <ModbusRTU.h>

#define SLAVE_ID 1
#define START_REGISTER_USED 0
#define NUMBER_REGISTER_USED 227

#define SENSOR_DATA_START_REGISTER 0
#define BUTTON_DATA_START_REGISTER 32
#define BUTTON_CHANGE_START_REGISTER 40
#define STATUS_START_REGISTER 41
#define PING_STATUS_START_REGISTER 42
#define CALIB_PARAMETERS_START_REGISTER 48
#define GET_STATUS_START_REGISTER 54

#define SENSOR_NUMBER   16
#define BUTTON_NUMBER   16

void My_Modbus_Run(void);
void My_Modbus_Setup(void);
uint16_t My_Modbus_Read_Register(float data);
void My_Modbus_Write_Register(uint16_t * array, uint16_t length, uint16_t reg_offset);


void My_Modbus_Write_Sensor(float *data);
void My_Modbus_Write_Button_State(uint8_t *data);
void My_Modbus_Write_Button_Change(uint16_t data);
void My_Modbus_Write_Status(uint16_t data);
void My_Modbus_Write_Ping_Status(uint16_t data, String phone_number);
void My_Modbus_Write_Calib_Parameters(float a, float b, float c);
void My_Modbus_Write_Get_Status(uint16_t data, String phone_number);
#endif
