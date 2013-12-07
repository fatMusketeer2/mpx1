#include "main.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

queue *readyQ;
queue *blockedQ;
queue *suspReadyQ;
queue *suspBlockedQ;


void initR2(){

readyQ = (queue*) sys_alloc_mem(sizeof(struct queue));
blockedQ = (queue*) sys_alloc_mem(sizeof(struct queue));
suspReadyQ = (queue*) sys_alloc_mem(sizeof(struct queue));
suspBlockedQ = (queue*) sys_alloc_mem(sizeof(struct queue));

readyQ->count = 0;
readyQ->head = NULL;
readyQ->tail = NULL;

blockedQ-> count = 0;
blockedQ-> head = NULL;
blockedQ-> tail = NULL;

suspReadyQ-> count = 0;
suspReadyQ-> head = NULL;
suspReadyQ-> tail = NULL;

suspBlockedQ-> count = 0;
suspBlockedQ-> head = NULL;
suspBlockedQ-> tail = NULL;
}

/* allocates a PCB
* parameters: none
* returns: pointer to the PCB if successful, or NULL if error.
*/
PCB * allocatePCB(int isComhand){
	PCB *newPCB = sys_alloc_mem(sizeof(PCB));
	if(isComhand){
		newPCB->stackBase = (unsigned char *) sys_alloc_mem(sizeof(unsigned char*)*(SYS_STACK_SIZE * 4));
		newPCB->stackTop =  newPCB->stackBase + (SYS_STACK_SIZE * 4)- sizeof(context);
	}else{
		newPCB->stackBase = (unsigned char *) sys_alloc_mem(sizeof(unsigned char*)*SYS_STACK_SIZE);
		newPCB->stackTop =  newPCB->stackBase + (SYS_STACK_SIZE)- sizeof(context);
	}
	return newPCB;
}

/* frees a PCB
* parameters: pointer to PCB being freed
* returns: 0 if successful, or ERRORCODE
*/
int freePCB( PCB * process){
	return sys_free_mem(process->stackBase) && sys_free_mem(process->loadAddress) && sys_free_mem(process);
}

/* sets up a PCB
* parameters: char * name, int class, int priority 
* returns: pointer to the PCB, or NULL if error.
*/
PCB* setupPCB(char * procName, int procPriority, int procClass){
	PCB *newPCB;

	if(findPCB(procName) != NULL){
		write("That process name already exists \n");
		return NULL;
	}else if(strlen(procName) > 15){
		write("Process Names must be 15 or less characters\n");
		return NULL;
	}

	if(strcmp(procName, "ComHandler") == 0){
		newPCB = allocatePCB(1);
		
		newPCB->memorySize = SYS_STACK_SIZE * 4;
	}else{
		newPCB = allocatePCB(0);
	}
	strcpy(newPCB->name, procName);
	newPCB->class = procClass;
	newPCB->priority = procPriority;
	newPCB->state = SUSP_READY;
	newPCB->next = NULL;
	newPCB->prev = NULL;
	newPCB->memorySize = SYS_STACK_SIZE;
	if(strcmp(newPCB->name, "ComHandler") == 0){
		newPCB->memorySize = SYS_STACK_SIZE * 4;
	}
	return newPCB;
}

//search all queues
PCB* findPCB(char * processName){
	 PCB * curr = readyQ->head;
	while(curr != NULL){
		if(strcmp(curr->name, processName)){
			curr = curr->next;
		}else{
			return curr;
		}
	}
	curr = blockedQ->head;
	while(curr != NULL){
		if(strcmp(curr->name, processName)){
			curr = curr->next;
		}else{
			return curr;
		}
	}
	curr = suspReadyQ->head;
	while(curr != NULL){
		if(strcmp(curr->name, processName)){
			curr = curr->next;
		}else{
			return curr;
		}
	}
	curr = suspBlockedQ->head;
	while(curr != NULL){
		if(strcmp(curr->name, processName)){
			curr = curr->next;
		}else{
			return curr;
		}
	}

	return NULL;
}


void insertPCB(PCB * process){
	 PCB *temp;
	 queue *q;
	 int i = 0;
	if(process->state == READY) q = readyQ;
	else if(process->state == BLOCKED) q = blockedQ;
	else if(process->state == SUSP_READY) q = suspReadyQ;
	else if(process->state == SUSP_BLOCKED) q = suspBlockedQ;

	temp = q->head;

	switch(process->state){
		case(READY):
		case(SUSP_READY): //priority queue
			if(temp == NULL){ //first element
				q->head = process;
				q->tail = process;
				process->next = NULL;
				process->prev = NULL;
			}else if(q->tail->priority >= process->priority){ //last element
				process->prev = q->tail;
				process->next = NULL;
				q->tail->next = process;
				q->tail = process;
			}else{ //any other element
				while(temp->priority >= process->priority){
					temp = temp->next;
					i++;
				}
				if(i == 0){ //process goes at head
					q->head = process;
				}
					process->next = temp;
					process->prev = temp->prev;
					temp->prev->next = process;
					temp->prev = process;

			}
			q->count++;
		break;
		case(BLOCKED): //FIFO
		case(SUSP_BLOCKED):
			if(temp == NULL){ //EMPTY QUEUE
				q->head = process;
				q->tail = process;
				process->next = NULL;
				process->prev = NULL;
			}else{ //ADD AT END
				process->prev = q->tail;
				process->next = NULL;
				q->tail->next = process;
				q->tail = process;

			}
		q->count++;
		break;
	}

}

