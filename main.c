/*
 * main.c
 *
 *  Created on: 04/09/2013
 *      Author: armz
 */

/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

/* Documentacion
1. revision del dimensionamiento de variables
2. Documentacion de rutinas de apoyo al loop main. (en) 


*/

#include "shell.h"

int8_t entries = CMD_ENTRIES;
uint8_t err = 0;
/*--- Global Variables ---*/

static char *my_argv[BUFLEN];  // my_argv es un buffer de apuntadores char
static int my_argc; // my_argc es el contador de argumentos

FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

/*--- help functions ---*/

int8_t slookup
(	const cmdEntry_t dict[], 
	const char     search[], 
	const int8_t   entries   )
/* find for an entry by comparing strings in the entries array
   return array[index] if found or -1 (not found)
 */
{
	int i; // iterator
	bool result = false;
	// printf("we're looking for: %s,\n",search);

	for (i = 0; i < entries; i++)
	{
		if (strcmp(search, dict[i].cmdName) == 0)
		{
			result = true;
			break; // breaks for loop
		}
	}

	if (result)
	{
		//	printf( "result=true at %i" , i );
		return i; // non zero , found entry[i] 
	}
	else
		return -1; // not found
}

/*	int	i;
	bool result =false;

	printf_P(PSTR("we're looking for:"));
	printf("%s\n",search);

	for (i=0; i<entries; i++){
		printf("entrie :%i %s\n",i,dict[i].cmdName);
		result= strcmp(search, dict[i].cmdName);
	}
	if (result==true) {
		printf("result at :%i",i);
		return i-1;
		//break;
	}else return -1;*/
//  esta funcion copia todos los parametros de la entrada en un buffer temporal

void fill_argv(char *tmp_argv)
/* fill the arguments vector 
   my_argv[0] is the command 
*/
{  char *foo = tmp_argv; //<<-foo points to user string beggining
	char ret[BUFLEN];     // return buffer
	int index = 0;        // index of buffer

	my_argc = 0;			// no aguments 
	memset(ret, 0, BUFLEN); // clear the return buffer

	while (*foo != '\0')   // 
	{ 
		if (index == MAXARGS) break; // hasta que 'index' iguale los argumentos
		if (*foo == ' ') // space is found in the user line
		{  
			// primero reserva especio en el buffer
			if (my_argv[index] == NULL) //if position is empty 
			  // reserve and clear  memory for parameter buffer size
			  // my_argv holds now the pointer at the index  
				my_argv[index] = (char *)calloc(sizeof(char), strlen(ret) + 1);
			else // fill this position with zero
			{ 
				memset(my_argv[index], 0, strlen(my_argv[index]));
			}// next copy strings into buffer arguments at index
			strncpy(my_argv[index], ret, strlen(ret)); // copia texto encontrado
			strncat(my_argv[index], "\0", 1);	   // y agrega '\0'
			memset(ret, 0, BUFLEN);  // borra texto encontrado
			index++; // siguiente argumento
		}
		else // not space yet,
		{
			strncat(ret, foo, 1); // find text and fill ret buffer
		}
		foo++; // next character on the line
	}

	my_argv[index] = (char *)malloc(sizeof(char) * strlen(ret) + 1);
	strncpy(my_argv[index], ret, strlen(ret));
	strncat(my_argv[index], "\0", 1);

	// scan the arg vector and count arguments available
	while ((my_argc < MAXARGS) && (my_argv[my_argc]) && (*my_argv[my_argc]))
	{
		my_argc++;
	}
} // fill_argv

void free_argv()
{
	int8_t index;
	for (index = 0; my_argv[index] != NULL; index++)
	{
		memset(my_argv[index], '\0', strlen(my_argv[index]) + 1);
		my_argv[index] = NULL;
		free(my_argv[index]);
	}
}

int main(void)
{
	char c;
	char *tmp = (char *)malloc(sizeof(char) * BUFLEN); // get input from user
	memset(tmp, '\0', BUFLEN); // clear input buffer
	char *cmd = (char *)malloc(sizeof(char) * BUFLEN); // command buffer
	memset(cmd, '\0', BUFLEN); // clear command buffer

	int8_t entry = 0; // no entries yet
	uart_init();	   // prepare user communications
	stdout = stdin = stderr = &uart_str;	// standard io as uart stream
	printf(ESC_ERASE_DISPLAY);  // clear terminal with scape code
	printf_P(ver);  // program version
	printf(prompt); // user prompt
	fflush(stdout); // clear output stream

	while (true)    // forever loop
	{ 
		c = getchar(); // user is typing on 
		switch (c)
		{
		case '\n': // user just press 'enter'
			if (tmp[0] == '\0') 
			{
				printf(prompt); // no user imput 

			} else  // user typed enter after the command line
			{ 
			   fill_argv(tmp);	// get parameters from user into my_argv
				strncpy(cmd, my_argv[0], strlen(my_argv[0])); // copia el comando
				strncat(cmd, "\0", 1);			      // le anexa el terminador
				entry = slookup(cmdSet_array, cmd, entries);
				if (entry != -1) // comando encontrado
					{ 
						// aqui se ejecuta la funcion
					// uint8_t err = cmdSet_array[entry].cmdFname(my_argv,my_argc); 	
							err = cmdSet_array[entry].cmdFname(); 
						if (err != 0)
							printf(PSTR("\nerror  %d!\n"), err);
					}
						else // comando no encontrado -libera y borra todo
					{
						printf("!");
						free_argv();
						memset(tmp, '\0', BUFLEN);
						memset(cmd, '\0', BUFLEN);
					}

				free_argv();
				memset(tmp, '\0', BUFLEN);
				memset(cmd, '\0', BUFLEN);
				printf(prompt);

			} 
			break; // startup again

		default: // user is typing 

			if (strlen(tmp) > BUFLEN) // user exceded typing
			{  // printf("\nBuffer exceded!\n"); 

			}
			else // user is filling ths buffer
			{
				strncat(tmp, &c, 1); // put character on buffer
			}
			break; // next character
		} // switch

	} // for loop
	printf_P(PSTR("I shouldn't be here!!\n\n"));
} // main



