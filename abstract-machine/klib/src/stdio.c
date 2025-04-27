#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *ptr = out;
  while (*fmt) {
    if (*fmt == '%') {
      fmt+= 2;
      switch (*fmt) {
        case 'd': { // 整数
          int num = va_arg(ap, int);
          char buf[32];
          int len = 0;

          if (num < 0) {
            *ptr++ = '-';
            num = -num;
          }
          do {
            buf[len++] = '0' + (num % 10);
            num /= 10;
          } while (num > 0);

          while (len--) {
            *ptr++ = buf[len];
          }
          break;
        }
        case 's': { // 字符串
          const char *str = va_arg(ap, const char *);
          while (*str) {
            *ptr++ = *str++;
          }
          break;
        }
      }
    }
    else {
      *ptr++ = *fmt;
    }
    fmt++;
  }
  *ptr = '\0';
  return ptr - out;  
  //panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int len = vsprintf(out, fmt, ap);
  va_end(ap);
  return len;
  //panic("Not implemented");
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
