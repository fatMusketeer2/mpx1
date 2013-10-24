
#include "commands.h"
#include "main.h"
#include "pcb.h"
#include "r3.h"

int executeCommand(int op, char* command){
  
	if(strcmpi(command, "exit") == 0) quit(op);
	else if(strcmpi(command, "help") == 0) help(op);
  else if(strcmpi(command, "version") == 0) version(op);
  else if(strcmpi(command, "ls") == 0) list(op);
	else if(strcmpi(command, "date") == 0) getDate(op);
	else if(strcmpi(command, "setdate") == 0) setDate(op);
	else if(strcmpi(command, "create_pcb") == 0) create_pcb(op);
	else if(strcmpi(command, "delete_pcb") == 0) delete_pcb(op);
	else if(strcmpi(command, "block") == 0) block(op);
	else if(strcmpi(command, "unblock") == 0) unblock(op);
	else if(strcmpi(command, "suspend") == 0) suspend(op);
	else if(strcmpi(command, "resume") == 0) resume(op);
	else if(strcmpi(command, "set_priority") == 0) set_priority(op);
	else if(strcmpi(command, "show_pcb") == 0) show_pcb(op);
	else if(strcmpi(command, "show_ready") == 0) show_ready(op);
	else if(strcmpi(command, "show_blocked") == 0) show_blocked(op);
	else if(strcmpi(command, "show_all") == 0) show_all(op);
	else if(strcmpi(command, "load") == 0) load(op);
	else if(strcmpi(command, "dispatch") == 0) dispatchComm(op);
	else if(strcmpi(command, "clear") == 0) clear();
  else return 0;
  
	return 1;
  
}

void quit(int op){
  
  switch(op) {
    case HELP:
      write("'exit': Terminates the shell.\n");
      break;
  }
  
}

void help(int op){
  
  char* command;
  int err;
  switch(op) {
    case EXEC:
      if((command = strtok(NULL, " ,-|")) != NULL) {
		if(executeCommand(HELP, command) == 0){
			write("Invalid command after 'help'\n");
		}
	  }
      else {
        write("Type 'help' to see this list.\n");
        write("'help <command>' displays the help menu for the command.\n\n");
        write("date\t\t\t:- displays the system's current date.\n");
        write("setdate [MM/DD/YYYY]\t:- allows user to set the system's date.\n");
        write("ls <path>\t\t:- lists directories files and folders.\n");
        write("version\t\t\t:- lists current fmOS version.\n");
        write("exit\t\t\t:- exits the fmOS shell.\n");
		//new methods
		write("create_pcb [NAME] [PRIORITY] [CLASS]:-  ...\n");
		write("\t...Creates a PCB and inserts into a queue\n");
		write("delete_pcb [NAME]\t:- deletes a pcb from the queue\n");
		write("block [NAME]\t\t:- puts a PCB into the blocked state\n");
		write("unblock [NAME]\t\t:- puts a PCB into the ready state\n");
		write("suspend [NAME]\t\t:- puts a PCB into the suspended state\n");
		write("resume [NAME]\t\t:- puts a PCB into the unsuspended state\n");
		write("set_priority [NAME] [PRIORITY]:- sets a PCB's priority\n");
		write("show_pcb [NAME]\t\t:- shows information about the PCB\n");
		write("show_ready\t\t:- shows all the PCBs in the ready queue\n");
		write("show_blocked\t\t:- shows all the PCBs in the blocked queue\n");
		write("show_all\t\t:- shows all the PCBs in all the queues\n");
      }
      break;
    case HELP:
      write("'help': Displays the list of commands for the system.\n");
      break;
  }
  
}

void version(int op){
 
  if(tooManyArgs()) return;
  
  switch(op) {
    case EXEC:
	  
      write("Current version: 2.0\n");
      break;
    case HELP:
	 
      write("'version': Displays the current shell version\n");
      break;
  }
  
}

void getDate(int op){
  
  //declare variables
  char buffer[50];
  
  if(tooManyArgs()) return;
 
  switch(op) {
    case EXEC:
      sys_get_date(MainShell->currentDate);
      sprintf(buffer,
              "%s %d, %d\n",
              getMonth(MainShell->currentDate->month),
              MainShell->currentDate->day,
              MainShell->currentDate->year);
      write(buffer);
      break;
    case HELP:
      write("'date': Displays the current system date.\n");
      break;
  }
  
}

