#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  if (s == NULL) return 0; 
  size_t len = 0;
  while (s[len] != '\0') {
    len++;
  }
  return len;
}

char *strcpy(char *dst, const char *src) {
  if (dst == NULL || src == NULL) return dst; // 处理空指针
  char *p = dst;
  while ((*p++ = *src++) != '\0');
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  if (dst == NULL || src == NULL) return dst; // 处理空指针
  char *p = dst;
  while (n-- && (*p++ = *src++) != '\0');
  while (n--) *p++ = '\0'; // 填充剩余的 '\0'
  return dst;
}

char *strcat(char *dst, const char *src) {
  if (dst == NULL || src == NULL) return dst; // 处理空指针
  char *p = dst;
  while (*p) p++; // 找到 dst 的末尾
  while ((*p++ = *src++) != '\0'); // 追加 src
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  if (s1 == NULL || s2 == NULL) return 0; // 处理空指针
  while (*s1 && *s2 && *s1 == *s2) {
    s1++;
    s2++;
  }
  return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  if (s1 == NULL || s2 == NULL) return 0; // 处理空指针
  while (n-- && *s1 && *s2 && *s1 == *s2) {
    s1++;
    s2++;
  }
  return n ? *(unsigned char *)s1 - *(unsigned char *)s2 : 0;
}

void *memset(void *s, int c, size_t n) {
  if (s == NULL) return s; // 处理空指针
  unsigned char *p = s;
  while (n--) *p++ = (unsigned char)c;
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  if (dst == NULL || src == NULL) return dst; // 处理空指针
  unsigned char *d = dst;
  const unsigned char *s = src;
  if (d < s) {
    while (n--) *d++ = *s++; // 从前往后复制
  } else {
    d += n;
    s += n;
    while (n--) *--d = *--s; // 从后往前复制
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  if (out == NULL || in == NULL) return out; // 处理空指针
  unsigned char *d = out;
  const unsigned char *s = in;
  while (n--) *d++ = *s++;
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  if (s1 == NULL || s2 == NULL) return 0; // 处理空指针
  const unsigned char *p1 = s1, *p2 = s2;
  while (n-- && *p1 == *p2) {
    p1++;
    p2++;
  }
  return n ? *p1 - *p2 : 0;
}

#endif
