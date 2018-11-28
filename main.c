/*
  6502 CPU Emulator

  Author: Creative GP
  2018/11/27
  My first emulator!
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define LLHH(l, h) ((l) + (h<<8))
#define CUT(x) (x)&0xFF
#define LOAD16(x) LLHH(mem[x],mem[x+1])

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

/*
  :memory usage:
  0000 - 00FF: Zero page
  0100 - 01FF: Stack
 */
u8 *mem = 0;

enum {
    ADR_ACCUMULATOR= 8,
    ADR_ABSOLUTE   = 3,
    ADR_ABSOLUTE_X = 7,
    ADR_ABSOLUTE_Y = 6,
    ADR_IMMEDIATE  = 2,
    ADR_IMPLIED    = 9,
    ADR_INDIRECT   = 10,
    ADR_INDIRECT_X = 0,
    ADR_INDIRECT_Y = 4,
    ADR_RELATIVE   = 11,
    ADR_ZEROPAGE   = 1,
    ADR_ZEROPAGE_X = 5,
    ADR_ZEROPAGE_Y = 12
} adrmode;
const u32 CyclicTasks[] =
{
};
const u8 Cycles[] =
{
    7, 6, 0, 0, 
};
u8 ExitRequired = 0;
enum {InterruptPeriod = 100};
u64 Counter = InterruptPeriod;

u8 A=0, X=0, Y=0, S=0xFF;
u16 PC=0;
union
{
    unsigned int P: 8;
    struct
    {
        unsigned int C: 1;
        unsigned int Z: 1;
        unsigned int I: 1;
        unsigned int D: 1;
        unsigned int B: 1;
        unsigned int unu: 1;
        unsigned int O: 1;
        unsigned int N: 1;
    } flags;
} SR = { .P = 0b00100000 };

u16 operand = 0;

inline void push(u8 data);
inline u8 pop();


int main(int argc, char *argv[], char *envp[])
{
    mem = calloc(1, 65536);
    if (!mem)
    {
        printf("faild memory allocation");
        return -1;
    }

    PC = LOAD16(0xFFFC);
    for (;;)
    {
        u8 op = mem[PC++];
        Counter -= Cycles[op];

        u8 a = (op & 0b11100000) >> 5;
        u8 b = (op &    0b11100) >> 2;
        u8 c = (op &       0b11);
        u16 tmppc = PC++;

        switch (op) {
            case 0x00: adrmode = ADR_IMPLIED; goto BRK;
        }

        switch (b) {
            case 0: operand = LOAD16(mem[CUT(tmppc+X)]); break;       // X,ind
            case 1: operand = mem[tmppc]; break;                      // zpg
            case 2: operand = mem[tmppc]; break;                      // #
            case 3: operand = LLHH(mem[tmppc], mem[PC++]); break;     // abs
            case 4: operand = LOAD16(CUT(mem[tmppc]+Y)); break;       // ind,Y
            case 5: operand = CUT(mem[tmppc]+X); break;               // zpg,X
            case 6: operand = LLHH(mem[tmppc], mem[PC++]) + Y; break; // abs,Y
            case 7: operand = LLHH(mem[tmppc], mem[PC++]) + X; break; // abs,X
        }
        adrmode = b;
        
        switch (c) {
            case 0:
                switch (a) {
                    case 5: goto LDY;
                    case 6: goto CPY;
                    case 7: goto CPX;
                }
                break;
            case 1:
                switch (a) {
                    case 0: goto ORA;
                    case 1: goto AND;
                    case 2: goto EOR;
                    case 3: goto ADC;
                    case 4: goto STA;
                    case 5: goto LDA;
                    case 6: goto CMP;
                    case 7: goto SBC;
                }
                break;
            case 2:
                switch (a) {
                    case 0: goto ASL;
                    case 1: goto ROL;
                    case 2: goto LSR;
                    case 3: goto ROR;
                    case 4: goto STX;
                    case 5: goto LDX;
                    case 6: goto DEC;
                    case 7: goto INC;
                }
                break;
        }

#include "inst.h"
ret:

        if (Counter <= 0) {
            /* check interrupts */
            Counter += InterruptPeriod;
            if (ExitRequired) break;
        }
    }

    free(mem);

    return 0;
}

inline void push(u8 data)
{
//    if (S == 0) Stack Overflow!
    mem[0x0100 + S--] = data;
}

inline u8 pop()
{
    return mem[0x0100 + S++];
}


