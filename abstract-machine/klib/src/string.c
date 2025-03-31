#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  if(s == NULL) return 0;
  size_t num = 0;
  while(s[num] != '\0')
  {
    num++;
  }
  return num;
  //panic("Not implemented");
}

char *strcpy(char *dst, const char *src) {
  if (src == NULL || dst == NULL) {   // 没有所指，直接返回dst
    return dst;
  }
  // 当成指向字符数组处理，所以即使没有空字符，导致内存访问越界，或修改了其他有用的数据也不管，因为这是函数调用者所需要保证的，下面一些string函数都是这样对带非字符串数组
  char *res = dst;
  do {
    *dst = *src;
    dst++;
    src++;
  } while(*src != '\0');  
  return res;
  //panic("Not implemented");
}

char *strncpy(char *dst, const char *src, size_t n) {
  panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
  if(dst == NULL || src == NULL) return dst;
  char *ans = dst;
  while (*dst != '\0') {
    ++dst;
  }
  do {
    *dst = *src;
    dst++;
    src++;
  } while(*src != '\0');  // 先做一次，可以保证将结束字符也复制进去
  return ans;
  //panic("Not implemented");
}

int strcmp(const char *s1, const char *s2) {
  if(s1 == NULL || s2 == NULL) return 0;
  while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2) {
    s1++;
    s2++;
  }
  return *s1 == *s2 ? 0 : (unsigned char)*s1 < (unsigned char)*s2 ? -1 : 1;
  //panic("Not implemented");
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  if(s == NULL) return s;
  unsigned char *src = s;   // 先讲传入得指针，做无符号字符解释
  while (n != 0) {
    --n;
    *src = c;
    ++src;
  }
  return s;
  //panic("Not implemented");
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  if(out == NULL || in == NULL || n == 0 || out == in) return out;
  unsigned char *dest = out;
  const unsigned char *src = in;
  while (n != 0) {
    *dest = *src;
    --n;
    ++dest;
    ++src;
  }
  return out;
  //panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n) {
  panic("Not implemented");
}

#endif
