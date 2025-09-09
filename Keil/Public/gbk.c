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
 UnicodeToGBKMap unicodeToGBKTable[] = {
    {0x5927, 0xB4F3}, // ��
    {0x4E2D, 0xD6D0}, // ��
    {0x5C0F, 0xD0A1}, // С
    {0x9634, 0xD2F5}, // ��
    {0x6674, 0xC7E7}, // ��
    {0x96E8, 0xD3EA}, // ��
    {0x96EA, 0xD1A9}, // ѩ
    {0x591A, 0xB6E0}, // ��
    {0x4E91, 0xD4C6}, // ��
    {0x96F7, 0xC0D7}, // ��
    {0x973E, 0xF7A8}, // ��
    {0x9635, 0xD5F3}, // ��
    {0x66B4, 0xB1A9}, // ��
    {0x5929, 0xCCEC}, // ��
    {0x6C14, 0xC6F8}, // ��
    {0x98CE, 0xB7E7}, // ��
    {0x5A01, 0xCEC0}, // ��
    {0x6D77, 0xBAA3}, // ��
    {0x6E29, 0xCEC2}, // ��
    {0x5EA6, 0xB6C8}, // ��
    {0x65F6, 0xCAB1}, // ʱ
    {0x95F4, 0xBCE4}, // ��
    {0x0000, 0x0000}  // ��ʾ����
};
// ��ӳ����в��� GBK ֵ
uint16_t unicodeToGBK(uint16_t unicode) {
    for (int i = 0; unicodeToGBKTable[i].unicode != 0; i++) {
        if (unicodeToGBKTable[i].unicode == unicode) {
            return unicodeToGBKTable[i].gbk;
        }
    }
    return 0x3F3F; // �Ҳ���ʱ���� '??' �� GBK ����
}

// UTF-8 ���뵥���ַ�
int utf8ToUnicode(const char* utf8, uint16_t* unicode) {
    const uint8_t* bytes = (const uint8_t*)utf8;
    if (bytes[0] < 0x80) { // ���ֽ� (ASCII)
        *unicode = bytes[0];
        return 1;
    } else if ((bytes[0] & 0xE0) == 0xC0) { // ���ֽ�
        if ((bytes[1] & 0xC0) != 0x80) return -1; // �Ƿ�����
        *unicode = ((bytes[0] & 0x1F) << 6) | (bytes[1] & 0x3F);
        return 2;
    } else if ((bytes[0] & 0xF0) == 0xE0) { // ���ֽ�
        if ((bytes[1] & 0xC0) != 0x80 || (bytes[2] & 0xC0) != 0x80) return -1;
        *unicode = ((bytes[0] & 0x0F) << 12) |
                   ((bytes[1] & 0x3F) << 6) |
                   (bytes[2] & 0x3F);
        return 3;
    }
    return -1; // �Ƿ�����
}

// UTF-8 ת GBK ��������
int utf8ToGBK(const char* utf8, char* gbk, size_t gbkSize) {
    size_t gbkLen = 0;
    const char* ptr = utf8;

    while (*ptr && gbkLen < gbkSize - 1) {
        uint16_t unicode;
        int utf8CharLen = utf8ToUnicode(ptr, &unicode);
        if (utf8CharLen < 0) {
            return -1; // �Ƿ��ַ�
        }
				printf("%d\n",unicode);
        uint16_t gbkCode = unicodeToGBK(unicode);
        if (gbkCode < 0x80) { // ���ֽ��ַ�
            gbk[gbkLen++] = (char)gbkCode;
        } else { // ˫�ֽ��ַ�
            if (gbkLen + 2 > gbkSize - 1) { // ��黺����
                return -2; // ����������
            }
            gbk[gbkLen++] = (char)(gbkCode >> 8);
            gbk[gbkLen++] = (char)(gbkCode & 0xFF);
        }

        ptr += utf8CharLen;
    }

    if (gbkLen < gbkSize) {
        gbk[gbkLen] = '\0'; // ����ַ�����ֹ��
    } else {
        return -2; // ���������㣬δд����ֹ��
    }

    return 0;
}



