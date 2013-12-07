#include "main.h"
#include "commands.h"

Shell* MainShell;

//******************************************************************************
// main
// Parameters: none
// Returns : 0 
// Functions called: sys_init, initR2, init R3, openDeviceDrivers, loadComHandler,
// loadProgram, dispatcher, closeDeviceDrivers, cleanR2, sys_exit
// Initializes the systems, ques, registes and IO Devices, Loads the command
// handler and IDLE process. Closes IO devices , cleans queues and exits.

int main(){

  sys_init(MODULE_F); //init MODULE_R2
  initR2();
  initR3();
  openDeviceDrivers();
	loadComHandler();
	loadProgram("IDLE",-128);

	showAllPCB();
	dispatcher();
  closeDeviceDrivers();

  cleanR2();

  
  sys_exit(); //gracefully exit sys.

  return 0;

}

//******************************************************************************
// loadComHandler
// Parameters: none
// Returns: nothing
// Functions called: setupPCB, FP_OFF, FP_SEG, insertPCB
// Loads the command handler as a system process with highest prority.

void loadComHandler(){
 PCB *thePCB;
 context *con;

 thePCB = setupPCB("ComHandler",127,SYS);
 thePCB->state= READY;
 con=(context*)thePCB->stackTop;
 con->IP = FP_OFF(&prompt);
 con->CS = FP_SEG(&prompt);
 con->FLAGS = 0x200;
 con->DS = _DS;
 con->ES = _ES;
 insertPCB(thePCB);
}

//******************************************************************************
// prompt
// Parameters: none
// Returns: nothing
// Functions called: write, trim, strtok, executeCommand, strmpi, emptyQueues, dispatcher
// Executes a while loop prompting the user for input, then reads and parses the input.

void prompt(){

  //declare variables.
	int retValue,i, result;
	char *command, *input, *arg;

  MainShell->isRunning = 1; // set isRunning to true.
  
  write("WELCOME to MPX \n");
  write("Type HELP for a command list\n\n");
	while(MainShell->isRunning){ //loop while MainShell is running.

		write(":>"); //prompt user for input.
		input = trim(read(100)); //read input and trim it.

		command = strtok(input, " ,-|");//grab command from input.

    result = executeCommand(EXEC, command); //execute command.

    if(strcmpi(command, "exit") == 0){
	write("Exiting..\n");
	MainShell->isRunning = 0; //catch exit command.
 
	   emptyQueues();
	   dispatcher();
		
	}
    if(result == 0 && strlen(input) != 0) write("Command not found!\n"); //command not valid

	sys_free_mem(input);
	}

}

//******************************************************************************
// write
// Parameters: string-pointer to char 
// Returns: nothing
// Functions called; strlen, sys_req
// print to terminal 

void write(char *string){

  //declare variables
  int slen = strlen(string);
  sys_req(WRITE, TERMINAL, string, &slen); //write to terminals

}

//******************************************************************************
// read
// Parameters: size- amount of characters to read
// Returns: pointer to char 
// Functions called; sys_alloc_mem, write, strlen, sys_req
// reads input from terminal 

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

//******************************************************************************
// trim
// Parameters: s- pointer to char
// Returns: pointer to char 
// Functions called; strrchr, issspace
// strips all leading and trailing spaces

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