/**** COMMAND  FUNCTIONS****/
int cmdHelp(void)
{
	// int cmdHelp  (void){
	for (int i = 0; i < entries; i++)
	{
		printf("%s %s\n", cmdSet_array[i].cmdName, cmdSet_array[i].definition);
	}
	return 0;
}

int cmdAdcRd(void)
{
	// int cmdAdcRd (void){
	printf_P(PSTR("ADC ch1 : 1.25V\n"));
	return 0;
}

int cmdSetOut(void)
{
	// int cmdSetOut(void){
	printf_P(PSTR("cmdSetOut executed!\n"));
	return 0;
}

int cmdClrOut(void)
{
	//	int cmdClrOut (void ){
	printf_P(PSTR("cmdClrOut executed!\n"));
	return 0;
}
int cmdCls(void)
{
	printf(ESC_ERASE_DISPLAY);
	return 0;
}

int cmdDiesel(void)
{
	//	int cmdDiesel (void){
	printf_P(PSTR("      Cap: 1500 lts\n"));
	printf_P(PSTR("     Disp: 1200 lts\n"));
	printf_P(PSTR("  On Time: 00.00:00:00\n"));
	printf_P(PSTR("Available: 80%% 14.28d 342hr\n"));
	return 0;
}

int cmdTemp(void)
{
	//	int cmdTemp (void){
	printf_P(PSTR("20 ºC\n"));
	return 0;
}

int cmdId(void)
{

	char dev[12];
	char name[24];
	char addr[64];
	int len = 8;
	eeprom_read_block((void *)dev, (const void *)eeDeviceId, len);
	len = 22;
	eeprom_read_block((void *)name, (const void *)eeName, len);
	len = 64;
	eeprom_read_block((void *)addr, (const void *)eeAddress, len);

	printf("  Id: %s\nName: %s\n Loc: %s\n", dev, name, addr);
	return 0;
}

int cmdSys(void)
{
	//	int cmdSys (void ){
	printf_P(ver);
	return 0;
}

int cmdSum(void)
{
	//	int cmdSum (void ){
	int A, B, C;
	C = 0;
	A = atoi(my_argv[1]);
	B = atoi(my_argv[2]);
	C = A + B;
	printf("%d\n", C);

	return 0;
}

int cmdSetLoc(void)
{
	int len = 64;
	char dev[len]; // size =64 character buffer

	strncpy(dev, my_argv[1], len);
	//	strncat(dev,"\0",1);
	eeprom_write_block((const void *)my_argv[1], (void *)eeAddress, len);
	printf("%s\n", dev);
	return 0;
}

int cmdSetNam(void)
{
	int len = 32;
	char dev[len]; // size =64 character buffer

	strncpy(dev, my_argv[1], len);
	//	strncat(dev,"\0",1);
	eeprom_write_block((const void *)my_argv[1], (void *)eeName, len);
	printf("%s\n", dev);
	return 0;
}

int cmdSetId(void)
{
	int len = 9;
	char dev[len]; // len=9

	eeprom_read_block((void *)dev, (const void *)eeDeviceId, len);
	printf("%s Changed to ", dev);
	eeprom_write_block((const void *)my_argv[1], (void *)eeDeviceId, len);
	eeprom_read_block((void *)dev, (const void *)eeDeviceId, len);
	printf("%s\n", dev);
	return 0;
}

int cmdDef(void)
{

	int find = 0;
	int entris = CMD_ENTRIES;
	char txt[8];

	memset(txt, 0, 7);
	strncpy(txt, my_argv[1], strlen(my_argv[1])); // copia el comando
	strncat(txt, "\0", 1);			      // le anexa el terminador
	find = slookup(cmdSet_array, txt, entris);
	if (find != -1)
	{
		printf("%s\n", cmdSet_array[find].definition);
	}
	else
	{
		printf_P(PSTR("def <cmd to find>\n"));
	}

	return 0;
}

int cmdNull(void)
{
	// int cmdNull (void ){
	printf_P(PSTR("Never reach this point!\n"));
	return 0;
}

// --- help

