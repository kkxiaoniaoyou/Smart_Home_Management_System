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
#include "beep.h"
#include <stdlib.h>
#include "ws2812.h"
#include "pwm.h"
#include "lsens.h"

#define		 ALI_USERNAME		    "ESP8266DUAN&k21dt3xDIVZ"
#define		 ALICLIENTLD		    "k21dt3xDIVZ.ESP8266DUAN|securemode=2\\,signmethod=hmacsha256\\,timestamp=1732686791158|"	
#define		 ALI_PASSWD			    "2691d0d687012ecee2a7e8c8cba99a8f7b461f5fc790ca4ddc0a4a23888558bd"
#define		 ALI_MQTT_HOSTURL	    "iot-06z00grum6hw08b.mqtt.iothub.aliyuncs.com"
#define		 ALI_PORT			    "1883"	

#define sub  "/sys/k21dt3xDIVZ/ESP8266DUAN/thing/service/property/set"	//��������
#define Topic  "/k21dt3xDIVZ/ESP8266DUAN/user/ESP8266DUAN"
//#define data  "{\\\"Temp\\\":25\\\,\\\"Wet\\\":50}"


		u8 temp = 0;
		u8 humi = 0;
		
		int i;
		cJSON *root;
		cJSON *array;
		cJSON *results0;
		cJSON *location;
		cJSON *now;
		char *zone;
		char *temperature;
		char *weather;
		//char *time2;

