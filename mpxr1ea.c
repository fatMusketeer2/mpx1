#include "MPXR1.h" 	//included by me
#include "help.h"	//included by me
#include <string.h>	//included by me
#include "commhndl.h"  //included by e.a
#include <stdlib.h>
#include <dos.h>
#include <dir.h>
#include <errno.h>
#include <conio.h>
#include <alloc.h>

#define VEC_ADDR (0x60L*4)

#define MAX_ALLOC 200

#define MAX_XPOS 79
#define MAX_YPOS 24

#define MAX_PATH_SIZE 50

/*
	System-dependent type definitions
*/

typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long longword;
typedef byte *address;

typedef struct params {
	int      op_code;
	int      device_id;
	char     *buf_p;
	int      *count_p;
	} params;

/*
	Global variables accessed only by support routines
*/

	static longword vec_save;  /* saved interrupt vector */
	static int mod_code;                /* module code */
	static struct date sys_date;        /* MPX system date */

	/* data structures for directory access */
	static char current_path[MAX_PATH_SIZE+1];
	static int num_entries;
	static struct ffblk file_block;

	/* handler presence flags */
	static flag sysc_hand;
	static flag trm_hand;
	static flag prt_hand;
	static flag com_hand;

	/* memory allocation table */
	static struct {
		void* original;
		void* aligned;
	} alloc_table[MAX_ALLOC];
	static int alloc_ix;                /* current index */
	static int num_alloc;               /* no. of allocated blocks */


int main(void){

	sys_init(MODULE_R1);
	commhndl();
	//help("void");

	return(0);




}

int sys_init (    int      modules  /* module code */
	     )

{
	mod_code = modules;
	vec_save = 0L;


	/* no handlers detected yet */
	sysc_hand = FALSE;
	trm_hand = FALSE;
	prt_hand = FALSE;
	com_hand = FALSE;

	/* initialize allocation table */
	for (alloc_ix=0; alloc_ix < MAX_ALLOC; alloc_ix++) {
		alloc_table[alloc_ix].original = NULL;
		alloc_table[alloc_ix].aligned = NULL;
	}
	alloc_ix = 0;
	num_alloc = 0;

	/* if we have reached Module R3, enable system call handling */
	if (modules >= MODULE_R3) sysc_hand = TRUE;

	/* if we have reached the final module, enable device handlers */
	if (modules >= MODULE_F) {
		trm_hand = TRUE;
		prt_hand = TRUE;
		com_hand = TRUE;
	}

	/* get system date */
	getdate(&sys_date);

	return (OK);
}


void help(char *name){

	printf("Passed %s \n",name);

	if(!(strcmpi(name,"void\n"))){
		printf("Type 'help' to see this list.\n");
		printf("Type 'help name' to find out more of the command 'name'.\n");
		printf("%-25s %-10s\n","DATE","Display the current system date.");
		printf("%-25s %-10s\n","SETDATE","Set/overwrite the current system date.");
		printf("%-25s %-10s\n","DIR","Display all files in current directory.");
		printf("%-25s %-10s\n","EXIT","Terminate.");
		printf("%-25s %-10s\n","HELP","Display List of System Commands.");
	 }
	else if(!(strcmpi(name,"date\n"))){
		printf("Display the current system date.\n");
		printf("DATE\n");

	}

	else if(!(strcmpi(name,"setdate"))){
		printf("Set/overwrite the current system date\n");
		printf("SETDATE [MM-DD-YYYY]\n");

	}
	else if(!(strcmpi(name,"dir"))){
		printf("Display all files and subdirectories in a directory.");
		printf("DIR \n");
	}

	else if(!(strcmpi(name,"exit"))){
		printf("Terminate\n");
		printf("EXIT\n");
	}

	else if(!(strcmpi(name,"help"))){
		printf("Display a List of System Commands\n");
		printf("HELP\n");
	}

	else printf("Command %s was not found.\n",name);
}

/*
	Procedure: sys_req

	Purpose: Request a system service

	Inputs:

		op_code           operation code
		device_id         device identifier
		buf_p             addresso f data buffer
		count_p           address of size of buffer

	Returns: Result or error code, depending on service

	Services supported:	IDLE     null operation
				READ     input from a device
				WRITE    output to a device
				CLEAR    clear terminal screen
				GOTOXY   absolute cursor position
				EXIT     terminate the caller

	Calls:   fgets
		strlen

	Globals: none

	Errors:  ERR_SUP_INVDEV    invalid device
		ERR_SUP_INVOPC    invalid operation code
		ERR_SUP_INVPOS    invalid character position
		ERR_SUP_RDFAIL    read failed
		ERR_SUP_WRFAIL    write failed

	Description:

	For Modules R1 through R4,
	this procedure is used only for terminal I/O.
	Later modules support both reading and
	writing for several devices.

	For Modules R1 through R4,
	sys_req uses the ANSI C function "fgets."
	Later modules may use alternate functions and device drivers.

*/


