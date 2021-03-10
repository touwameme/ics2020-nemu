#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include<stdlib.h>
#include<unistd.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int head=0;
static uint32_t choose(uint32_t n){
 
  return rand()%n;


}
static inline void gen(char c){
  char p[2];
  p[0] = c;
  p[1]= '\0';
  strcat(buf,p);

}

static inline void gen_numz(){
  char number[]="0123456789";
  int index=choose(10);
  char p[2];
  gen(number[index]);
  switch(choose(2)){
    case 0:
      gen_numz();
      break;
    case 1:
      break;
  }

}
static inline void gen_num(){
  char number[]="123456789";
  int index=choose(9);
  gen(number[index]);
  gen_numz();

}

static inline void gen_blank(){
  switch(choose(2)){
    case 0:
        gen(' ');
        gen_blank();
        break;
    case 1:
        break;
  }

}

static inline void gen_rand_op(){
    char oprands[]="+-*/";
    int index=choose(4);
    gen(oprands[index]);

}

static inline void gen_rand_expr() {
  gen_blank();
  switch(choose(3)){
    case 0:
    gen_num();break;
    case 1:
        gen ('('); 
        gen_num();
        gen(')'); 
        break;
    default: gen_rand_expr();gen_rand_op();gen_rand_expr();break;

  }
  }
   


int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 10;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    head=0;
    buf[head]='\0';
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
