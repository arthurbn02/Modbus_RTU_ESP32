#include "my_modbus.h"
#include "bts_convert.h"
ModbusRTU mb;
uint16_t arr[100];

String phone = "0987654321";
uint8_t button_data[100];
float sensor_data[100];

void My_Modbus_Run(void)
{
  for(int i = 0; i < SENSOR_NUMBER; i++)
  {
    sensor_data[i] = random(1, 20) + 0.5;
  }
  for(int i = 0; i < 20; i++)
  {
    button_data[i] = i + 100;
  }
  My_Modbus_Write_Sensor(sensor_data);
  My_Modbus_Write_Button_Change(1);
  My_Modbus_Write_Button_State(button_data);
  My_Modbus_Write_Calib_Parameters(4.5, 5.5, 6.5);
  My_Modbus_Write_Get_Status(1, phone);
  My_Modbus_Write_Ping_Status(1, phone);
  My_Modbus_Write_Status(1);
  mb.task();
  yield();
}

void My_Modbus_Setup(void)
{
    Serial2.begin(9600);
    mb.begin(&Serial2);
    mb.slave(SLAVE_ID);
    mb.addHreg(START_REGISTER_USED, 0, NUMBER_REGISTER_USED);
}

uint16_t My_Modbus_Read_Register(uint16_t reg_address)
{
  uint16_t  temp;
  mb.Hreg(reg_address, temp);
  return temp;
}

void My_Modbus_Write_Register(uint16_t * array, uint16_t length, uint16_t reg_offset)
{
  for(int i = 0; i < length; i++)
  {
    mb.Hreg(reg_offset + i, array[i]);
  }
}

void My_Modbus_Write_Sensor(float *data)
{
  for(int i = 0; i < SENSOR_NUMBER; i++)
  {
    uint16_t *temp_data = Convert_From_Float_To_Uint16(data[i]);
    mb.Hreg(SENSOR_DATA_START_REGISTER + i*2, temp_data[0]);
    mb.Hreg(SENSOR_DATA_START_REGISTER + i*2 + 1, temp_data[1]);
  }
}

void My_Modbus_Write_Button_State(uint8_t *data)
{
  for(int i = 0; i < BUTTON_NUMBER; i += 2)
  {
    uint16_t temp_data = Convert_From_Bytes_To_Uint16(data[i], data[i + 1]);
    mb.Hreg(BUTTON_DATA_START_REGISTER + i/2, temp_data);
  }
}

void My_Modbus_Write_Button_Change(uint16_t data)
{
  mb.Hreg(BUTTON_CHANGE_START_REGISTER, data);
}

void My_Modbus_Write_Status(uint16_t data)
{
  mb.Hreg(STATUS_START_REGISTER, data);
}

void My_Modbus_Write_Ping_Status(uint16_t data, String phone_number)
{
  mb.Hreg(PING_STATUS_START_REGISTER, data);
  for(int i = 0; i < 10; i += 2)
  {
    uint16_t temp_data = Convert_From_Bytes_To_Uint16(phone_number[i], phone_number[i + 1]);
    mb.Hreg(PING_STATUS_START_REGISTER + i/2 + 1, temp_data);
  }
}

void My_Modbus_Write_Calib_Parameters(float a, float b, float c)
{
  uint16_t *temp_data1 = Convert_From_Float_To_Uint16(a);
  mb.Hreg(CALIB_PARAMETERS_START_REGISTER + 0, temp_data1[0]);
  mb.Hreg(CALIB_PARAMETERS_START_REGISTER + 1, temp_data1[1]);
  uint16_t *temp_data2 = Convert_From_Float_To_Uint16(b);
  mb.Hreg(CALIB_PARAMETERS_START_REGISTER + 2, temp_data2[0]);
  mb.Hreg(CALIB_PARAMETERS_START_REGISTER + 3, temp_data2[1]);
  uint16_t *temp_data3 = Convert_From_Float_To_Uint16(c);
  mb.Hreg(CALIB_PARAMETERS_START_REGISTER + 4, temp_data3[0]);
  mb.Hreg(CALIB_PARAMETERS_START_REGISTER + 5, temp_data3[1]);
}

void My_Modbus_Write_Get_Status(uint16_t data, String phone_number)
{
  mb.Hreg(GET_STATUS_START_REGISTER, data);
  for(int i = 0; i < 10; i += 2)
  {
    uint16_t temp_data = Convert_From_Bytes_To_Uint16(phone_number[i], phone_number[i + 1]);
    mb.Hreg(GET_STATUS_START_REGISTER + i/2 + 1, temp_data);
  }
}