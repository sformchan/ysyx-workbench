#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LEN 1024
#define MAX_HISTORY 64

static char history[MAX_HISTORY][MAX_LINE_LEN];
static int history_size = 0;

static void set_noncanonical_mode(int fd, struct termios *old_tios) {
    struct termios tios;
    tcgetattr(fd, old_tios);
    tios = *old_tios;
    tios.c_lflag &= ~(ICANON | ECHO); // 非规范模式 & 关闭回显
    tios.c_cc[VMIN] = 1;
    tios.c_cc[VTIME] = 0;
    tcsetattr(fd, TCSANOW, &tios);
}

static void restore_terminal(int fd, struct termios *old_tios) {
    tcsetattr(fd, TCSANOW, old_tios);
}

char *readline(const char *prompt) {
    static char line[MAX_LINE_LEN];
    struct termios old_tios;
    set_noncanonical_mode(STDIN_FILENO, &old_tios);

    int pos = 0;
    unsigned char ch;

    if (prompt) {
        (void)write(STDOUT_FILENO, prompt, strlen(prompt));
    }

    while (read(STDIN_FILENO, &ch, 1) == 1) {
        if (ch == '\n' || ch == '\r') {
            line[pos] = '\0';
            (void)write(STDOUT_FILENO, "\n", 1);
            break;
        } else if (ch == 127 || ch == '\b') {
            if (pos > 0) {
                pos--;
                (void)write(STDOUT_FILENO, "\b \b", 3);
            }
        } else if (pos < MAX_LINE_LEN - 1 && ch >= 32 && ch <= 126) {
            line[pos++] = ch;
            (void)write(STDOUT_FILENO, &ch, 1);
        }
    }

    if (pos > 0 && (history_size == 0 || strcmp(line, history[history_size - 1]) != 0)) {
        if (history_size < MAX_HISTORY) {
            strcpy(history[history_size++], line);
        }
    }

    restore_terminal(STDIN_FILENO, &old_tios);
    return line;
}
