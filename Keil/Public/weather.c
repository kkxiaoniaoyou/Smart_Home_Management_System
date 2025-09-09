#include "SysTick.h"
#include "gbk.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <float.h>
#include <stddef.h> 
#include "usart.h"
#include "wifi_function.h"
#include "weather.h"
#include "cJSON.h"
// �����ṹ�嶨��
//��֪����

WEATHER_TypeDef weather_info = {"Clear", "20"};
void parse_weather_json(const char* json_str) {
    char* text_start;
    char* temp_start;
    char* text_end;
    char* temp_end;
    int text_len, temp_len;
    int i;
    char* now_section;
    printf("Debug: Parsing JSON: %s\r\n", json_str);
    
    // ��ӡ�ַ�����ʮ������ֵ����������
    printf("Debug: JSON hex values: ");
    for(i = 0; json_str[i] != '\0' && i < 100; i++) {
        printf("%02X ", (unsigned char)json_str[i]);
    }
    printf("\r\n");
    
    // ����"now"����
    now_section = strstr(json_str, "\"now\":{");
    if (!now_section) {
        printf("Debug: 'now' section not found\r\n");
        return;
    }
    
    // ��"now"�����в�����������
    text_start = strstr(now_section, "\"text\":\"");
    if (text_start) {
        text_start += 8;  // ���� "\"text\":\""
        text_end = strchr(text_start, '\"');
        if (text_end) {
            text_len = text_end - text_start;
            if (text_len < sizeof(weather_info.text)) {
                memset(weather_info.text, 0, sizeof(weather_info.text));
                strncpy(weather_info.text, text_start, text_len);
                weather_info.text[text_len] = '\0';
                printf("Debug: Parsed weather text: [%s]\r\n", weather_info.text);
            }
        }
    }
    
    // ��λtemperature�ֶεĿ�ʼλ�� - �޸�������ʽ
    temp_start = strstr(now_section, "\"temperature\":\"");
    if (temp_start) {
        temp_start += strlen("\"temperature\":\"");  // ʹ��strlen��ȷ����ȷ����ǰ׺
        
        printf("Debug: Found temperature field. Raw context: ");
        for(i = -10; i < 20 && temp_start[i] != '\0'; i++) {
            printf("%c", temp_start[i]);
        }
        printf("\r\n");
        
        // ���ҽ���������
        temp_end = strchr(temp_start, '\"');
        if (temp_end) {
            temp_len = temp_end - temp_start;
            printf("Debug: Temperature length: %d\r\n", temp_len);
            
            // ��ӡʵ���ҵ����¶�ֵ�������ã�
            printf("Debug: Temperature chars: ");
            for(i = 0; i < temp_len; i++) {
                printf("[%c]", temp_start[i]);
            }
            printf("\r\n");
            
            if (temp_len > 0 && temp_len < sizeof(weather_info.temperature)) {
                memset(weather_info.temperature, 0, sizeof(weather_info.temperature));
                strncpy(weather_info.temperature, temp_start, temp_len);
                weather_info.temperature[temp_len] = '\0';
                printf("Debug: Parsed temperature: [%s]\r\n", weather_info.temperature);
            } else {
                printf("Debug: Invalid temperature length\r\n");
            }
        } else {
            printf("Debug: Temperature end quote not found\r\n");
        }
    } else {
        printf("Debug: Temperature field not found\r\n");
    }
    
    printf("Debug: Final weather info - text: [%s], temperature: [%s]\r\n", 
           weather_info.text, weather_info.temperature);
					 
					 


}















// ��ȡ������Ϣ
void Get_Weather() {
        uint8_t i;
    char *last_ipd;
    char request[256];
    char at_cipsend[20];
    char *json_start, *actual_json;
    
    printf("Debug: Starting Get_Weather\r\n");
    
    // ����TCP����
    if (!ESP8266_Cmd("AT+CIPSTART=\"TCP\",\"api.seniverse.com\",80", "OK", NULL, 5000)) {
        printf("Debug: TCP connection failed\r\n");
        return;
    }
    printf("Debug: TCP connected\r\n");
    delay_ms(1000);

    // ����HTTP����
    snprintf(request, sizeof(request),
        "GET /v3/weather/now.json?key=St1xRjbURvHdc__-E&location=weihai&language=en&unit=c HTTP/1.1\r\n"
        "Host: api.seniverse.com\r\n"
        "Connection: close\r\n"
        "\r\n");

    // �������ݳ�������
    sprintf(at_cipsend, "AT+CIPSEND=%d", strlen(request));
    if (!ESP8266_Cmd(at_cipsend, ">", NULL, 1000)) {
        printf("Debug: Send length command failed\r\n");
        goto close_conn;
    }
    delay_ms(100);

    // ����HTTP����
    if (ESP8266_Cmd(request, "+IPD", NULL, 5000)) {
        printf("Debug: Response received: %s\r\n", strEsp8266_Fram_Record.Data_RX_BUF);
        
        // �������һ��+IPD��ʼ�����ݿ�
        last_ipd = strstr(strEsp8266_Fram_Record.Data_RX_BUF, "+IPD");
        while(1) {
            char *next_ipd = strstr(last_ipd + 4, "+IPD");
            if(next_ipd == NULL) break;
            last_ipd = next_ipd;
        }
        
        if(last_ipd) {
            // ����+IPD,xxx:����
            json_start = strchr(last_ipd, ':');
            if(json_start) {
                json_start++; // ����ð��
                // �ҵ�ʵ�ʵ�JSON��ʼλ��
                actual_json = strstr(json_start, "{");
                if(actual_json) {
                    // �ҵ����Ƴ�CLOSED
                    char* closed = strstr(actual_json, "CLOSED");
                    if(closed) {
                        *closed = '\0';
                    }
                    printf("Debug: Clean JSON data: %s\r\n", actual_json);
                    parse_weather_json(actual_json);
                }
            }
        }
    } else {
        printf("Debug: HTTP request failed\r\n");
    }

close_conn:
    delay_ms(1000);
    // ���Զ�ιر�����

    for(i = 0; i < 3; i++) {
        if(ESP8266_Cmd("AT+CIPCLOSE=0", "OK", NULL, 1000)) {
					delay_ms(1000);
					delay_ms(1000);
					delay_ms(1000);
					delay_ms(1000);
            printf("Debug: Connection closed successfully\r\n");
            break;
        }
        printf("Debug: Close attempt %d failed\r\n", i+1);
        delay_ms(500);
    }
		
		
		
	
}