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

PHP:
push(SR.P);
goto ret;

PLA:
A = pull();
SR.flags.N = SIGNBIT(A);
if (A == 0) SR.flags.N = 1; 
goto ret;

PLP:
SR.P = pull();
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
    if (SIGNBIT(operand)) SR.flags.N = 1;
    if (operand == 0) SR.flags.Z = 0;
    Y = operand;
} else {
    if (SIGNBIT(operand)) SR.flags.N = 1;
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
    if (SIGNBIT(A)) SR.flags.N = 1;
} else {
    A = A | mem[operand];
    if (A == 0) SR.flags.Z = 1;
    if (SIGNBIT(A)) SR.flags.N = 1;
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
    if (SIGNBIT(A)) SR.flags.N = 1;
} else {
    A = A & mem[operand];
    if (A == 0) SR.flags.Z = 1;
    if (SIGNBIT(A)) SR.flags.N = 1;
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
    if (SIGNBIT(A)) SR.flags.N = 1;
} else {
    A = A ^ mem[operand];
    if (A == 0) SR.flags.Z = 1;
    if (SIGNBIT(A)) SR.flags.N = 1;
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
    if (SIGNBIT(A)) SR.flags.N = 1;
} else {
    u16 tmp = A + mem[operand] + SR.flags.C;
    if (tmp != tmp&0xFF) SR.flags.C = 1;
    A = tmp&0xFF;
    if (A == 0) SR.flags.Z = 1;
    if (SIGNBIT(A)) SR.flags.N = 1;
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
    if (SIGNBIT(A)) SR.flags.N = 1;
} else {
    A = mem[operand];
    if (A == 0) SR.flags.Z = 1;
    if (SIGNBIT(A)) SR.flags.N = 1;
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
    if (A < operand) {
        SR.flags.N = 1;
        SR.flags.C = 1;
    }
    if (A == operand) SR.flags.Z = 1;
} else {
    if (A < mem[operand]) {
        SR.flags.N = 1;
        SR.flags.C = 1;
    }
    if (A == mem[operand]) SR.flags.Z = 1;
}
goto ret;

/*
  SBC  Subtract Memory from Accumulator with Borrow

  A - M - C -> A                   N Z C I D V
                                   + + + - - +

  addressing    assembler    opc  bytes  cyles
  --------------------------------------------
  immidiate     SBC #oper     E9    2     2
  zeropage      SBC oper      E5    2     3
  zeropage,X    SBC oper,X    F5    2     4
  absolute      SBC oper      ED    3     4
  absolute,X    SBC oper,X    FD    3     4*
  absolute,Y    SBC oper,Y    F9    3     4*
  (indirect,X)  SBC (oper,X)  E1    2     6
  (indirect),Y  SBC (oper),Y  F1    2     5*
 */
SBC:
if (adrmode == ADR_IMMEDIATE) {
    i16 tmp = A - operand - SR.flags.C;
    A = tmp&0xFF;
    if (SIGNBIT(A)) SR.flags.N = 1;
    if (A == 0)     SR.flags.Z = 1;
    if (tmp != A)   SR.flags.C = 1;
} else {
    i16 tmp = A - mem[operand] - SR.flags.C;
    A = tmp&0xFF;
    if (SIGNBIT(A)) SR.flags.N = 1;
    if (A == 0)     SR.flags.Z = 1;
    if (tmp != A)   SR.flags.C = 1;
}
goto ret;

/*
  ASL  Shift Left One Bit (Memory or Accumulator)

  C <- [76543210] <- 0             N Z C I D V
                                   + + + - - -

  addressing    assembler    opc  bytes  cyles
  --------------------------------------------
  accumulator   ASL A         0A    1     2
  zeropage      ASL oper      06    2     5
  zeropage,X    ASL oper,X    16    2     6
  absolute      ASL oper      0E    3     6
  absolute,X    ASL oper,X    1E    3     7
 */
ASL:
if (adrmode == ADR_ACCUMULATOR) {
    if (SIGNBIT(A)) SR.flags.C = 1;
    A <<= 1;
    if (A == 0)     SR.flags.Z = 1;
    if (SIGNBIT(A)) SR.flags.N = 1;
} else {
    if (SIGNBIT(mem[operand])) SR.flags.C = 1;
    mem[operand] <<= 1;
    if (mem[operand] == 0)     SR.flags.Z = 1;
    if (SIGNBIT(mem[operand])) SR.flags.N = 1;
}
goto ret;

