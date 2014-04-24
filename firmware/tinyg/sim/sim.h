#ifndef SIM_SIM_H_ONCE
#define SIM_SIM_H_ONCE

typedef struct PORT_struct
{
    uint8_t DIR;  /* I/O Port Data Direction */
    uint8_t DIRSET;  /* I/O Port Data Direction Set */
    uint8_t DIRCLR;  /* I/O Port Data Direction Clear */
    uint8_t DIRTGL;  /* I/O Port Data Direction Toggle */
    uint8_t OUT;  /* I/O Port Output */
    uint8_t OUTSET;  /* I/O Port Output Set */
    uint8_t OUTCLR;  /* I/O Port Output Clear */
    uint8_t OUTTGL;  /* I/O Port Output Toggle */
    uint8_t IN;  /* I/O port Input */
    uint8_t INTCTRL;  /* Interrupt Control Register */
    uint8_t INT0MASK;  /* Port Interrupt 0 Mask */
    uint8_t INT1MASK;  /* Port Interrupt 1 Mask */
    uint8_t INTFLAGS;  /* Interrupt Flag Register */
    uint8_t reserved_0x0D;
    uint8_t reserved_0x0E;
    uint8_t reserved_0x0F;
    uint8_t PIN0CTRL;  /* Pin 0 Control Register */
    uint8_t PIN1CTRL;  /* Pin 1 Control Register */
    uint8_t PIN2CTRL;  /* Pin 2 Control Register */
    uint8_t PIN3CTRL;  /* Pin 3 Control Register */
    uint8_t PIN4CTRL;  /* Pin 4 Control Register */
    uint8_t PIN5CTRL;  /* Pin 5 Control Register */
    uint8_t PIN6CTRL;  /* Pin 6 Control Register */
    uint8_t PIN7CTRL;  /* Pin 7 Control Register */
} PORT_t;

#define NUL (char)0x00  //  ASCII NUL char (0) (not "NULL" which is a pointer)

typedef uint16_t flags_t;
typedef uint8_t buffer_t;

int xio_gets(const uint8_t dev, char *buf, const int size);
FILE *xio_open(const uint8_t dev, const char *addr, const flags_t flags);




#define DEL (char)0x7F

#define XIO_DEV_COUNT 2
#define RX_BUFFER_SIZE (buffer_t)255                 // buffer_t can be 8 bits
#define TX_BUFFER_SIZE (buffer_t)255                 // buffer_t can be 8 bits
#define XOFF_TX_LO_WATER_MARK (TX_BUFFER_SIZE * 0.05)        // % to issue XON

#define XIO_DEV_USB (0)
#define XIO_DEV_PGM (1)

#define PGMFILE (const char *)               // extends pgmspace.h

#define XIO_BLOCK		((uint16_t)1<<0)		// enable blocking reads
#define XIO_NOBLOCK		((uint16_t)1<<1)		// disable blocking reads
#define XIO_XOFF 		((uint16_t)1<<2)		// enable XON/OFF flow control
#define XIO_NOXOFF 		((uint16_t)1<<3)		// disable XON/XOFF flow control
#define XIO_ECHO		((uint16_t)1<<4)		// echo reads from device to stdio
#define XIO_NOECHO		((uint16_t)1<<5)		// disable echo
#define XIO_CRLF		((uint16_t)1<<6)		// convert <LF> to <CR><LF> on writes
#define XIO_NOCRLF		((uint16_t)1<<7)		// do not convert <LF> to <CR><LF> on writes
#define XIO_IGNORECR	((uint16_t)1<<8)		// ignore <CR> on reads
#define XIO_NOIGNORECR	((uint16_t)1<<9)		// don't ignore <CR> on reads
#define XIO_IGNORELF	((uint16_t)1<<10)		// ignore <LF> on reads
#define XIO_NOIGNORELF	((uint16_t)1<<11)		// don't ignore <LF> on reads
#define XIO_LINEMODE	((uint16_t)1<<12)		// special <CR><LF> read handling
#define XIO_NOLINEMODE	((uint16_t)1<<13)		// no special <CR><LF> read handling

#define XIO_BAUD_115200 (5)

#define PGM_FLAGS (XIO_BLOCK | XIO_CRLF | XIO_LINEMODE)

typedef struct xioDEVICE {						// common device struct (one per dev)
	// references and self references
	uint16_t magic_start;						// memory integrity check
	uint8_t dev;								// self referential device number
	FILE file;									// stdio FILE stream structure
	void *x;									// extended device struct binding (static)

	// function bindings
	FILE *(*x_open)(const uint8_t dev, const char *addr, const flags_t flags);
	int (*x_ctrl)(struct xioDEVICE *d, const flags_t flags);	 // set device control flags
	int (*x_gets)(struct xioDEVICE *d, char *buf, const int size);// non-blocking line reader
	int (*x_getc)(FILE *);						// read char (stdio compatible)
	int (*x_putc)(char, FILE *);				// write char (stdio compatible)
	void (*x_flow)(struct xioDEVICE *d);		// flow control callback function

	// device configuration flags
	uint8_t flag_block;
	uint8_t flag_echo;
	uint8_t flag_crlf;
	uint8_t flag_ignorecr;
	uint8_t flag_ignorelf;
	uint8_t flag_linemode;
	uint8_t flag_xoff;							// xon/xoff enabled

	// private working data and runtime flags
	int size;									// text buffer length (dynamic)
	uint8_t len;								// chars read so far (buf array index)
	uint8_t signal;								// signal value
	uint8_t flag_in_line;						// used as a state variable for line reads
	uint8_t flag_eol;							// end of line detected
	uint8_t flag_eof;							// end of file detected
	char *buf;									// text buffer binding (can be dynamic)
	uint16_t magic_end;
} xioDev_t;

xioDev_t            ds[XIO_DEV_COUNT];

typedef struct {
  uint32_t unused;
} USART_t;

typedef struct xioUSART {
	uint8_t fc_char_rx;			 			// RX-side flow control character to send
	volatile uint8_t fc_state_rx;			// flow control state on RX side
	volatile uint8_t fc_state_tx;			// flow control state on TX side

	volatile buffer_t rx_buf_tail;			// RX buffer read index
	volatile buffer_t rx_buf_head;			// RX buffer write index (written by ISR)
	volatile buffer_t rx_buf_count;			// RX buffer counter for flow control

	volatile buffer_t tx_buf_tail;			// TX buffer read index  (written by ISR)
	volatile buffer_t tx_buf_head;			// TX buffer write index
	volatile buffer_t tx_buf_count;

	USART_t *usart;							// xmega USART structure
	PORT_t	*port;							// corresponding port

	volatile char rx_buf[RX_BUFFER_SIZE];	// (written by ISR)
	volatile char tx_buf[TX_BUFFER_SIZE];
} xioUsart_t;

buffer_t xio_get_tx_bufcount_usart(const xioUsart_t *dx);
uint8_t xio_test_assertions(void);
void xio_init(void);
void cli(void);
int xio_ctrl(const uint8_t dev, const flags_t flags);
int xio_set_baud(const uint8_t dev, const uint8_t baud_rate);

#endif  // SIM_SIM_H_ONCE
