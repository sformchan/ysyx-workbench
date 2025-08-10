#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)



// int printf(const char *fmt, ...) {

//   panic("Not implemented");
// }

// printf implementation using your vsprintf
static int utoa(unsigned int num, char *buf, int base, int uppercase) {
    const char *digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
    char tmp[32];
    int len = 0;

    // 先逆序放到 tmp
    do {
        tmp[len++] = digits[num % base];
        num /= base;
    } while (num > 0);

    // 正序写回 buf
    for (int i = 0; i < len; i++) {
        buf[i] = tmp[len - 1 - i];
    }
    return len;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
    char *ptr = out;

    while (*fmt) {
        if (*fmt != '%') {
            *ptr++ = *fmt++;
            continue;
        }

        fmt++; // skip '%'
        int left_align = 0, zero_pad = 0, width = 0;
        if (*fmt == '-') { left_align = 1; fmt++; }
        if (*fmt == '0') { zero_pad = 1; fmt++; }

        while (*fmt >= '0' && *fmt <= '9') {
            width = width * 10 + (*fmt++ - '0');
        }

        if (*fmt == '%') { *ptr++ = '%'; fmt++; continue; }

        char buf[64];
        int len = 0;
        int neg = 0; // 负号标志

        switch (*fmt) {
            case 'd': {
                int num = va_arg(ap, int);
                if (num < 0) { neg = 1; num = -num; }
                len = utoa((unsigned int)num, buf, 10, 0);
                break;
            }
            case 'u': {
                unsigned int num = va_arg(ap, unsigned int);
                len = utoa(num, buf, 10, 0);
                break;
            }
            case 'x': case 'X': {
                unsigned int num = va_arg(ap, unsigned int);
                len = utoa(num, buf, 16, *fmt == 'X');
                break;
            }
            case 'o': {
                unsigned int num = va_arg(ap, unsigned int);
                len = utoa(num, buf, 8, 0);
                break;
            }
            case 'c': {
                char ch = (char)va_arg(ap, int);
                buf[0] = ch;
                len = 1;
                break;
            }
            case 's': {
                const char *str = va_arg(ap, const char *);
                while (str[len]) { buf[len] = str[len]; len++; }
                break;
            }
            case 'p': {
                uintptr_t p = (uintptr_t)va_arg(ap, void *);
                buf[0] = '0';
                buf[1] = 'x';
                len = 2 + utoa((unsigned int)(p), buf + 2, 16, 0);
                break;
            }
            default: {
                buf[0] = '%';
                buf[1] = *fmt ? *fmt : '\0';
                len = (*fmt) ? 2 : 1;
                break;
            }
        }

        // 计算总长度（加负号）
        int total_len = len + (neg ? 1 : 0);
        int padding = (width > total_len) ? (width - total_len) : 0;

        if (!left_align) {
            char pad = zero_pad ? '0' : ' ';
            while (padding-- > 0) *ptr++ = pad;
        }

        if (neg) *ptr++ = '-';
        for (int i = 0; i < len; i++) *ptr++ = buf[i];

        if (left_align) while (padding-- > 0) *ptr++ = ' ';

        fmt++;
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

int printf(const char *fmt, ...) {
    char buffer[512];
    va_list ap;
    va_start(ap, fmt);
    int len = vsprintf(buffer, fmt, ap);
    va_end(ap);
    for (int i = 0; i < len; i++) {
        putch(buffer[i]);
    }
    return len;
}


int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
