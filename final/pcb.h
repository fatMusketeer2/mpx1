
 

#define RUNNING 1
#define READY 2
#define BLOCKED 3
#define SUSP_READY 4
#define SUSP_BLOCKED 5
#define SYS 1
#define APP 0
#define SYS_STACK_SIZE 1024




typedef struct PCB {
        char name[15];
        int class;
        int state;
        int priority;
        int memorySize;
		unsigned char *loadAddress;
		unsigned char *execAddress;
        unsigned char *stackBase;
        unsigned char *stackTop;
        struct PCB *prev;
        struct PCB *next;
       
} PCB;

typedef struct queue{
        int count;
        PCB *head;
        PCB *tail;
} queue;

extern queue *readyQ;
extern queue *blockedQ;
extern queue *suspReadyQ;
extern queue *suspBlockedQ;



void initR2();
void cleanR2();

PCB* allocatePCB(int);
int freePCB(struct PCB*);
PCB* setupPCB(char * name, int pri, int class);
PCB* findPCB(char * name);
void insertPCB(struct PCB*);
void removePCB(struct PCB*);

//commands
void createPCB(char * name, int pri, int class);
void deletePCB(char * name);
void blockPCB(char * name);
void unblockPCB(char * name);
void suspendPCB(char * name);
void resumePCB(char * name);
void setPCBPriority(char* name, int pri);
void showPCB(char * name);
int showReadyPCB(int showAll);
int showBlockedPCB(int showAll);
void showAllPCB();
PCB* getNextReady();
void emptyQueues();