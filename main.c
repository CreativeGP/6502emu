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

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef uint64_t u64;

/*
  :memory usage:
  0000 - 00FF: Zero page
  0100 - 01FF: Stack
 */
u8 *mem = 0;

const u8 cycles[] =
{
    7, 6, 0, 0, 
};

u8 A=0, X=0, Y=0, S=0xFF;
u16 PC=0;
union
{
    unsigned int P: 7;
    struct
    {
        unsigned int C: 1;
        unsigned int Z: 1;
        unsigned int I: 1;
        unsigned int D: 1;
        unsigned int B: 1;
        unsigned int O: 1;
        unsigned int N: 1;
    } flags;
} SR = 0;


inline void push(u8 data)
{
//    if (S == 0) Stack Overflow!
    mem[0x0100 + S--] = data;
}

inline u8 pop()
{
    return mem[0x0100 + S++];
}


int main(int argc, char *argv[], char *envp[])
{
    mem = calloc(1, 65536);
    if (!mem)
    {
        printf("faild memory allocation");
        return -1;
    }

    u64 counter = UINT64_MAX;
    PC = mem[0xFFFD]<<8 + mem[0xFFFC];
    for (;;)
    {
        u8 op = mem[PC++];
        counter -= cycles[op];

        u8 a = (op & 0b11100000) >> 5;
        u8 b = (op &    0b11100) >> 2;
        u8 c = (op &       0b11);
        u16 operand = 0;
        switch (b) {
            case 0: operand = X + mem[PC++]; break; // X,ind
            case 1: operand =     mem[PC++]; break; // zpg
            case 2: operand =     mem[PC++]; break; // #
            case 3: operand = mem[PC++] + mem[PC++]<<8; break; // abs
            case 4: operand = X + mem[PC++]; break; // ind,Y
            case 5: operand = X + mem[PC++]; break; // zpg,X
            case 6: operand = X + mem[PC++]; break; // abs,Y
            case 7: operand = Y + mem[PC++]; break; // abs,X
        if (c == 1) {
            
            if (a == 0)
        }

ret:

    }

    free(mem);

    return 0;
}

#include "inst.h"
