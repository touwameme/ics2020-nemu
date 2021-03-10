#include "watchpoint.h"
#include "expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* create_new_wp(){
WP* n_wp;
if(free_!=NULL){
  n_wp= free_;
  free_ = free_->next;
  n_wp->next=NULL;
  return n_wp;
}
else{
  printf("no enough space for new watchpoints\n");
  assert(0);
  return 0;
}

}

void join_new_wp(WP * n_wp){
  if (head == NULL){
    head= n_wp;
  }
  else{
    WP* tail;
    tail = head;
    while(tail->next != NULL){
      tail = tail ->next;
    }
    tail->next = n_wp;
  }
}

void free_wp(int NO){
  int state= 0;
  WP* tail = head;
  while(tail != NULL){
    if(tail->NO == NO){
      if (tail == head){
        head = head ->next;
      }
      else{
        WP* pretail= head;
        while(pretail->next != tail){
          pretail= pretail->next;
        }
        pretail->next = tail ->next;

      }
      state = 1;
      break;
    }
    else{
      tail = tail ->next;
    }

  }
  if (state == 1){
    printf("delete watchpoint NO:%d\n",NO);
  }
  else{
    printf("can not find watchpoint NO:%d\n",NO);
  }
}

void info_watchpoints(){
  if (head == NULL){
    printf("no watchpoint\n");
  }
  else{
    printf("NO\tEXPR\tVALUE\n");
    WP* tail = head;
    while(tail != NULL){
      printf("%d\t%s\t%u\n",tail->NO,tail->expr,tail->value);
      tail = tail->next;
    }
  }
}

void delete_all(){
  if (head== NULL){
    printf("no watchpoints\n");
  }
  else{
    head = NULL;
    printf("delete all watchpoints\n");
  }

}

int update_value(WP* wp){
  bool success = true;
  uint32_t value;
  value = expr(wp->expr,&success);
  if(success){
    wp->value = value;
    return 1;
  }
  else{
    return 0;
  }

}

WP* ret_head(){
  return head;
}