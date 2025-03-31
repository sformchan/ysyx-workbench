#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t num = 0;
  while(s[num] != '\0')
  {
    num++;
  }
  return num + 1;
  //panic("Not implemented");
}

char *strcpy(char *dst, const char *src) {
  size_t i;
  for(i = 0; src[i] != '\0'; i++)
  {
    dst[i] = src[i];
  }
  dst[i] = src[i];
  return dst;
  panic("Not implemented");
}

char *strncpy(char *dst, const char *src, size_t n) {
  panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
  size_t length = strlen(dst);
  size_t i;
  for(i = 0; src[i] != '\0'; i++)
  {
    dst[length + i] = src[i];
  }
  dst[length + i] = '\0';
  return dst;
  //panic("Not implemented");
}

int strcmp(const char *s1, const char *s2) {
  while(*s1 && (*s1 == *s2))
  {
    s1++;
    s2++;
  }
  return (int) *s1 - (int) *s2;
  //panic("Not implemented");
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  unsigned char *dst = (unsigned char *)s;  
  unsigned char value = (unsigned char)c;   
  for (size_t i = 0; i < n; i++) {
    dst[i] = value;  // 按字节设置内存
  }
  return s;

  //panic("Not implemented");
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  size_t i;
  unsigned char *dst = (unsigned char *)out;
  const unsigned char *src = (const unsigned char *)in;
  
  for (i = 0; i < n; i++) {
    dst[i] = src[i];
  }
  return out;
  //panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n) {
  panic("Not implemented");
}

#endif
