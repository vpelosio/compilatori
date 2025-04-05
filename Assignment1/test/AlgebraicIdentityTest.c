#include <stdio.h>
#include <stdlib.h>

void AlgebraicIdentitySumTest()
{
   int startValue = 20;
   int ident1 = startValue + 0;
   int ident2 = 0 + startValue;
   int ident3 = 0 + 15;
   int ident4 = 15 + 0;
   int testSum1 = startValue + 50; /* Non deve essere sostituita */
   int testSum2 = 50 + startValue; /* Non deve essere sostituita */
   printf("%d %d %d %d %d %d", ident1, ident2, ident3, ident4, testSum1, testSum2);
}

void AlgebraicIdentityTestMul()
{
   int startValue = 20;
   int ident1 = startValue * 1;
   int ident2 = 1 * startValue;
   int ident3 = 1 * 12;
   int ident4 = 12 * 1;
   int testMul1 = startValue * 3; /* Non deve essere sostituita */ 
   int testMul2 = 3 * startValue; /* Non deve essere sostituita */
   printf("%d %d %d %d %d %d", ident1, ident2, ident3, ident4, testMul1, testMul2);
}
