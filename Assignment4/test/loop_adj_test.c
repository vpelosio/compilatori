void guardedLoopsAdjTest(int guard)
{
    int A[20];
    
    for(int i=0;i<10;i++)
    {
        A[i] = i*2;
    }

    for(int i=10;i<20;i++)
    {
        A[i] = i/2;
    }
}

void unguardedLoopsAdjTest()
{
    int A[20];
    int i = 0, j = 10;
    
    /* Rotated loops not guarded */
    do
    {
        A[i] = i*2;
        i++;
    }while(i<10);
    
    do
    {
        A[j] = j/2;
        j++;
    }while(j<20);
}

/* To pass the normal form check the loop must be guarded */
void notAdjGuardedLoopsTest(int *x)
{
    int A[20];
    
    /* Guarded rotated loops */
    for(int i=0;i<10;i++)
    {
        A[i] = i*2;
    }

    *x = 10;

    for(int i=10;i<20;i++)
    {
        A[i] = i/2;
    }
}

void notAdjUnguardedLoopsTest(int *x)
{
    int A[20];
    int i = 0, j = 10;
    
    /* Rotated loops not guarded */
    do
    {
        A[i] = i*2;
        i++;
    }while(i<10);

    *x = 10;
    
    do
    {
        A[j] = j/2;
        j++;
    }while(j<20);
}
