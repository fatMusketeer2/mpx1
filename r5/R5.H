#define CLOSED 1
#define OPEN 2
#define WRITING 3
#define READING 4
#define UNSET 0
#define SET 1

//OPEN ERRORS
#define INVALID_E_FLAG -101
#define INVALID_BRD -102
#define ALREADY_OPEN -103
//CLOSE ERRORS
#define PORT_CLOSED -201

//READ ERRORS
#define PORT_CLOSED_READ -301
#define INVALID_BUFFER_ADDRESS_READ -302
#define INVALID_COUNT_READ -303
#define DEVICE_BUSY_READ -304
//WRITE ERRORS
#define PORT_CLOSED_WRITE -401
#define INVALID_BUFFER_ADDRESS_WRITE -402
#define INVALID_COUNT_WRITE -403
#define DEVICE_BUSY_WRITE -404

#define RING_BUFFER_SIZE 150


#define COM1_INT_ID 0x0C
#define COM1_BASE 0x3F8
#define COM1_INT_EN COM1_BASE+1
#define COM1_BRD_LSB COM1_BASE
#define COM1_BRD_MSB COM1_BASE+1
#define COM1_INT_ID_REG COM1_BASE+2
#define COM1_LC COM1_BASE+3
#define COM1_MC COM1_BASE+4
#define COM1_LS COM1_BASE+5
#define COM1_MS COM1_BASE+6
#define PIC_MASK 0x21
#define PIC_CMD 0x20


void interrupt(*oldfunc)(void);
int com_write(char *buf_p, int *count_p);
int com_read(char *buf_p, int *count_p);
int com_open(int *eflag_p, int baud);
int com_close(void);
void interrupt handler(void);
void input_int(void);
void output_int(void);



typedef struct DeviceControlBlock {
        int flag; //OPENED, or CLOSED
        int *event_flag; //SET, or UNSET
        int status; // IDLE, READING,or WRITING
        char * in_buff;
        int * in_count;
        int in_done;
        char * out_buff;
        int * out_count;
        int out_done;
        char ring_buffer[RING_BUFFER_SIZE]; //SIZE = 150
        int ring_buffer_in;
        int ring_buffer_out;
        int ring_buffer_count;
} DeviceControlBlock;


