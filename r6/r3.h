
#define R3_H
//#define IDLE 1
typedef struct context{
        unsigned int BP, DI, SI, DS, ES;
        unsigned int DX, CX, BX, AX;
        unsigned int IP, CS, FLAGS;
} context;

typedef struct params {
        int op_code;
        int device_id;
        unsigned char *buf_addr;
        int *count_ddr;
} params;

void interrupt sys_call();
void interrupt dispatcher();

//r3 proto types
void load_procs();
void test1_R3();
void test2_R3();
void test3_R3();
void test4_R3();
void test5_R3();
void loadUpQueue(char[],int,int);
void initR3();

//r4 
void loadProgram(char *, int);


