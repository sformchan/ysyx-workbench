#ifndef WATCHPOINT_H
#define WATCHPOINT_H

#include <stdint.h>
#include <stdbool.h>

// 定义监视点的最大数量
#define NR_WP 32

// 监视点结构体
typedef struct watchpoint {
    int NO;                      // 监视点编号
    struct watchpoint *next;      // 指向下一个监视点
    char *expr;                  // 表达式
    uint32_t old_value;          // 监视点的旧值
    bool enable;                 // 是否启用监视点
} WP;

// 监视点池和头指针
extern WP wp_pool[NR_WP];         // 监视点池
extern WP *head;                  // 监视点链表头
extern WP *free_;                 // 空闲监视点链表

// 初始化监视点池
void init_wp_pool(void);

// 创建新的监视点
WP *new_wp(void);

// 释放监视点
void free_wp(WP *wp);

// 设置监视点
void set_wp(char *expr_str);

// 显示当前所有的监视点
void display_wp(void);

// 删除指定编号的监视点
void delete_wp(int num);

// 解析表达式
word_t expr(char *e, bool *success);

#endif // WATCHPOINT_H

