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

#define SIGNBIT(x) (((x)&0b10000000) >> 7)
#define LSBIT(x)   ((x)&0b00000001)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int8_t i16;
typedef int32_t i32;
typedef int64_t i64;

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
    7, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 0, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    6, 6, 0, 0, 3, 4, 6, 0, 4, 2, 2, 0, 4, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    6, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 3, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    6, 6, 0, 0, 0, 3, 5, 0, 4, 2, 2, 0, 5, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    0, 6, 0, 0, 3, 3, 3, 0, 2, 0, 2, 0, 4, 4, 4, 0,
    2, 6, 0, 0, 4, 4, 4, 0, 2, 5, 2, 0, 0, 5, 0, 0,
    2, 6, 2, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 6, 0,
    2, 5, 0, 0, 4, 4, 4, 0, 2, 4, 2, 0, 4, 4, 4, 0,
    2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 3, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0
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
inline u8 pull();


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

#define ABS_OP operand = LLHH(mem[PC++], mem[PC++])
#define ABSX_OP operand = LLHH(mem[PC++], mem[PC++])+X
#define REL_OP operand = mem[PC++]
#define ZPG_OP operand = mem[PC++]
#define IMM_OP operand = mem[PC++]
#define ZPGX_OP operand = CUT(mem[PC++]+X)
#define IND_OP i8 tmp = mem[PC++]; operand = LOAD16(tmp)

        switch (op) {
            case 0x00: { adrmode = ADR_IMPLIED;    goto BRK; }
            case 0x98: { adrmode = ADR_IMPLIED;    goto TYA; }
            case 0xEA: { adrmode = ADR_IMPLIED;    goto NOP; }
            case 0xCA: { adrmode = ADR_IMPLIED;    goto DEX; }
            case 0xBA: { adrmode = ADR_IMPLIED;    goto TSX; }
            case 0xAA: { adrmode = ADR_IMPLIED;    goto TAX; }
            case 0x9A: { adrmode = ADR_IMPLIED;    goto TXS; }
            case 0x8A: { adrmode = ADR_IMPLIED;    goto TXA; }
            case 0xF8: { adrmode = ADR_IMPLIED;    goto SED; }
            case 0xE8: { adrmode = ADR_IMPLIED;    goto INX; }
            case 0xD8: { adrmode = ADR_IMPLIED;    goto CLD; }
            case 0xC8: { adrmode = ADR_IMPLIED;    goto INY; }
            case 0xB8: { adrmode = ADR_IMPLIED;    goto CLV; }
            case 0xA8: { adrmode = ADR_IMPLIED;    goto TAY; }
            case 0x18: { adrmode = ADR_IMPLIED;    goto CLC; }
            case 0x88: { adrmode = ADR_IMPLIED;    goto DEY; }
            case 0x78: { adrmode = ADR_IMPLIED;    goto SEI; }
            case 0x68: { adrmode = ADR_IMPLIED;    goto PLA; }
            case 0x60: { adrmode = ADR_IMPLIED;    goto RTS; }
            case 0x58: { adrmode = ADR_IMPLIED;    goto CLI; }
            case 0x48: { adrmode = ADR_IMPLIED;    goto PHA; }
            case 0x40: { adrmode = ADR_IMPLIED;    goto RTI; }
            case 0x38: { adrmode = ADR_IMPLIED;    goto SEC; }
            case 0x28: { adrmode = ADR_IMPLIED;    goto PLP; }
            case 0x08: { adrmode = ADR_IMPLIED;    goto PHP; }

            case 0x90: { REL_OP; adrmode = ADR_RELATIVE;    goto BCC; }
            case 0x30: { REL_OP; adrmode = ADR_RELATIVE;    goto BMI; }
            case 0xB0: { REL_OP; adrmode = ADR_RELATIVE;    goto BCS; }
            case 0xF0: { REL_OP; adrmode = ADR_RELATIVE;    goto BEQ; }
            case 0x70: { REL_OP; adrmode = ADR_RELATIVE;    goto BVS; }
            case 0x10: { REL_OP; adrmode = ADR_RELATIVE;    goto BPL; }
            case 0xD0: { REL_OP; adrmode = ADR_RELATIVE;    goto BNE; }
            case 0x50: { REL_OP; adrmode = ADR_RELATIVE;    goto BVC; }

            case 0x2C: { ABS_OP; adrmode = ADR_ABSOLUTE;    goto BIT; }
            case 0x8C: { ABS_OP; adrmode = ADR_ABSOLUTE;    goto STY; }
            case 0xAC: { ABS_OP; adrmode = ADR_ABSOLUTE;    goto LDY; }
            case 0x20: { ABS_OP; adrmode = ADR_ABSOLUTE;    goto JSR; }
            case 0xEC: { ABS_OP; adrmode = ADR_ABSOLUTE;    goto CPX; }
            case 0x4C: { ABS_OP; adrmode = ADR_ABSOLUTE;    goto JMP; }
            case 0xCC: { ABS_OP; adrmode = ADR_ABSOLUTE;    goto CPY; }

            case 0xBC: { ABSX_OP; adrmode = ADR_ABSOLUTE_X;    goto LDY; }

            case 0x24: { ZPG_OP; adrmode = ADR_ZEROPAGE;    goto BIT; }
            case 0x84: { ZPG_OP; adrmode = ADR_ZEROPAGE;    goto STY; }
            case 0xA4: { ZPG_OP; adrmode = ADR_ZEROPAGE;    goto LDY; }
            case 0xC4: { ZPG_OP; adrmode = ADR_ZEROPAGE;    goto CPY; }
            case 0xE4: { ZPG_OP; adrmode = ADR_ZEROPAGE;    goto CPX; }

            case 0x94: { ZPGX_OP; adrmode = ADR_ZEROPAGE_X;    goto STY; }
            case 0xB4: { ZPGX_OP; adrmode = ADR_ZEROPAGE_X;    goto LDY; }

            case 0x6C: { IND_OP; adrmode = ADR_INDIRECT;      goto JMP; }

            case 0xA0: { IMM_OP; adrmode = ADR_IMMEDIATE;    goto LDY; }
            case 0xC0: { IMM_OP; adrmode = ADR_IMMEDIATE;    goto CPY; }
            case 0xE0: { IMM_OP; adrmode = ADR_IMMEDIATE;    goto CPX; }
        }

        u8 a = (op & 0b11100000) >> 5;
        u8 b = (op &    0b11100) >> 2;
        u8 c = (op &       0b11);
        u16 tmppc = PC++;

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

inline u8 pull()
{
    return mem[0x0100 + S++];
}


