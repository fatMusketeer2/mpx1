#include <dos.h>
#include <stdlib.h>
#include "pcb.h"
#include "r3.h"
#include "mpx_supt.h"
#include <string.h>

unsigned short ss_save;
unsigned short sp_save;
unsigned short new_ss;
unsigned short new_sp;
unsigned short ss;
unsigned short sp;
PCB *cop;
char sys_stack[SYS_STACK_SIZE];
params *param_ptr;


void interrupt dispatcher()
{

        if(ss == NULL) {
                ss = _SS;
                sp = _SP;
        }
        cop = getNextReady();
        if(cop == NULL) {
                _SS = ss;
                _SP = sp;
                ss_save = NULL;
                sp_save = NULL;
        }
        else {
                removePCB(cop);
                cop->state = RUNNING;
                new_ss = FP_SEG(cop->stackTop);
                new_sp = FP_OFF(cop->stackTop);
                _SS = new_ss;
                _SP = new_sp;
        }
}

void interrupt sys_call()
{
        ss_save = _SS;
        sp_save = _SP;
        param_ptr = (params*)((unsigned char *)MK_FP(ss_save, sp_save) + sizeof(context));

        new_ss = FP_SEG(&sys_stack);
        new_sp = FP_OFF(&sys_stack) + SYS_STACK_SIZE;
        _SS = new_ss;
        _SP = new_sp;
        
        if(param_ptr->op_code == IDLE) {
                //removePCB(cop);
                cop->state = READY;
                cop->stackTop = (unsigned char *)MK_FP(ss_save,sp_save);
                insertPCB(cop);
        }
        else {
                //removePCB(cop);
                freePCB(cop);
                cop = NULL;
        }
        _SS = ss_save;
        _SP = sp_save;
        dispatcher();
}


void load_procs()
{
        loadUpQueue("test1",0,APP);
        loadUpQueue("test2",0,APP);
        loadUpQueue("test3",0,APP);
        loadUpQueue("test4",0,APP);
        loadUpQueue("test5",0,APP);
}

void loadUpQueue(char* name,int pri, int class)
{
        PCB *thePCB;
        context *con;
        
        thePCB = setupPCB(name, pri, class);
        con = (context*) thePCB->stackTop;
        if(strcmp(name, "test1") == 0) {
                con->IP = FP_OFF(&test1_R3);
                con->CS = FP_SEG(&test1_R3);
        } else if(strcmp(name, "test2") == 0) {
                con->IP = FP_OFF(&test2_R3);
                con->CS = FP_SEG(&test2_R3);
        } else if(strcmp(name, "test3") == 0) {
                con->IP = FP_OFF(&test3_R3);
                con->CS = FP_SEG(&test3_R3);
        } else if(strcmp(name, "test4") == 0) {
                con->IP = FP_OFF(&test4_R3);
                con->CS = FP_SEG(&test4_R3);
        } else if(strcmp(name, "test5") == 0) {
                con->IP = FP_OFF(&test5_R3);
                con->CS = FP_SEG(&test5_R3);
        }

        con->FLAGS = 0x200;
        con->DS = _DS;
        con->ES = _ES;
        insertPCB(thePCB);
}

void initR3() {
       
        
        sys_set_vec(sys_call);

        ss_save = NULL;
        sp_save = NULL;
        new_ss = NULL;
        new_sp = NULL;
		ss = NULL;
		sp = NULL;
        cop = NULL;

}

