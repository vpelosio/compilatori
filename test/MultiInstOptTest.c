#include <stdio.h>

void testAddMultInstOpt()
{
    int b;
    scanf("%d", &b);

    int a = 1 + b;
    int g = a + 2;
    int h = g - 9;
    int c = a - 1;
    int d = c + 7 * 20;
    int e = b + 7;
    int f = a + 9;
    int i = g - 2;
    int j = i * 9;

    printf("%d", c);
}

void testSubMultInstOpt()
{
    int b;
    scanf("%d", &b);

    int a = b - 1;
    int c = 1 + a;
    int d = c + 7 * 20;

    printf("%d", c);
}

void testMulMultInstOpt()
{
    unsigned int b;
    scanf("%u", &b);

    unsigned int a = b * 2;
    int c = a / 2U;
    int d = c + 7 * 20;
    int e = 5 * b;
    int f = 5U / b;

    printf("%d", c);
}