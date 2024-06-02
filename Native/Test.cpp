#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#include "NativeInterface.h"

uint64_t CSharpInstanceMethod(void* instance)
{
  return (uint64_t)instance;
}

uint64_t CSharpInstanceMethodMulti(void* instance, int a, int b)
{
  return ((uint64_t)instance)+a+b;
}

char WrappedGotString[4096];
int WrappedGotLevel=0;

void WrappedLogCheck(int logLevel, const char* log)
{
  WrappedGotLevel = logLevel;
  strcpy(WrappedGotString,log);
}

typedef void (*LogPrint)(int level, const char* format, ...);

int main(int argc, char** argv)
{
  // Verify Instance Trampoline from Native
  auto trampoline = (uint64_t(*)())allocate_trampoline((void*)12345,0, (void*)CSharpInstanceMethod);
  if (12345!=trampoline())
  {
    printf("allocate_trampoline - trampoline() function failed!\n");
    return 1;
  }
  // Verify Instance Trampoline with 2 params
  auto trampolineMulti = (uint64_t(*)(int,int))allocate_trampoline((void*)12345,2, (void*)CSharpInstanceMethodMulti);
  if ((12345+5+9)!=trampolineMulti(5,9))
  {
    printf("allocate_trampoline - trampoline(5,9) function failed!\n");
    return 1;
  }
  if ((12345+20+100)!=trampolineMulti(20,100))
  {
    printf("allocate_trampoline - trampoline(20,100) function failed!\n");
    return 1;
  }
  // Verify wrapped printer
  auto wrapped = (LogPrint)allocate_printer((void*)WrappedLogCheck);
  wrapped(0,"NoParams");
  if (WrappedGotLevel!=0)
  {
    printf("allocate_printer - wrapped(0,'NoParams') log level does not match");
    return 2;
  }
  if (strcmp(WrappedGotString, "NoParams")!=0)
  {
    printf("allocate_printer - wrapped(0,'NoParams') log string does not match");
    return 2;
  }

  printf("Success!\n"); 
  return 0;
}
