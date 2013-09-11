#include "mpx_supt.h" 	//included by e.a
#include "MPXR1ea.h"
#include <string.h>	//included by e.a


int main(void){

	sys_init(MODULE_R1);
	commhndl();
	return(0);




}



void help(char *name){

	int retValue, max;
	char buff[500];
	max=500;

	sprintf(buff,"Passed %s",name);
	max = strlen(buff);
	sys_req(WRITE,TERMINAL,buff,&max);

	if(!(strcmpi(name,"void\n"))){
		sprintf(buff,"Type 'help' to see this list.\n");
		max = strlen(buff);
		sys_req(WRITE,TERMINAL,buff,&max);

		sprintf(buff,"Type 'help name' to find out more of the command 'name'.\n");
		max = strlen(buff);
		sys_req(WRITE,TERMINAL,buff,&max);

		sprintf(buff,"%-25s %-10s\n","DATE","Display the current system date.");
		max = strlen(buff);
		sys_req(WRITE,TERMINAL,buff,&max);

		sprintf(buff,"%-25s %-10s\n","SETDATE","Set/overwrite the current system date.");
		max = strlen(buff);
		sys_req(WRITE,TERMINAL,buff,&max);

		sprintf(buff,"%-25s %-10s\n","DIR","Display all files in current directory.");
		max = strlen(buff);
		sys_req(WRITE,TERMINAL,buff,&max);

		sprintf(buff,"%-25s %-10s\n","EXIT","Terminate.");
		max = strlen(buff);
		sys_req(WRITE,TERMINAL,buff,&max);

		sprintf(buff,"%-25s %-10s\n","HELP","Display List of System Commands.");
		max = strlen(buff);
		sys_req(WRITE,TERMINAL,buff,&max);
	 }
	else if(!(strcmpi(name,"date\n"))){
		sprintf(buff,"Display the current system date.\n");
		max = strlen(buff);
		sys_req(WRITE,TERMINAL,buff,&max);

		sprintf(buff,"DATE\n");
		max = strlen(buff);
		sys_req(WRITE,TERMINAL,buff,&max);

	}

	else if(!(strcmpi(name,"setdate\n"))){
		sprintf(buff,"Set/overwrite the current system date\n");
		max = strlen(buff);
		sys_req(WRITE,TERMINAL,buff,&max);

		sprintf(buff,"SETDATE [MM-DD-YYYY]\n");
		max = strlen(buff);
		sys_req(WRITE,TERMINAL,buff,&max);

	}
	else if(!(strcmpi(name,"dir\n"))){
		sprintf(buff,"Display all files and subdirectories in a directory.");
		max = strlen(buff);
		sys_req(WRITE,TERMINAL,buff,&max);

		sprintf(buff,"DIR \n");
		max = strlen(buff);
		sys_req(WRITE,TERMINAL,buff,&max);
	}

	else if(!(strcmpi(name,"exit\n"))){
		sprintf(buff,"Terminate\n");
		max = strlen(buff);
		sys_req(WRITE,TERMINAL,buff,&max);

		sprintf(buff,"EXIT\n");
		max = strlen(buff);
		sys_req(WRITE,TERMINAL,buff,&max);
	}

	else if(!(strcmpi(name,"help\n"))){
		sprintf(buff,"Display a List of System Commands\n");
		max = strlen(buff);
		sys_req(WRITE,TERMINAL,buff,&max);

		sprintf(buff,"HELP\n");
		max = strlen(buff);
		sys_req(WRITE,TERMINAL,buff,&max);
	}

	else{   sprintf(buff,"Command %s was not found.\n",name);
		max = strlen(buff);
		sys_req(WRITE,TERMINAL,buff,&max);
	}
}



/////////////////////////////////////////////////////////////////////

void commhndl(){


	char buff[100],cmndPrmpt[2];

	int retValue;
	int max=100;

	cmndPrmpt[0]='?';
	cmndPrmpt[1]='~';

	max=2;
	retValue=sys_req(WRITE,TERMINAL,cmndPrmpt,&max);

	max=100;
	retValue=sys_req(READ,TERMINAL,buff,&max);
	help(buff);

	sprintf(buff,"Return Value : %d",retValue);
	max = strlen(buff);
	sys_req(WRITE,TERMINAL,buff,&max);
}
