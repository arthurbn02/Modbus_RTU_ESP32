#include "my_modbus.h"
#include "bts_convert.h"
ModbusRTU mb;
uint16_t arr[100];

char phone[11] = "0987654321";
uint8_t button_data[100];
float sensor_data[100];

static void MB_Slave_Debug_Data(uint8_t *data, uint16_t length);

void MB_Slave_Run(void)
{
  for(int i = 0; i < SENSOR_NUMBER; i++)
  {
    sensor_data[i] = random(1, 20) + 0.5;
  }
  for(int i = 0; i < 20; i++)
  {
    button_data[i] = i + 100;
  }
  MB_Slave_Write_Sensor(sensor_data);
  MB_Slave_Write_Button_Change(1);
  MB_Slave_Write_Button_State(button_data);
  MB_Slave_Write_Calib_Parameters(4.5, 5.5, 6.5);
  MB_Slave_Write_Get_Status(1, phone);
  MB_Slave_Write_Ping_Status(1, phone);
  MB_Slave_Write_Status(1);
  mb.task();
  yield();
}

void MB_Slave_Setup(void)
{
    Serial2.begin(9600);
    mb.begin(&Serial2);
    mb.slave(SLAVE_ID);
    mb.addHreg(START_REGISTER_USED, 0, NUMBER_REGISTER_USED);
}

uint16_t MB_Slave_Read_Register(uint16_t reg_address)
{
  uint16_t  temp;
  mb.Hreg(reg_address, temp);
  return temp;
}

void MB_Slave_Write_Register(uint16_t * array, uint16_t length, uint16_t reg_offset)
{
  for(int i = 0; i < length; i++)
  {
    mb.Hreg(reg_offset + i, array[i]);
  }
}

void MB_Slave_Write_Sensor(float *data)
{
  for(int i = 0; i < SENSOR_NUMBER; i++)
  {
    uint16_t *temp_data = Convert_From_Float_To_Uint16(data[i]);
    mb.Hreg(SENSOR_DATA_START_REGISTER + i*2, temp_data[0]);
    mb.Hreg(SENSOR_DATA_START_REGISTER + i*2 + 1, temp_data[1]);
  }
}

void MB_Slave_Write_Button_State(uint8_t *data)
{
  for(int i = 0; i < BUTTON_NUMBER; i += 2)
  {
    uint16_t temp_data = Convert_From_Bytes_To_Uint16(data[i], data[i + 1]);
    mb.Hreg(BUTTON_DATA_START_REGISTER + i/2, temp_data);
  }
}

void MB_Slave_Write_Button_Change(uint16_t data)
{
  mb.Hreg(BUTTON_CHANGE_START_REGISTER, data);
}

void MB_Slave_Write_Status(uint16_t data)
{
  mb.Hreg(STATUS_START_REGISTER, data);
}

void MB_Slave_Write_Ping_Status(uint16_t data, String phone_number)
{
  mb.Hreg(PING_STATUS_START_REGISTER, data);
  for(int i = 0; i < 10; i += 2)
  {
    uint16_t temp_data = Convert_From_Bytes_To_Uint16(phone_number[i], phone_number[i + 1]);
    mb.Hreg(PING_STATUS_START_REGISTER + i/2 + 1, temp_data);
  }
}

void MB_Slave_Write_Calib_Parameters(float a, float b, float c)
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

void MB_Slave_Write_Get_Status(uint16_t data, String phone_number)
{
  mb.Hreg(GET_STATUS_START_REGISTER, data);
  for(int i = 0; i < 10; i += 2)
  {
    uint16_t temp_data = Convert_From_Bytes_To_Uint16(phone_number[i], phone_number[i + 1]);
    mb.Hreg(GET_STATUS_START_REGISTER + i/2 + 1, temp_data);
  }
}

void MB_Slave_Read_Led_Logic(uint8_t *data)
{
  uint8_t led_number = data[3], value = data[4];
  Serial.println("Led number: " + String(led_number));
  Serial.println("Value: " + String(value));
}

