/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "utils.h"

//extern uint64_t g_nr_guest_inst;

//#ifndef CONFIG_TARGET_AM
FILE *log_fp = NULL;

void init_log(const char *log_file) {
  log_fp = stdout;
  if (log_file != NULL) {
    //printf("Trying to open log file: %s\n", log_file);
    FILE *fp = fopen(log_file, "w");
    if (!fp) {
      perror("fopen");
    }
    Assert(fp, "Can not open '%s'", log_file);
    log_fp = fp;
  }
  Log("Log is written to %s", log_file ? log_file : "stdout");
}

bool log_enable() {
//   return MUXDEF(CONFIG_TRACE, (g_nr_guest_inst >= CONFIG_TRACE_START) &&
//          (g_nr_guest_inst <= CONFIG_TRACE_END), false);
	return true;
}

void log_write(const char *fmt, ...) {
    if (!log_enable() || log_fp == NULL) {
        return; // 日志没开启或者文件无效，直接返回
    }

    va_list args;
    va_start(args, fmt);
    vfprintf(log_fp, fmt, args);
    va_end(args);

    fflush(log_fp); // 立即刷新
}
//#endif
