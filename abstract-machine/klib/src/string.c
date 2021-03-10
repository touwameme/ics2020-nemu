#include <klib.h>
#include <stdint.h>
#include <stdio.h>
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t i = 0;
  while(s[i] != '\0'){
    i++;
  }
  return i;
}

char *strcpy(char* dst,const char* src) {
  int i =0;
  while(src[i]!= '\0'){
    dst[i] = src[i];
    i++;
  }
  dst[i]= '\0';
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  int i =0;
  char * tem= dst;
  while(*src != '\0' && i<n){
    *tem = *src;tem++;src++;i++;
  }
  *tem ='\0';
  return dst;
}
char* strcat(char* dst, const char* src){
  size_t i;
  size_t dest_len = strlen(dst);
  for(i=0;src[i]!='\0';i++)
      dst[dest_len + i] = src[i];
  dst[dest_len + i] = '\0';
      return dst;
}
char* strncat(char* dst, const char* src,size_t n) {
  size_t dest_len = strlen(dst);
      size_t i;
      for (i = 0 ; i < n && src[i] != '\0' ; i++)
          dst[dest_len + i] = src[i];
      dst[dest_len + i] = '\0';
      return dst;
}

int strcmp(const char* s1, const char* s2) {
  size_t i;
  for (i=0;(s1[i]==s2[i])&& (s1[i]!='\0');i++){}
  
  return s1[i]-s2[i];
}

int strncmp(const char* s1, const char* s2, size_t n) {
  size_t i;
  for (i = 0; i < n && s1[i] == s2[i]&& s1[i] != '\0'; i++){}
  return  (i<n)?s1[i]-s2[i]:0;
}

void* memset(void* v,int c,size_t n) {
  size_t i;
  char* p = (char *)v;
  for (i=0;i<n;i++)
    {*p = c;p++ ;}
  return v;
}

void* memmove(void* dst,const void* src,size_t n) {
  size_t i;
  char* pdst = (char *)dst;
  char* psrc = (char *)src;
  for (i=0;i<n;i++)
    {pdst[i] = psrc[i];}
  return dst;
}

void* memcpy(void* out, const void* in, size_t n) {
    size_t i;
    char* pout = (char *)out;
    char* pin = (char *)in;
    for (i=0;i<n;i++)
      {pout[i] = pin[i];}
    return out;
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n) {
  size_t i;
  char *p1 = (char*)s1;
  char *p2 = (char*)s2;
  for (i = 0; i < n ; i++)
       if(p1[i] != p2[i]) return p1[i]-p2[i];
  return 0;
}

void set_tail (char* p){
    while(*p != '\0')
    {p++;}
}


#endif
