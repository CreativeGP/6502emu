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

/*
  LDY  Load Index Y with Memory
  
  M -> Y                           N Z C I D V
                                   + + - - - -
  
  addressing    assembler    opc  bytes  cyles
  --------------------------------------------
  immidiate     LDY #oper     A0    2     2
  zeropage      LDY oper      A4    2     3
  zeropage,X    LDY oper,X    B4    2     4
  absolute      LDY oper      AC    3     4
  absolute,X    LDY oper,X    BC    3     4*
*/
LDY:
if (adrmode == ADR_IMMEDIATE) {
    if ((u8)operand < 0) SR.flags.N = 1;
    if (operand == 0) SR.flags.Z = 0;
    Y = operand;
} else {
    if ((u8)mem[operand] < 0) SR.flags.N = 1;
    if (mem[operand] == 0) SR.flags.Z = 0;
    Y = mem[operand];
}
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

