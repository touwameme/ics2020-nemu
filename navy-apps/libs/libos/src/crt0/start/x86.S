.globl  _start
_start:
  movl $0, %ebp
  movl %eax,%esp
  push %eax

  movl $0 ,%eax
  call call_main
