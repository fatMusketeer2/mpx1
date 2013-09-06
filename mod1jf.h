#include <stdio.h>
#include <string.h>

#define EXIT 		0
#define HELP 		1
#define VERSION 	2
#define DATE 		3
#define SET_DATE	4
#define LS			5


int main();
void commhand();
int match(char * command);
void help(int command);
void version();
void displayDate();
void setDate(char * d, char* m, char *y);
void ls(char * path);
