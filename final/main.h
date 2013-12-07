
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "date.h"
#include "mpx_supt.h"
#include "r3.h"
#include <dos.h>
#include "pcb.h"






struct ShellStruct;
typedef struct ShellStruct {
  date_rec* currentDate;
  char* currentPath;
  char* prevCommands;
  int isRunning;
  char* prompt;

} Shell;




int main();
void loadComHandler();
void prompt();
void write(char *string);
char* read();
char* trim(char *s);

static Shell* MainShell;
