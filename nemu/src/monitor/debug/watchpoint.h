#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>
#define EXPR_LEN_WP 100

typedef struct watchpoint {
  int NO;
  char expr[EXPR_LEN_WP];
  uint32_t value;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */

} WP;


WP* create_new_wp();
void join_new_wp(WP *);
void free_wp(int);
void info_watchpoints();
void delete_all();
int update_value(WP*);
WP* ret_head();
#endif