void setDate(int op){
  
  //declare variables
  char* date;
  int values[3], i=0;
 
  switch(op) {
    case EXEC:
      if((date = strtok(NULL, " ,-|")) != NULL) {
	    
	   if(tooManyArgs()) return;
	  
        for(date = strtok(date, "/"); date && i < 3; date = strtok(NULL, "/")) {
          values[i] = atoi(date);
          i++;
        }
        if(isDateValid(values[0], values[1], values[2])) {
          MainShell->currentDate->month = values[0];
          MainShell->currentDate->day = values[1];
          MainShell->currentDate->year = values[2];
          sys_set_date(MainShell->currentDate);
        } else write("Date is invalid!\n");
      } else write("Incorrect command syntax: Date expected.\n");
      break;
    case HELP:
	  if(tooManyArgs()) return;
      write("'setdate [mm/dd/yyyy]': Sets the date using the specified format.\n");
      break;
  }
  
}

void list(int op){
  
  //declare variables
  char* path;
  char nameBuff[16], buffer[50];
  int buffSize = 16;
  long fileSize;
  
  switch(op) {
    case EXEC:
      //open current directory 
      if((path = strtok(NULL, " ,-|")) != NULL) sys_open_dir(path);
      else sys_open_dir("\0");
	 
	if(tooManyArgs()) return;
	 
      while(sys_get_entry(nameBuff, buffSize, &fileSize) != ERR_SUP_NOENTR){
        sprintf(buffer, "%s.mpx\t%d\n", nameBuff, fileSize);
        write(buffer);
      }
      sys_close_dir();
      break;
    case HELP:
		if(tooManyArgs()) return;
      write("'ls': Lists the current working directory.\n");
      break;
  }
  
}


void create_pcb(int op){
	
	//declare variables;
	 PCB * newPCB;
	char * name;
	char * classString;
	char * priorityString;
	int class = -1;
	int priority;
	
	switch(op){
		case EXEC:
			if((name = strtok(NULL, " ,-|")) != NULL){
				if((priorityString = strtok(NULL, " ,|")) != NULL){
					priority = atoi(priorityString);
					if(priority < -128 || priority > 127){
						write("Priority must be an integer between -128 and +127\n");
						break;
					}
					if((classString = strtok(NULL, " ,-|")) != NULL){
					
					    if(tooManyArgs()) return;
						if(strcmpi(classString, "APP") == 0 || strcmpi(classString, "APPLICATION") == 0){ 
							class = APP;
						}
						if(strcmpi(classString, "SYS") == 0 || strcmpi(classString, "SYSTEM") == 0){
							class = SYS;
						}
						if(class == -1){
						write("The third argument must either by APP OR SYS\n");
						break;
						}
					} else {
					write("Incorrect command syntax: Try CREATE_PCB [NAME] [PRIORITY] [CLASS]\n");
					break;
					}
				} else {
					write("Incorrect command syntax: Try CREATE_PCB [NAME] [PRIORITY] [CLASS]\n");
					break;
				}
			}else {
				write("Incorrect command syntax: Try CREATE_PCB [NAME] [PRIORITY] [CLASS]\n");
				break;
			}	

			createPCB(name, priority, class);
			break;
		case HELP:
			if(tooManyArgs()) return;
			 write("'Create_pcb [name] [priority] [class]': Creates a process and puts it in the ready, suspended state.\n");
			 write("First argument - process name: must be unique and less than 15 characters.\n");
			 write("Second argument - priority: must be an integer between -128 to 127\n");
			 write("Third argument - class: either application or system\n");
			 write("Class can be abbreviated by APP or SYS\n");
			 break;
	}
	
}
void delete_pcb(int op){
	char * name;
	
	switch(op) {
		case EXEC:
			
			if((name = strtok(NULL, " ,-|")) != NULL) {
				if(tooManyArgs()) return;
			deletePCB(name);
			} else write("Incorrect command syntax: Try delete_pcb [NAME]\n");
			 
			 break;
		case HELP:
		if(tooManyArgs()) return;
		  write("'Delete_pcb [name]': Deletes the process by the name given\n");
		  break;
	}
}

