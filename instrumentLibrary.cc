#define __GNU_SOURCE
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <dlfcn.h>
#include "instruction.h"
#include "hook.h"
#include "signal.h"
#include <setjmp.h>

FILE *hook_errors;
char *current_fn;
char *lib;
jmp_buf env;

static void dofunction();
void hookfailed();

// Try to call the hook  
void callHook(const char *fn, const char *lib)
{
  IgHook::TypedData<void()>dofunction_hook = { { 0, fn, 0, lib, &dofunction, 0, 0, 0 } };
  IgHook::hook(dofunction_hook.raw); 
  (*dofunction_hook.typed.chain)();
}

static void handler(int sig, siginfo_t *si, void *unused)
{
  printf("Got SIGSEGV at address: 0x%llx\n",(long long) si->si_addr);
  hookfailed();
  longjmp(env,1);
}    

// instrumented
void unsupported(unsigned char *insns, char *fn, char *lib, FILE *errors)
{
  // open error file and write unsupported functions
  errors = fopen("errors.txt","a+"); 
  fprintf(errors, "From library: %s, function: %s\n",lib, fn);
  for (int i = 0; i < 8; i++)
  {
    fprintf(errors, "%x ",insns[i]);
  }
  fprintf(errors, "\n");
  fclose(errors);
}

void hookfailed()
{
  hook_errors = fopen("hook_errors.txt", "a+");
  fprintf(hook_errors, "From library: %s, function: %s\n", lib, current_fn);
  fclose(hook_errors);
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("Usage: ./instrumentLibrary PATH_TO_LIB\n");
    exit(1);
  }

  // Enable sighandler for SIGSEGV
  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = handler;
  if (sigaction(SIGSEGV, &sa, NULL) == -1) 
  {
    printf("Sigaction failed");
    exit(1);
  }

  // use nm to get symbols from the library
  FILE *nmout;
  lib = argv[1];
  char cmd[1024];
  snprintf(cmd, 1024, "nm -D %s", lib);
  if (! (nmout = popen(cmd, "r")))
  {
    printf("popen failed\n");
    exit(1);
  }

  // Open the library
  dlerror();
  void *handle = dlopen(lib, RTLD_NOW);
  if (!handle)
  {
    pclose(nmout);
    printf("dlopen failed: %s\n", dlerror());
    exit(1);
  }

  FILE *errors;
  char buffer[1024];
  char fn[1024];
  unsigned char *sym;
  char *start;
  int n = 0;
  Instruction myInstruction;
  int length = 0;
  while(true)
  {
    setjmp(env);
    // Read symbol name from nmout file
    if (! (fgets(buffer, sizeof(buffer), nmout)))
      break;
    if (! (start = strstr(buffer, "T ")))
    {
      if (! (start = strstr(buffer, "t ")))
      { 
        continue;
      }
    }
    strcpy(fn, &buffer[start - buffer + 2]);
    // replace ending '\n' with '\0'
    fn[strlen(fn)-1] = '\0';
    printf("Sym %s\n", fn);
    current_fn = fn;

    // try to find symbol from the library, if not found continue to next
    sym = (unsigned char *)dlsym(handle, fn);
    if (!sym)
      continue;

    // need at least 5bytes space
    while (n < 5)
    {
      if (! (length = myInstruction.decodeInstruction(sym, n)))
      {
        unsupported(sym, fn, lib, errors);
        n = 5;
        myInstruction.clear();
        continue;
      }
      // Move to next instruction
      sym += length;
      n += length;
      myInstruction.clear();
    }
    n = 0;
    callHook(fn, lib);
  }
  printf("done\n"); 
  dlclose(handle);
  pclose(nmout);
  return 0;
}

void dofunction() {}
