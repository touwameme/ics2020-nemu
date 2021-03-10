

#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>


#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define NRM_MODE 1
#define FMT_MODE 2
#define BUF_SIZE 1000
char buf1[BUF_SIZE] = {0};
char buf2[BUF_SIZE] = {0};

int vsprintf(char *out, const char *fmt, va_list ap) {
  char * p = out;
  assert(&buf1[0]!=0);
  char *s = buf1;
  char * ss ;
  int mode = NRM_MODE;
  int length = 0;
  char fill_ch = '\0';
  int fillin = 0;
  while(*fmt != '\0'){
    switch (mode)
    {
    case NRM_MODE : 
      {if (*fmt == '%'){mode = FMT_MODE;length = 0;fillin = 0;fill_ch='\0';fmt++;}
      else{*p= *fmt;p++;fmt++ ;break;}
      }
    case FMT_MODE :    
        switch (*fmt)
        {
        case '%':*p = '%';p++;fmt++; mode = NRM_MODE;break;
        case 'c':*p =(char)(va_arg(ap,int));p++;fmt++; mode = NRM_MODE;break;
        case 's': ss=va_arg(ap,char*);strcpy(p,ss); p+= strlen(p); fmt++;mode = NRM_MODE;fillin=0;break;
        case 'd': 
				  if(!fillin){itoa(va_arg(ap,int),s,10);strcpy(p,s);}
                  else{
                      if(fill_ch!='\0'){itoa_fill(va_arg(ap,int),s,10,fill_ch,length);}
                      else{itoa(va_arg(ap,int),s,10);}strncpy(p,s,length);} 
                  p+= strlen(p); fmt++;mode = NRM_MODE;fillin=0;break;
        case 'p':         
        case 'x': if(!fillin){itoa(va_arg(ap,int),s,16);strcpy(p,s);}
                  else{
                     if(fill_ch!='\0'){itoa_fill(va_arg(ap,int),s,16,fill_ch,length);}
                    else{itoa(va_arg(ap,int),s,16);}strncpy(p,s,length);} 
                      p+= strlen(p);fmt++;mode = NRM_MODE;fillin=0;break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
          fillin = 1;
          length = length*10 + (*fmt-'0');
          fmt++;
          break;
        }
        case '0':
        { if(!fillin){
          fillin = 1;
          fill_ch = '0';
        }
        else{
          length = length*10 + (*fmt-'0');
          fmt++;
        }
          break;
        }
        default:panic("\ninvalid format\n");
        }
    
        break;
        default:panic("\ninvalid format\n");
    }
    


  }
  *p = '\0';
  return strlen(out);
}

int sprintf(char *out, const char *fmt, ...) {
  int len = 0;
  va_list ap;
  va_start(ap,fmt);
  len = vsprintf(out, fmt, ap);
  //printf("prt\n");
  va_end(ap);
  return len;
}
int printf(const char *fmt, ...) {
  int len=0;
  va_list ap;
  va_start(ap,fmt);
  len = vsprintf(buf2, fmt, ap);
  va_end(ap);
  size_t i;
  for (i=0;buf2[i]!='\0';i++)
  { putch(buf2[i]);}
  return len;
}


int snprintf(char *out, size_t n, const char *fmt, ...) {

  va_list ap;
  va_start(ap,fmt);
  vsprintf(buf2, fmt, ap);
  va_end(ap);
  size_t i;
  for (i=0;buf2[i]!='\0'&& i< n ;i++)
  { out[i] = buf2[i];
  }
  return i;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  assert(0);
  return 0;
}

#endif


/*

#include <am.h>
	#include <klib.h>
	#include <klib-macros.h>
	#include <stdarg.h>
  #include"stdlib.c"
	
	#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
	
	int printf(const char *fmt, ...)
	{
	//return 0;
	
	char buf1[8142] = {0};
	
	va_list ap;
	va_start(ap, fmt);
	int ret = vsprintf(buf1, fmt, ap);
	va_end(ap);
	
	char *tmp = buf1;
	while (*tmp) {
	putch(*tmp++);
	}
	
	return ret;
	}
	
	int vsprintf(char *out, const char *fmt, va_list ap)
	{
	while (*fmt) {
	char c = *fmt;
	if (c == '%' && *(fmt+1)) {
	unsigned int tmp;
	int stmp;
	char buf2[24] = {0}, *s;
	memset(buf2, 0, 24);
	
	switch(*(fmt+1)) {
	case 's':
	s = va_arg(ap, char *);
	strcpy(out, s);
	out += strlen(s);
	fmt += 2;
	break;
	case 'd':
	stmp = va_arg(ap, int);
	if (stmp < 0) {
	stmp *= (-1);
	*out = '-';
	out ++;
	}
	itoa(stmp, buf2);
	strcpy(out, buf2);
	out += strlen(buf2);
	fmt += 2;
	break;
	case 'u':
	tmp = va_arg(ap, unsigned int);
	itoa(tmp, buf2);
	strcpy(out, buf2);
	out += strlen(buf2);
	fmt += 2;
	break;
	case 'p':
	case 'x':
	tmp = va_arg(ap, unsigned int);
	itoa_hex(tmp, buf2);
	strcpy(out, buf2);
	out += strlen(buf2);
	fmt += 2;
	break;
	default:
	//assert(0);
	*out++ = *fmt++;
	}
	} else {
	*out++ = *fmt++;
	}
	}
	
	*out = '\0';
	
	return strlen(out);
	}
	
	int sprintf(char *out, const char *fmt, ...)
	{
	assert(out && fmt);
	
	va_list ap;
	va_start(ap, fmt);
	int ret = vsprintf(out, fmt, ap);
	va_end(ap);
	
	return ret;
	}
	
	int snprintf(char *out, size_t n, const char *fmt, ...) {
	return 0;
	}
	
	int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
	return 0;
	}
	
	#endif
*/
  