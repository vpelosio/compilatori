
void known_loop_exec_time(int a, int b)
{
    int n = 15;
    int m = 15;
    int A[30];

    for (int i = 0; i < n*2; i++)
    {
        A[i] = i*2;
    }
    for (int i = 0; i < n*2; i++)
    {
        A[i] = i*2;
    }

    for (int i = 0; i < a; i++)
    {
        A[i] = i*2;
    }
    for (int i = 0; i < a-1; i++)
    {
        A[i] = i*2;
    }

    for (int i = 0; i < n; i++)
    {
        A[i] = i*2;
    }
    
    for (int i = 0; i < m; i++)
    {
        A[i] = i*2;
    }
}

void unknown_loop_exec_time(int a, int b, int n, int m)
{
    int A[n*2], B[m];

    for (int i = 0; i < n*2; i++)
    {
        A[i] = i*2;
    }
    for (int i = 0; i < n*2; i++)
    {
        A[i] = i*2;
    }

    for (int i = 0; i < a; i++)
    {
        A[i] = i*2;
    }
    for (int i = 0; i < a-1; i++)
    {
        A[i] = i*2;
    }

    for (int i = 0; i < n; i++)
    {
        A[i] = i*2;
    }
    
    for (int i = 0; i < m; i++)
    {
        B[i] = i*2;
    }
}