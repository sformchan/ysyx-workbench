#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

//#define __NATIVE_USE_KLIB__
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {    //tested

  size_t len = 0; ///0
  while(*s != '\0')
  {
    len++;
    s++;
  }
  return len;
  //panic("Not implemented");
}

char *strcpy(char *dst, const char *src) {   //tested
  char *res = dst;  //store the start of the dst
  while ((*dst++ = *src++) != '\0');
  return res;
  
  //panic("Not implemented");
}

char *strncpy(char *dst, const char *src, size_t n) {   //tested
	/* The  strncpy()  function is similar, except that at most n bytes of src
	are copied.  Warning: If there is no null byte among the first n  bytes
	of src, the string placed in dest will not be null-terminated.

	If  the  length of src is less than n, strncpy() writes additional null
	bytes to dest to ensure that a total of n bytes are written. */
  char *res = dst;
  while (*src != '\0' && n != 0) {
    *dst = *src;
    ++dst;
    ++src;
    --n;
  }
  while (n != 0) {
    *dst = '\0';
    ++dst;
    --n;
  }
  return res;
  //panic("Not implemented");
}

char *strcat(char *dst, const char *src) {  //tested
  char *ans = dst;
  // Move dst to the end of the string
  while (*dst != '\0') {
    ++dst;
  }

  // Copy src to the end of dst
  while (*src != '\0') {
    *dst = *src;
    dst++;
    src++;
  }

  // Append null terminator
  *dst = '\0';

  return ans;
}


int strcmp(const char *s1, const char *s2) {    //tested
	/* The  strcmp()  function compares the two strings s1 and s2.  The locale
	is not taken into account (for  a  locale-aware  comparison,  see  str‐
	coll(3)).  The comparison is done using unsigned characters. */
  while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2) {
    s1++;
    s2++;
  }
  return *s1 == *s2 ? 0 : (unsigned char)*s1 < (unsigned char)*s2 ? -1 : 1;
  //panic("Not implemented");
}

int strncmp(const char *s1, const char *s2, size_t n) {    //tested

  while (n != 0 && *s1 != '\0' && *s2 != '\0' && *s1 == *s2) {
    --n;
    ++s1;
    ++s2;
  }
  return *s1 == *s2 || n == 0 ? 0 : (unsigned char)*s1 < (unsigned char)*s2 ? -1 : 1;
  //panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {  //tested
  unsigned char *src = s;   // switch to unsigned char (get the lowest 8 bits as a byte)
  while (n != 0) {
    --n; //--
    *src = c;
    ++src;
  }
  return s;
  //panic("Not implemented");
}

void *memmove(void *dst, const void *src, size_t n) {    //tested
  unsigned char *dest = dst;
  const unsigned char *source = src;
  if (dst < src) {
    while (n != 0) {
      --n;
      *dest = *source;
      ++dest;
      ++source;
    }
  } else {
    dest += n;
    source += n;
    while (n != 0) {
      --n;
      --dest;
      --source;
      *dest = *source;
    }
  }
  return dst;
  //panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {   //tested
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

 int memcmp(const void *s1, const void *s2, size_t n) {    //tested
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
