#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstdlib>
#include "instruction.h"
#include <dlfcn.h>

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    std::cout << "usage: ./testInstruction <instruction in hex>" << std::endl;
    exit(1);
  }
  
  Instruction myInstruction;
  unsigned char *sym = (unsigned char *)dlsym(RTLD_NEXT, argv[1]);
  if (!sym)
  {
    printf("couldnt find a symbol\n");
    exit(1);
  }
  int i;
  for (i = 0; i < 11; i++)
  {
    printf("1: %x\n",sym[i]);
  }
  int length, patches;
  int n = 0;
  while ( n < 15)
  {
    printf("sym[0] = %x\n", *sym);
    length = myInstruction.decodeInstruction(sym, 0);
    if (length == 0)
    {
      std::cout << "Invalid instruction" << std::endl;
      exit(1);
    }
    for (i = 0; i < length; i++)
    {
      printf("%d: %x\n",i,sym[i]);
    }

    patches = myInstruction.getPatch();
    printf("length: %d, patches: %x\n",length,patches);
    n += length;
    sym += length;
    myInstruction.clear();
  }
  return 0;
}
