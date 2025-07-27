#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)



// int printf(const char *fmt, ...) {

//   panic("Not implemented");
// }

// printf implementation using your vsprintf
int printf(const char *fmt, ...) {
	char buffer[512];  // fixed buffer size; adjust as needed
	va_list ap;
	va_start(ap, fmt);
	int len = vsprintf(buffer, fmt, ap);
	va_end(ap);
  
	for (int i = 0; i < len; i++) {
	  putch(buffer[i]);  // output each char
	}
  
	return len;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *ptr = out;
  while (*fmt) {
    if (*fmt == '%') {
      fmt++;

      // Parse flags '-' and '0'
      int left_align = 0;
      int zero_pad = 0;
      while (*fmt == '-' || *fmt == '0') {
        if (*fmt == '-') left_align = 1;
        else if (*fmt == '0') zero_pad = 1;
        fmt++;
      }

      // Parse field width
      int width = 0;
      while (*fmt >= '0' && *fmt <= '9') {
        width = width * 10 + (*fmt - '0');
        fmt++;
      }

      // Parse precision
      int precision = -1;  // -1 means no precision specified
      if (*fmt == '.') {
        fmt++;
        precision = 0;
        while (*fmt >= '0' && *fmt <= '9') {
          precision = precision * 10 + (*fmt - '0');
          fmt++;
        }
      }

      if (*fmt == '%') {
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
            unum = (unsigned int)(-(num + 1)) + 1;  // safe INT_MIN handling
          } else {
            unum = (unsigned int)num;
          }

          // Convert number to string in reverse order
          do {
            buf[len++] = '0' + (unum % 10);
            unum /= 10;
          } while (unum > 0);

          // Calculate zeros to pad for precision
          int zeros_to_pad = 0;
          if (precision > len) zeros_to_pad = precision - len;

          // Width padding excludes zeros added by precision and sign
          int width_padding = width - zeros_to_pad - len - (num < 0 ? 1 : 0);
          if (width_padding < 0) width_padding = 0;

          // According to standard, if precision specified, zero_pad flag ignored
          if (precision >= 0) zero_pad = 0;

          if (!left_align) {
            char pad_char = zero_pad ? '0' : ' ';
            for (int i = 0; i < width_padding; i++) *ptr++ = pad_char;
          }

          // Print sign if negative
          if (num < 0) *ptr++ = '-';

          // Print zeros for precision
          for (int i = 0; i < zeros_to_pad; i++) *ptr++ = '0';

          // Print digits
          while (len--) *ptr++ = buf[len];

          if (left_align) {
            for (int i = 0; i < width_padding; i++) *ptr++ = ' ';
          }

          fmt++;
          break;
        }
        case 's': {
          const char *str = va_arg(ap, const char *);
          int len = 0;
          while (str[len] != '\0') len++;

          // Precision limits max chars printed for %s
          if (precision >= 0 && precision < len) {
            len = precision;
          }

          int width_padding = width - len;
          if (width_padding < 0) width_padding = 0;

          if (!left_align) {
            for (int i = 0; i < width_padding; i++) *ptr++ = ' ';
          }

          for (int i = 0; i < len; i++) {
            *ptr++ = str[i];
          }

          if (left_align) {
            for (int i = 0; i < width_padding; i++) *ptr++ = ' ';
          }

          fmt++;
          break;
        }
        default: {
          // Unsupported specifier: print literally
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
