#include <dos.h>
#include <stdio.h>
#include "mpx_supt.h"
#include "r5.h"


DeviceControlBlock DCB;

int com_write(char *buff, int *count){
	int code;
	
	//check params
	if(DCB.flag != OPEN){
		return PORT_NOT_OPEN_WRITE;
	}else if(DCB.status != IDLE){
		return DEVICE_IS_BUSY_WRITE;
	}else if(buff == NULL){
		return INVALID_BUFFER_ADDRESS_WRITE;
	}else if(count == NULL){
		return INVALID_COUNT_WRITE;
	}
	
	DCB.out_buff = buff;
	DCB.out_count = count;
	DCB.out_done = 0;
	DCB.status = WRITING;
	*(DCB.event_flag) = UNSET;
	
	outportb(COM1_BASE, *DCB.out_buff);
	DCB.out_buff++;
	DCB.out_done++;
	
	disable();
	code = inportb(COM1_INT_EN);
	code |= 0x02;
	outportb(COM1_INT_EN, code);
	enable();
	return OK;
}

int com_open(int *eflag_p, int baud) {
        int baud_divisor;
        int mask;

        if(eflag_p == NULL) {
                return INVALID_EVENT_FLAG_POINTER;
        }


        if(baud <= 0) {
                return INVALID_BAUD_RATE_DIVISOR;
        }


        if(DCB.flag == OPEN) {
                return PORT_ALREADY_OPEN;
        }


        DCB.flag = OPEN;
        DCB.event_flag = eflag_p;
        DCB.status = IDLE;
        DCB.ring_buffer_in = 0;
        DCB.ring_buffer_out = 0;
        DCB.ring_buffer_count = 0;


        oldfunc = getvect(COM1_INT_ID);
        setvect(COM1_INT_ID, &handler);
        baud_divisor = 115200 / (long) baud;


        //This will allow changing of BRD registers
        outportb(COM1_LC, 0x80);
        //Set the baud rate divisors
        outportb(COM1_BRD_LSB, baud_divisor & 0xFF);
        outportb(COM1_BRD_MSB, (baud_divisor >> 8) & 0xFF);
        //Back to normal
        outportb(COM1_LC, 0x03);


        //Enable PIC Mask register level
        disable();
        mask = inportb(PIC_MASK);
        mask &= 0x10;
        outportb(PIC_MASK, mask);
        enable();


        //Enable COM1 interrupts
        outportb(COM1_MC, 0x08);
        //Set Data Available Interrupt bit in the interrupt enable register
        outportb(COM1_INT_EN, 0x01);
        return OK;
}




int com_close(void) {
        int mask;


        if(DCB.flag != OPEN) {
                return PORT_NOT_OPEN;
        }
        DCB.flag = CLOSED;


        // Set PIC mask level
        disable();
        mask = inportb(PIC_MASK);
        mask &= 0x80;
        outportb(PIC_MASK, mask);
        enable();


        // This will disable serial interrupts
        outportb(COM1_MC, 0x00);
        outportb(COM1_INT_EN, 0x00);


        // Restore the original interrupt vector
        setvect(COM1_INT_ID, oldfunc);


        return OK;
}


void interrupt handler(void){
		
        int int_id;
		
         //determine if port is open
		 if(DCB.flag != OPEN){
			outportb(PIC_CMD, 0x20);
			return;
		 }

         //Read Interrupt ID
         int_id=inportb(COM1_INT_ID);
		 int_id &=0x07;	


         //calling appropiate second-level handler
         if(int_id == 2) input_int();
         else if(int_id == 4) output_int();


         //clear interrupt
         outportb(PIC_CMD,0x20);

}//end new_handler


void input_int(void){

        unsigned char tempInput;


        //Read character from input register
         tempInput=inportb(COM1_BASE);


        //If current status is not Reading store characther in ring buffer
         if(DCB.status!=READ){
                DCB.ring_buffer[DCB.ring_buffer_count]=tempInput;
                (DCB.ring_buffer_count)++;


         }


         //otherwise store the character in the requestor's input buffer
         else{
               // DCB.in_buff[DCB.in_count]=tempInput;
                (DCB.in_count)++;
         //if the count is not completed and the character is not CR return
         //no completion


         //otherwise the transfer is complete
                DCB.status=IDLE;
                *(DCB.event_flag)=1;


         }
}//end input_int()


void output_int(void){
        int temp_input;
		
        //if the current status is not WRITE ignore and return
        if(DCB.status!=WRITE){return;}


        //otherwise check if the count has not been exhausted
		if(DCB.out_done < *(DCB.out_count)) {
			outportb(COM1_BASE, *DCB.out_buff);
			DCB.out_buff++;
			DCB.out_done++;
			return;
		}

        //otherwise transfer complete
        else{
                DCB.status=IDLE;
                *(DCB.event_flag)=1;
				*(DCB.out_count) = DCB.out_done;
				disable();
                temp_input=inportb(COM1_INT_EN);
                temp_input=temp_input & 0x02;
                outportb(COM1_INT_EN,temp_input);
				enable();
        }
}//end output_interrupt

