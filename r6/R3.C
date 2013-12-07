#include <dos.h>
#include <stdlib.h>
#include "main.h"
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
iod *tempIOD;

iocb* terminal_iocb;
iocb* com_iocb;

// ***************************************************************************
// Dispatcher
// Parameters: none
// Returns: nothing
// Functions called: getNextReady, removePCB
// Prepares the first process of the ready queue for execution by switching to
// 	its stack. 


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

// ****************************************************************************
// sys_call
// Parameters: none
// Returns: nothing
// Functions called: UnblockPCB, trm_read, trm_write, trm_clear, free, com_read,
// 	com_write, insertPCB, ioScheduler, dispatcher.

void interrupt sys_call()
{
		ss_save = _SS;
        sp_save = _SP;
        param_ptr = (params*)((unsigned char *)MK_FP(ss_save, sp_save) + sizeof(context));
        new_ss = FP_SEG(&sys_stack);
        new_sp = FP_OFF(&sys_stack) + SYS_STACK_SIZE;
        _SS = new_ss;
        _SP = new_sp;
        
		trm_getc();
        if(terminal_iocb->event_flag){
				
                terminal_iocb->event_flag=0;
                tempIOD=terminal_iocb->head;
                terminal_iocb->head=terminal_iocb->head->next;
                //free(tempIOD);
                terminal_iocb->count--;
			
                unblockPCB(tempIOD->name);
				
                 //process request
                 if(tempIOD->reqType==READ)
                        trm_read(tempIOD->buffer,terminal_iocb->head->count);
                 else if(tempIOD->reqType==WRITE)
                        trm_write(tempIOD->buffer,terminal_iocb->head->count);
                 else if(tempIOD->reqType==CLEAR)
                        trm_clear();
			free(tempIOD);
        }
        if(com_iocb->event_flag){
                com_iocb->event_flag=0;
                tempIOD=com_iocb->head;
                com_iocb->head=com_iocb->head->next;
                free(tempIOD);
                com_iocb->count--;

                unblockPCB(tempIOD->name);

                 //process request

                 if(com_iocb->head->reqType==READ)
                        com_read(com_iocb->head->buffer,com_iocb->head->count);
                 else if(terminal_iocb->head->reqType==WRITE)
                        com_write(terminal_iocb->head->buffer,terminal_iocb->head->count);

        }
		
		
         switch(param_ptr->op_code){
             case IDLE:
				
				cop->state = READY;
				cop->stackTop = (unsigned char *)MK_FP(ss_save,sp_save);
				insertPCB(cop);
                break;
			case READ:
			case WRITE:
			case CLEAR:
				ioScheduler();
			break;
            case EXIT:
				freePCB(cop);
				cop = NULL;
                break;
        }
        _SS = ss_save;
        _SP = sp_save;
		
        dispatcher();
}

//******************************************************************************
//Parameter: none 
//Returns: nothing 
//Functions called: sys_set_vec
//sets up temporary register variables for stack segment and stack pointer.
//Sets up control structures for I/O devices 

void initR3() {

	sys_set_vec(sys_call);

	ss_save = NULL;
	sp_save = NULL;
	new_ss = NULL;
	new_sp = NULL;
		ss = NULL;
		sp = NULL;
	cop = NULL;

	terminal_iocb->event_flag=0;
	terminal_iocb->count=0;
	terminal_iocb->head= NULL;
	terminal_iocb->tail= NULL;

	com_iocb->event_flag=0;
	com_iocb->count=0;
	com_iocb->head= NULL;
	com_iocb->tail= NULL;



}

//******************************************************************************
// loadProgram
// Parameters: file- name of MPX file to be loaded
//	      priority- integer from 127 to -128 
// Functions called: sys_check_program, write, setupPCB, sys_load_program
//	insertPCB
// Allocates program memory and loads program into that memory

