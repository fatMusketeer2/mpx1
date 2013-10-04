#include <mpx_supt.h>
#include <r1.h>

int executeCommand(int op, char* command){
  
	if(strcmpi(command, "exit") == 0) quit(op);
	else if(strcmpi(command, "help") == 0) help(op);
  else if(strcmpi(command, "version") == 0) version(op);
	else if(strcmpi(command, "date") == 0) getDate(op);
	else if(strcmpi(command, "setdate") == 0) setDate(op);
	else if(strcmpi(command, "ls") == 0) list(op);
  //else if(strcmpi(command, "clear") == 0) clear();
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
  
  switch(op) {
    case EXEC:
      if((command = strtok(NULL, " ,-|")) != NULL) executeCommand(HELP, command);
      else {
        write("Type 'help' to see this list.\n");
        write("'help <command>' displays the help menu for the command.\n\n");
        write("date\t\t:- displays the system's current date.\n");
        write("setdate [MM/DD/YYYY]\t\t:- allows user to set the system's date.\n");
        write("ls <path>\t\t:- lists directories files and folders.\n");
        write("version\t\t:- lists current fmOS version.\n");
        write("exit\t\t:- exits the fmOS shell.\n");
      }
      break;
    case HELP:
      write("'help': Displays the list of commands for the system.\n");
      break;
  }
  
}

void version(int op){
  
  switch(op) {
    case EXEC:
      write("Current version: 1.0\n");
      break;
    case HELP:
      write("'version': Displays the current shell version\n");
      break;
  }
  
}

void getDate(int op){
  
  //declare variables
  char buffer[50];
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
      else sys_open_dir(MainShell->currentPath);
      while(sys_get_entry(nameBuff, buffSize, &fileSize) != ERR_SUP_NOENTR){
        sprintf(buffer, "%s.mpx\t%d\n", nameBuff, fileSize);
        write(buffer);
        break;
      }
      sys_close_dir();
      break;
    case HELP:
      write("'ls': Lists the current working directory.\n");
      break;
  }
  
}