int main()
{
	int pwm_value = 0,pwm_flag=0; //���
	u8 lsens_value=0;  //����ǿ��
	u8 LsensValue=90; //������ֵ
	u8 TempValue=30; //�¶���ֵ
	u8 *timea=(u8 *)malloc(sizeof(u8) * 50);
	u8 *p1,*p2,*p3,*p4,*p5; //ʱ��ָ�� , ������ֵָ��
	bool flag=false;   //���㱨ʱ
	int flagtime = 0;  //���㱨ʱ
	u8 i=0;
	int mysql = 1;    //���ݿ⴫��ÿ3��ѭ������һ��
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����� ��2��
	LED_Init(); 
	RGB_LED_Init();
	USART1_Init(115200);
	Lsens_Init();//����ǿ��
	TFTLCD_Init();			//LCD��ʼ��
	KEY_Init();
	BEEP_Init();
	ADC_Temp_Init();
	DHT11_Init();
	TIM3_CH2_PWM_Init(500,72-1); //Ƶ����2Kh   //����
	TIM_SetCompare2(TIM3,400);
	My_EXTI_Init();
	
	delay_ms(200);	
	
	WiFi_Config();
	ESP8266_Choose ( ENABLE );
	ESP8266_Net_Mode_Choose ( STA );
	ESP8266_Cmd ( "AT", "OK", NULL, 200 );
	
	PC_Usart("����������\r\n");
	

	while(! ESP8266_JoinAP("ckwc","12345678"))
	{
		PC_Usart("��������ʧ��,������\r\n");
		delay_ms(5000);
	}
	PC_Usart("��������ɹ�\r\n");
	
	
//����
	Get_Weather();
	parse_weather_json(strEsp8266_Fram_Record .Data_RX_BUF);
	strEsp8266_Fram_Record .InfBit .FramLength = 0;


	LCD_ShowFontHZ(120,50,"�־���");


	delay_ms(1000);
	PC_Usart("���Ӱ�����\r\n");
	
	// ���ð����˺�,����
	while(!ESP8266_MQTT_UserConfig(NULL,ALI_USERNAME,ALI_PASSWD)){PC_Usart("�û�����ʧ��\r\n");}
	while(!ESP8266_MQTT_ClientIdSet(ALICLIENTLD)){PC_Usart("ClientID����ʧ��\r\n");}
	delay_ms(1000);
	while(!ESP8266_MQTT_Connect(ALI_MQTT_HOSTURL,ALI_PORT)){PC_Usart("���ӷ�����ʧ��,������\r\n");}
	PC_Usart("���ӷ������ɹ�\r\n");
	
//	while(!ESP8266_MQTT_Sub(sub)){PC_Usart("���ķ���Topicʧ��\r\n");}
//	PC_Usart("���ĳɹ�\r\n");
	//ESP8266_Cmd("AT+MQTTSUB?","OK",0,200);
//	while(!ESP8266_MQTT_Pub(Topic,data)){PC_Usart("��Ϣ����Topicʧ��\r\n");}
	
	
	ESP8266_Cmd("AT+CIPSNTPCFG=1,8,\"ntp1.aliyun.com\",\"ntp.sjtu.edu.cn\"","OK",0,200);
	RGB_LED_Clear();
	LED2=1;
	int ledtime = 0;  //��ֹRGB����LED1һֱ��
	
	
	while(1)
	{
		LED2=1;
		delay_ms(200);
		if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"on114"))
			{
				RGB_LED_Green();
				//LED1=0;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"off514"))//�жϰ����Ʒ��������������Ƿ���off
			{
				RGB_LED_Clear();
				//LED1=1;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"&"))
			{
				p4 = strchr(strEsp8266_Fram_Record .Data_RX_BUF,'&');
				p5 = p4 + 1;
				LsensValue = (*p5) *10 +  *(p5+1) - 16;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"*"))
			{
				p4 = strchr(strEsp8266_Fram_Record .Data_RX_BUF,'*');
				p5 = p4 + 1;
				TempValue = (*p5) *10 +  *(p5+1) - 16;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
		
		//LCD��ʾ����
		LCD_ShowFontHZ(20,100,"����");
		LCD_ShowString(80,100,tftlcd_data.width,tftlcd_data.height,24,": ");
		LCD_ShowString(180,100,tftlcd_data.width,tftlcd_data.height,24,(u8*)weather_info.temperature);
		LCD_ShowFontHZ(210,100,"��");
		LCD_ShowString(150,100,tftlcd_data.width,tftlcd_data.height,24," ");
		LCD_ShowString(100,100,tftlcd_data.width,tftlcd_data.height,24,"WeiHai");
		LCD_ShowString(100,130,tftlcd_data.width,tftlcd_data.height,24,(u8*)weather_info.text);
		
		
			
		delay_ms(250);
		if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"on114"))
			{
				RGB_LED_Green();
				//LED1=0;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"off514"))//�жϰ����Ʒ��������������Ƿ���off
			{
				RGB_LED_Clear();
				//LED1=1;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"&"))
			{
				p4 = strchr(strEsp8266_Fram_Record .Data_RX_BUF,'&');
				p5 = p4 + 1;
				LsensValue = (*p5) *10 +  *(p5+1) - 16;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"*"))
			{
				p4 = strchr(strEsp8266_Fram_Record .Data_RX_BUF,'*');
				p5 = p4 + 1;
				TempValue = (*p5) *10 +  *(p5+1) - 16;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			
			
			
			//LCD��ʾ��ʪ�ȹ�����Ϣ
		if(!DHT11_Read_Data(&temp,&humi)){
				LCD_ShowFontHZ(20,230,"�¶�");
				LCD_ShowxNum(140,230,temp,5,24,0);
				LCD_ShowFontHZ(210,230,"��");
			
				LCD_ShowFontHZ(20,270,"ʪ��");
				LCD_ShowxNum(140,270,humi,5,24,0);
				LCD_ShowString(210,270,tftlcd_data.width,tftlcd_data.height,24,"%RH");
			
				lsens_value=Lsens_Get_Val();
				LCD_ShowFontHZ(20,310,"����");
				LCD_ShowxNum(140,310,lsens_value,5,24,0);
				LCD_ShowString(210,310,tftlcd_data.width,tftlcd_data.height,24,"Lux");
			
				LCD_ShowFontHZ(20,350,"�¶���ֵ");
				LCD_ShowxNum(140,350,TempValue,5,24,0);
				LCD_ShowFontHZ(210,350,"��");

				LCD_ShowFontHZ(20,390,"������ֵ");
				LCD_ShowxNum(140,390,LsensValue,5,24,0);
				LCD_ShowString(210,390,tftlcd_data.width,tftlcd_data.height,24,"Lux");
			}	 
		
		delay_ms(250);
		if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"on114"))
			{
				RGB_LED_Green();
				//LED1=0;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"off514"))//�жϰ����Ʒ��������������Ƿ���off
			{
				RGB_LED_Clear();
				//LED1=1;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}		
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"&"))
			{
				p4 = strchr(strEsp8266_Fram_Record .Data_RX_BUF,'&');
				p5 = p4 + 1;
				LsensValue = (*p5) *10 +  *(p5+1) - 16;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"*"))
			{
				p4 = strchr(strEsp8266_Fram_Record .Data_RX_BUF,'*');
				p5 = p4 + 1;
				TempValue = (*p5) *10 +  *(p5+1) - 16;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			//delay_ms(300);

			
			//�����ݴ���΢��С���򣬺Ͱ���������ģ�ͼ�¼
//			if(temp>0&&mysql%3==0)
			if(temp>=0)
			{
				char cCmd [120];
				//�������ݸ�΢��С����
				sprintf (cCmd,"AT+MQTTPUB=0,\"%s\",\"{\\\"Temp\\\":%d\\\,\\\"Wet\\\":%d\\\,\\\"Lsens\\\":%d}\",1,0",Topic,temp,humi,lsens_value);
				//sprintf (cCmd,"AT+MQTTPUB=0,\"%s\",\"{\\\"Temp\\\":%d\\\,\\\"Wet\\\":%d}\",1,0",Topic,temp,humi);
				ESP8266_Cmd(cCmd,"OK",0,900);
				delay_ms(200);
			}
			
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"on114"))
			{
				RGB_LED_Green();
				//LED1=0;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"off514"))//�жϰ����Ʒ��������������Ƿ���off
			{
				RGB_LED_Clear();
				//LED1=1;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"&"))
			{
				p4 = strchr(strEsp8266_Fram_Record .Data_RX_BUF,'&');
				p5 = p4 + 1;
				LsensValue = (*p5) *10 +  *(p5+1) - 16;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"*"))
			{
				p4 = strchr(strEsp8266_Fram_Record .Data_RX_BUF,'*');
				p5 = p4 + 1;
				TempValue = (*p5) *10 +  *(p5+1) - 16;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			
			//��¼�ϴ����ݿ�
			if(mysql%6==0)
			{
				char cCmd [120];
				//�������ݸ�����������ģ�� //��¼�¶�
				ESP8266_Cmd("AT+MQTTPUBRAW=0,\"/sys/k21dt3xDIVZ/ESP8266DUAN/thing/event/property/post\",23,1,0","OK",0,400);
				sprintf (cCmd,"{\"params\":{\"Temp\":%d}}",temp);
				ESP8266_Cmd(cCmd,"OK",0,400);
				delay_ms(200);
				
				//��¼ʪ��
				ESP8266_Cmd("AT+MQTTPUBRAW=0,\"/sys/k21dt3xDIVZ/ESP8266DUAN/thing/event/property/post\",23,1,0","OK",0,400);
				sprintf (cCmd,"{\"params\":{\"Wet\":%d}}",humi);
				ESP8266_Cmd(cCmd,"OK",0,400);
				delay_ms(200);
				
				//��¼����ǿ��
				ESP8266_Cmd("AT+MQTTPUBRAW=0,\"/sys/k21dt3xDIVZ/ESP8266DUAN/thing/event/property/post\",23,1,0","OK",0,400);
				sprintf (cCmd,"{\"params\":{\"Lsens\":%d}}",lsens_value);
				ESP8266_Cmd(cCmd,"OK",0,400);
				delay_ms(200);
				
				//��¼������ֵ
				ESP8266_Cmd("AT+MQTTPUBRAW=0,\"/sys/k21dt3xDIVZ/ESP8266DUAN/thing/event/property/post\",28,1,0","OK",0,400);
				sprintf (cCmd,"{\"params\":{\"LsensValue\":%d}}",LsensValue);
				ESP8266_Cmd(cCmd,"OK",0,400);
				delay_ms(200);

				//��¼�¶���ֵ
				ESP8266_Cmd("AT+MQTTPUBRAW=0,\"/sys/k21dt3xDIVZ/ESP8266DUAN/thing/event/property/post\",28,1,0","OK",0,400);
				sprintf (cCmd,"{\"params\":{\"TempValue\":%d}}",TempValue);
				ESP8266_Cmd(cCmd,"OK",0,400);
				PC_Usart("�¶ȡ�ʪ�ȡ�����ǿ�ȡ�����ǿ����ֵ���¶���ֵ���ϴ�����������ģ��\r\n");
				mysql = 0;
			}
			
			
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"on114"))
			{
				RGB_LED_Green();
				//LED1=0;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"off514"))//�жϰ����Ʒ��������������Ƿ���off
			{
				RGB_LED_Clear();
				//LED1=1;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"&"))
			{
				p4 = strchr(strEsp8266_Fram_Record .Data_RX_BUF,'&');
				p5 = p4 + 1;
				LsensValue = (*p5) *10 +  *(p5+1) - 16;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"*"))
			{
				p4 = strchr(strEsp8266_Fram_Record .Data_RX_BUF,'*');
				p5 = p4 + 1;
				TempValue = (*p5) *10 +  *(p5+1) - 16;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			delay_ms(100);
			//�¶ȳ���30����������,��RGB���,�����ʼת
			if(temp>=TempValue && ledtime==0)
			{
				BEEP=1;
				delay_ms(100);
				RGB_LED_Red();
				ledtime = 1;
				//pwm_flag= 1;
				delay_ms(100);
				TIM_SetCompare2(TIM3,200);
				delay_ms(200);
			}
			//delay_ms(200);
			if(temp<TempValue && ledtime == 1)
			{
				//pwm_flag = 0;
				TIM_SetCompare2(TIM3,400);
				delay_ms(100);
				BEEP=0;
				delay_ms(100);
				RGB_LED_Clear();
				ledtime == 0;
				delay_ms(200);
				//pwm_value=0;
			}

			LED2=1;
			LED2=1;
			delay_ms(200);
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"on114"))
			{
				RGB_LED_Green();
				//LED1=0;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"off514"))//�жϰ����Ʒ��������������Ƿ���off
			{
				RGB_LED_Clear();
				//LED1=1;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"&"))
			{
				p4 = strchr(strEsp8266_Fram_Record .Data_RX_BUF,'&');
				p5 = p4 + 1;
				LsensValue = (*p5) *10 +  *(p5+1) - 16;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"*"))
			{
				p4 = strchr(strEsp8266_Fram_Record .Data_RX_BUF,'*');
				p5 = p4 + 1;
				TempValue = (*p5) *10 +  *(p5+1) - 16;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			
			
			//ʱ��
			ESP8266_Time(timea);
			p1 = strchr(strEsp8266_Fram_Record .Data_RX_BUF,':');
			p2 = strstr(strEsp8266_Fram_Record .Data_RX_BUF,"OK");
			*p2='\0';
			//���㱨ʱ
			p3=strchr(p1+1,':');
			if(*(p3+1)=='5'&& *(p3+2)=='9')
			{
				flag=true;
				PC_Usart("�������㱨ʱ\r\n");
				BEEP=0;
			}

			if(  flag && (*(p3+1)=='0' && *(p3+2)=='0')  )
			{

				flag=false;
				//��n��
				for(int n=0;n<=flagtime;n++)
				{
					BEEP=!BEEP;
					delay_ms(200);
				}
				BEEP=0;
			}

			
			
		  LCD_ShowString(10,170,tftlcd_data.width*2,tftlcd_data.height*2,24,p1+1);
			delay_ms(200);
			
		
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"on114"))
			{
				RGB_LED_Green();
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"off514"))//�жϰ����Ʒ��������������Ƿ���off
			{
				RGB_LED_Clear();
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"&"))
			{
				p4 = strchr(strEsp8266_Fram_Record .Data_RX_BUF,'&');
				p5 = p4 + 1;
				LsensValue = (*p5) *10 +  *(p5+1) - 16;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"*"))
			{
				p4 = strchr(strEsp8266_Fram_Record .Data_RX_BUF,'*');
				p5 = p4 + 1;
				TempValue = (*p5) *10 +  *(p5+1) - 16;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			
			delay_ms(200);
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"on114"))
			{
				RGB_LED_Green();
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"off514"))//�жϰ����Ʒ��������������Ƿ���off
			{
				RGB_LED_Clear();
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"&"))
			{
				p4 = strchr(strEsp8266_Fram_Record .Data_RX_BUF,'&');
				p5 = p4 + 1;
				LsensValue = (*p5) *10 +  *(p5+1) - 16;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			if(strstr((const char*)strEsp8266_Fram_Record .Data_RX_BUF,"*"))
			{
				p4 = strchr(strEsp8266_Fram_Record .Data_RX_BUF,'*');
				p5 = p4 + 1;
				TempValue = (*p5) *10 +  *(p5+1) - 16;
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				delay_ms(200);
			}
			mysql++;
	}
}



