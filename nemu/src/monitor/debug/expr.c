#include <isa.h>
#include <string.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdlib.h>
#include <memory/paddr.h>
enum {
  TK_NOTYPE = 256, TK_EQ,TK_NEQ,TK_AND,TK_OR,TK_NEG,TK_DEREF,TK_NOT,TK_SE,TK_BE,
  TK_S,TK_B,
  /* TODO: Add more token types */

};
enum{
  BRACK=1,NUM,C_OR,C_AND,C_EQNE,C_CMP4,C_ADD_MINUS,C_PLUS_DIVID,C_POS_NEG_NOT,C_DER,
  };

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
 {"\\(",'('},		//bracket_l
  {"\\)",')'},		//bracket_r

  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"!=",TK_NEQ},           //unequal
  {"<=",TK_SE},       //<=
  {">=",TK_BE} ,       //>=
  {"<",TK_S},         //<
  {">",TK_B},         // >
  {"0[xX][0-9a-fA-F]+",'x'},//0x number
  {"[0-9]+",'d'},	// integral
  {"-",'-'},		//minus
  {"\\*",'*'},		//multiply
  {"/",'/'},		//divide
  {"\\$[a-zA-Z]+",'r'}, //register
  {"&&",TK_AND}, //AND
  {"\\|\\|",TK_OR},// OR
  {"!",TK_NOT},//NOT

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  int class;
  char str[32];
} Token;

static Token tokens[256] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
       // char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

      // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //    i, rules[i].regex, position, substr_len, substr_len, substr_start);

        
        
      
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
		case TK_NOTYPE:	
        break;
		case 'd':
    case 'x':
    case 'r':
          tokens[nr_token].type=rules[i].token_type;
          tokens[nr_token].class=NUM;
          if (substr_len>=31){
          printf("number of digits should be under 31\n");
          return false;
        }
        strncpy(tokens[nr_token].str,e+position,substr_len);
        tokens[nr_token].str[substr_len]='\0';
        nr_token +=1;
        break;
		case '(':
		case ')':
        tokens[nr_token].type=rules[i].token_type;
        tokens[nr_token].class=BRACK;
        nr_token +=1;
        break;
    case '+':
    case '-':
        tokens[nr_token].type=rules[i].token_type;
        tokens[nr_token].class=C_ADD_MINUS;
        nr_token +=1;
        break;
		case '*':
		case '/':
        tokens[nr_token].type=rules[i].token_type;
        tokens[nr_token].class=C_PLUS_DIVID;
        nr_token +=1;
        break;
    case TK_S:
    case TK_B:
    case TK_SE:
    case TK_BE:
        tokens[nr_token].type=rules[i].token_type;
        tokens[nr_token].class=C_CMP4;
        nr_token +=1;
        break;
    case TK_EQ:
    case TK_NEQ:
        tokens[nr_token].type=rules[i].token_type;
        tokens[nr_token].class=C_EQNE;
        nr_token +=1;
        break;
    case TK_AND:
        tokens[nr_token].type=rules[i].token_type;
        tokens[nr_token].class=C_AND;
        nr_token +=1;
        break;
    case TK_OR:
        tokens[nr_token].type=rules[i].token_type;
        tokens[nr_token].class=C_OR;
        nr_token +=1;
        break;

	
    default: assert(0);
        }
      position += substr_len;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  
  return true;
}


int state_wrong=0;

static bool check_parentheses(int p,int q){
  int l=0;
  int r=0;
  if ((tokens[p].type=='(')||(tokens[p].type==')')||(tokens[q].type=='(')||(tokens[q].type=='(')){
  
  while(p<=q){
    if (tokens[p].type=='(')
      l++;
    if (tokens[p].type==')')
      r++;
    if (l<=r)
      break;
    p++;
  }
  if (l==r){
    if (q==p)
      return true;
    else{
      return false;

    }
  
  }

  else{
    state_wrong=1;
    return false;
  }

} 

else
{
  return false;
}

}

static void process_NEG(){
  int i;
  for (i=0;i<nr_token;i++){
      if(tokens[i].type=='-' && (i==0 ||tokens[i-1].type=='(' ||tokens[i-1].class==C_ADD_MINUS|| tokens[i-1].class==C_PLUS_DIVID||tokens[i-1].class==C_POS_NEG_NOT||tokens[i-1].class==C_AND||tokens[i-1].class==C_OR||tokens[i-1].class==C_EQNE))
      {tokens[i].type=TK_NEG;
       tokens[i].class=C_POS_NEG_NOT;
      }
  }
}

