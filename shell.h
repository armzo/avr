/*
 * michel.h
 *
 *  Created on: 04/09/2013
 *      Author: armz
 */

#ifndef MICHEL_H_
#define MICHEL_H_
/*
 *  michel.h
 *  michel
 *
 *  Created by Adrian Ramirez on 3/25/11.
 *  Copyright 2011 KLabs. All rights reserved.
 *
 */

/*
 *  theCmdShell.h
 *  theCmdShell
 *
 *  Created by Adrian Ramirez on 3/21/11.
 *  Copyright 2011 KLabs. All rights reserved.

 testing ...
 22SP17 : prueba de este shell con ardiono nano funciona ok
          se modifican updates de eeprom a writes
 21Mz11 : funciona el localizador de comandos aunque con scanf
 23MZ11 : se tiene funcionando el parser de parametros my_argv & my_argc
 29mz11 : continuamos se realiza un sistema de rastreos
 02sp11 : se perdio desarrollo por mucha causas x 5 meses
 02sp11 : iniciamos con problemas:
 1- se sale del loop main : Parece que pierde la secuencia de busqueda de comandos sin razon
 accion : realizar el sistema de busquedas de manera secuencial (eliminar busquedas rapidas temporalmente)
  revisar la rutina rs232 - el cristal esta a 11059200 o probar aumentando buffer rs232
  parece que al comparar el comando hay diferencias y no lo reconoce

 */
/*uart.h defs*/

/* UART baud rate */

/*--- includes ---*/
#include <stdbool.h>
#include <avr/io.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#define UART_BAUD 9600
#define speeed  ( F_CPU / (16UL * UART_BAUD)) - 1
#define CMD_ENTRIES 16

uint8_t EEMEM eeDeviceId[12];
uint8_t EEMEM eeName[24];
uint8_t EEMEM eeAddress[64];

/*
 * Perform UART startup initialization.
 */
void uart_init(void);

/*
 * Send one character to the UART.
 */
int	uart_putchar(char c, FILE *stream);

/*
 * Size of internal line buffer used by uart_getchar().
 */
#define RX_BUFSIZE 48
const char version[] PROGMEM = "\navr_sh Rls1v0\n";
const char  ver[] PROGMEM ="\nARShell-R1A0\n(c) Armz 2017\n";

/*
 * Receive one character from the UART.  The actual reception is
 * line-buffered, and one character is returned from the buffer at
 * each invokation.
 */

int	uart_getchar(FILE *stream);
void ioinit(void);

// michel defs start here

/*--- defines ---*/
#define prompt	"sh: "
#define 	ANSI_ESCAPE_SEQUENCE(EscapeSeq)   "\33[" EscapeSeq
#define 	ESC_ERASE_DISPLAY   ANSI_ESCAPE_SEQUENCE("2J")

#define BUFLEN	16 
#define	MAXARGS 4

//#define version "\nShell ARls1.0\n(c) Armz 2013\n"
#define CMD_BUFFER_SIZE 32
#define CMD_MAX_LEN (CMD_BUFFER_SIZE-1)
#define PAR_MAX_LEN 2
#define SHELL_CMD_SET


//prog_char device[]="DeviceID: GDL-001\n";
//prog_char address[]="Location: Ofi Vallarta, Av Vallarta 3960B  Jard. Vallarta Zapopan Jal. 45027\n";
// typedef int (*cmdFunc) (char *argvect[],char argcnt );  // the command Function

typedef int (*cmdFunc)(void);  // the command Function
typedef struct _cmdEntry {
	const char *cmdName;		// command Name
	const char *definition;	// command definition
	    cmdFunc cmdFname;	  // command Function name
} cmdEntry_t;

int8_t slookup (const cmdEntry_t dict[], const char search[],  const int8_t entries );


void params(void);

int cmdHelp   (void);
int cmdAdcRd  (void);
int cmdSetOut (void);
int cmdClrOut (void);
int cmdDiesel (void);
int cmdTemp   (void);
int cmdId     (void);
int cmdSys    (void);
int cmdSum    (void);
int cmdDef    (void);
int cmdNull   (void);
int cmdCls    (void);
int cmdSetId  (void);
int cmdSetLoc (void);
int cmdSetNam (void);
int cmdId     (void);


/* el conteo numero de entradas debe iniciar en 1 e incluir
   la entrada cn el comando null
 */


const cmdEntry_t cmdSet_array[] =  {
/*
[cmdName] [definition			      ][cmdFname        ]
---------------------------------------------------------------- */
{ /*cmd0*/ "?","Despliega la ayuda",		cmdHelp		},
{ /*cmd1*/ "adc","Lectura de ADC <#canal>",	cmdAdcRd	},
{ /*cmd2*/ "add","suma dos numeros enteros",	cmdSum		},
{ /*cmd3*/ "clr","Clear OutPin <pinName>",	cmdClrOut	},
{ /*cmd4*/ "des","des <cmd> descipcion",        cmdDef		},
{ /*cmd5*/ "fuel","Status del combustible",     cmdDiesel	},
{ /*cmd6*/ "help","despliega la ayuda",         cmdHelp		},
{ /*cmd7*/ "dev","identificador del sitio",     cmdId		},
{ /*cmd8*/ "set", "Set Output Pin <pinName>",   cmdSetOut	},
{ /*cmd9*/ "ver","version del systema en uso",  cmdSys		},
{ /*cmdA*/ "temp","Temperatura de la sala",     cmdTemp		},
{ /*cmdB*/ "cls","Limpiar pantalla",            cmdCls		},
{ /*cmdC*/ "sid","set device ID",               cmdSetId	},
{ /*cmdD*/ "sloc","set Location address",       cmdSetLoc	},
{ /*cmdE*/ "snam","Set SiteName",               cmdSetNam	},
{ /*cmdF*/ "id", "Devide data",                 cmdId		},
}; // end file


#endif /* MICHEL_H_ */
