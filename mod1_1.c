#include <mpx_supt.h>
#include <mod1.h>


int main(){
	
	sys_init(MODULE_R1);
	commhand();
	sys_exit();
 return (1);
}

void commhand(){

	char prompt[5] = {'M','P','X','>', ' '};
	int pro = 5;
	char buff[100];
	int max = 100;
	int exit = 0;
	int retValue,i, command;
	char *token;
	char comm [2][50];
	while(!exit){
		comm[0][0] = '\0';
		comm[1][0] = '\0';

		sys_req(WRITE,TERMINAL,prompt, &pro);	
		retValue=sys_req(READ,TERMINAL,buff, &max);
		token = strtok(buff, "\n");
		token = strtok(token, " ");
		i = 0;
		while(token != NULL){
			strcpy(comm[i], token);
			token = strtok(NULL, " ");
			i++;
		}

	command = match(comm[0]);

	switch(command){
		case EXIT: //exit
		exit = 1;
		break;
		case HELP: //help
			if(i == 1){
			printf("\n%d\n",i);
				help(-1); // no arguments
		}else if(i == 2){
		printf("\n%d\n",i);
			help(match(comm[1])); // an argument
		}else{
			printf("\n%d\n",i);
			printf("\nToo many arguments,HELP accepts 0 or 1 arguments\n\n");
		}	
		break;
		case VERSION: //version
			version();
			break;
		case DATE: //display date
			displayDate();
			break;
		case SET_DATE: //set date
			if(i == 1){
				printf("\nSETDATE requires 1 argument formatted DD/MM/YY\n\n");
			}else
				setDate(comm[1]); // Date is entered like DD/MM/YYYY
				break;
		case LS: //display directory
			if(i == 1){
				ls(NULL);
			}else if(i == 2){
				ls(comm[1]);
			}else{
				printf("Too many arguments, LS accepts 0 or 1 arguments\n\n");
			}	
			break;

		default:
			printf("That is not a valid command.\n\n");
			break;
		}
	}

}




int match(char * command){
	int c = -1;
	
	if(strcmpi(command, "exit") == 0){
		c = EXIT;
	}
	else if(strcmpi(command, "help") == 0){
		c = HELP;
	}
    else if(strcmpi(command, "version") == 0){
		c = VERSION;
	}
	else if(strcmpi(command, "date") == 0){
		c = DATE;
	}
	else if(strcmpi(command, "setdate") == 0){
		c = SET_DATE;
	}
	else if(strcmpi(command, "ls") == 0){
		c = LS;
	}
	
	return c;
}	

void help(int command){

  int retValue, max;
	char buff[500];
	max=500;
	
	switch(command){
		case EXIT: //exit
			sprintf(buff,"Terminate\n");
		  max = strlen(buff);
		  sys_req(WRITE,TERMINAL,buff,&max);

		  sprintf(buff,"EXIT\n");
		  max = strlen(buff);
		  sys_req(WRITE,TERMINAL,buff,&max);
		  
			break;
		case VERSION: //version
			sprintf(buff,"Print the current version of MPX and completion date.\n");
		  max = strlen(buff);
		  sys_req(WRITE,TERMINAL,buff,&max);
		  
		  sprintf(buff,"VERSION\n");
		  max = strlen(buff);
		  sys_req(WRITE,TERMINAL,buff,&max);
		  
			break;
		case DATE: //display date
			sprintf(buff,"Display the current system date.\n");
		  max = strlen(buff);
		  sys_req(WRITE,TERMINAL,buff,&max);

		  sprintf(buff,"DATE\n");
		  max = strlen(buff);
		  sys_req(WRITE,TERMINAL,buff,&max);
			break;
		case SET_DATE: //set date
			sprintf(buff,"Set/overwrite the current system date\n");
		  max = strlen(buff);
		  sys_req(WRITE,TERMINAL,buff,&max);

		  sprintf(buff,"SETDATE [MM-DD-YYYY]\n");
		  max = strlen(buff);
		  sys_req(WRITE,TERMINAL,buff,&max);
			break;
		case LS: //display directory
			sprintf(buff,"Display all files and subdirectories in a directory.");
		  max = strlen(buff);
		  sys_req(WRITE,TERMINAL,buff,&max);

		  sprintf(buff,"LS \n");
		  max = strlen(buff);
		  sys_req(WRITE,TERMINAL,buff,&max);
			break;

		default:
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
			break;
	}
	
}



void version(){
	printf("\nTHE VERSION IS 1.0\n\n");
}

void displayDate(){
	printf("\nThe date will be displayed here\n\n");
}

void setDate(char* s){
	printf("\nThe Date will be set here to %s \n\n",s);
}

void ls(char * path){
	char pathName[50];
	char nameBuff[15];
	int buffSize = 15;
	long fileSize;
	int status =0;
	int count =0;
	//open current directory 
	if(path == NULL){
		sys_open_dir("\0"); 
	}else{
		sys_open_dir(path);
	}
	while(TRUE){
		status = sys_get_entry(nameBuff, buffSize, &fileSize);
		if(status != ERR_SUP_NOENTR){
			printf("%s.mpx\t%d\n", nameBuff, fileSize);
			count++;
		}else{
			if (count == 0){
				printf("\nTHERE ARE NO MPX FILES IN %s\n\n", path);
			}
			break;
		}
		
	}
	
	sys_close_dir();
        	        

}