void removePCB(PCB * process){
	 PCB *temp;
	 queue *q;

	if(process->state == READY) q = readyQ;
	else if(process->state == BLOCKED) q = blockedQ;
	else if(process->state == SUSP_READY) q = suspReadyQ;
	else if(process->state == SUSP_BLOCKED) q = suspBlockedQ;
	if(q->head == process || q->tail == process){ // if process is head or tail
		if(q->head == process){ // if process is head
			q->head = process->next;
			if (q->head != NULL){
				q->head->prev = NULL;
			}
		}
		if(q->tail == process) { // if process is tail
			q->tail = process->prev;
			if (q->tail != NULL){
				q->tail->next = NULL;
			}
		}			// if process is tail, set new tail;
	}else{ //if process is somewhere in the middle
		process->prev->next = process->next;
		process->next->prev = process->prev;
	}

	q->count--;

}




void createPCB(char * name, int pri, int class){
	 PCB * newPCB = setupPCB(name,pri,class);


	if(newPCB != NULL){
		insertPCB(newPCB);
		write("process inserted into the suspended ready queue\n");
	};
}
void deletePCB(char * name){
	PCB * thePCB;

	thePCB = findPCB(name);
	if(thePCB == NULL){
		write("There is no process by that name\n");
		return;
	}

	//if(thePCB->class == SYS){
	//	write("Unable to terminate SYSTEM processes\n");
	//	return;
       //	}
	removePCB(thePCB);
	freePCB(thePCB);
	write("The process was successfully deleted\n");

}

void blockPCB(char * name){
	PCB * thePCB;

	thePCB = findPCB(name);
	if(thePCB == NULL){
		write("There is no process by that name\n");
		return;
	}
	removePCB(thePCB);
	if(thePCB->state == READY) thePCB->state = BLOCKED;
	else if(thePCB->state == SUSP_READY) thePCB->state = SUSP_BLOCKED;
	insertPCB(thePCB);
	write("The process was successfully blocked\n");
}
void unblockPCB(char * name){
	PCB * thePCB;

	thePCB = findPCB(name);
	if(thePCB == NULL){
		write("There is no process by that name\n");
		return;
	}
	removePCB(thePCB);
	if(thePCB->state == BLOCKED) thePCB->state = READY;
	else if(thePCB->state == SUSP_BLOCKED) thePCB->state = SUSP_READY;
	insertPCB(thePCB);
	write("The process was successfully unblocked\n");
}

void suspendPCB(char * name){

	PCB * thePCB;

	thePCB = findPCB(name);
	if(thePCB == NULL){
		write("There is no process by that name\n");
		return;
	}

	if(thePCB->class == SYS){
		write("Unable to suspend SYSTEM processes\n");
		return;
	}
	removePCB(thePCB);
	if(thePCB->state == READY) thePCB->state = SUSP_READY;
	else if(thePCB->state == BLOCKED) thePCB->state = SUSP_BLOCKED;
	insertPCB(thePCB);
	write("The process was successfully suspended\n");


}

void resumePCB(char * name){
	PCB * thePCB;

	thePCB = findPCB(name);
	if(thePCB == NULL){
		write("There is no process by that name\n");
		return;
	}

	if(thePCB->class == SYS){
		write("Unable to resume SYSTEM processes\n");
		return;
	}
	removePCB(thePCB);
	if(thePCB->state == SUSP_READY) thePCB->state = READY;
	else if(thePCB->state == SUSP_BLOCKED) thePCB->state = BLOCKED;
	insertPCB(thePCB);
	write("The process was successfully resumed\n");

}

void setPCBPriority(char * name, int pri){
	PCB * thePCB;

	thePCB = findPCB(name);
	if(thePCB == NULL){
		write("There is no process by that name\n");
		return;
	}
	if(thePCB->class == SYS){
		write("Unable to set new priority of SYSTEM processes\n");
		return;
	}if(thePCB->class == SYS){
		write("Unable to suspend SYSTEM process\n");
		return;
	}

	thePCB->priority = pri;
	removePCB(thePCB);
	insertPCB(thePCB);
	write("The new priority was set\n");

}

