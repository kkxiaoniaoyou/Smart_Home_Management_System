#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "adc_temp.h"
#include "tftlcd.h"
#include "key.h"
#include "dht11.h"
#include "exti.h"
#include "wifi_config.h"
#include "wifi_function.h"
#include "cJSON.h"
#include "weather.h"
#include "time.h"
#include <string.h>
#include "aliyun.h"


void connect_config()
{
	
	ESP8266_Cmd("AT+MQTTUSERCFG=0,1,\"NULL\",\"8266&k21duLSs825\",\"dcea6e64c47bbb4339225af55bf35c6af81c9fd2633bbe8f090635adf88ba723\",0,0,\"\"","OK",0,2000);
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);

	ESP8266_Cmd("AT+MQTTCLIENTID=0,\"k21duLSs825.8266|securemode=2\\,signmethod=hmacsha256\\,timestamp=1732681577358|\"","OK",0,2000);
	delay_ms(1000);
	delay_ms(1000);
		delay_ms(1000);
	ESP8266_Cmd("AT+MQTTCONN=0,\"iot-06z00c2sxcd8uir.mqtt.iothub.aliyuncs.com\",1883,1","OK",0,2000);
		delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	ESP8266_Cmd("AT+MQTTSUB=0,\"/k21duLSs825/8266/user/get\",1","OK",0,2000);
		delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	ESP8266_Cmd("AT+MQTTPUB=O,\"/k21duLSs825/8266/user/update\",\"{\\\"temp\\\":1}\",1,0","OK",0,2000);
		delay_ms(1000);
		delay_ms(1000);

/*	ESP8266_Cmd("AT+MQTTUSERCFG=0,1,\"NULL\",\"APP666&k21duLSs825\",\"e2ead80943e2b1c59d33bb119eb994bb352f414367a633983b728bacc5166088\",0,0,\"\"","OK",0,2000);
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);

	ESP8266_Cmd("AT+MQTTCLIENTID=0,\"k21duLSs825.APP666|securemode=2\\,signmethod=hmacsha256\\,timestamp=1732685846310|\"","OK",0,2000);
	delay_ms(1000);
	delay_ms(1000);
		delay_ms(1000);
	ESP8266_Cmd("AT+MQTTCONN=0,\"iot-06z00c2sxcd8uir.mqtt.iothub.aliyuncs.com\",1883,1","OK",0,2000);
for(int i=0;i<10;i++)
	{
				delay_ms(1000);
	}
	ESP8266_Cmd("AT+MQTTSUB=0,\"/k21duLSs825/APP666/user/get\",1","OK",0,2000);
		delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	ESP8266_Cmd("AT+MQTTPUB=O,\"/k21duLSs825/APP666/user/update\",\"{\\\"temp\\\":1}\",1,0","OK",0,2000);
		delay_ms(1000);
		delay_ms(1000);*/
}