#include <stdio.h>
#include <stdbool.h>

void same_known_loop_count(int a, int b)
{
    int count = 0;
    int sum = 0;
    int n = 15;
    int m = 15;

    for (int i = 0; i < n; i++)
    {
        count++;
    }
    for (int i = n; i < n*2; i++)
    {
        count++;
    }

    for (int i = 0; i < a; i++)
    {
        sum += i;
    }
    for (int i = 0; i < a; i++)
    {
        sum += i;
    }

    for (size_t i = 0; i < n; i++)
    {
        /* code */
    }
    
    for (size_t i = 0; i < m; i++)
    {
        /* code */
    }
}

void different_loop_count(int a, int b)
{
    int n, m, count = 0;
    scanf("%d", &n);
    scanf("%d", &m);

    for (int i = 0; i < n; i++)
    {
        count++;
    }

    for (int i = 0; i < m; i++)
    {
        count++;
    }
    
    for (size_t i = 0; i < a; i++)
    {
        /* code */
    }
    
    for (size_t i = 0; i < b; i++)
    {
        /* code */
    }
}

/*void same_unknown_loop_count()
{
    int n;
    scanf("%d", n);

    for (size_t i = 0; i < n; i++)
    {
        
    }
}*/