/*
  ROL  Rotate One Bit Left (Memory or Accumulator)

  C <- [76543210] <- C             N Z C I D V
                                   + + + - - -

  addressing    assembler    opc  bytes  cyles
  --------------------------------------------
  accumulator   ROL A         2A    1     2
  zeropage      ROL oper      26    2     5
  zeropage,X    ROL oper,X    36    2     6
  absolute      ROL oper      2E    3     6
  absolute,X    ROL oper,X    3E    3     7
*/
ROL:
if (adrmode == ADR_ACCUMULATOR) {
    u8 tmp = CUT(A<<1) + SR.flags.C;
    SR.flags.C = SIGNBIT(A);
    A = tmp;
    if (A == 0)     SR.flags.Z = 1;
    if (SIGNBIT(A)) SR.flags.N = 1;
} else {
    u8 tmp = CUT(mem[operand]<<1) + SR.flags.C;
    SR.flags.C = SIGNBIT(mem[operand]);
    mem[operand] = tmp;
    if (mem[operand] == 0)     SR.flags.Z = 1;
    if (SIGNBIT(mem[operand])) SR.flags.N = 1;
}
goto ret;

/*
  LSR  Shift One Bit Right (Memory or Accumulator)

  0 -> [76543210] -> C             N Z C I D V
                                   - + + - - -

  addressing    assembler    opc  bytes  cyles
  --------------------------------------------
  accumulator   LSR A         4A    1     2
  zeropage      LSR oper      46    2     5
  zeropage,X    LSR oper,X    56    2     6
  absolute      LSR oper      4E    3     6
  absolute,X    LSR oper,X    5E    3     7
*/
LSR:
if (adrmode == ADR_ACCUMULATOR) {
    SR.flags.C = LSBIT(A);
    A >>= 1;
    if (A == 0)     SR.flags.Z = 1;
} else {
    SR.flags.C = LSBIT(mem[operand]);
    mem[operand] >>= 1;
    if (mem[operand] == 0)     SR.flags.Z = 1;
}
goto ret;

/*
  ROR  Rotate One Bit Right (Memory or Accumulator)

  C -> [76543210] -> C             N Z C I D V
                                   + + + - - -

  addressing    assembler    opc  bytes  cyles
  --------------------------------------------
  accumulator   ROR A         6A    1     2
  zeropage      ROR oper      66    2     5
  zeropage,X    ROR oper,X    76    2     6
  absolute      ROR oper      6E    3     6
  absolute,X    ROR oper,X    7E    3     7
*/
ROR:
if (adrmode == ADR_ACCUMULATOR) {
    u8 tmp = (A>>1) + (SR.flags.C<<8);
    SR.flags.C = LSBIT(A);
    A = tmp;
    if (A == 0)     SR.flags.Z = 1;
    if (SIGNBIT(A)) SR.flags.N = 1;
} else {
    u8 tmp = (mem[operand]>>1) + (SR.flags.C<<8);
    SR.flags.C = LSBIT(mem[operand]);
    mem[operand] = tmp;
    if (mem[operand] == 0)     SR.flags.Z = 1;
    if (SIGNBIT(mem[operand])) SR.flags.N = 1;
}
goto ret;

/*
  STX  Store Index X in Memory

  X -> M                           N Z C I D V
                                   - - - - - -

  addressing    assembler    opc  bytes  cyles
  --------------------------------------------
  zeropage      STX oper      86    2     3
  zeropage,Y    STX oper,Y    96    2     4
  absolute      STX oper      8E    3     4
 */
STX:
mem[operand] = X;
goto ret;

/*
  LDX  Load Index X with Memory

  M -> X                           N Z C I D V
                                   + + - - - -

  addressing    assembler    opc  bytes  cyles
  --------------------------------------------
  immidiate     LDX #oper     A2    2     2
  zeropage      LDX oper      A6    2     3
  zeropage,Y    LDX oper,Y    B6    2     4
  absolute      LDX oper      AE    3     4
  absolute,Y    LDX oper,Y    BE    3     4*
*/
LDX:
if (adrmode == ADR_IMMEDIATE) {
    X = operand;
    SR.flags.N = SIGNBIT(X);
    if (X == 0) SR.flags.Z = 1;
} else {
    X = mem[operand];
    SR.flags.N = SIGNBIT(X);
    if (X == 0) SR.flags.Z = 1;
}
goto ret;

/*
  DEC  Decrement Memory by One

  M - 1 -> M                       N Z C I D V
                                   + + - - - -

  addressing    assembler    opc  bytes  cyles
  --------------------------------------------
  zeropage      DEC oper      C6    2     5
  zeropage,X    DEC oper,X    D6    2     6
  absolute      DEC oper      CE    3     3
  absolute,X    DEC oper,X    DE    3     7
*/
DEC:
mem[operand] = CUT(mem[operand]-1);
SR.flags.N = SIGNBIT(mem[operand]);
if (mem[operand] == 0) SR.flags.Z = 1;
goto ret;

INC:
mem[operand] = CUT(mem[operand]+1);
SR.flags.N = SIGNBIT(mem[operand]);
if (mem[operand] == 0) SR.flags.Z = 1;
goto ret;

