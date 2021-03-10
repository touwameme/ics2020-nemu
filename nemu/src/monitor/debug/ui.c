#include <isa.h>
#include "expr.h"
#include "watchpoint.h"
#include <memory/paddr.h>
#include <monitor/monitor.h>
#include <monitor/difftest.h>
#include <isa/x86.h>

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include<string.h>
void cpu_exec(uint64_t);
int is_batch_mode();


extern int detach;
/* We use the `readline' library to provide more flexibility to read from stdin. */

static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
  return 0;
}





static int cmd_q(char *args) {
	nemu_state.state=NEMU_QUIT;
	return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args){
	char * arg = strtok(NULL," ");
	if (arg==NULL){
	  cpu_exec(1);
	  return 0;
	}
	else{
		int n = atoi(arg);
		if (n == 0){
			printf("input a number as parameter\n");
			return 0;
		}
		else{
			cpu_exec(n);
			return 0;
		}
	}
}

void info_eflags(){
  int i ;
  char* flags[FLAGS_NUM] = {"CF","ZF","SF","IF","DF","OF"};
  u_int32_t flagindex[FLAGS_NUM]= {EFLAG_CF,EFLAG_ZF,EFLAG_SF,EFLAG_IF,EFLAG_DF,EFLAG_OF};
  for (i=0;i<FLAGS_NUM;i++){
    printf("%s\t%d\n",flags[i],(cpu.eflags & flagindex[i] )!=0);
  }
}

static int cmd_d(char *args){
    char *arg = strtok(NULL," ");
    if(arg==NULL){
      printf("delete all watchpoints? press y/n to comfirm\n");
      char c;
      int n;
      n=scanf("%c",&c);
      if ((n==1)&&(c=='y')){
          delete_all();
      }
      else{
        printf("delete cancelled\n");
      }
    }
    else{
      int NO;
      sscanf(arg,"%d",&NO);
      free_wp(NO);
    }
  return 0;
}


static int cmd_info(char*args){
  char* arg = strtok(NULL," ");
  if (arg==NULL){
	  printf("input r or w as parameter\n");
	  return 0;
  }
  else{
	if (strcmp(arg,"r")==0)	{	
	  isa_reg_display();
    printf("idt: limit :%x  base :%x \n",cpu.idt.limit,cpu.idt.base);
    printf("cs: %x\n",cpu.cs);
  	  return 0;
	}
	else{
		if(strcmp(arg,"w")==0){
      info_watchpoints();
	    return 0;
		}
		else{
      if(strcmp(arg,"f")==0){
        info_eflags();
        return 0;
      }
      else{
			  printf("input r or w as parameter\n");
			  return 0;
      }
		}
	}
  }
}


static int cmd_p(char *args){
	char* arg=strtok(NULL,"");
	bool success;
	printf("%s\n",arg);
	success=true;
  uint32_t value;
  value=expr(arg,&success);
  if (success){
    printf("%u\n",value);
  }
  else{
    printf("bad expression\n");

  }
	return 0;
}

static int cmd_px(char *args){
	char* arg=strtok(NULL,"");
	bool success;
	printf("%s\n",arg);
	success=true;
  uint32_t value;
  value=expr(arg,&success);
  if (success){
    printf("0x%x\n",value);
  }
  else{
    printf("bad expression\n");

  }
	return 0;
}

static int cmd_x(char *args){
	char* arg =strtok(NULL," ");
	int num= atoi(arg);
	if (num==0){
		printf("error:input x N 0xXXXX\n");
		return 0;
	}
	arg =strtok(NULL," ");
	if(strncmp(arg,"0x",2)==0){
		arg+=2;
		char str[10];
		strcpy(str,arg);
		int add;
		int i;
		sscanf(str,"%x",&add);
		for (i=0;i<num;i++){
		if (add>PMEM_SIZE)
			printf("overflow address\n");
		else{	
			printf("0x%x\t",add);
			int j;
			for (j=0;j<4;j++){
			int value= paddr_read(add,1);
			printf("%02x ",value);
			add += 1;
			}
			printf("\n");
		}
		}
	}
	else
		printf("error:input x N 0xXXXX\n");


return 0;
}

static int cmd_b(char * args){
  char* arg =strtok(NULL,"");
  if (arg== NULL)
    printf("please input:b addr \n");
  else{
  WP* nwp;
  nwp = create_new_wp();
  char exp[128];
  sprintf(exp,"$pc==%s",arg);
  strcpy(nwp->expr,exp);
  if(update_value(nwp)){
  join_new_wp(nwp);
  printf("set breakpoint%d at %s\n",nwp->NO,arg);
  }
  else
    printf("bad expression\n");
  }
  return 0;
}


static int cmd_w(char * args){
  char* arg =strtok(NULL,"");
  if (arg== NULL){
    printf("please input:w expr \n");
    return 0;
  }
  else{
  WP* nwp;
  nwp = create_new_wp();
  strcpy(nwp->expr,arg);
  if(update_value(nwp)){
  join_new_wp(nwp);
  printf("set watchpoint NO:%d  EXPR:%s\n",nwp->NO,nwp->expr);
  }
  else{
    printf("bad expression\n");
  }
  return 0;
  }
}


static int cmd_detach(char *args){
  detach = 1;
  printf("close diff_test\n");
  return 0;
}

static int cmd_attach(char *args){

  detach = 0;
  isa_difftest_attach();
  printf("open diff_test\n");
  return 0;
}




static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  {"si","execute next instruction",cmd_si },
  {"info","display information of regs or watching point",cmd_info},
  {"x","print value in memory",cmd_x},
  {"p","calculate expression",cmd_p},
  {"px","calculate expression,print in 0x form",cmd_px},
  {"w","set watchpoint",cmd_w},
  {"d","deletewatchpoints",cmd_d},
  {"b","set breakpoint",cmd_b},
  {"det","close difftest",cmd_detach},
  {"att","open difftest",cmd_attach},
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop() {
  if (is_batch_mode()) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}



