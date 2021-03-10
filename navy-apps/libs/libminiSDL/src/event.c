#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

static uint8_t keystate[83]={0};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  int len = 0;
  char buf[64];
  len  = NDL_PollEvent(buf,64);
 
  if(len ==0) 
      {return 0;}
  else{
     //printf("%s\n",buf);
    if(buf[0]=='k' && buf[1]=='u')
        {ev->type = SDL_KEYUP;}
    else if(buf[0]=='k' && buf[1]=='d')
        { ev->type = SDL_KEYDOWN;}
    else{assert(0);}
    char buf2[16];
    strcpy(buf2,buf+3);
    buf2[strlen(buf2)-1]='\0';

    for(int i =0;i<83 ;i++){
      //printf("%d [%s]||||[%s]||||\n",i,keyname[i],buf2);
      if(strcmp((const char*)buf2,(const char*)keyname[i]) == 0){
        ev->key.keysym.sym = i;
        //printf("%s",buf2);

        keystate[i]= (ev->type == SDL_KEYDOWN);
        return 1;
      }
    }
    //return 1;
    //assert(0);

  }

}

int SDL_WaitEvent(SDL_Event *event) {
  
  while(!SDL_PollEvent(event));
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  assert(0);
  return 0;
}
uint8_t* SDL_GetKeyState(int *numkeys) {
  
  return keystate;
}