void showPCB(char * name){
	 PCB * thePCB;
	char * string;


	thePCB = findPCB(name);
	if(thePCB == NULL){
	write("There is no process by that name\n");
	return;
	}

	write("\nNAME: ");
	write(name);

	write("\nclass: ");
	if(thePCB->class == APP) write("Application\n");
	else if(thePCB->class == SYS) write("System\n");

	write("STATE: ");
	if(thePCB->state == READY) write("Ready");
	else if(thePCB->state == BLOCKED) write("Blocked");
	else if(thePCB->state == SUSP_READY) write("Ready-suspended");
	else if(thePCB->state == SUSP_BLOCKED) write("Blocked-suspended");

	write("\nPRIORITY: ");
	sprintf(string, "%d\n", thePCB->priority);
	write(string);
}

int showReadyPCB(int showAll){
	int i = 1;
	 PCB * curr = readyQ->head;
	char buff[30];

	char * string;

			if(readyQ->count == 0 && suspReadyQ->count == 0){
				if(showAll == 0){
					write("\nREADY QUEUE IS EMPTY\n");
				}
				return 1;
			}

			write("\nNAME\tCLASS\tPRIORITY\tSTATE\n");
			while(curr != NULL){
				if(i == 18){
					pageinate();
					i = 1;
					write("NAME\t\tCLASS\tPRIORITY\tSTATE\n");
				}

				write(curr->name);
				if(curr->class == APP) write("\tApplication");
				else if(curr->class == SYS) write("\tSystem");

				sprintf(string, "  %d  ", curr->priority);
				write(string);

				write("Ready\n");

				curr = curr->next;
				i++;
			}

			curr = suspReadyQ->head;

			while(curr != NULL){
				if(i == 18){
					pageinate();
					i = 1;
					write("NAME\t\tCLASS\tPRIORITY\tSTATE\n");
				}

				write(curr->name);
				if(curr->class == APP) write("\tApplication");
				else if(curr->class == SYS) write("\tSystem");

				sprintf(string, "  %d  ", curr->priority);
				write(string);

				write("Ready-suspended\n");

				curr = curr->next;
				i++;
			}
			return i;
}

int showBlockedPCB(int showAll){
	int i = 1;
	PCB * curr = blockedQ->head;
	char buff[30];


	if(showAll == 1) i = readyQ->count +suspReadyQ->count;
	if(blockedQ->count == 0 && suspBlockedQ->count == 0){
		if(showAll == 0){
			write("\nBLOCKED QUEUE IS EMPTY\n");
		}

		return 1;
	}


	if(showAll == 0 && i == 0) write("\nNAME\t\CLASS\tPRIORITY\tSTATE\n");

	while(curr != NULL){
		if(i == 18){
			pageinate();
			i = 1;
			write("NAME\t\tCLASS\tPRIORITY\tSTATE\n");
		}

		write(curr->name);
		if(curr->class == APP) write("\tApplication");
		else if(curr->class == SYS) write("\tSystem");
		sprintf(buff, "  %d  ", curr->priority);
		write(buff);

		write("Blocked\n");

		curr = curr->next;
		i++;
	}

	curr = suspBlockedQ->head;

	while(curr != NULL){
		if(i == 18){
			pageinate();
			i = 1;
			write("NAME\t\tCLASS\tPRIORITY\tSTATE\n");
		}

		write(curr->name);
		if(curr->class == APP) write("\tApplication");
		else if(curr->class == SYS) write("\tSystem");

		sprintf(buff, "  %d  ", curr->priority);
		write(buff);

		write("Blocked-suspended\n");

		curr = curr->next;
		i++;
	}
	return i;
}
void showAllPCB(){
	int countReady;
	int countBlocked;
	countReady = showReadyPCB(1);
	countBlocked = showBlockedPCB(1);
	if(countReady == 1 && countBlocked == 1) write("\nAll queues are empty\n");

}

void cleanR2(){

	emptyQueues();

	sys_free_mem(readyQ);
	sys_free_mem(suspReadyQ);
	sys_free_mem(blockedQ);
	sys_free_mem(suspBlockedQ);
}

void emptyQueues(){
	 PCB * curr = readyQ->head;

	while(curr != NULL){
		removePCB(curr);
		freePCB(curr);
		curr = readyQ->head;
	}

	curr = suspReadyQ->head;

	while(curr != NULL){
		removePCB(curr);
		freePCB(curr);
		curr = suspReadyQ->head;
	}
	curr = blockedQ->head;

	while(curr != NULL){
		removePCB(curr);
		freePCB(curr);
		curr = blockedQ->head;
	}
	curr = suspBlockedQ->head;

	while(curr != NULL){
		removePCB(curr);
		freePCB(curr);
		curr = suspBlockedQ->head;
	}

}


PCB* getNextReady(){

	return readyQ->head;
}






