#include <dos.h>
#include <stdio.h>
#include "mpx_supt.h"
#include "r5.h"


DeviceControlBlock * DCB;

int com_write(char *buff, int *count){
	int mask;

	if(DCB->flag != OPEN) return PORT_CLOSED_WRITE;
	else if(DCB->status != IDLE) return DEVICE_BUSY_WRITE;
	else if(buff == NULL) return INVALID_BUFFER_ADDRESS_WRITE;
	else if(count == NULL) return INVALID_COUNT_WRITE;
	
	//set DCB fields.
	DCB->out_buff = buff;
	DCB->out_count = count;
	DCB->out_done = 0;
	DCB->status = WRITING;
	*(DCB->event_flag) = UNSET;
	
	
	outportb(COM1_BASE, *DCB->out_buff);
	DCB->out_buff++;
	DCB->out_done++;
	
	//mask
	disable();
	mask = inportb(COM1_INT_EN);
	mask |= 0x02;
	outportb(COM1_INT_EN, mask);
	enable();
	
	return 0;
}

int com_read(char *buff, int *count){
	if(DCB->flag != OPEN) return PORT_CLOSED_READ;
	else if(DCB->status != IDLE) return DEVICE_BUSY_READ;
	else if(buff == NULL) return INVALID_BUFFER_ADDRESS_READ;
	else if(count == NULL) return INVALID_COUNT_READ;

	//set DCB fields
	DCB->in_buff = buff;
	DCB->in_count = count;
	DCB->in_done = 0;
	*(DCB->event_flag) = 0;
	DCB->status = READING;
	
	disable();
	while(DCB->in_done < *DCB->in_count && DCB->ring_buffer_count > 0 && DCB->ring_buffer[DCB->ring_buffer_out] != '\r'){
		DCB->in_buff[DCB->in_done] = DCB->ring_buffer[DCB->ring_buffer_out];
		DCB->in_done++;
		DCB->ring_buffer_out++;
		if(DCB->ring_buffer_out >= RING_BUFFER_SIZE){
			DCB->ring_buffer_out = 0;
		}
		DCB->ring_buffer_count--;
	}
	enable();
	
	
	if(DCB->in_done < *DCB->in_count){
		return 0;
	}

	DCB->in_buff[DCB->in_done] = '\0';
	DCB->status = IDLE;
	*(DCB->event_flag) = SET;
	*(DCB->in_count) = DCB->in_done;
	return DCB->in_done;
	
}

int com_open(int *eflag_p, int baud_rate) {
        int baud_rate_div;
        int mask;

        if(eflag_p == NULL) {
                return INVALID_E_FLAG;
        }


        if(baud_rate <= 0) {
                return INVALID_BRD;
        }


        if(DCB->flag == OPEN) {
                return ALREADY_OPEN;
        }

		//initialize all the DCB fields
        DCB->flag = OPEN;
        DCB->event_flag = eflag_p;
        DCB->status = IDLE;
        DCB->ring_buffer_in = 0;
        DCB->ring_buffer_out = 0;
        DCB->ring_buffer_count = 0;

		
        oldfunc = getvect(COM1_INT_ID);
        setvect(COM1_INT_ID, &handler);
        baud_rate_div = 115200 / (long) baud_rate;


        //change BRD reg
        outportb(COM1_LC, 0x80);
		//set the baud_rate_div
        outportb(COM1_BRD_LSB, baud_rate_div & 0xFF);
        outportb(COM1_BRD_MSB, (baud_rate_div >> 8) & 0xFF);
        //cant change BRD
        outportb(COM1_LC, 0x03);


        //Enable PIC Mask
        disable();
        mask = inportb(PIC_MASK);
        mask &= ~0x10;
        outportb(PIC_MASK, mask);
        enable();


        //Enable COM1 interrupts
        outportb(COM1_MC, 0x08);
        //Set Data Available Interrupt bit in the interrupt enable register
        outportb(COM1_INT_EN, 0x01);
        return 0;
}




int com_close(void) {
        int mask;


        if(DCB->flag != OPEN) {
                return PORT_CLOSED;
        }
		else{
			DCB->flag = CLOSED;

			// Set PIC mask level
			disable();
			mask = inportb(PIC_MASK);
			mask &= ~0x80;
			outportb(PIC_MASK, mask);
			enable();


			// disable serial interrupts
			outportb(COM1_MC, 0x00);
			outportb(COM1_INT_EN, 0x00);

			// Put original interrupt vector back
			setvect(COM1_INT_ID, oldfunc);

		}
        return 0;
}


void interrupt handler(void){
		
        int int_id;
        
         //determine if port is open
		 if(DCB->flag != OPEN) outportb(PIC_CMD, 0x20);
		else{

			 //Read Interrupt ID
			 int_id=inportb(COM1_INT_ID_REG);
			 int_id &= 0x07;	

			 //calling appropiate second-level handler
			 if(int_id == 2) output_int();
			 else if(int_id == 4) input_int();
		  
			 //clear interrupt
			 outportb(PIC_CMD,0x20);
		}
}//end new_handler


void input_int(void){

        unsigned char tempInput;

        //Read character from input register
         tempInput=inportb(COM1_BASE);


        //If current status is not Reading store characther in ring buffer
         if(DCB->status!=READING){
			if(DCB->ring_buffer_count >= RING_BUFFER_SIZE) return;
			else{
                DCB->ring_buffer[DCB->ring_buffer_in]=tempInput;
                DCB->ring_buffer_in++;
				DCB->ring_buffer_count++;
				
				if(DCB->ring_buffer_in >= RING_BUFFER_SIZE){
					DCB->ring_buffer_in = 0;
				}
			}

         }
         //otherwise store the character in the requestor's input buffer
         else{
               	DCB->in_buff[DCB->in_done]=tempInput;
                DCB->in_done++;
         //if the count is not completed and the character is not CR return
         //no completion


         //otherwise the transfer is complete
		if(tempInput == '\r' || DCB->in_done >= *DCB->in_count) {
			DCB->in_buff[DCB->in_done -1] = '\0';
			*(DCB->event_flag)=SET;
            DCB->status=IDLE;
		    *(DCB->in_count) = DCB->in_done;
		}


         }
}//end input_int()


void output_int(void){
        int temp_input;

        //if the current status is not WRITE ignore
        if(DCB->status!=WRITING);

        //otherwise check if the count has not been exhausted
		else if(DCB->out_done < *(DCB->out_count)) {
			
			outportb(COM1_BASE, *(DCB->out_buff));
			DCB->out_buff++;
			DCB->out_done++;
			
		}
        else {  //otherwise transfer complete
            DCB->status=IDLE;
            *(DCB->event_flag)=SET;
		    *(DCB->out_count) = DCB->out_done;
		    disable();
            temp_input=inportb(COM1_INT_EN);
            temp_input&= ~0x02;
            outportb(COM1_INT_EN,temp_input);
			enable();
        }
}//end output_interrupt

