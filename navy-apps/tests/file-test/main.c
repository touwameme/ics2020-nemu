#include <stdio.h>
#include <assert.h>

int main() {
  printf("file test\n");
  FILE *fp = fopen("/share/files/num", "r+");
  assert(fp);
  //printf("fp at %p\n",fp);

  fseek(fp, 0, SEEK_END);
  //printf("fp at %p\n",fp);
  long size = ftell(fp);
  assert(size == 5000);

  fseek(fp, 500 * 5, SEEK_SET);
  //printf("fp at %p\n",fp);
  int i, n;
  for (i = 500; i < 1000; i ++) {
    fscanf(fp, "%d", &n);
    //printf("n is %d  i is %d\n",n,i);
    assert(n == i + 1);
  }

  fseek(fp, 0, SEEK_SET);
  //printf("fp at %p\n",fp);
  for (i = 0; i < 500; i ++) {
    fprintf(fp, "%4d\n", i + 1 + 1000);
  }

  for (i = 500; i < 1000; i ++) {
    fscanf(fp, "%d", &n);
    assert(n == i + 1);
  }

  fseek(fp, 0, SEEK_SET);
  for (i = 0; i < 500; i ++) {
    fscanf(fp, "%d", &n);
    assert(n == i + 1 + 1000);
  }

  fclose(fp);

  printf("PASS!!!\n");

  return 0;
}
