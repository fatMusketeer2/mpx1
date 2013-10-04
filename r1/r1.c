#include <mpx_supt.h>
#include <r1.h>

Shell* MainShell;

int main(){
  
  sys_init(MODULE_R1); //init MODULE_R1
	prompt(); //start the prompt
  sys_exit(); //gracefully exit sys.
  
  return 0;
  
}

void prompt(){
  
  //declare variables.
	int retValue,i, result;
	char *command, *input;
  
  MainShell->isRunning = 1; // set isRunning to true.
	while(MainShell->isRunning){ //loop while MainShell is running.
    
		write(":>"); //prompt user for input.
		input = trim(read(100)); //read input and trim it.
    
		command = strtok(input, " ,-|");//grab command from input.
    //sys_free_mem(input); //free input memory.
    result = executeCommand(EXEC, command); //execute command.
    
    if(strcmpi(command, "exit") == 0) MainShell->isRunning = 0; //catch exit command.
    if(result == 0) write("Command not found!\n"); //command not valid
    
	}

}

void write(char *string){
  
  //declare variables
  int slen = strlen(string);
  sys_req(WRITE, TERMINAL, string, &slen); //write to terminals
  
}

char* read(int size){
  
  //declare variables
  int lc, max = size;
  char* input = sys_alloc_mem(max);
  
  if(input == 0) write("can't allocate memory!");
  sys_req(READ, TERMINAL, input, &max);
  
  //remove trailing '\n' if input contains it.
  lc = strlen(input)-1;
  if(input[lc] == '\n') input[lc] = '\0';
  
  return input;
  
}

char* trim(char* s)
{
  char* e=strrchr(s, '\0'); /* Find the final null */
  while(s<e && isspace(*s)) /* Scan forward */
    ++s;
  while (e>s && isspace(*(e-1))) /* scan back from end */
    --e; 
  *e='\0'; /* terminate new string */
  return s;
}