import sys
from enum import Enum

ACCUMULATOR= 8
ABSOLUTE   = 3
ABSOLUTE_X = 7
ABSOLUTE_Y = 6
IMMEDIATE  = 2
IMPLIED    = 9
INDIRECT   = 1
INDIRECT_X = 0
INDIRECT_Y = 4
RELATIVE   = 11
ZEROPAGE   = 1
ZEROPAGE_X = 5
ZEROPAGE_Y = 12


f = open(sys.argv[1], 'r')

for line in f:
    sp = line.split(' ')
    inst = sp[0]
    if len(sp) == 1:
        if inst == "BRK": print("00")
        if inst == "TYA": print("98")
        if inst == "NOP": print("EA")
        if inst == "DEX": print("CA")
        if inst == "TSX": print("BA")
        if inst == "TAX": print("AA")
        if inst == "TXS": print("9A")
        if inst == "TXA": print("8A")
        if inst == "SED": print("F8")
        if inst == "INX": print("E8")
        if inst == "CLD": print("D8")
        if inst == "INY": print("C8")
        if inst == "CLV": print("B8")
        if inst == "TAY": print("A8")
        if inst == "CLC": print("18")
        if inst == "DEY": print("88")
        if inst == "SEI": print("78")
        if inst == "PLA": print("68")
        if inst == "RTS": print("60")
        if inst == "CLI": print("58")
        if inst == "PHA": print("48")
        if inst == "RTI": print("40")
        if inst == "SEC": print("38")
        if inst == "PLP": print("28")
        if inst == "PHP": print("08")
        if inst == "ASL": print("0A")
        if inst == "ROL": print("2A")
        if inst == "LSR": print("4A")
        if inst == "ROR": print("6A")
        continue
    
    oper = line.split(' ')[1]
    oper = oper[:-1] if oper[-1] == '\n' else oper
    num  = ""


    adrmode = IMPLIED
    if oper[0] == "#":
        adrmode = IMMEDIATE
        num = oper[1:3]

    elif oper[0] == "$" and len(oper) == 5 and oper[4] == "X":
        adrmode = ZEROPAGE_X
        num = oper[1:3]
    elif oper[0] == "$" and len(oper) == 5 and oper[4] == "Y":
        adrmode = ZEROPAGE_Y
        num = oper[1:3]
    elif oper[0] == "$" and len(oper) == 3:
        adrmode = ZEROPAGE
        num = oper[1:3]

    elif oper[0] == "$" and len(oper) == 7 and oper[6] == "X":
        adrmode = ABSOLUTE_X
        num = oper[1:5]
    elif oper[0] == "$" and len(oper) == 7 and oper[6] == "Y":
        adrmode = ABSOLUTE_Y
        num = oper[1:5]
    elif oper[0] == "$" and len(oper) == 5:
        adrmode = ABSOLUTE
        num = oper[1:5]

    elif oper[0] == "(" and len(oper) == 7 and oper[5] == "X":
        adrmode = INDIRECT_X
        num = oper[2:4]
    elif oper[0] == "(" and len(oper) == 7 and oper[6] == "Y":
        adrmode = INDIRECT_Y
        num = oper[2:4]
    elif oper[0] == "(" and len(oper) == 7:
        adrmode = INDIRECT
        num = oper[2:6]

    if adrmode == RELATIVE:
        if inst == "BCC":
            print("90",num)
            continue
        if inst == "BMI":
            print("30",num)
            continue
        if inst == "BCS":
            print("B0",num)
            continue
        if inst == "BEQ":
            print("F0",num)
            continue
        if inst == "BVS":
            print("70",num)
            continue
        if inst == "BPL":
            print("10",num)
            continue
        if inst == "BNE":
            print("D0",num)
            continue
        if inst == "BVC":
            print("50",num)
            continue
    
    elif adrmode == ABSOLUTE:
        if inst == "BIT":
            print("2C",num)
            continue
        if inst == "STY":
            print("8C",num)
            continue
        if inst == "LDY":
            print("AC",num)
            continue
        if inst == "JSR":
            print("20",num)
            continue
        if inst == "CPX":
            print("EC",num)
            continue
        if inst == "JMP":
            print("4C",num)
            continue
        if inst == "CPY":
            print("CC",num)
            continue

    elif adrmode == ABSOLUTE_X:
        if inst == "LDY":
            print("BC",num)
            continue

    elif adrmode == ZEROPAGE:
        if inst == "BIT":
            print("24",num)
            continue
        if inst == "STY":
            print("84",num)
            continue
        if inst == "LDY":
            print("A4",num)
            continue
        if inst == "CPY":
            print("C4",num)
            continue
        if inst == "CPX":
            print("E4",num)
            continue

    elif adrmode == ZEROPAGE_X:
        if inst == "STY":
            print("94",num)
            continue
        if inst == "LDY":
            print("B4",num)
            continue

    elif adrmode == INDIRECT:
        if inst == "JMP":
            print("6C",num)
            continue

    elif adrmode == IMMEDIATE:
        if inst == "LDY":
            print("A0",num)
            continue
        if inst == "CPY":
            print("C0",num)
            continue
        if inst == "CPX":
            print("E0",num)
            continue

    print(inst, oper, num)

    b = adrmode

    if b == INDIRECT_X: b = IMMEDIATE
    if inst == "LDY": print(hex(0 + (b << 2) + (5 << 5))[2:].upper(), num)
    if inst == "CPY": print(hex(0 + (b << 2) + (6 << 5))[2:].upper(), num)
    if inst == "CPX": print(hex(0 + (b << 2) + (7 << 5))[2:].upper(), num)
    b = adrmode

    if inst == "ORA": print(hex(1 + (b << 2) + (0 << 5))[2:].upper(), num)
    if inst == "AND": print(hex(1 + (b << 2) + (1 << 5))[2:].upper(), num)
    if inst == "EOR": print(hex(1 + (b << 2) + (2 << 5))[2:].upper(), num)
    if inst == "ADC": print(hex(1 + (b << 2) + (3 << 5))[2:].upper(), num)
    if inst == "STA": print(hex(1 + (b << 2) + (4 << 5))[2:].upper(), num)
    if inst == "LDA": print(hex(1 + (b << 2) + (5 << 5))[2:].upper(), num)
    if inst == "CMP": print(hex(1 + (b << 2) + (6 << 5))[2:].upper(), num)
    if inst == "SBC": print(hex(1 + (b << 2) + (7 << 5))[2:].upper(), num)

    if b == IMMEDIATE: b = INDIRECT_X
    if inst == "ASL": print(hex(2 + (b << 2) + (0 << 5))[2:].upper(), num)
    if inst == "ROL": print(hex(2 + (b << 2) + (1 << 5))[2:].upper(), num)
    if inst == "LSR": print(hex(2 + (b << 2) + (2 << 5))[2:].upper(), num)
    if inst == "ROR": print(hex(2 + (b << 2) + (3 << 5))[2:].upper(), num)
    if inst == "STX": print(hex(2 + (b << 2) + (4 << 5))[2:].upper(), num)
    if inst == "LDX": print(hex(2 + (b << 2) + (5 << 5))[2:].upper(), num)
    if inst == "DEC": print(hex(2 + (b << 2) + (6 << 5))[2:].upper(), num)
    if inst == "INC": print(hex(2 + (b << 2) + (7 << 5))[2:].upper(), num)
    b = adrmode
    
    
f.close()
