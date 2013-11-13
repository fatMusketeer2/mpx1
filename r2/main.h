
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "date.h"
#include "mpx_supt.h"

struct ShellStruct;
typedef struct ShellStruct {
  date_rec* currentDate;
  char* currentPath;
  char* prevCommands;
  int isRunning;
} Shell;

extern Shell* MainShell;

int main();
void prompt();
void write(char *string);
char* read();
char* trim(char *s);

