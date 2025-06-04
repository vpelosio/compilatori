void dep_free_loop()
{
   int n = 20;
   int A[20], B[20];

   for(int i=0;i<n;i++)
     A[i] = i*2;

   for(int i=0;i<n;i++)
     B[i] = i*2;
}

void dep_instr_loop()
{
   int n = 20;
   int A[20], B[20];

   for(int i=0;i<10;i++)
      A[i] = i*2;

   for(int j=10;j<20;j++)
      B[j] = A[j-10]*3;
}

void negative_dep_instr_loop()
{
   int n = 20;
   int A[20], B[20];

   for(int i=0;i<n;i++)
      A[i] = i*2;

   for(int i=0;i<n;i++)
      B[i] = A[i+2]*3;
}
