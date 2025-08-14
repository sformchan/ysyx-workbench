// #include <stdio.h>
// #include <termios.h>
// #include <unistd.h>
// #include <string.h>
// #include <stdlib.h>

// #define MAX_LINE_LEN 1024

// static void set_noncanonical_mode(int fd, struct termios *old_tios) {
//     struct termios newtios;
//     tcgetattr(fd, old_tios);
//     newtios = *old_tios;
//     newtios.c_lflag &= ~(ICANON | ECHO); // 关闭规范模式和回显
//     newtios.c_cc[VMIN] = 1;
//     newtios.c_cc[VTIME] = 0;
//     tcsetattr(fd, TCSANOW, &newtios);
// }

// static void restore_terminal(int fd, struct termios *old_tios) {
//     tcsetattr(fd, TCSANOW, old_tios);
// }

// char *readline_(const char *prompt) {
//     struct termios oldt;
//     char buf[MAX_LINE_LEN];
//     int pos = 0;
//     char ch;

//     if (prompt)
// 		(void)write(STDOUT_FILENO, prompt, strlen(prompt));

//     set_noncanonical_mode(STDIN_FILENO, &oldt);

//     while (read(STDIN_FILENO, &ch, 1) == 1) {
//         if (ch == '\n' || ch == '\r') {
//             (void)write(STDOUT_FILENO, "\n", 1);
//             break;
//         } else if (ch == 127 || ch == '\b') { // 退格处理
//             if (pos > 0) {
//                 pos--;
//                 (void)write(STDOUT_FILENO, "\b \b", 3);
//             }
//         } else if (pos < MAX_LINE_LEN - 1 && ch >= 32 && ch <= 126) {
//             buf[pos++] = ch;
//             (void)write(STDOUT_FILENO, &ch, 1);
//         }
//     }

//     buf[pos] = '\0';
//     restore_terminal(STDIN_FILENO, &oldt);

//     // 返回堆上的字符串，调用者记得 free()
//     return strdup(buf);
// }

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LEN 1024
#define HISTORY_SIZE 100

static char *history[HISTORY_SIZE];
static int history_count = 0;   // 已存历史数量
static int history_index = 0;   // 当前浏览的历史索引（相对于 history_count）

static void set_noncanonical_mode(int fd, struct termios *old_tios) {
    struct termios newtios;
    tcgetattr(fd, old_tios);
    newtios = *old_tios;
    newtios.c_lflag &= ~(ICANON | ECHO);
    newtios.c_cc[VMIN] = 1;
    newtios.c_cc[VTIME] = 0;
    tcsetattr(fd, TCSANOW, &newtios);
}

static void restore_terminal(int fd, struct termios *old_tios) {
    tcsetattr(fd, TCSANOW, old_tios);
}

static void add_history(const char *cmd) {
    if (cmd[0] == '\0') return; // 空命令不存
    if (history_count < HISTORY_SIZE) {
        history[history_count++] = strdup(cmd);
    } else {
        free(history[0]);
        memmove(history, history + 1, sizeof(char*) * (HISTORY_SIZE - 1));
        history[HISTORY_SIZE - 1] = strdup(cmd);
    }
}

static void clear_line(int pos) {
    for (int i = 0; i < pos; i++) {
        write(STDOUT_FILENO, "\b \b", 3);
    }
}

char *readline_(const char *prompt) {
    struct termios oldt;
    char buf[MAX_LINE_LEN];
    int pos = 0;
    char ch;

    if (prompt) write(STDOUT_FILENO, prompt, strlen(prompt));

    set_noncanonical_mode(STDIN_FILENO, &oldt);
    history_index = history_count; // 初始在“最新”位置

    while (read(STDIN_FILENO, &ch, 1) == 1) {
        if (ch == '\n' || ch == '\r') {
            write(STDOUT_FILENO, "\n", 1);
            break;
        }
        else if (ch == 127 || ch == '\b') { // 退格
            if (pos > 0) {
                pos--;
                write(STDOUT_FILENO, "\b \b", 3);
            }
        }
        else if (ch == 27) { // ESC 开头，可能是方向键
            char seq[2];
            if (read(STDIN_FILENO, seq, 2) == 2) {
                if (seq[0] == '[' && seq[1] == 'A') { // ↑
                    if (history_index > 0) {
                        clear_line(pos);
                        history_index--;
                        strcpy(buf, history[history_index]);
                        pos = strlen(buf);
                        write(STDOUT_FILENO, buf, pos);
                    }
                }
                else if (seq[0] == '[' && seq[1] == 'B') { // ↓
                    if (history_index < history_count) {
                        clear_line(pos);
                        history_index++;
                        if (history_index == history_count) {
                            pos = 0;
                            buf[0] = '\0';
                        } else {
                            strcpy(buf, history[history_index]);
                            pos = strlen(buf);
                        }
                        write(STDOUT_FILENO, buf, pos);
                    }
                }
            }
        }
        else if (pos < MAX_LINE_LEN - 1 && ch >= 32 && ch <= 126) {
            buf[pos++] = ch;
            write(STDOUT_FILENO, &ch, 1);
        }
    }

    buf[pos] = '\0';
    restore_terminal(STDIN_FILENO, &oldt);
    add_history(buf);

    return strdup(buf);
}

