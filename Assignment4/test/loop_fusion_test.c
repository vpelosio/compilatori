void loops_same_indvar_values()
{
    int A[20];

    for(int i=0;i<20;i++)
    {
        A[i] = i*2;
    }

    for(int j=0;j<20;j++)
    {
        A[j] = A[j]*2;
    }
}

void loops_different_indvar_values_same_tripcount()
{
    int A[30];

    for(int i=0;i<10;i++)
    {
        A[i] = i*2;
    }

    for(int j=0;j<30;j+=2)
    {
        A[j] = j*3;
    }
}