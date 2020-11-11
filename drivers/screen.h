#ifndef SCREEN_H
#define SCREEN_H

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80

#define WHITE_ON_BLACK 0x0f
#define RED_ON_WHITE 0xf4

#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

/**
 * 清理屏幕 
 */
void clear_screen();

/**
 * 从屏幕的指定位置开始打印字符串
 */
void kprint_at(char *message, int col, int row);

/**
 * 从屏幕光标所在位置开始打印字符串
 */
void kprint(char *message);

/**
 * 清空当前屏幕光标上一位置打印的字符
 */
void kprint_backspace();

#endif