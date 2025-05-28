#include <stdio.h>
#include <stdbool.h>

void same_known_loop_count(int a, int b)
{
    int count = 0;
    int sum = 0;
    int prod = 1;
    int n = 15;
    int m = 15;

    for (int i = 0; i < n*2; i++)
    {
        count++;
    }
    for (int i = 0; i < n*2; i++)
    {
        count++;
    }

    for (int i = 0; i < a; i++)
    {
        sum += i;
    }
    for (int i = 0; i < a-1; i++)
    {
        sum += i;
    }

    for (size_t i = 0; i < n; i++)
    {
        prod *= i;
    }
    
    for (size_t i = 0; i < m; i++)
    {
        prod *= i;
    }
}

void different_loop_count(int a, int b, int n, int m)
{
    int count = 0;
    int sum = 0;
    int prod = 1;


    for (int i = 0; i < n*2; i++)
    {
        count++;
    }
    for (int i = 0; i < n*2; i++)
    {
        count++;
    }

    for (int i = 0; i < a; i++)
    {
        sum += i;
    }
    for (int i = 0; i < a-1; i++)
    {
        sum += i;
    }

    for (size_t i = 0; i < n; i++)
    {
        prod *= i;
    }
    
    for (size_t i = 0; i < m; i++)
    {
        prod *= i;
    }
}