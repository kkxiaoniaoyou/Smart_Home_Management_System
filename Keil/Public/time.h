#ifndef  __TIME_H
#define  __TIME_H


int Get_Year(char *y);
int Get_Moonth(char *m);
int Get_Day(char *d);
int Get_Times(char *h, char *m, char *s);
void cJSON_Time_Parse(void);
void Get_current_time(void);
void convert_time_format(const char* input_time, char* output_time);
extern unsigned char Time_buff[100];

#endif
