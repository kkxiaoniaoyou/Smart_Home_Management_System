#ifndef  __WEATHER_H
#define __WEATHER_H

typedef struct {
    char text[20];        // ÌìÆøÃèÊö
    char temperature[4];  // ÎÂ¶È
} WEATHER_TypeDef;



extern WEATHER_TypeDef weather_info;

void Get_Weather();

void parse_weather_json(const char* json_str);

#endif