void MB_Slave_Read_Led_Blink(uint8_t *data)
{
  led_blink_message_e message_state;
  uint16_t number_byte = data[DEFAULT_NUMBER_DATA], led_number, time_on, time_off;
  message_state = LB_LED_NUMBER_STATE;
  for(uint16_t i = DEFAULT_DATA_START; i < DEFAULT_DATA_START + number_byte; i += 2)
  { 
    switch (message_state)
    {
      case LB_LED_NUMBER_STATE:
        led_number = Convert_From_Bytes_To_Uint16(data[i + 1], data[i]);
        message_state = LB_TIME_ON_STATE;
        break;
      case LB_TIME_ON_STATE:
        time_on = Convert_From_Bytes_To_Uint16(data[i + 1], data[i]);
        message_state = LB_TIME_OFF_STATE;
        break;
      case LB_TIME_OFF_STATE:
        time_off = Convert_From_Bytes_To_Uint16(data[i + 1], data[i]);
        message_state = LB_LED_NUMBER_STATE;
        break;
      default:
        break;
    }
  }
  Serial.println("Led blink Led Number: " + String(led_number) + " " + "Time on: " + String(time_on) + " " +  "Time off: " + String(time_off));
}
void MB_Slave_Read_Ping_Response(uint8_t *data)
{
  ping_respone_message_e message_state;
  uint16_t number_byte = data[DEFAULT_NUMBER_DATA], temp_position = 0;
  uint8_t phone_number[10], ping_response_data[60];
  message_state = PR_PHONE_NUMBER_STATE;
  for(uint16_t i = DEFAULT_DATA_START; i < DEFAULT_DATA_START + number_byte; i ++)
  {
    switch (message_state)
    {
      case PR_PHONE_NUMBER_STATE:
        phone_number[temp_position++] = data[i];
        if(temp_position == 10)
        {
          temp_position = 0;
          message_state = PR_DATA_STATE;
        }
        break;
      case PR_DATA_STATE:
        ping_response_data[temp_position++] = data[i];
        if(temp_position == 60)
        {
          temp_position = 0;
          message_state = PR_PHONE_NUMBER_STATE;
        }
        break;
      default:
        break;
    }
  }
  Serial.print("Ping Resonse Phone Number: ");
  MB_Slave_Debug_Data(phone_number, 10);
  Serial.println();
  Serial.println("Ping Resonse Data: ");
  MB_Slave_Debug_Data(ping_response_data, 60);
  Serial.println();
}
void MB_Slave_Read_Update_Frequence(uint8_t *data)
{
  uint16_t number_byte = data[DEFAULT_NUMBER_DATA], temp_position = 0;
  uint8_t frequen_arr[4];
  int32_t frequency;
  for(uint16_t i = DEFAULT_DATA_START; i < DEFAULT_DATA_START + number_byte; i ++)
  {
    frequen_arr[temp_position++] = data[i];
  }
  frequency = Convert_From_Bytes_To_Int(frequen_arr[0], frequen_arr[1], frequen_arr[2], frequen_arr[3]);
  Serial.println("Config frequency:" + String(frequency));
}
void MB_Slave_Read_Response_Status(uint8_t *data)
{
  respone_status_message_e message_state;
  uint16_t number_byte = data[DEFAULT_NUMBER_DATA], temp_position = 0, mqtt_disconnect_time;
  uint8_t phone_number[10], mac_address[6], location[20], pin_rate, sim_rate, reset_time, mqtt_state;
  float sensor_current;
  uint32_t ping_time, number_mqtt_success, success_mqtt_time;
  message_state = RS_PHONE_NUMBER_STATE;
  for(uint16_t i = DEFAULT_DATA_START; i < DEFAULT_DATA_START + number_byte; i ++)
  {
    switch (message_state)
    {
      case RS_PHONE_NUMBER_STATE:
        phone_number[temp_position++] = data[i];
        if(temp_position == 10)
        {
          temp_position = 0;
          message_state = RS_MAC_ADDRESS_STATE;
        }
        break;
      case RS_MAC_ADDRESS_STATE:
        mac_address[temp_position++] = data[i];
        if(temp_position == 6)
        {
          temp_position = 0;
          message_state = RS_LOCATION_STATE;
        }
        break;
      case RS_LOCATION_STATE:
        location[temp_position++] = data[i];
        if(temp_position == 20)
        {
          temp_position = 0;
          message_state = RS_SENSOR_DATA_STATE;
        }
        break;
      case RS_SENSOR_DATA_STATE:
        uint8_t temp_data[4];
        temp_data[temp_position++] = data[i];
        if(temp_position == 4)
        {
          sensor_current = Convert_From_Bytes_To_Float(temp_data[0], temp_data[1], temp_data[2], temp_data[3]);
          temp_position = 0;
          message_state = RS_PIN_SIM_STATE;
        }
        break;
      case RS_PIN_SIM_STATE:
        if(temp_position++ == 0)
        {
          pin_rate = data[i];
        }
        else if(temp_position == 1)
        {
          sim_rate = data[i];
          temp_position = 0;
          message_state = RS_INTERNET_STATUS_STATE;
        }
        break;
      case RS_INTERNET_STATUS_STATE:
        uint8_t temp_data[4];
        temp_data[temp_position++] = data[i];
        if(temp_position == 4)
        {
          ping_time = Convert_From_Bytes_To_Int(temp_data[0], temp_data[1], temp_data[2], temp_data[3]);
          temp_position = 0;
          message_state = RS_DISCONNECT_TIME_STATE;
        }
        break;
      case RS_DISCONNECT_TIME_STATE:
        uint8_t temp_data[2];
        temp_data[temp_position++] = data[i];
        if(temp_position == 2)
        {
          mqtt_disconnect_time = Convert_From_Bytes_To_Uint16(temp_data[0], temp_data[1]);
          temp_position = 0;
          message_state = RS_NUMBER_MQTT_SUCCESS_STATE;
        }
        break;
      case RS_NUMBER_MQTT_SUCCESS_STATE:
        uint8_t temp_data[4];
        temp_data[temp_position++] = data[i];
        if(temp_position == 4)
        {
          number_mqtt_success = Convert_From_Bytes_To_Int(temp_data[0], temp_data[1], temp_data[2], temp_data[3]);
          temp_position = 0;
          message_state = RS_TIME_MQTT_STATE;
        }
        break;
      case RS_TIME_MQTT_STATE:
        uint8_t temp_data[4];
        temp_data[temp_position++] = data[i];
        if(temp_position == 4)
        {
          success_mqtt_time = Convert_From_Bytes_To_Int(temp_data[0], temp_data[1], temp_data[2], temp_data[3]);
          temp_position = 0;
          message_state = RS_MQTT_STATUS_STATE;
        }
        break;
      case RS_MQTT_STATUS_STATE:
        if(temp_position++ == 0)
        {
          reset_time = data[i];
        }
        else if(temp_position == 1)
        {
          mqtt_state = data[i];
          temp_position = 0;
          message_state = RS_PHONE_NUMBER_STATE;
        }
        break;
      default:
        break;
    }
  }
  Serial.print("Resonse Status Phone Number: ");
  MB_Slave_Debug_Data(phone_number, 10);
  Serial.println();
  Serial.println("Resonse Status Mac Address: ");
  MB_Slave_Debug_Data(mac_address, 20);
  Serial.println();
  Serial.print("Resonse Status Location: ");
  MB_Slave_Debug_Data(location, 20);
  Serial.println();
  Serial.println("Resonse Status Sensor Data: " + String(sensor_current));
  Serial.println("Resonse Status Pin Rate: " + String(pin_rate));
  Serial.println("Resonse Status Sim Rate: " + String(sim_rate));
  Serial.println("Resonse Status Ping Time: " + String(ping_time));
  Serial.println("Resonse Status MQTT Disconnect Time: " + String(mqtt_disconnect_time));
  Serial.println("Resonse Status Number MQTT Success: " + String(number_mqtt_success));
  Serial.println("Resonse Status Success MQTT Time: " + String(success_mqtt_time));
  Serial.println("Resonse Status Signal Status: ");
  Serial.println("Resonse Status Reset Time: " + String(reset_time));
  Serial.println("Resonse Status Mqtt State: " + String(mqtt_state));
}
void MB_Slave_Read_Config_Location(uint8_t *data)
{
  config_location_message_e message_state;
  uint16_t number_byte = data[DEFAULT_NUMBER_DATA], temp_position = 0;
  uint8_t longitude[4], latitude[4];
  message_state = CL_LONGITUDE_STATE;
  for(uint16_t i = DEFAULT_DATA_START; i < DEFAULT_DATA_START + number_byte; i ++)
  {
    switch (message_state)
    {
      case CL_LONGITUDE_STATE:
        longitude[temp_position++] = data[i];
        if(temp_position == 4)
        {
          temp_position = 0;
          message_state = CL_LATITUDE_STATE;
        }
        break;
      case CL_LATITUDE_STATE:
        latitude[temp_position++] = data[i];
        if(temp_position == 4)
        {
          temp_position = 0;
          message_state = CL_LONGITUDE_STATE;
        }
        break;
      default:
        break;
    }
  }
  Serial.print("Config Location Longitude: ");
  MB_Slave_Debug_Data(longitude, 4);
  Serial.println();
  Serial.println("Config Location Latitude: ");
  MB_Slave_Debug_Data(latitude, 4);
  Serial.println();
}

