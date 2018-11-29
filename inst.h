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

/*
  EOR  Exclusive-OR Memory with Accumulator

  A EOR M -> A                     N Z C I D V
                                   + + - - - -

  addressing    assembler    opc  bytes  cyles
  --------------------------------------------
  immidiate     EOR #oper     49    2     2
  zeropage      EOR oper      45    2     3
  zeropage,X    EOR oper,X    55    2     4
  absolute      EOR oper      4D    3     4
  absolute,X    EOR oper,X    5D    3     4*
  absolute,Y    EOR oper,Y    59    3     4*
  (indirect,X)  EOR (oper,X)  41    2     6
  (indirect),Y  EOR (oper),Y  51    2     5*
*/
EOR:
if (adrmode == ADR_IMMEDIATE) {
    A = A ^ operand;
    if (A == 0) SR.flags.Z = 1;
    if ((i8)A < 0) SR.flags.N = 1;
} else {
    A = A ^ mem[operand];
    if (A == 0) SR.flags.Z = 1;
    if ((i8)A < 0) SR.flags.N = 1;
}
goto ret;

/*
  ADC  Add Memory to Accumulator with Carry

  A + M + C -> A, C                N Z C I D V
                                   + + + - - +

  addressing    assembler    opc  bytes  cyles
  --------------------------------------------
  immidiate     ADC #oper     69    2     2
  zeropage      ADC oper      65    2     3
  zeropage,X    ADC oper,X    75    2     4
  absolute      ADC oper      6D    3     4
  absolute,X    ADC oper,X    7D    3     4*
  absolute,Y    ADC oper,Y    79    3     4*
  (indirect,X)  ADC (oper,X)  61    2     6
  (indirect),Y  ADC (oper),Y  71    2     5*
 */
ADC:
if (adrmode == ADR_IMMEDIATE) {
    u16 tmp = A + operand + SR.flags.C;
    if (tmp != tmp&0xFF) SR.flags.C = 1;
    A = tmp&0xFF;
    if (A == 0) SR.flags.Z = 1;
    if ((i8)A < 0) SR.flags.N = 1;
} else {
    u16 tmp = A + mem[operand] + SR.flags.C;
    if (tmp != tmp&0xFF) SR.flags.C = 1;
    A = tmp&0xFF;
    if (A == 0) SR.flags.Z = 1;
    if ((i8)A < 0) SR.flags.N = 1;
}
goto ret;

/*
  STA  Store Accumulator in Memory

  A -> M                           N Z C I D V
                                   - - - - - -

  addressing    assembler    opc  bytes  cyles
  --------------------------------------------
  zeropage      STA oper      85    2     3
  zeropage,X    STA oper,X    95    2     4
  absolute      STA oper      8D    3     4
  absolute,X    STA oper,X    9D    3     5
  absolute,Y    STA oper,Y    99    3     5
  (indirect,X)  STA (oper,X)  81    2     6
  (indirect),Y  STA (oper),Y  91    2     6
 */
STA:
mem[operand] = A;
goto ret;

/*
  LDA  Load Accumulator with Memory

  M -> A                           N Z C I D V
                                   + + - - - -

  addressing    assembler    opc  bytes  cyles
  --------------------------------------------
  immidiate     LDA #oper     A9    2     2
  zeropage      LDA oper      A5    2     3
  zeropage,X    LDA oper,X    B5    2     4
  absolute      LDA oper      AD    3     4
  absolute,X    LDA oper,X    BD    3     4*
  absolute,Y    LDA oper,Y    B9    3     4*
  (indirect,X)  LDA (oper,X)  A1    2     6
  (indirect),Y  LDA (oper),Y  B1    2     5*
 */
LDA:
if (adrmode == ADR_IMMEDIATE) {
    A = operand;
    if (A == 0) SR.flags.Z = 1;
    if ((i8)A < 0) SR.flags.N = 1;
} else {
    A = mem[operand];
    if (A == 0) SR.flags.Z = 1;
    if ((i8)A < 0) SR.flags.N = 1;
}
goto ret;

/*
  CMP  Compare Memory with Accumulator

  A - M                            N Z C I D V
                                   + + + - - -

  addressing    assembler    opc  bytes  cyles
  --------------------------------------------
  immidiate     CMP #oper     C9    2     2
  zeropage      CMP oper      C5    2     3
  zeropage,X    CMP oper,X    D5    2     4
  absolute      CMP oper      CD    3     4
  absolute,X    CMP oper,X    DD    3     4*
  absolute,Y    CMP oper,Y    D9    3     4*
  (indirect,X)  CMP (oper,X)  C1    2     6
  (indirect),Y  CMP (oper),Y  D1    2     5*
 */
CMP:
if (adrmode == ADR_IMMEDIATE) {
    i8 tmp = A - operand;
    if (tmp < 0) {
        SR.flags.N = 1;
        SR.flags.C = 1;
    }
    if (tmp == 0) SR.flags.Z = 1;
} else {
    i8 tmp = A - mem[operand];
    if (tmp < 0) {
        SR.flags.N = 1;
        SR.flags.C = 1;
    }
    if (tmp == 0) SR.flags.Z = 1;
}
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

