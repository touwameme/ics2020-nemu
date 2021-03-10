#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;

void call_main(uintptr_t *args) {
/*
char *empty[] =  {NULL };
  environ = empty;
  exit(main(0, empty, empty));
  assert(0);
*/

//printf("call_main  main  args is %s\n",args);
int argc;
 argc = *args;
 args = (uintptr_t*)((int)(args) + sizeof(int));
uintptr_t* arg_area = args;
//printf("argc %d\n",argc);

 char* argv[argc];
 for(int i = 0;i<argc;i++){
   argv[i] = (char*)arg_area[i];
  //printf("arg%d %p %s\n",i,arg_area[i],arg_area[i]);
 }

 //printf("arg%d %p %s\n",1,arg_area[1],arg_area[1]);
 args=(uintptr_t*)((int)(args)+sizeof(char*)*(1+argc));
 uintptr_t* en_area = args;

int env_count = 0;
//sprintf("callmain en0 %s\n",*en_area);

 for(int i=0;en_area[i] != 0;i++){
   env_count++;}
 if(env_count != 0){
 //printf("ev %d\n",env_count);
 char* envp[env_count];
 for(int i=0;i<env_count;i++){
   envp[i] = (char*)en_area[i];
 }
 environ = envp;
}
else{
  environ = NULL;
}
 
args=(uintptr_t*)((int)(args)+sizeof(char*)*(1+env_count));

  

/*
  for(int i=0;i<argc;i++){
    printf("argv[%d]  %s\n",i,argv[i]);
  } 
  if(env_count != 0){
  for(int i=0;i<env_count;i++){
    printf("envp[%d]  %s\n",i,environ[i]);
  } 
  }
  printf("we\n");
  */
  exit(main(argc, argv, environ));
  assert(0);
}
