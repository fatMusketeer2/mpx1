
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
    char comm [4][50];  
	while(!exit){
		comm[0][0] = '\0';
		comm[1][0] = '\0';
		comm[2][0] = '\0';
		comm[3][0] = '\0';
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
				if(i = 0){
					help(-1); // no arguments
				}else if(i = 1){
					help(match(comm[1])); // an argument
				}else{
				 printf("Too many arguments,HELP accepts 0 or 1 arguments\n\n");
				}				
				break;
			case VERSION: //version
				version();
				break;
			case DATE: //display date
				displayDate();
				break;
			case SET_DATE: //set date
				setDate(comm[1], comm[2], comm[3]); // Date is entered like DD MM YYYY
				break;
			case LS: //display directory
				if(i = 1){
				 ls(NULL);
				}else if(i = 2){
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
	
	switch(command){
		case EXIT: //exit
			printf("the exit help will go here\n\n");
			break;
		case VERSION: //version
			printf("the version help will go here\n\n");
			break;
		case DATE: //display date
			printf("the display date help will go here\n\n");
			break;
		case SET_DATE: //set date
			printf("the set date help will go here\n\n");
			break;
		case LS: //display directory
			printf("the ls help will go here\n\n");
			break;

		default:
			printf("the default help will go here\n\n");
			break;
	}
	
}



void version(){
	printf("\nTHE VERSION IS 1.0\n\n");
}

void displayDate(){
	printf("\nThe date will be displayed here\n\n");
}

void setDate(char* d, char* m, char *y){
	printf("\nThe Date will be set here to %s %s %s\n\n",d,m,y);
}

void ls(char * path){
	printf("\nThe display directory will be here\n\n");
}



