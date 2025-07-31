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
	int len = 0;
	do {
	  buf[len++] = digits[num % base];
	  num /= base;
	} while (num > 0);
	return len;
  }
  
  int vsprintf(char *out, const char *fmt, va_list ap) {
	char *ptr = out;
	while (*fmt) {
	  if (*fmt != '%') {
		*ptr++ = *fmt++;
		continue;
	  }
  
	  fmt++;  // skip '%'
	  int left_align = 0, zero_pad = 0, width = 0;
	  if (*fmt == '-') { left_align = 1; fmt++; }
	  if (*fmt == '0') { zero_pad = 1; fmt++; }
  
	  while (*fmt >= '0' && *fmt <= '9') {
		width = width * 10 + (*fmt++ - '0');
	  }
  
	  if (*fmt == '%') {
		*ptr++ = '%'; fmt++; continue;
	  }
  
	  char buf[32];
	  int len = 0;
  
	  switch (*fmt) {
		case 'd': {
		  int num = va_arg(ap, int);
		  unsigned int unum = (num < 0) ? -num : num;
		  len = utoa(unum, buf, 10, 0);
		  if (num < 0) buf[len++] = '-';
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
		  *ptr++ = ch;
		  fmt++;
		  continue;
		}
		case 's': {
		  const char *str = va_arg(ap, const char *);
		  while (*str) *ptr++ = *str++;
		  fmt++;
		  continue;
		}
		case 'p': {
		  uintptr_t p = (uintptr_t)va_arg(ap, void *);
		  *ptr++ = '0'; *ptr++ = 'x';
		  len = utoa((unsigned int)(p), buf, 16, 0);  // assume 32-bit pointer
		  break;
		}
		default: {
		  *ptr++ = '%';
		  if (*fmt) *ptr++ = *fmt;
		  fmt++;
		  continue;
		}
	  }
  
	  // padding
	  int padding = width - len;
	  if (!left_align) {
		char pad = zero_pad ? '0' : ' ';
		while (padding-- > 0) *ptr++ = pad;
	  }
	  while (len-- > 0) *ptr++ = buf[len];
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
