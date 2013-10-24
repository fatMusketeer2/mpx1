
#define EXEC 		  0
#define HELP 		  1

//r1
int executeCommand(int op, char* command);
void quit(int op);
void help(int op);
void version(int op);
void getDate(int op);
void setDate(int op);
void list(int op);
void load(int op);
void dispatchComm(int op);
void clear();
//r2
void create_pcb(int op);
void delete_pcb(int op);
void block(int op);
void unblock(int op);
void suspend(int op);
void resume(int op);
void set_priority(int op);
void show_pcb(int op);
void show_ready(int op);
void show_blocked(int op);
void show_all(int op);
int tooManyArgs();