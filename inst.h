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
