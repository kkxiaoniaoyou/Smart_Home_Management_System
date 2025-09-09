#include "stm32f10x.h"
#include "string.h"
#include "stdlib.h"
#include <stdio.h>
#include "stm32f10x.h"
#include "wifi_function.h"
#include "wifi_config.h"
#include "time.h"
#include "SysTick.h"
unsigned char Time_buff[100];
//λ�������ȷ����
int DAYS, MOONS, YEARS, TIMES;
/*���ڱ���Сʱ�����ӣ������ı���*/
int hour_return;//Сʱ
int min_return; //����
int sec_return; //����

#define  YEAR_ADD_DRES 11
#define  MOON_ADD_DRES 15
#define  DAYS_ADD_DRES 17

#define  HOURS_ADD_DRES 19
#define  MINUTES_ADD_DRES 21
#define  SECONDS_ADD_DRES 23

//������̨��ȡʱ���API
#define Time_TCP		"AT+CIPSTART=\"TCP\",\"www.beijing-time.org\",80"
//������̨��ȡʱ��GET����
#define Time_GET		"GET http://www.beijing-time.org/time15.asp"


//�õ��꺯�������꿪ʼ���ַ������ȹ��������ʹ�ò�һ���ķ�����
//����ֵ����λ�õĵ�ַ
//����ֵ�� ���͵�10������λ��
int Get_Year(char *y)
{

    int year_return;
    char *year_temp;
    char year[5] = {0};
    char i;
//��Ļ�ȡ��Ҫ��ȡһ���ַ�������Ȼû����ȡ
    year_temp = y;

    for(i = 0; i < 4; i++)
    {
        year[i] = *year_temp;
        year_temp ++;
    }

    year_return =  atoi(&year[0]);
    return year_return;
}

//�õ��·ݺ���
//����ֵ���·�λ�õĵ�ַ
//����ֵ�� ���͵�10������λ��
int Get_Moonth(char *m)
{
    int moonth_return;
    moonth_return = atoi(m) / 100000000; //ȡ�·�
    return moonth_return;
}

//�õ����ں���
//����ֵ������λ�õĵ�ַ
//����ֵ�� ���͵�10������λ��
int Get_Day(char *d)
{


    int day_return;
    day_return = atoi(d) / 1000000; //ȡ����

    return day_return;
}

//�õ�ʱ��
//����ֵ��ʱ���λ�õĵ�ַ
//����ֵ�� ���͵�10���Ƶ�ʱ��������
int Get_Times(char *h, char *m, char *s)
{
    int time_return;
    int hour_return;
    int min_return;
    int sec_return;

    hour_return = atoi(h) / 10000; //ȡСʱ
    min_return = atoi(m) / 100; //ȡ����
    sec_return = atoi(s);   //ȡ����

    time_return = hour_return * 3600 + min_return * 60 + sec_return; //ת����������

    return time_return;
}



void cJSON_Time_Parse(void)
{
    char *data_pt;
    char *day_string;
    char *moon_string;
    char *year_string;
    char *hour_string;
    char *minute_string;
    char *second_string;


    data_pt = strstr((const char *)strEsp8266_Fram_Record .Data_RX_BUF, (const char *)"sysTime1");  //Ѱ�ҵ�ʱ�����ĵ�ַ

//    printf("%s\r\n",Time_buff);
    if(data_pt != NULL)
    {
        day_string = data_pt + DAYS_ADD_DRES;          //���ڵ�ַ
        moon_string = data_pt + MOON_ADD_DRES;         //�·ݵ�ַ
        year_string = data_pt + YEAR_ADD_DRES;        //��ݵ�ַ
        hour_string = data_pt + HOURS_ADD_DRES;       //Сʱ��ַ
        minute_string = data_pt + MINUTES_ADD_DRES;   //���ӵ�ַ
        second_string = data_pt + SECONDS_ADD_DRES;   //���е�ַ

        //��ʱ����Ϣ���ݸ�ȫ�ֱ���
        DAYS = Get_Day(day_string);
        MOONS = Get_Moonth(moon_string);
        YEARS = Get_Year(year_string);
        TIMES = Get_Times(hour_string, minute_string, second_string);
		
		hour_return = TIMES/3600;//Сʱ
		min_return = (TIMES%3600)/60; //����
        sec_return = (TIMES%3600)%60+2; //����
		//printf("ʱ���ȡ������ɹ�\r\n");

    }
    else
    {
        //printf("ʱ���ȡʧ��\r\n");
    }
}

void Get_current_time(void)
{

int i;
    ESP8266_Cmd("AT+CIPSTART=\"TCP\",\"www.beijing-time.org\",80", "CONNECT",0,2000);
    //printf("6.���������������ɹ� OK\r\n");
	delay_ms(2000);
	delay_ms(2000);
    ESP8266_Cmd("AT+CIPMODE=1", "OK",0,2000);//����͸��ģʽ
	delay_ms(2000);
	delay_ms(2000);
	 ESP8266_Cmd("AT+CIPSEND", "OK",0,2000);//����͸��ģʽ
    //printf("7.����͸��ģʽ�ɹ� OK\r\n");
	delay_ms(2000);
	delay_ms(2000);
    /*sizeof(Time_GET)��������sizeof��������strlenû����*/
	while(1){
     ESP8266_Cmd(Time_GET, "OK",0,2000);
	delay_ms(2000);
	delay_ms(2000);
	delay_ms(2000);
		PC_Usart("%s\n",strEsp8266_Fram_Record .Data_RX_BUF);
		for(i=0;i<5;i++)
		{
			delay_ms(1000);
		}
			
	}
//	ESP8266_Cmd("+++", "OK",0,2000);


    //printf("9.�˳�͸��ģʽ�ɹ� OK\r\n");

}

void convert_time_format(const char* input_time, char* output_time) {
	
	
	
	
    // �������ڴ洢���ڡ�ʱ����·ݵı���
    int year, day;
    char month[4], time[9]; // month has 3 chars + null terminator, time hh:mm:ss is 8 chars + null terminator
    
    // ʹ�� sscanf ������ʱ���ַ�������ȡ����
    sscanf(input_time, "%*s %3s %d %8s %d", month, &day, time, &year);
    
    // �·�ת����
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    int month_index = -1;
    
    // �����·ݶ�Ӧ������
    for (int i = 0; i < 12; i++) {
        if (strcmp(month, months[i]) == 0) {
            month_index = i + 1;
            break;
        }
    }

    // ʹ�� sprintf ��������ַ���
    sprintf(output_time, "%04d-%02d-%02d %s", year, month_index, day, time);
}
