#ifndef __GBK_H
#define __GBK_H 	 
#include <stddef.h> 
typedef unsigned short uint16_t; // 16 λ�޷�������
typedef unsigned char uint8_t;  // 8 λ�޷�������

// �򵥵� Unicode �� GBK ӳ���ʾ����
typedef struct {
    uint16_t unicode;
    uint16_t gbk;
} UnicodeToGBKMap;

extern UnicodeToGBKMap unicodeToGBKTable[];
uint16_t unicodeToGBK(uint16_t unicode);
int utf8ToUnicode(const char* utf8, uint16_t* unicode);
int utf8ToGBK(const char* utf8, char* gbk, size_t gbkSize);
	
#endif
