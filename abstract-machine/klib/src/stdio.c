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
      fmt++;
      if (*fmt == '%') {  // literal %
        *ptr++ = '%';
        fmt++;
        continue;
      }
      switch (*fmt) {
        case 'd': {
          int num = va_arg(ap, int);
          unsigned int unum;
          char buf[32];
          int len = 0;

          if (num < 0) {
            *ptr++ = '-';
            // Handle INT_MIN safely
            unum = (unsigned int)(-(num + 1)) + 1;
          } else {
            unum = (unsigned int)num;
          }

          do {
            buf[len++] = '0' + (unum % 10);
            unum /= 10;
          } while (unum > 0);

          while (len--) {
            *ptr++ = buf[len];
          }
          fmt++;
          break;
        }
        case 's': {
          const char *str = va_arg(ap, const char *);
          while (*str) {
            *ptr++ = *str++;
          }
          fmt++;
          break;
        }
        default: {
          // Unsupported specifier, just print it literally with '%'
          *ptr++ = '%';
          if (*fmt) {
            *ptr++ = *fmt++;
          }
          break;
        }
      }
    } else {
      *ptr++ = *fmt++;
    }
  }
  *ptr = '\0';
  return ptr - out;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int len = vsprintf(out, fmt, ap);
  va_end(ap);
  return len;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
