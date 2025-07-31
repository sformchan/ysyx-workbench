#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LEN 1024

static void set_noncanonical_mode(int fd, struct termios *old_tios) {
    struct termios newtios;
    tcgetattr(fd, old_tios);
    newtios = *old_tios;
    newtios.c_lflag &= ~(ICANON | ECHO); // 关闭规范模式和回显
    newtios.c_cc[VMIN] = 1;
    newtios.c_cc[VTIME] = 0;
    tcsetattr(fd, TCSANOW, &newtios);
}

static void restore_terminal(int fd, struct termios *old_tios) {
    tcsetattr(fd, TCSANOW, old_tios);
}

char *readline(const char *prompt) {
    struct termios oldt;
    char buf[MAX_LINE_LEN];
    int pos = 0;
    char ch;

    if (prompt)
        write(STDOUT_FILENO, prompt, strlen(prompt));

    set_noncanonical_mode(STDIN_FILENO, &oldt);

    while (read(STDIN_FILENO, &ch, 1) == 1) {
        if (ch == '\n' || ch == '\r') {
            write(STDOUT_FILENO, "\n", 1);
            break;
        } else if (ch == 127 || ch == '\b') { // 退格处理
            if (pos > 0) {
                pos--;
                write(STDOUT_FILENO, "\b \b", 3);
            }
        } else if (pos < MAX_LINE_LEN - 1 && ch >= 32 && ch <= 126) {
            buf[pos++] = ch;
            write(STDOUT_FILENO, &ch, 1);
        }
    }

    buf[pos] = '\0';
    restore_terminal(STDIN_FILENO, &oldt);

    // 返回堆上的字符串，调用者记得 free()
    return strdup(buf);
}
