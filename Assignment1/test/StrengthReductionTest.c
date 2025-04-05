#include <stdio.h>

void testMulPowerOf2()
{
    int val1, val2;
    scanf("%d %d", &val1, &val2);
    
    int powOfTwo = 16;
    int prod1 = 16 * val1;          /* literal(lhs) * variabile(rhs) */
    int prod2 = val1 * 16;          /* variabile(rhs) * literal(rhs) */
    int prod3 = 16 * 9;             /* literal(lhs) * literal(rhs) */ /* VIENE CALCOLATO DA CLANG ANCHE CON -O0. CHIEDERE INFO AL PROFESSORE */
    int prod4 = val1 * powOfTwo;    /* variabile(lhs) * variabile(rhs) */
    int prod5 = val1 * val2;        /* variabile(lhs) * variabile(rhs), entrambe sconociute a compile-time */
    printf("%d %d %d %d %d\n", prod1, prod2, prod3, prod4, prod5);  
}

void testMulNotPowerOf2()
{
    int val1, val2;
    scanf("%d %d", &val1, &val2);
    
    int notPowOfTwo = 23;
    int prod1 = 23 * val1;           /* literal(lhs) * variabile(rhs) */
    int prod2 = val1 * 23;           /* variabile(rhs) * literal(rhs) */
    int prod3 = 23 * 9;              /* literal(lhs) * literal(rhs) */
    int prod4 = val1 * notPowOfTwo;  /* variabile(lhs) * variabile(rhs)*/
    int prod5 = val1 * val2;         /* variabile(lhs) * variabile(rhs), entrambe sconosciute a compile-time */
    printf("%d %d %d %d %d\n", prod1, prod2, prod3, prod4, prod5);  
}

void testMulPowerOfTwoMinusOne()
{
    int val1, val2;
    scanf("%d %d", &val1, &val2);
    
    int powOfTwoMinusOne = 15;
    int prod1 = 15 * val1;                /* literal(lhs) * variabile(rhs) */
    int prod2 = val1 * 15;                /* variabile(rhs) * literal(rhs) */
    int prod3 = 15 * 9;                   /* literal(lhs) * literal(rhs) */
    int prod4 = val1 * powOfTwoMinusOne;  /* variabile(lhs) * variabile(rhs)*/
    int prod5 = val1 * val2;              /* variabile(lhs) * variabile(rhs), entrambe sconosciute a compile-time */
    printf("%d %d %d %d %d\n", prod1, prod2, prod3, prod4, prod5);  
}

void testMulPowerOfTwoPlusOne()
{
    int val1, val2;
    scanf("%d %d", &val1, &val2);
    
    int powOfTwoPlusOne = 17;
    int prod1 = 17 * val1;               /* literal(lhs) * variabile(rhs) */
    int prod2 = val1 * 17;               /* variabile(rhs) * literal(rhs) */
    int prod3 = 17 * 9;                  /* literal(lhs) * literal(rhs) */
    int prod4 = val1 * powOfTwoPlusOne;  /* variabile(lhs) * variabile(rhs)*/
    int prod5 = val1 * val2;             /* variabile(lhs) * variabile(rhs), entrambe sconosciute a compile-time */
    printf("%d %d %d %d %d\n", prod1, prod2, prod3, prod4, prod5);  
}

void testUnsignedDivisionPowerOfTwo()
{
    unsigned int val1, val2;
    scanf("%d %d", &val1, &val2);
    
    unsigned int powOfTwo = 8;
    unsigned int div1 = 8/val1;          /* literal(lhs) / variabile(rhs) */
    unsigned int div2 = val1/16;         /* variabile(rhs) / literal(rhs) */
    unsigned int div3 = 20/8;            /* literal(lhs) / literal(rhs) */
    unsigned int div4 = val1/powOfTwo;   /* variabile(lhs) / variabile(rhs)*/
    unsigned int div5 = val1/val2;       /* variabile(lhs) / variabile(rhs), entrambe sconosciute a compile-time*/
    printf("%d %d %d %d %d\n", div1, div2, div3, div4, div5);  
}