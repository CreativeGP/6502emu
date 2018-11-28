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
    if ((i8)operand < 0) SR.flags.N = 1;
    if (operand == 0) SR.flags.Z = 0;
    Y = operand;
} else {
    if ((i8)mem[operand] < 0) SR.flags.N = 1;
    if (mem[operand] == 0) SR.flags.Z = 0;
    Y = mem[operand];
}
goto ret;

/*
  CPY  Compare Memory and Index Y

  Y - M                            N Z C I D V
                                   + + + - - -

  addressing    assembler    opc  bytes  cyles
  --------------------------------------------
  immidiate     CPY #oper     C0    2     2
  zeropage      CPY oper      C4    2     3
  absolute      CPY oper      CC    3     4
*/
CPY:
if (adrmode == ADR_IMMEDIATE) {
    if (operand > Y) {
        SR.flags.C = 1;
        SR.flags.N = 1;
    }
    if (operand == Y) SR.flags.Z = 1;
} else {
    if (mem[operand] > Y) {
        SR.flags.C = 1;
        SR.flags.N = 1;
    }
    if (mem[operand] == Y) SR.flags.Z = 1;
}
goto ret;

/*
  CPX  Compare Memory and Index X

  X - M                            N Z C I D V
                                   + + + - - -

  addressing    assembler    opc  bytes  cyles
  --------------------------------------------
  immidiate     CPX #oper     E0    2     2
  zeropage      CPX oper      E4    2     3
  absolute      CPX oper      EC    3     4
 */
CPX:
if (adrmode == ADR_IMMEDIATE) {
    if (operand > X) {
        SR.flags.C = 1;
        SR.flags.N = 1;
    }
    if (operand == X) SR.flags.Z = 1;
} else {
    if (mem[operand] > X) {
        SR.flags.C = 1;
        SR.flags.N = 1;
    }
    if (mem[operand] == X) SR.flags.Z = 1;
}
goto ret;

/*
  ORA  OR Memory with Accumulator

  A OR M -> A                      N Z C I D V
                                   + + - - - -

  addressing    assembler    opc  bytes  cyles
  --------------------------------------------
  immidiate     ORA #oper     09    2     2
  zeropage      ORA oper      05    2     3
  zeropage,X    ORA oper,X    15    2     4
  absolute      ORA oper      0D    3     4
  absolute,X    ORA oper,X    1D    3     4*
  absolute,Y    ORA oper,Y    19    3     4*
  (indirect,X)  ORA (oper,X)  01    2     6
  (indirect),Y  ORA (oper),Y  11    2     5*
*/
ORA:
if (adrmode == ADR_IMMEDIATE) {
    A = A | operand;
    if (A == 0) SR.flags.Z = 1;
    if ((i8)A < 0) SR.flags.N = 1;
} else {
    A = A | mem[operand];
    if (A == 0) SR.flags.Z = 1;
    if ((i8)A < 0) SR.flags.N = 1;
}
goto ret;

/*
  AND  AND Memory with Accumulator

  A AND M -> A                     N Z C I D V
                                   + + - - - -

  addressing    assembler    opc  bytes  cyles
  --------------------------------------------
  immidiate     AND #oper     29    2     2
  zeropage      AND oper      25    2     3
  zeropage,X    AND oper,X    35    2     4
  absolute      AND oper      2D    3     4
  absolute,X    AND oper,X    3D    3     4*
  absolute,Y    AND oper,Y    39    3     4*
  (indirect,X)  AND (oper,X)  21    2     6
  (indirect),Y  AND (oper),Y  31    2     5*
 */
AND:
if (adrmode == ADR_IMMEDIATE) {
    A = A & operand;
    if (A == 0) SR.flags.Z = 1;
    if ((i8)A < 0) SR.flags.N = 1;
} else {
    A = A & mem[operand];
    if (A == 0) SR.flags.Z = 1;
    if ((i8)A < 0) SR.flags.N = 1;
}
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