void MB_Slave_Read_Config_MQTT_Server(uint8_t *data)
{
  config_mqtt_message_e message_state;
  uint16_t number_byte = data[DEFAULT_NUMBER_DATA], temp_position = 0;
  uint8_t mqtt_address[20], mqtt_port[4], user[20], password[20], receive_data[20], send_data[20];
  message_state = CM_MQTT_ADDRESS_STATE;
  for(uint16_t i = DEFAULT_DATA_START; i < DEFAULT_DATA_START + number_byte; i ++)
  {
    switch (message_state)
    {
      case CM_MQTT_ADDRESS_STATE:
        mqtt_address[temp_position++] = data[i];
        if(temp_position == 20)
        {
          temp_position = 0;
          message_state = CM_MQTT_PORT_STATE;
        }
        break;
      case CM_MQTT_PORT_STATE:
        mqtt_port[temp_position++] = data[i];
        if(temp_position == 4)
        {
          temp_position = 0;
          message_state = CM_USER_STATE;
        }
        break;
      case CM_USER_STATE:
        user[temp_position++] = data[i];
        if(temp_position == 20)
        {
          temp_position = 0;
          message_state = CM_PASSWORD_STATE;
        }
        break;
      case CM_PASSWORD_STATE:
        password[temp_position++] = data[i];
        if(temp_position == 20)
        {
          temp_position = 0;
          message_state = CM_DATA_RECEIVE_STATE;
        }
        break;
      case CM_DATA_RECEIVE_STATE:
        receive_data[temp_position++] = data[i];
        if(temp_position == 20)
        {
          temp_position = 0;
          message_state = CM_DATA_SEND_STATE;
        }
        break;
      case CM_DATA_SEND_STATE:
        send_data[temp_position++] = data[i];
        if(temp_position == 20)
        {
          temp_position = 0;
          message_state = CM_MQTT_ADDRESS_STATE;
        }
        break;
      default:
        break;
    }
  }
  Serial.print("Config MQTT Sever Address: ");
  MB_Slave_Debug_Data(mqtt_address, 20);
  Serial.println();
  Serial.println("Config MQTT Port: ");
  MB_Slave_Debug_Data(mqtt_port, 4);
  Serial.println();
  Serial.print("Config MQTT User: ");
  MB_Slave_Debug_Data(user, 20);
  Serial.println();
  Serial.println("Config MQTT Password: ");
  MB_Slave_Debug_Data(password, 20);
  Serial.println();
  Serial.print("Config MQTT Receive Data: ");
  MB_Slave_Debug_Data(receive_data, 20);
  Serial.println();
  Serial.println("Config MQTT Send Data: ");
  MB_Slave_Debug_Data(send_data, 20);
  Serial.println();
}

