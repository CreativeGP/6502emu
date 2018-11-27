/*
  BRK  Force Break

  interrupt,                       N Z C I D V
  push PC+2, push SR               - - - 1 - -

  addressing    assembler    opc  bytes  cyles
  --------------------------------------------
  implied       BRK           00    1     7
*/              
BRK:
SR.flags.I = 1;
push(PC+2);
push(SR.P);
ExitRequired = 1;    
//interrupt
goto ret;

LDY:
goto ret;

CPY:
goto ret;

CPX:
goto ret;

ORA:
goto ret;

AND:
goto ret;

EOR:
goto ret;

ADC:
goto ret;

STA:
goto ret;

LDA:
goto ret;

CMP:
goto ret;

SBC:
goto ret;

ASL:
goto ret;

ROL:
goto ret;

LSR:
goto ret;

ROR:
goto ret;

STX:
goto ret;

LDX:
goto ret;

DEC:
goto ret;

INC:
goto ret;

