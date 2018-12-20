import sys
import struct
import re
from enum import Enum

offset = 0
defines = {}
label_ind = {}
labels = []

binary = ""

def printf(*s): print(s)
def printb(*s):
    global binary, offset
    print(s)
    for sh in s:
        offset += len(sh)
        binary += sh

binprint = printb

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

def little_endian(s):
    if len(s) == 4:
        return s[2:] + s[:2]
    else:
        return s

def mn2hex(s):
    if s[0].isalpha():
        if s in defines: s = defines[s]
#        if s in labels: s = defines[s]
    # String -> Hex
    if s[0] == '"':
        s = '$' + hex(ord(s[1]))[2:].upper()
    
    if s[0] == '$':
        return little_endian(s[1:])
    elif s[0] == '%':
        return little_endian(hex(int(s[1:], 2))[2:].zfill(2))
    else:
        return little_endian(s)

f = open(sys.argv[1], 'r')
contents = f.read()
contents = re.sub(r'(.*);.*', r'\1', contents)

for line in contents.split("\n"):
    sp = line.split()
    if len(sp) == 0: continue
    inst = sp[0]
    if inst[-1] == ':':
        label_ind[inst[:-1]] = len(labels)
        labels.append(0)
        continue

print(label_ind)
    
for line in contents.split("\n"):
    sp = line.split()
    if len(sp) == 0: continue
    
    inst = sp[0]
    if inst[0] == ';': continue

    if inst[-1] == ':':
        labels[label_ind[inst[:-1]]] = int(offset/2)
        continue
    
    if len(sp) == 1:
        if inst == "BRK": binprint("00")
        if inst == "TYA": binprint("98")
        if inst == "NOP": binprint("EA")
        if inst == "DEX": binprint("CA")
        if inst == "TSX": binprint("BA")
        if inst == "TAX": binprint("AA")
        if inst == "TXS": binprint("9A")
        if inst == "TXA": binprint("8A")
        if inst == "SED": binprint("F8")
        if inst == "INX": binprint("E8")
        if inst == "CLD": binprint("D8")
        if inst == "INY": binprint("C8")
        if inst == "CLV": binprint("B8")
        if inst == "TAY": binprint("A8")
        if inst == "CLC": binprint("18")
        if inst == "DEY": binprint("88")
        if inst == "SEI": binprint("78")
        if inst == "PLA": binprint("68")
        if inst == "RTS": binprint("60")
        if inst == "CLI": binprint("58")
        if inst == "PHA": binprint("48")
        if inst == "RTI": binprint("40")
        if inst == "SEC": binprint("38")
        if inst == "PLP": binprint("28")
        if inst == "PHP": binprint("08")
        if inst == "ASL": binprint("0A")
        if inst == "ROL": binprint("2A")
        if inst == "LSR": binprint("4A")
        if inst == "ROR": binprint("6A")
        continue

    if sp[1] == 'EQU':
        defines[sp[0]] = sp[2]
        continue
    
    oper = sp[1]
    oper = oper[:-1] if oper[-1] == '\n' else oper
    num  = ""


    adrmode = IMPLIED
    if oper[0] == "#":
        adrmode = IMMEDIATE
        num = mn2hex(oper[1:])

    elif oper[0] == "(" and oper[-2] == "X":
        adrmode = INDIRECT_X
        num = mn2hex(oper[1:-3])
    elif oper[0] == "(" and oper[-1] == "Y":
        adrmode = INDIRECT_Y
        num = mn2hex(oper[1:-3])
    elif oper[0] == "(":
        adrmode = INDIRECT
        num = mn2hex(oper[1:-1])

    elif oper[-1] == "X":
        num = mn2hex(oper[:-2])
        if len(num) == 2:
            adrmode = ZEROPAGE_X
        else:
            adrmode = ABSOLUTE_X
            
    elif oper[-1] == "Y":
        num = mn2hex(oper[:-2])
        if len(num) == 2:
            adrmode = ZEROPAGE_Y
        else:
            adrmode = ABSOLUTE_Y

    # TODO:　相対参照はアルファベット以外も対応する
    elif oper[0].isalpha():
        print(binary)
        num = '*' + chr(ord('G') + label_ind[oper])
        adrmode = RELATIVE

    else:
        num = mn2hex(oper)
        if len(num) == 2:
            adrmode = ZEROPAGE
        else:
            adrmode = ABSOLUTE
    
    num = num.zfill(2)

    if adrmode == RELATIVE:
        if inst == "BCC":
            binprint("90",num)
            continue
        if inst == "BMI":
            binprint("30",num)
            continue
        if inst == "BCS":
            binprint("B0",num)
            continue
        if inst == "BEQ":
            binprint("F0",num)
            continue
        if inst == "BVS":
            binprint("70",num)
            continue
        if inst == "BPL":
            binprint("10",num)
            continue
        if inst == "BNE":
            binprint("D0",num)
            continue
        if inst == "BVC":
            binprint("50",num)
            continue
    
    elif adrmode == ABSOLUTE:
        if inst == "BIT":
            binprint("2C",num)
            continue
        if inst == "STY":
            binprint("8C",num)
            continue
        if inst == "LDY":
            binprint("AC",num)
            continue
        if inst == "JSR":
            binprint("20",num)
            continue
        if inst == "CPX":
            binprint("EC",num)
            continue
        if inst == "JMP":
            binprint("4C",num)
            continue
        if inst == "CPY":
            binprint("CC",num)
            continue

    elif adrmode == ABSOLUTE_X:
        if inst == "LDY":
            binprint("BC",num)
            continue

    elif adrmode == ZEROPAGE:
        if inst == "BIT":
            binprint("24",num)
            continue
        if inst == "STY":
            binprint("84",num)
            continue
        if inst == "LDY":
            binprint("A4",num)
            continue
        if inst == "CPY":
            binprint("C4",num)
            continue
        if inst == "CPX":
            binprint("E4",num)
            continue

    elif adrmode == ZEROPAGE_X:
        if inst == "STY":
            binprint("94",num)
            continue
        if inst == "LDY":
            binprint("B4",num)
            continue

    elif adrmode == INDIRECT:
        if inst == "JMP":
            binprint("6C",num)
            continue

    elif adrmode == IMMEDIATE:
        if inst == "LDY":
            binprint("A0",num)
            continue
        if inst == "CPY":
            binprint("C0",num)
            continue
        if inst == "CPX":
            binprint("E0",num)
            continue

    print(inst, oper, num)

    b = adrmode

    if b == INDIRECT_X: b = IMMEDIATE
    if inst == "LDY": binprint(hex(0 + (b << 2) + (5 << 5))[2:].zfill(2).upper(), num)
    if inst == "CPY": binprint(hex(0 + (b << 2) + (6 << 5))[2:].zfill(2).upper(), num)
    if inst == "CPX": binprint(hex(0 + (b << 2) + (7 << 5))[2:].zfill(2).upper(), num)
    b = adrmode

    if inst == "ORA": binprint(hex(1 + (b << 2) + (0 << 5))[2:].zfill(2).upper(), num)
    if inst == "AND": binprint(hex(1 + (b << 2) + (1 << 5))[2:].zfill(2).upper(), num)
    if inst == "EOR": binprint(hex(1 + (b << 2) + (2 << 5))[2:].zfill(2).upper(), num)
    if inst == "ADC": binprint(hex(1 + (b << 2) + (3 << 5))[2:].zfill(2).upper(), num)
    if inst == "STA": binprint(hex(1 + (b << 2) + (4 << 5))[2:].zfill(2).upper(), num)
    if inst == "LDA": binprint(hex(1 + (b << 2) + (5 << 5))[2:].zfill(2).upper(), num)
    if inst == "CMP": binprint(hex(1 + (b << 2) + (6 << 5))[2:].zfill(2).upper(), num)
    if inst == "SBC": binprint(hex(1 + (b << 2) + (7 << 5))[2:].zfill(2).upper(), num)

    if b == IMMEDIATE: b = INDIRECT_X
    if inst == "ASL": binprint(hex(2 + (b << 2) + (0 << 5))[2:].zfill(2).upper(), num)
    if inst == "ROL": binprint(hex(2 + (b << 2) + (1 << 5))[2:].zfill(2).upper(), num)
    if inst == "LSR": binprint(hex(2 + (b << 2) + (2 << 5))[2:].zfill(2).upper(), num)
    if inst == "ROR": binprint(hex(2 + (b << 2) + (3 << 5))[2:].zfill(2).upper(), num)
    if inst == "STX": binprint(hex(2 + (b << 2) + (4 << 5))[2:].zfill(2).upper(), num)
    if inst == "LDX": binprint(hex(2 + (b << 2) + (5 << 5))[2:].zfill(2).upper(), num)
    if inst == "DEC": binprint(hex(2 + (b << 2) + (6 << 5))[2:].zfill(2).upper(), num)
    if inst == "INC": binprint(hex(2 + (b << 2) + (7 << 5))[2:].zfill(2).upper(), num)
    b = adrmode
    
f.close()

# ラベル解決
for i in range(0,len(binary),2):
    if not binary[i] in '1234567890ABCDEF':
        dst = labels[ord(binary[i+1]) - ord('G')] - int(i/2)
        pay = hex(256+dst if dst < 0 else dst)[2:].zfill(2).upper()
        binary = binary[:i] + pay + binary[i+2:]

# print(binary)
# for i in range(0,len(binary),4):
#     try:
#         binary = binary[:i] + binary[i+2] + binary[i+3] + binary[i] + binary[i+1] + binary[i+4:]
#     except IndexError:
#         binary = binary[:i]  + '00' + binary[i] + binary[i+1]

if binary != "":
    print(binary)

    report = ""
    with open('out.bin', 'wb') as fout:
        fout.write(bytes.fromhex(binary))
        # bary = map(lambda s: int(s, 16), [binary[i:i+2] for i in range(0, len(binary), 2)])
        # for i in range(0,bary,2):
        #     fout.write(x.to_bytes(1, byteorder='little'))
