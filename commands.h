#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define EXEC 		  0
#define HELP 		  1

int executeCommand(int op, char* command);
void quit(int op);
void help(int op);
void version(int op);
void getDate(int op);
void setDate(int op);
void list(int op);