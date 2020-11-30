#include "keyboard.h"
#include "screen.h"
#include "../cpu/isr.h"
#include "../cpu/ports.h"
#include "../libc/utils.h"
#include "../libc/string.h"

/**
 * 对输入字符串的处理，键入回车后回调此方法
 */
extern void user_input(char *input);

#define BACKSPACE   0x0E
#define ENTER       0x1C
#define LSHIFT      0x2A
#define RSHIFT      0x36

#define SHIFT_EXCLAMATION   0x02    /* ! */
#define SHIFT_AT            0x03    /* @ */
#define SHIFT_NUMBER        0x04    /* # */
#define SHIFT_DOLLAR        0x05    /* $ */
#define SHIFT_PERCENT       0x06    /* % */
#define SHIFT_CARET         0x07    /* ^ */
#define SHIFT_AND           0x08    /* & */
#define SHIFT_MUL           0x09    /* * */
#define SHIFT_LBRACKET      0x0a    /* ( */
#define SHIFT_RBRACKET      0x0b    /* ) */
#define SHIFT_UNDERLINE     0x0c    /* _ */
#define SHIFT_ADD           0x0d    /* + */
#define SHIFT_COLON         0x27    /* : */
#define SHIFT_DQUOTATION    0x28    /* " */
#define SHIFT_LABRACKET     0x33    /* < */
#define SHIFT_RABRACKET     0x34    /* > */
#define SHIFT_QUESTION      0x35    /* ? */

#define BUFFER_SIZE 256
static char key_buffer[BUFFER_SIZE];
static uint8_t has_shift = 0;
static uint8_t is_enable = 0;

#define SC_MAX 57
const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6", 
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E", 
        "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl", 
        "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", 
        "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", 
        "/", "RShift", "Keypad *", "LAlt", "Spacebar"};
const char sc_ascii[] = {
    '?', '?', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '?',
    '?', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '?', 
    '?', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', '?', 
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' '};

static char shift_helper(char c, uint8_t scancode) {
    if (scancode == SHIFT_EXCLAMATION)
        return '!';
    else if (scancode == SHIFT_AT)
        return '@';
    else if (scancode == SHIFT_NUMBER)
        return '#';
    else if (scancode == SHIFT_DOLLAR)
        return '$';
    else if (scancode == SHIFT_PERCENT)
        return '%';
    else if (scancode == SHIFT_CARET)
        return '^';
    else if (scancode == SHIFT_AND)
        return '&';
    else if (scancode == SHIFT_MUL)
        return '*';
    else if (scancode == SHIFT_LBRACKET)
        return '(';
    else if (scancode == SHIFT_RBRACKET)
        return ')';
    else if (scancode == SHIFT_UNDERLINE)
        return '_';
    else if (scancode == SHIFT_ADD)
        return '+';
    else if (scancode == SHIFT_COLON)
        return ':';
    else if (scancode == SHIFT_DQUOTATION)
        return '"';
    else if (scancode == SHIFT_LABRACKET)
        return '<';
    else if (scancode == SHIFT_RABRACKET)
        return '>';
    else if (scancode == SHIFT_QUESTION)
        return '?';
    return c;
}

static void keyboard_callback(registers_t regs) {
    /* The PIC leaves us the scancode in port 0x60 */
    uint8_t scancode = port_byte_in(0x60);
    if (!is_enable)
        return;
    if (scancode > SC_MAX) return;
    if (scancode == BACKSPACE) {
        if (strlen(key_buffer) > 0) {
            backspace(key_buffer);
            kprint_backspace();
        }
    } else if (scancode == ENTER) {
        kprint("\n");
        user_input(key_buffer); /* kernel-controlled function */
        key_buffer[0] = '\0';
    } else if (scancode == LSHIFT || scancode == RSHIFT)
        has_shift = 1;
    else {
        char letter = sc_ascii[(int)scancode];
        if (has_shift && letter >= 'a' && letter <= 'z')
            letter = letter - 'a' + 'A';
        else if (has_shift)
            letter = shift_helper(letter, scancode);
        char str[2] = {letter, '\0'};
        size_t len = strlen(key_buffer);
        if (len == BUFFER_SIZE - 1)
            key_buffer[BUFFER_SIZE - 1] = letter;
        else
            append(key_buffer, letter);
        kprint(str);
        has_shift = 0;
    }
    UNUSED(regs);
}

int input_len() {
    int i = 0;
    for (; i < BUFFER_SIZE; i++)
        if (key_buffer[i] == '\0')
            return i;
    return -1;
}

void init_keyboard() {
    update_interrupt_handler(IRQ1, keyboard_callback);
}

void clear_key_buffer() {
    key_buffer[0] = '\0';
}

void enable_keyboard() {
    is_enable = 1;
}

void disable_keyboard() {
    is_enable = 0;
}