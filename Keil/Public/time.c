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
//位数是随机确定的
int DAYS, MOONS, YEARS, TIMES;
/*用于保存小时，分钟，秒数的变量*/
int hour_return;//小时
int min_return; //分钟
int sec_return; //秒数

#define  YEAR_ADD_DRES 11
#define  MOON_ADD_DRES 15
#define  DAYS_ADD_DRES 17

#define  HOURS_ADD_DRES 19
#define  MINUTES_ADD_DRES 21
#define  SECONDS_ADD_DRES 23

//苏宁后台获取时间的API
#define Time_TCP		"AT+CIPSTART=\"TCP\",\"www.beijing-time.org\",80"
//苏宁后台获取时间GET报文
#define Time_GET		"GET http://www.beijing-time.org/time15.asp"


//得到年函数（以年开始的字符串长度过长，因此使用不一样的方法）
//输入值是年位置的地址
//返回值是 整型的10进制四位数
int Get_Year(char *y)
{

    int year_return;
    char *year_temp;
    char year[5] = {0};
    char i;
//年的获取须要提取一次字符串，不然没法读取
    year_temp = y;

    for(i = 0; i < 4; i++)
    {
        year[i] = *year_temp;
        year_temp ++;
    }

    year_return =  atoi(&year[0]);
    return year_return;
}

//得到月份函数
//输入值是月份位置的地址
//返回值是 整型的10进制两位数
int Get_Moonth(char *m)
{
    int moonth_return;
    moonth_return = atoi(m) / 100000000; //取月份
    return moonth_return;
}

//得到日期函数
//输入值是日期位置的地址
//返回值是 整型的10进制两位数
int Get_Day(char *d)
{


    int day_return;
    day_return = atoi(d) / 1000000; //取日期

    return day_return;
}

//得到时间
//输入值是时间的位置的地址
//返回值是 整型的10进制的时间总秒数
int Get_Times(char *h, char *m, char *s)
{
    int time_return;
    int hour_return;
    int min_return;
    int sec_return;

    hour_return = atoi(h) / 10000; //取小时
    min_return = atoi(m) / 100; //取分钟
    sec_return = atoi(s);   //取秒数

    time_return = hour_return * 3600 + min_return * 60 + sec_return; //转换成总秒数

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


    data_pt = strstr((const char *)strEsp8266_Fram_Record .Data_RX_BUF, (const char *)"sysTime1");  //寻找到时间结果的地址

//    printf("%s\r\n",Time_buff);
    if(data_pt != NULL)
    {
        day_string = data_pt + DAYS_ADD_DRES;          //日期地址
        moon_string = data_pt + MOON_ADD_DRES;         //月份地址
        year_string = data_pt + YEAR_ADD_DRES;        //年份地址
        hour_string = data_pt + HOURS_ADD_DRES;       //小时地址
        minute_string = data_pt + MINUTES_ADD_DRES;   //分钟地址
        second_string = data_pt + SECONDS_ADD_DRES;   //秒中地址

        //将时间信息传递给全局变量
        DAYS = Get_Day(day_string);
        MOONS = Get_Moonth(moon_string);
        YEARS = Get_Year(year_string);
        TIMES = Get_Times(hour_string, minute_string, second_string);
		
		hour_return = TIMES/3600;//小时
		min_return = (TIMES%3600)/60; //分钟
        sec_return = (TIMES%3600)%60+2; //秒数
		//printf("时间获取并处理成功\r\n");

    }
    else
    {
        //printf("时间获取失败\r\n");
    }
}

void Get_current_time(void)
{

int i;
    ESP8266_Cmd("AT+CIPSTART=\"TCP\",\"www.beijing-time.org\",80", "CONNECT",0,2000);
    //printf("6.访问苏宁服务器成功 OK\r\n");
	delay_ms(2000);
	delay_ms(2000);
    ESP8266_Cmd("AT+CIPMODE=1", "OK",0,2000);//开启透传模式
	delay_ms(2000);
	delay_ms(2000);
	 ESP8266_Cmd("AT+CIPSEND", "OK",0,2000);//开启透传模式
    //printf("7.开启透传模式成功 OK\r\n");
	delay_ms(2000);
	delay_ms(2000);
    /*sizeof(Time_GET)，必须用sizeof函数，用strlen没有用*/
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


    //printf("9.退出透传模式成功 OK\r\n");

}

void convert_time_format(const char* input_time, char* output_time) {
	
	
	
	
    // 声明用于存储日期、时间和月份的变量
    int year, day;
    char month[4], time[9]; // month has 3 chars + null terminator, time hh:mm:ss is 8 chars + null terminator
    
    // 使用 sscanf 从输入时间字符串中提取数据
    sscanf(input_time, "%*s %3s %d %8s %d", month, &day, time, &year);
    
    // 月份转换表
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    int month_index = -1;
    
    // 查找月份对应的数字
    for (int i = 0; i < 12; i++) {
        if (strcmp(month, months[i]) == 0) {
            month_index = i + 1;
            break;
        }
    }

    // 使用 sprintf 构建输出字符串
    sprintf(output_time, "%04d-%02d-%02d %s", year, month_index, day, time);
}
