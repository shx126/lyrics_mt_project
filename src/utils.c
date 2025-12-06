#include "utils.h"

// 字符串去掉末尾 \r \n
void trim_newline(char *s) {
    int len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[len - 1] = '\0';
        len--;
    }
}

// 计算 UTF-8 字符串中，前 n 个"字符"占用多少字节
int utf8_bytes_for_chars(const char *s, int n_chars) {
    int bytes = 0;
    int count = 0;
    unsigned char c;

    while (s[bytes] != '\0' && count < n_chars) {
        c = (unsigned char)s[bytes];
        int char_len = 1;

        if ((c & 0x80) == 0x00) {          // 0xxxxxxx, ASCII
            char_len = 1;
        } else if ((c & 0xE0) == 0xC0) {   // 110xxxxx, 2 字节
            char_len = 2;
        } else if ((c & 0xF0) == 0xE0) {   // 1110xxxx, 3 字节（大部分中文）
            char_len = 3;
        } else if ((c & 0xF8) == 0xF0) {   // 11110xxx, 4 字节（表情等）
            char_len = 4;
        }

        bytes += char_len;
        count++;
    }
    return bytes;
}

// 清屏
void clear_screen(void) {
    printf("\033[2J\033[H");
}