static void process_DER(){
  int i;
  for (i=0;i<nr_token;i++){
      if(tokens[i].type=='*' && (i==0 ||tokens[i-1].type=='(' ||tokens[i-1].class==C_ADD_MINUS|| tokens[i-1].class==C_PLUS_DIVID||tokens[i-1].class==C_POS_NEG_NOT||tokens[i-1].class==C_AND||tokens[i-1].class==C_OR||tokens[i-1].class==C_EQNE||tokens[i-1].class==C_DER))
      {tokens[i].type=TK_DEREF;
       tokens[i].class=C_DER;
      }
  }
}
 
static int find_op_leftbind(int p , int q,int class){
    int op=-1 ;
    int inbracket=0;
    int i;
    for (i=q;i>=p;i--){
      if (tokens[i].type==')')
        {
          inbracket++;
        }
        if (tokens[i].type=='(')
        {
          inbracket--;
        }
      if (inbracket!=0)
        continue;
      if (tokens[i].class==class)
      {
        op=i;
        break;
      }
    }
  return op;
} 

static int find_op_rightbind(int p , int q,int class){
    int op=-1 ;
    int inbracket=0;
    int i;
    for (i=p;i<=q;i++){
      if (tokens[i].type==')')
        {
          inbracket++;
        }
        if (tokens[i].type=='(')
        {
          inbracket--;
        }
      if (inbracket!=0)
        continue;
      if (tokens[i].class==class)
      {
        op=i;
        break;
      }
    }
  return op;
} 

static uint32_t pder(uint32_t add){
  if(add<=PMEM_SIZE){
  int i;
  uint32_t value=0;
  uint32_t byte;
  for (i=0;i<4;i++){
    byte=paddr_read(add+i,1);
    value+=byte<<(8*i);
  }
  return value;
  }
  else{
    printf("Error:address %u is out of the physical memory\n",add);
    return 0;
  }

}

static uint32_t eval(int p, int q,bool* success) {
  if (p > q) {
    * success=false; 
    return 0;
    /* Bad expression */
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    if(tokens[p].type=='d')
      return atoi(tokens[p].str);
    if(tokens[p].type=='x'){
      uint32_t value;
      sscanf(tokens[p].str,"0x%x",&value);
      return value;
    }
    if(tokens[p].type=='r'){
      uint32_t value;
      bool suc=true;
      char reg[10];
      sscanf(tokens[p].str,"$%s",reg);
      value = isa_reg_str2val(reg,&suc);
      if (suc){
      return value;}
      else{
        printf("Error: cannot access the register:%s\n",reg);
        return 0;
      }
    }
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1,success);
  }
  else {
    int op;
    int class;
    for(class=C_OR;class<=C_PLUS_DIVID;class++){
    op= find_op_leftbind(p,q,class);
    if (op != -1)
      break;
    
    }
    if (op == -1){
        for(class=C_POS_NEG_NOT;class<=C_DER;class++){
         op= find_op_rightbind(p,q,class);
        if (op != -1)
            break;
    }

    }
    if (op == -1){
      *success=false;
      return 0;
    }
    uint32_t val1=0;
    uint32_t val2 = eval(op + 1, q,success);
    int op_type=tokens[op].type;
    switch (op_type) {
      case '+': 
          val1 = eval(p, op - 1,success);
          return val1 + val2;
      case '-': 
          val1 = eval(p, op - 1,success);
          return val1 - val2;
      case '*': 
          val1 = eval(p, op - 1,success);
          return val1 * val2;
      case '/': 
        val1 = eval(p, op - 1,success);
        if(val2!=0)
          return val1 / val2;
        else
        {
          *success=false;
          printf("divided by 0\n");
        }
        break;
      case TK_NEG:return 0-val2;
      case TK_S:
          val1 = eval(p, op - 1,success);
          return val1 < val2;
      case TK_SE:
          val1 = eval(p, op - 1,success);
          return val1 <= val2;
      case TK_B:
          val1 = eval(p, op - 1,success);
          return val1 > val2;
      case TK_BE:
          val1 = eval(p, op - 1,success);
          return val1 >= val2;
      case TK_EQ:
          val1 = eval(p, op - 1,success);
          return val1 == val2;
      case TK_AND:
          val1 = eval(p, op - 1,success);
          return val1 && val2;
      case TK_NEQ:
          val1 = eval(p, op - 1,success);
          return val1 != val2;
      case TK_OR:
          val1 = eval(p, op - 1,success);
          return val1 || val2;
      case TK_NOT:
          return !val2;
      case TK_DEREF:
          return pder(val2);
      default: assert(0);
    }
      
  
  }
  return 0;
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

process_NEG();
process_DER();

uint32_t value;
value= eval(0,nr_token-1,success);
  /* TODO: Insert codes to evaluate the expression. */

if (!success){
  return 0;
  }
else{
  return value;
}
}
