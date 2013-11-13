#define CLOSED 1
#define OPEN 2

#define WRITING 10
#define READING 11

#define OK 0

#define UNSET 0
#define SET 1

#define INVALID_EVENT_FLAG_POINTER -101
#define INVALID_BAUD_RATE_DIVISOR -102
#define PORT_ALREADY_OPEN -103
#define PORT_NOT_OPEN -201
#define PORT_NOT_OPEN_READ -301
#define INVALID_BUFFER_ADDRESS -302
#define INVALID_COUNT -303
#define DEVICE_IS_BUSY -304
#define PORT_NOT_OPEN_WRITE -401
#define INVALID_BUFFER_ADDRESS_WRITE -402
#define INVALID_COUNT_WRITE -403
#define DEVICE_IS_BUSY_WRITE -404

#define RING_BUFFER_SIZE 100

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
#define EOI 0x20


int com_open(int *eflag_p, int baud);
void interrupt(*oldfunc)(void);
int com_close(void);
void interrupt handler(void);
void input_int(void);
void output_int(void);
int com_read(char *buf_p, int *count_p);
int com_write(char *buf_p, int *count_p);



typedef struct DeviceControlBlock {
        int flag;
        int *event_flag; 
        int status;
        char * in_buff;
        int * in_count;
        int in_done;
        char * out_buff;
        int * out_count;
        int out_done;
        char ring_buffer[RING_BUFFER_SIZE];
        int ring_buffer_in;
        int ring_buffer_out;
        int ring_buffer_count;
} DeviceControlBlock;