void params(void)
{
	int x = 0;
	// scan the arg vector and count arguments available
	while ((x < MAXARGS) && (my_argv[x]) && (*my_argv[x]))
	{
		printf_P(PSTR("\nParam[%d] : %s"), x, my_argv[x]); // Debugging
		x++;
	}
}

//**************************************************************
// uart.c code from here. -------------UUU AAA RRR TTTT

/*
 * Initialize the UART to 9600 Bd, tx/rx, 8N1.
 */

void uart_init(void)
{

	// UART Init

	UCSR0C |= _BV(UCSZ01) | _BV(UCSZ00); // 8 bit character size.
	UBRR0H = 0;
	UBRR0L = speeed;
	UCSR0B |= _BV(TXEN0) | _BV(RXEN0) | _BV(RXCIE0); // Enable  Transmit,  Receive, Receive Interrupt

	// Call fdevopen. This is what binds printf to put_uart_char. The first
	// parameter is the address of our function that will output a single
	// character, the second parameter is an optional parameter that is
	// used for get functions, ie. receiving a character from the UART.
	// This is the function that uses malloc.

	//	fdevopen(&put_uart_char, NULL);

	// Enable the interrupts globally.

	// SREG |= _BV(SREG_I);
}

/*
 * Send character c down the UART Tx, wait until tx holding register
 * is empty.
 */
int uart_putchar(char c, FILE *stream)
{
	if (c == '\a')
	{
		fputs("*ring*\n", stdout);
		return 0;
	}

	if (c == '\n')
		uart_putchar('\r', stream);

	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	/* Wait for empty transmit buffer */
	//	while ( !( UCSRA & (1<<UDRE)) )
	/* Put data into buffer, sends the data */

	return 0;
}

/*
 * Receive a character from the UART Rx.
 *
 * This features a simple line-editor that allows to delete and
 * re-edit the characters entered, until either CR or NL is entered.
 * Printable characters entered will be echoed using uart_putchar().
 *
 * Editing characters:
 *
 * . \b (BS) or \177 (DEL) delete the previous character
 * . ^u kills the entire input buffer
 * . ^w deletes the previous word
 * . ^r sends a CR, and then reprints the buffer
 * . \t will be replaced by a single space
 *
 * All other control characters will be ignored.
 *
 * The internal line buffer is RX_BUFSIZE (80) characters long, which
 * includes the terminating \n (but no terminating \0).  If the buffer
 * is full (i. e., at RX_BUFSIZE-1 characters in order to keep space for
 * the trailing \n), any further input attempts will send a \a to
 * uart_putchar() (BEL character), although line editing is still
 * allowed.
 *
 * Input errors while talking to the UART will cause an immediate
 * return of -1 (error indication).  Notably, this will be caused by a
 * framing error (e. g. serial line "break" condition), by an input
 * overrun, and by a parity error (if parity was enabled and automatic
 * parity recognition is supported by hardware).
 *
 * Successive calls to uart_getchar() will be satisfied from the
 * internal buffer until that buffer is emptied again.
 */
int uart_getchar(FILE *stream)
{
	uint8_t c;
	char *cp, *cp2;
	static char b[RX_BUFSIZE];
	static char *rxp;

	if (rxp == 0)
		for (cp = b;;)
		{
			loop_until_bit_is_set(UCSR0A, RXC0);
			if (UCSR0A & _BV(FE0))
				return _FDEV_EOF;
			if (UCSR0A & _BV(DOR0))
				return _FDEV_ERR;
			c = UDR0;

			/* behaviour similar to Unix stty ICRNL */
			if (c == '\r')
				c = '\n';
			if (c == '\n')
			{
				*cp = c;
				uart_putchar(c, stream);
				rxp = b;
				break;
			}
			else if (c == '\t')
				c = ' ';

			if ((c >= (uint8_t)' ' && c <= (uint8_t)'\x7e') || c >= (uint8_t)'\xa0')
			{
				if (cp == b + RX_BUFSIZE - 1)
					uart_putchar('\a', stream);
				else
				{
					*cp++ = c;
					uart_putchar(c, stream);
				}
				continue;
			}

			switch (c)
			{
			case 'c' & 0x1f:
				return -1;

			case '\b':
			case '\x7f':
				if (cp > b)
				{
					uart_putchar('\b', stream);
					uart_putchar(' ', stream);
					uart_putchar('\b', stream);
					cp--;
				}
				break;

			case 'r' & 0x1f:
				uart_putchar('\r', stream);
				for (cp2 = b; cp2 < cp; cp2++)
					uart_putchar(*cp2, stream);
				break;

			case 'u' & 0x1f:
				while (cp > b)
				{
					uart_putchar('\b', stream);
					uart_putchar(' ', stream);
					uart_putchar('\b', stream);
					cp--;
				}
				break;

			case 'w' & 0x1f:
				while (cp > b && cp[-1] != ' ')
				{
					uart_putchar('\b', stream);
					uart_putchar(' ', stream);
					uart_putchar('\b', stream);
					cp--;
				}
				break;
			}
		}

	c = *rxp++;
	if (c == '\n')
		rxp = 0;

	return c;
}
