
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
void clear();
void pageinate();
//r2
void create_pcb(int op);
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

//r3-r4
void load(int op);
void loadProcs(int op);
void dispatchComm(int op);
void terminate(int op);

//r5
void com_openT(int op);
void com_closeT(int op); 
void com_readT(int op);
void com_writeT(int op);

//bonus
void change_prompt(int op);
