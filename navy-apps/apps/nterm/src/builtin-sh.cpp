#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

char* get_arg(char* p){
  while((*p !=' ')&&(*p != '\0')){
    p  = (char*)((int)p+1);
  }
  char* tem = p;
  if(*tem == '\0') return NULL;
  else{
    *p = '\0';p  = (char*)((int)p+1);
    while((*p ==' ')&&(*p != '\0')){p  = (char*)((int)p+1);}
  }
  if(*p== '\0') return NULL;
  else{return p;}
  
}

static void sh_handle_cmd(const char *cmd) {
//printf("pa4-version\n");
if(strcmp(cmd,"exit\n")== 0){
    exit(0);
  }
  if(strcmp(cmd,"clear\n")== 0){
    SDL_Clear();
  }
  int n = strlen(cmd);
  char buf[100]={'\0'};
  strncpy(buf,cmd,n-1);
  char* file =buf;
  while(*file ==' '){file=(char*)(int(file) +1);}
  int i;
  
  char* empty[]={NULL};
  static char* argv[10] = {};
  char** args = argv;
  for(i=0;i<10;i++){
    char* tem = get_arg(file);
    if(tem != NULL){argv[i] = tem;}
    else{argv[i]=NULL;break;}
  }  

  //printf("file %s\n",file);
  //printf("-----\n");
  if(i>0){
  for (int t=0;t<i;t++){
  //printf("arg%d %s\n",t,argv[t]);
  }
  }
  else{args=empty;}
  //printf("-----\n");


  execvpe(file,args,empty);
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