int sys_req (   int      op_code, /* operation code */
		int      device_id,        /* device id */
		char*    buf_p,            /* I/O buffer */
		int*     count_p           /* address of count */
		)

{
	int      rval;    /* result or error code */
	char     *rp;     /* return pointer for fgets */
	char     rc;      /* return char for fputc */
	params    *param_p; /* pointer to parameter record in stack */
	flag     docall;  /* true if system call interrupt wanted */
	int      ix;      /* temporary index */



	docall = FALSE;
	rval = OK;
	switch (op_code) {

	case IDLE:
		if (sysc_hand) docall = TRUE;
		break;


	case READ:
		switch (device_id) {

		case TERMINAL:
			if (trm_hand) docall = TRUE;
			else {
				rp = fgets(buf_p,*count_p,stdin);
				if (rp==NULL) rval = ERR_SUP_RDFAIL;
				else rval = strlen(buf_p);
			}
			break;

		case COM_PORT:
			if (com_hand) docall = TRUE;
			else rval = ERR_SUP_INVDEV;
			break;

		default:
			rval = ERR_SUP_INVDEV;

		}
		break;

	case WRITE:
		switch (device_id) {

		case TERMINAL:
			if (trm_hand) docall = TRUE;
			else {
				rval = *count_p;
				for (ix=0; ix<*count_p; ix++) {
					rc = fputc(buf_p[ix],stdout);
					if (rc == EOF) {
						rval = ERR_SUP_WRFAIL;
						break;
					}
				}

			}
			break;

		case COM_PORT:
			if (com_hand) docall = TRUE;
			else rval = ERR_SUP_INVDEV;
			break;

		case PRINTER:
			if (prt_hand) docall = TRUE;
			else rval = ERR_SUP_INVDEV;
			break;

		default:
			rval = ERR_SUP_INVDEV;

		}
		break;


	case CLEAR:
		if (device_id==TERMINAL) {
			if (trm_hand) docall = TRUE;
			else {
			     clrscr();
				rval = 0;
			}
		}
		else rval = ERR_SUP_INVDEV;
		break;

	case GOTOXY:
		if (device_id==TERMINAL) {
			if (trm_hand) docall = TRUE;
			else {
				if (*count_p != 2) rval = ERR_SUP_WRFAIL;
				else if ((*buf_p<0)
					|| (*buf_p > MAX_XPOS))
					rval = ERR_SUP_WRFAIL;
				else if ((*(buf_p+1)<0)
					|| (*(buf_p+1) > MAX_YPOS))
					rval = ERR_SUP_WRFAIL;
				else {
					gotoxy(*buf_p + 1,
						*(buf_p+1) + 1);
					rval = 0;
				}
			}
		}
		else rval = ERR_SUP_INVDEV;
		break;

	/* EXIT - terminate the calling process */
	/* legal only when a system call handler is present */
	case EXIT:
		if (sysc_hand) docall = TRUE;
		else rval = ERR_SUP_INVOPC;
		break;

	default:
		rval = ERR_SUP_INVOPC;

	}

	/* Invoke user's call handler if present.

	   If the call is an IO request, return will occur
	   only after the request is complete.
	*/
	if (docall && (rval==OK)) {

		/* invoke the call handler */
	_SP = _SP - sizeof(params);
		param_p = (params*) MK_FP(_SS,_SP);
	       param_p->op_code = op_code;
	       param_p->device_id = device_id;
	       param_p->buf_p = buf_p;
	       param_p->count_p = count_p;
	       geninterrupt(0x60);
	       rval = _AX;
	       _SP = _SP + sizeof(params);

		/* for I/O operations, return count value */
		switch(op_code) {

		case READ:
		case WRITE:
		case CLEAR:
		case GOTOXY:

			rval = *count_p;

		}


	}

	return(rval);

}

/////////////////////////////////////////////////////////////////////

void commhndl(){

	params p1;
	char buff[100];
	int retValue,i;
	p1.op_code=WRITE;
	p1.device_id=TERMINAL;
	p1.buf_p=buff;
	*p1.count_p=2;

	buff[0]='?';
	buff[1]='~';

	retValue=sys_req(p1.op_code,p1.device_id,p1.buf_p,p1.count_p);
	p1.op_code=READ;
	*p1.count_p=100;

	retValue=sys_req(p1.op_code,p1.device_id,p1.buf_p,p1.count_p);
	help(buff);

	printf("Return Value : %d",retValue);

}
