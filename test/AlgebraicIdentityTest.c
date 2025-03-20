#include <stdio.h>
#include <stdlib.h>

int main()
{
  int x = 20;
  int y = x + 0;
  int z = 0 + x;
  int a = x + 30; /* Must not be removed */
  printf("Result of x+0 is: %d\n", x+0);
  printf("Result of 0+x is: %d\n", 0+x);
  printf("Result of a is: %d\n", a);
  printf("Result of x+19 is: %d\n", x+19);
  return 0;
}
