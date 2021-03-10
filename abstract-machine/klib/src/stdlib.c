
#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;




int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next/65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}

int abs(int x) {
  return (x < 0 ? -x : x);
}

int atoi(const char* nptr) {
  int x = 0;
  while (*nptr == ' ') { nptr ++; }
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr ++;
  }
  return x;
}





char* itoa(long int n,char* str,int base) {
  int neg = 0;
  uint32_t abs;
  if (n<0){ neg = 1; abs = ~n + 1;}
  else {abs = n;}
  size_t j = 0;
if(n==0){
  str[0]=(char)'0';
  str[1]='\0';
  return str;
}
switch (base){
  case 10:{
      while(abs!=0){
     str[j] = (char)('0'+(abs %10));
     abs /=10;
     j++;
      }
      break;
  }
  case 16:{
      int temp;
      while (abs != 0){
        temp = abs%16;
        abs/=16;
        if(temp<10) str[j] = (char)('0'+temp);
        else str[j] = (char)('a'+temp-10);
        j++;
      }
      break;

  }
  default :printf("\nwrong base is %c \n",(char)('0'+base));panic("\nwrong base\n");
  }
if (neg){
    str[j]='-';
    j++;
  }
  str[j]='\0';
  j--;
  char tem;
  int cnt;
  for(cnt=0;cnt<((j+1)/2);cnt++){
    tem =str[cnt];
    str[cnt]=str[j-cnt];
    str[j-cnt]= tem;
  }


return str;
}


char* itoa_fill(int n,char* str,int base,char ch,int len) {
  size_t i;;
  for (i=0;i<len;i++){
    str[i] = ch;
  }
  str[i] = '\0';
  int neg = 0;
  uint32_t abs;
  if (n<0){ neg = 1; abs = ~n + 1;}
  else {abs = n;}
  size_t j = 0;

switch (base){
  case 10:{
      while(abs!=0){
     str[j] = (char)('0'+(abs %10));
     abs /=10;
     j++;
      }
      while(j<len) {j++;}
      break;
  }
  case 16:{
      int temp;
      while (abs != 0){
        temp = abs%16;
        abs/=16;
        if(temp<10) str[j] = (char)('0'+temp);
        else str[j] = (char)('a'+temp-10);
        j++;
      }
      while(j<len) {j++;}
      break;

  }
  default :printf("\nwrong base is %c \n",(char)('0'+base));panic("\nwrong base\n");
  }
if (neg){
    str[j]='-';
    j++;
  }
  str[j]='\0';
  j--;
  char tem;
  int cnt;
  for(cnt=0;cnt<((j+1)/2);cnt++){
    tem =str[cnt];
    str[cnt]=str[j-cnt];
    str[j-cnt]= tem;
  }


return str;
}
static char* old_addr;

void *malloc(size_t size) {
  
  static int first = 1;
  if(first){
    extern Area heap;
    old_addr = heap.start;
    first = 0;
  }

  char* old = (void*)old_addr;
  old_addr +=size;
  assert((uintptr_t)heap.start <= (uintptr_t)old_addr && (uintptr_t)old_addr <(uintptr_t)heap.end);
  return old;
}

void free(void *ptr) {
}




#endif

/*


#include <am.h>
	#include <klib.h>
	#include <klib-macros.h>
	
	#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
	static unsigned long int next = 1;
	
	int rand(void) {
	// RAND_MAX assumed to be 32767
	next = next * 1103515245 + 12345;
	return (unsigned int)(next/65536) % 32768;
	}
	
	void srand(unsigned int seed) {
	next = seed;
	}
	
	int abs(int x) {
	return (x < 0 ? -x : x);
	}
	
	int atoi(const char* nptr) {
	int x = 0;
	while (*nptr == ' ') { nptr ++; }
	while (*nptr >= '0' && *nptr <= '9') {
	x = x * 10 + *nptr - '0';
	nptr ++;
	}
	return x;
	}
	
	// add by cyl. 
	void itoa(unsigned int n, char *buf)
	{
	char tmp[2] = {0};
	if (n < 10) {
	tmp[0] = n+'0';
	strcat(buf, tmp);
	return;
	}
	itoa(n / 10u, buf);
	itoa(n % 10u, buf);
	}
	
	void itoa_hex(unsigned int n, char *buf)
	{
	char tmp[2] = {0};
	if (n < 16) {
	if (n < 10) {
	tmp[0] = n + '0';
	} else {
	tmp[0] = n - 10 + 'a';
	}
	strcat(buf, tmp);
	return;
	}
	itoa_hex(n / 16u, buf);
	itoa_hex(n % 16u, buf);
	}
	
	static void *my_malloc(size_t size)
	{
	assert(size > 0);
	
	static unsigned int addr;
	static int flag = 1;
	if (flag) {
	extern Area heap;
	addr = (unsigned int)heap.start;
	flag = 0;
	}
	
	unsigned int tmp = addr;
	addr += size;
	
	return (void *)tmp;
	}
	
	void *malloc(size_t size) {
	assert(0);
	return my_malloc(size);
	}
	
	void free(void *ptr) {
	}
	
	#endif

  */