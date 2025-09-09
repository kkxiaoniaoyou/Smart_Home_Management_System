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
    {0x5927, 0xB4F3}, // 大
    {0x4E2D, 0xD6D0}, // 中
    {0x5C0F, 0xD0A1}, // 小
    {0x9634, 0xD2F5}, // 阴
    {0x6674, 0xC7E7}, // 晴
    {0x96E8, 0xD3EA}, // 雨
    {0x96EA, 0xD1A9}, // 雪
    {0x591A, 0xB6E0}, // 多
    {0x4E91, 0xD4C6}, // 云
    {0x96F7, 0xC0D7}, // 雷
    {0x973E, 0xF7A8}, // 霾
    {0x9635, 0xD5F3}, // 阵
    {0x66B4, 0xB1A9}, // 暴
    {0x5929, 0xCCEC}, // 天
    {0x6C14, 0xC6F8}, // 气
    {0x98CE, 0xB7E7}, // 风
    {0x5A01, 0xCEC0}, // 威
    {0x6D77, 0xBAA3}, // 海
    {0x6E29, 0xCEC2}, // 温
    {0x5EA6, 0xB6C8}, // 度
    {0x65F6, 0xCAB1}, // 时
    {0x95F4, 0xBCE4}, // 间
    {0x0000, 0x0000}  // 表示结束
};
// 从映射表中查找 GBK 值
uint16_t unicodeToGBK(uint16_t unicode) {
    for (int i = 0; unicodeToGBKTable[i].unicode != 0; i++) {
        if (unicodeToGBKTable[i].unicode == unicode) {
            return unicodeToGBKTable[i].gbk;
        }
    }
    return 0x3F3F; // 找不到时返回 '??' 的 GBK 编码
}

// UTF-8 解码单个字符
int utf8ToUnicode(const char* utf8, uint16_t* unicode) {
    const uint8_t* bytes = (const uint8_t*)utf8;
    if (bytes[0] < 0x80) { // 单字节 (ASCII)
        *unicode = bytes[0];
        return 1;
    } else if ((bytes[0] & 0xE0) == 0xC0) { // 两字节
        if ((bytes[1] & 0xC0) != 0x80) return -1; // 非法序列
        *unicode = ((bytes[0] & 0x1F) << 6) | (bytes[1] & 0x3F);
        return 2;
    } else if ((bytes[0] & 0xF0) == 0xE0) { // 三字节
        if ((bytes[1] & 0xC0) != 0x80 || (bytes[2] & 0xC0) != 0x80) return -1;
        *unicode = ((bytes[0] & 0x0F) << 12) |
                   ((bytes[1] & 0x3F) << 6) |
                   (bytes[2] & 0x3F);
        return 3;
    }
    return -1; // 非法编码
}

// UTF-8 转 GBK 的主函数
int utf8ToGBK(const char* utf8, char* gbk, size_t gbkSize) {
    size_t gbkLen = 0;
    const char* ptr = utf8;

    while (*ptr && gbkLen < gbkSize - 1) {
        uint16_t unicode;
        int utf8CharLen = utf8ToUnicode(ptr, &unicode);
        if (utf8CharLen < 0) {
            return -1; // 非法字符
        }
				printf("%d\n",unicode);
        uint16_t gbkCode = unicodeToGBK(unicode);
        if (gbkCode < 0x80) { // 单字节字符
            gbk[gbkLen++] = (char)gbkCode;
        } else { // 双字节字符
            if (gbkLen + 2 > gbkSize - 1) { // 检查缓冲区
                return -2; // 缓冲区不足
            }
            gbk[gbkLen++] = (char)(gbkCode >> 8);
            gbk[gbkLen++] = (char)(gbkCode & 0xFF);
        }

        ptr += utf8CharLen;
    }

    if (gbkLen < gbkSize) {
        gbk[gbkLen] = '\0'; // 添加字符串终止符
    } else {
        return -2; // 缓冲区不足，未写入终止符
    }

    return 0;
}



