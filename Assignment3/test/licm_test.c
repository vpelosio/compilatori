void test(int a, int b)
{
   int c,d,e,f,g;
   while(1)
   {
      c = a+b; // must be hoisted
      if(a)
      {
          c = a*b; // doesn't dominate all uses of c
          d = a*2; // dead at exit, must be hoisted
          f = c*7; // c instr is loop invariant but can't be hoisted so even if f is dead, since has c has dep can't be hoised
          if(f)
          {
             g = a*a; // cannot be hoisted, doesn't dominate all loop exits
             break;
           }
      }
      else
      {
          c = a-b; // doesn't dominate all uses of c
          e = c*c; // cannot be hoisted, e is not dead at loop exit
      }
      if(c) break;
   }
   a = e*c;
}