void block(int op){
	char * name;
	switch(op) {
		case EXEC:
			
			 if((name = strtok(NULL, " ,-|")) != NULL) {
				if(tooManyArgs()) return;
				blockPCB(name);
			} else write("Incorrect command syntax: Try block [NAME]\n");
			 
			 break;
		case HELP:
		if(tooManyArgs()) return;
		  write("'block [name]': Moves the process to the blocked state\n");
		  break;
	}
}
void unblock(int op){
	char * name;
	char * arg;
	switch(op) {
		case EXEC:
			
			 if((name = strtok(NULL, " ,-|")) != NULL) {
				if(tooManyArgs()) return;
				unblockPCB(name);
			} else write("Incorrect command syntax: Try unblock [NAME]\n");
			 
			 break;
		case HELP:
			if(tooManyArgs()) return;
		  write("'unblock [name]': Moves the process to the unblocked state\n");
		  break;
	}
}

void suspend(int op){
	char * name;
	switch(op) {
		case EXEC:
			
			 if((name = strtok(NULL, " ,-|")) != NULL) {
				if(tooManyArgs()) return;
				suspendPCB(name);
			} else write("Incorrect command syntax: Try suspend [NAME]\n");
			 
			 break;
		case HELP:
		  if(tooManyArgs()) return;
		  write("'suspend [name]': Moves the process to the suspended state\n");
		  break;
	}
}

void resume(int op){
	char * name;

	switch(op) {
		case EXEC:
			
			 if((name = strtok(NULL, " ,-|")) != NULL) {
			 if(tooManyArgs()) return;
				resumePCB(name);
				
			} else write("Incorrect command syntax: Try 'resume [NAME]'\n");
			 
			 break;
		case HELP:
		if(tooManyArgs()) return;
		  write("'resume [name]': Moves the process to the not suspended state\n");
		  break;
	}
}

void set_priority(int op){
	char * name;
	char * stringPriority;
	int newPriority;
	switch(op) {
		case EXEC:
			
			 if((name = strtok(NULL, " ,-|")) != NULL) {
				
				if((stringPriority = strtok(NULL, " ,-|")) != NULL) {
				if(tooManyArgs()) return;
					newPriority = atoi(stringPriority);
					if(newPriority < -128 || newPriority > 127){
						write("Invalid priority, must be an integer between -128 and +127\n");
						break;
					}else setPCBPriority(name, newPriority);
				} else write("Incorrect command syntax: Try 'resume [NAME] [PRIORITY]'\n");
			} else write("Incorrect command syntax: Try 'resume [NAME] [PRIORITY]'\n");
			 
			 break;
		case HELP:
		if(tooManyArgs()) return;
		  write("'set_priority [name] [priority]': sets the priority of the process\n");
		  write("priority must be between -128 and +127\n");
		  break;
	}
}
void show_pcb(int op){
	char * name;

	switch(op) {
		case EXEC:
			
			 if((name = strtok(NULL, " ,-|")) != NULL) {
				if(tooManyArgs()) return;
				showPCB(name);
			} else write("Incorrect command syntax: Try 'show_pcb [NAME]'\n");
			 
			 break;
		case HELP:
		if(tooManyArgs()) return;
		  write("'show_pcb [name]': shows information about the process\n");
		  break;
	}
}

void show_ready(int op){
	if(tooManyArgs()) return;
	switch(op) {
		case EXEC:
			showReadyPCB(0);
				
			break;
		case HELP:
		  write("'show_ready': shows all the processes in the ready queue\n");
		  break;
	}

}

void show_blocked(int op){
	if(tooManyArgs()) return;
	switch(op) {
		case EXEC:
			showBlockedPCB(0);
			break;
		case HELP:
		  write("'show_blocked': shows all the processes in the blocked queue\n");
		  break;
	}
}
void show_all(int op){
	if(tooManyArgs()) return;
	switch(op){
		case EXEC:
			showAllPCB();
		break;
		case HELP:
			write("'show_all': shows all the processes in all the queue");
		break;

		}
}

void load(int op){
	if(tooManyArgs()) return;
	switch(op){
		case EXEC:
			load_procs();
			break;
		case HELP:
			write("NEED TO WRITE HELP HERE");
			break;
		}		

}

void dispatchComm(int op){
	if(tooManyArgs()) return;
	switch(op){
		case EXEC:
			dispatcher();
			break;
		case HELP:
			write("NEED TO WRITE HELP");
		break;

		}


}


void clear(){
	sys_req(CLEAR, TERMINAL, NULL, NULL);
}

int tooManyArgs(){
	
	if((strtok(NULL, " ,-|")) != NULL) {  //check for excess arguments 
		 write("Invalid command, to many arguments\n");
		 return 1;
	}
	return 0;
}
