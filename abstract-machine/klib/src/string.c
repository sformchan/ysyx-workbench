#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

//#define __NATIVE_USE_KLIB__
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {

  size_t len = 0; ///0
  while(*s != '\0')
  {
    len++;
    s++;
  }
  return len;
  //panic("Not implemented");
}

char *strcpy(char *dst, const char *src) {

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

  char *ans = dst;
  while (*src != '\0' && n != 0) {
    *dst = *src;
    ++dst;
    ++src;
    --n;
  }
  // 将额外的空字符写入dest，直到写入了n个字符的总数。
  while (n != 0) {
    *dst = '\0';
    ++dst;
    --n;
  }
  return ans;
  //panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
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
 
  while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2) {
    s1++;
    s2++;
  }
  return *s1 == *s2 ? 0 : (unsigned char)*s1 < (unsigned char)*s2 ? -1 : 1;
  //panic("Not implemented");
}

int strncmp(const char *s1, const char *s2, size_t n) {

  while (n != 0 && *s1 != '\0' && *s2 != '\0' && *s1 == *s2) {
    --n;
    ++s1;
    ++s2;
  }
  // 当比较了n次后，即新n变为0时，此时两个字符串也是相等得，memcmp同理
  return *s1 == *s2 || n == 0 ? 0 : (unsigned char)*s1 < (unsigned char)*s2 ? -1 : 1;
  //panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  unsigned char *src = s;   // 先讲传入得指针，做无符号字符解释
  while (n != 0) {
    --n; //--
    *src = c;
    ++src;
  }
  return s;
  //panic("Not implemented");
}


//void *memmove(void *dst, const void *src, size_t n) {
  // unsigned char *dest = dst;
  // const unsigned char *source = src;
  // if (dst < src) {
  //   while (n != 0) {
  //     --n;
  //     *dest = *source;
  //     ++dest;
  //     ++source;
  //   }
  // } else {
  //   dest += n;
  //   source += n;
  //   while (n != 0) {
  //     --n;
  //     --dest;
  //     --source;
  //     *dest = *source;
  //   }
  // }
  // return dst;
  //return 0;
  // //panic("Not implemented");
//}

void *memcpy(void *out, const void *in, size_t n) {
  //printf("==> using klib memcpy\n");
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
   const unsigned char *src1 = s1;
   const unsigned char *src2 = s2;
   while (n != 0 && *src1 != '\0' && *src2 != '\0' && *src1 == *src2) {
     --n;
     ++src1;
     ++src2;
   }
   return *src1 == *src2 || n == 0 ? 0 : *src1 < *src2 ? -1 : 1;
   //panic("Not implemented");
}

#endif
