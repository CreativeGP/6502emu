/*
  BRK  Force Break

  interrupt,                       N Z C I D V
  push PC+2, push SR               - - - 1 - -

  addressing    assembler    opc  bytes  cyles
  --------------------------------------------
  implied       BRK           00    1     7
*/              
0x00:
state.flags.I = 1;
push(PC+2)
push(SR)
//interrupt
goto ret;

/*
ORA  OR Memory with Accumulator

A OR M -> A                      N Z C I D V
                                 + + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
(indirect,X)  ORA (oper,X)  01    2     6
*/              
0x01:
A = A | mem[PC++];
if ((i8)A < 0) SR.flags.N = 1
if (A == 0) SR.flags.Z = 1
goto ret;


/*
ORA  OR Memory with Accumulator

A OR M -> A                      N Z C I D V
                                 + + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
(indirect,X)  ORA (oper,X)  01    2     6
*/              
0x01:
A = A | mem[PC++];
if ((i8)A < 0) SR.flags.N = 1
if (A == 0) SR.flags.Z = 1
goto ret;

/*
  MP  Compare Memory with Accumulator

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

