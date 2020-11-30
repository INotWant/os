#include "welcome.h"
#include "../cpu/timer.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "../libc/mem.h"

#define LOG_ROWS 5

static char *logo[] = {
    "  _____             _   __    __            _   ",
    "  \\_   \\_ __   ___ | |_/ / /\\ \\ \\__ _ _ __ | |_ ",
    "   / /\\/ '_ \\ / _ \\| __\\ \\/  \\/ / _` | '_ \\| __|",
    "/\\/ /_ | | | | (_) | |_ \\  /\\  / (_| | | | | |_ ",
    "\\____/ |_| |_|\\___/ \\__| \\/  \\/ \\__,_|_| |_|\\__|",
};

static void print_strs(char *strs[], int strs_len, int reservation_lines , int reservation_pre_line) {
    for (int i = 0; i < reservation_lines; i++)
        kprint("\n");
    for (int i = 0; i < strs_len; i++) {
        for (int j = 0; j < reservation_pre_line; j++)
            kprint(" ");
        kprint(strs[i]);
        kprint("\n");
    }
}

static void vanishing_animation(char *strs[], int strs_len, int retention_time, int reservation_lines , int reservation_pre_line) {
    char *strs_copy[strs_len];
    int line_size = strlen(strs[0]);    /* 每行字符数 */
    for (int i = 0; i < strs_len; i++) {
        strs_copy[i] = memory_malloc(line_size + 1);
        memory_copy((uint8_t *)strs[i], (uint8_t *)strs_copy[i], line_size + 1);
    }
    print_strs(strs_copy, strs_len, reservation_lines, reservation_pre_line);
    sleep(retention_time);
    int mid = line_size / 2, left, right;
    if (line_size % 2 != 0) {
        for (int i = 0; i < strs_len; i++)
            strs_copy[i][mid] = ' ';
        clear_screen();
        print_strs(strs_copy, strs_len, reservation_lines, reservation_pre_line);
        sleep(retention_time);
        left = mid - 1;
        right = mid + 1;
    } else {
        left = mid - 1;
        right = mid;
    }
    while (left >= 0 && right < line_size) {
        for (int i = 0; i < strs_len; i++) {
            strs_copy[i][left] = ' ';
            strs_copy[i][right] = ' ';
        }
        clear_screen();
        print_strs(strs_copy, strs_len, reservation_lines, reservation_pre_line);
        sleep(retention_time);
        ++right;
        --left;
    }
    for (int i = 0; i < strs_len; i++) 
        memory_free(strs_copy[i]);
}

void welcome() {
    disable_cursor();
    sleep(500);
    clear_screen();

    int reservation_lines = (MAX_ROWS + 1) / 2 - (LOG_ROWS + 1) / 2 + 1;
    int reservation_pre_line = (MAX_COLS + 1) / 2 - (strlen(logo[0]) + 1) / 2 + 1;
    /* 消失动画 */
    vanishing_animation(logo, LOG_ROWS, 20, reservation_lines, reservation_pre_line);

    /* 闪烁动画 */
    for (int i = 0; i < 3; i++) {
        sleep(600);
        clear_screen();
        sleep(300);
        print_strs(logo, LOG_ROWS, reservation_lines, reservation_pre_line);
    }

    sleep(500);
    clear_screen();
    enable_cursor(0, 0);
}