void loadProgram(char* file, int priority) {

	int prog_len_p;
	int start_offset_p;
	int check_ret;
	int sys_load_ret;
	PCB *newPCB;
	context *con;

	check_ret = sys_check_program("\0", file, &prog_len_p, &start_offset_p);
	if(check_ret == ERR_SUP_FILNFD) {
			write("File not found in the current directory\n");
	    return;
	}else if(check_ret == ERR_SUP_FILINV){
			write("File is not valid\n");
			return;
		}
	if(strcmp(file, "IDLE")==0){
		newPCB = setupPCB(file, priority, SYS);
		newPCB->state=READY;
		}
		else newPCB = setupPCB(file,priority, APP);
		if(newPCB != NULL){
			newPCB->memorySize = prog_len_p;
			newPCB->loadAddress = (unsigned char *)sys_alloc_mem(prog_len_p);
			newPCB->execAddress = (start_offset_p + newPCB->loadAddress);
			con = (context*) newPCB->stackTop;
			con->IP = FP_OFF(newPCB->execAddress);
			con->CS = FP_SEG(newPCB->execAddress);
			con->DS = _DS;
			con->ES = _ES;
			con->FLAGS = 0x200;
			sys_load_ret = sys_load_program(newPCB->loadAddress, newPCB->memorySize, "\0", file);
		}else{
			return;
		}

	if(sys_load_ret != 0) {
		write("SYS_LOAD Error try again\n");
				free(newPCB);
	} else {
		insertPCB(newPCB);
		printf("First HERE!!\n");
		write("Process is loaded in the suspend-ready queue\n");
		printf("AND here\n");
		}

}

//******************************************************************************
// ioScheduler
// Parameters: none
// Returns: nothing
// Functions called: sys_alloc_mem, strcpy, trm_read, trm_write, trm_clear,
// 	trm_gotoxy, blockPCB, com_read, com_write
//Opens terminal and com_port devices

void ioScheduler(){
	
	       static int result;
		
 if(param_ptr->device_id==TERMINAL){
        iod *newIOD;
        newIOD = (iod *)sys_alloc_mem(sizeof(struct iod));
        newIOD->reqPCB=cop;
        strcpy(newIOD->name,cop->name);
        newIOD->buffer=param_ptr->buf_addr;
        newIOD->count=param_ptr->count_ddr;
        newIOD->reqType=param_ptr->op_code;

        //if waiting queue for device is empty
        if(terminal_iocb->count==0){
                 terminal_iocb->head=newIOD;
                 terminal_iocb->tail=newIOD;
                 terminal_iocb->count++;

                 //process request

                 if(terminal_iocb->head->reqType==READ)
                        result=trm_read(terminal_iocb->head->buffer,terminal_iocb->head->count);
                 else if(terminal_iocb->head->reqType==WRITE)
                        result=trm_write(terminal_iocb->head->buffer,terminal_iocb->head->count);
                 else if(terminal_iocb->head->reqType==CLEAR)
                        result=trm_clear();
                 if(result!=0)
                        printf("ERROR!!");
                 else
                        blockPCB(terminal_iocb->head->name);

                }
        //if waiting queue for device is not empty
        else{
                terminal_iocb->tail->next=newIOD;
                terminal_iocb->tail=newIOD;
                terminal_iocb->count++;

                }
        }

 else if(param_ptr->device_id==COM_PORT){
        iod *newIOD;
        newIOD = (iod *)sys_alloc_mem(sizeof(struct iod));
        newIOD->reqPCB=cop;
        strcpy(newIOD->name,cop->name);
        newIOD->buffer=param_ptr->buf_addr;
        newIOD->count=param_ptr->count_ddr;
        newIOD->reqType=param_ptr->op_code;

        //if waiting queue for device is empty
        if(com_iocb->count==0){
                 com_iocb->head=newIOD;
                 com_iocb->tail=newIOD;
                 com_iocb->count++;

                 //process request

                 if(com_iocb->head->reqType==READ)
                        result=com_read(com_iocb->head->buffer,com_iocb->head->count);
                 else if(terminal_iocb->head->reqType==WRITE)
                        result=com_write(terminal_iocb->head->buffer,terminal_iocb->head->count);

                 if(result!=0)
                        printf("ERROR!!");
                 else
                        blockPCB(com_iocb->head->name);

                }
        //if waiting queue for device is not empty
        else{
                com_iocb->tail=newIOD;
                com_iocb->count++;

                }
        }


}//end ioScheduler

//******************************************************************************
// openDeviceDrivers
// Parameters: none
// Returns: nothing
// Functions called: sys_alloc_mem, sizeof, trm_open, com_open
// Opens terminal and com_port devices

void openDeviceDrivers(){

 terminal_iocb = (iocb *) sys_alloc_mem(sizeof(iocb));
 com_iocb = (iocb *) sys_alloc_mem(sizeof(iocb));
 trm_open(&terminal_iocb->event_flag);
 com_open(&com_iocb->event_flag,1200);
}


//******************************************************************************
// closeDeviceDrivers
// Parameters: none
// Returns: nothing
// Functions called: trm_close, com_close
// closes terminal and com_port devices

void closeDeviceDrivers(){
	trm_close();
	com_close();

}