//USED in ModbusRTU.cpp in modbus-esp8266 library
void MB_Slave_Filter_Read_Message(uint8_t *data)
{
  uint8_t function = data[0];
  switch(function)
  {
    case 0x06:
      MB_Slave_Read_Led_Logic(data);
      break;
    case 0x10:
      MB_Slave_Filter_Read_Multi_Register(data);
      break;
    default:
      break;
  }
}
void MB_Slave_Filter_Read_Multi_Register(uint8_t *data)
{
  uint16_t start_address = Convert_From_Bytes_To_Uint16(data[2], data[1]);
  switch (start_address)
  {
  case LED_BLINK_START_REGISTER:
    MB_Slave_Read_Led_Blink(data);
    break;
  case PING_RESPONSE_START_REGISTER:
    MB_Slave_Read_Ping_Response(data);
    break;
  case CONFIG_FREQUENCE_START_REGISTER:
    MB_Slave_Read_Update_Frequence(data);
    break;
  case RESPONSE_STATUS_START_REGISTER:
    MB_Slave_Read_Response_Status(data);
    break;
  case CONFIG_LOCATION_START_REGISTER:
    MB_Slave_Read_Config_Location(data);
    break;
  case CONFIG_MQTT_SEVER_START_REGISTER:
    MB_Slave_Read_Config_MQTT_Server(data);
    break;
  default:
    break;
  }
}

void MB_Slave_Debug_Data(uint8_t *data, uint16_t length)
{
  for(uint16_t i = 0; i < length; i++)
  {
    Serial.printf("%p ", data[i]);
  }
}