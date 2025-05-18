#include <stdio.h>

void guardedLoopsAdjTest(int guard)
{
    int i = 0;
    int A[20];
    
    /* Guarded rotated loops */
    if(guard)
    {
        do
        {
            A[i] = i*2;
            i++;
        } while (i<10);
    }


    if(guard)
    {
        do
        {
            A[i] = i/2;
            i++;
        } while (i<20);
    }
}

void notGuardedLoopsAdjTest()
{
    int A[20];
    int i = 0;
    
    /* Rotated loops not guarded */
    do
    {
        A[i] = i*2;
        i++;
    }while(i<10);

    
    //if(i%13==0) return;

    do
    {
        A[i] = i/2;
        i++;
    }while(i<20);
}
