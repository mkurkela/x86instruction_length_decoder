#include <iostream>
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
  void *sym = dlsym(RTLD_NEXT, argv[1]);
  int length, patches;
  length = myInstruction.decodeInstruction(sym);
  if (length == 0)
  {
    std::cout << "Invalid instruction" << std::endl;
  }
  patches = myInstruction.getPatch();
  std::cout << "The length: " << length <<" patches " << patches << std::endl;
  return 0;
}
