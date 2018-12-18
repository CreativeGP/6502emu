# CPUのエミュレータを作る

どうも！どうも！！！

今回はCPUのエミュレータを作る回です。低レイヤの勉強のために最初に簡単なエミュを書こうかなと思います。今回僭越にもエミュらせていただくCPUは、**6502**です。理由はNESに使われているらしいからです（界隈では有名なんですか、6502？）。



## 基本的な情報

まずはエミュるCPUの情報を集めなければなりません。

僕は[Wikipedia](https://en.wikipedia.org/wiki/MOS_Technology_6502)しか見ませんでした（）



## 大まかな流れ

[http://fms.komkon.org/EMUL8/HOWTO.html#LABG](http://fms.komkon.org/EMUL8/HOWTO.html#LABG)

ここを見ると大体出来るのですが、ここから引用します。

```c
Counter=InterruptPeriod;
PC=InitialPC;

for(;;)
{
  OpCode=Memory[PC++];
  Counter-=Cycles[OpCode];

  switch(OpCode)
  {
    case OpCode1:
    case OpCode2:
    ...
  }

  if(Counter<=0)
  {
    /* Check for interrupts and do other */
    /* cyclic tasks here                 */
    ...
    Counter+=InterruptPeriod;
    if(ExitRequired) break;
  }
}
```

無限ループの中で、次のコードを読んで、必要サイクルカウンタを減らして、働いて、割り込みとかを処理する、って感じです。



## いざ、書く

Emacsをスチャッと立ち上げて不意に次のようなコードを書きます。

```C
/*
  6502 CPU Emulator

  Author: Creative GP
  2018/11/27
  My first emulator!
 */

#include <stdio.h>

int main(int argc, char *argv[], char *envp[])
{
    
}

```

envpはなんかカッコいいので付けました。環境情報が入ってくるらしいです。僕の環境では以下のような文字列１つだけしか入っていませんでした。

```
PWD=/home/cgp/Develop/cpuemu/6502
```

実行したディレクトリですね。



まぁこんなものは良くて、まずはメモリをどうにかしましょう。



## メモリをどうにかする

まずはメモリについて深く調べてみます。

6502の場合アドレスは16-bitなので、メモリの量は$2^{16} =65536$バイトです。



> **Zero Page**
>
> 6502など計算用レジスタが少ないCPUでは、計算のためにメモリを直接使うことが良くある。良くあるので、番地が長すぎると嫌だ。なので、#00xx -> #xx, #000x -> #xと書けるようにしよう！的なノリのモードらしいです。なので、#0000 ~ #00FFが沢山利用されます。これで高速化されるらしい。



コードとしてはこんな感じでとりあえず書いてみる、、

```C
int main(int argc, char *argv[], char *envp[])
{
    mem = calloc(1, 65536);
    if (!mem)
    {
        printf("faild memory allocation");
        return -1;
    }

    u8 A, X, Y, S;
    u16 PC;
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
    } reg;


    for (;;)
    {
        
    }

    free(mem);

    return 0;
}

```

メモリの割り当てについてはこれじゃ駄目的な記述があった気がするんだけど、理由を読んでも良く分らなかったので普通に実装した。



## プログラム実行部分を書く

プログラム実行部分を書いていく。とりあえず最初に、PCの初期化と寿命を決める必要があるらしい。

PCの初期値は[ここらへん](http://forum.6502.org/viewtopic.php?t=1708)を見ると、#[#FFFD#FFFC]にならなければならないと書いてある（reset vectorというらしい）。素直にやるけどここらへんはコマンドライン引数でも調整できるようにしなきゃかもしれない。

```c
u64 count = UINT64_MAX;
PC = mem[0xFFFD]<<8 + mem[0xFFFC];
```



## オペコード解読大会 🧐 

ここら辺は[これ](https://www.masswerk.at/6502/6502_instruction_set.html)を聖書のように熟読しながら書いていくわけです。

まず、無秩序に並んでいるようなオペコード達は実は若干の規則があるよっていう話が最後らへんに書いてあるので、それを利用しながら効率的にオペコード解読、オペランドの展開などもやっていきます。

オペコードをバイナリで並べて"aaabbbcc"で各文字の位置に来るバイナリを区切って読んだ時に[^1]、bはオペランドの展開方法を**大体**表していて、a,cはオペコードを**大体**表しているという規則があります。

```c
u8 a = (op & 0b11100000) >> 5;
u8 b = (op &    0b11100) >> 2;
u8 c = (op &       0b11);
```



次は、オペランドの展開をやります。

|    b | オペランドの展開方法 |
| ---: | :------------------- |
|    0 | X,ind                |
|    1 | zpg                  |
|    2 | #                    |
|    3 | abs                  |
|    4 | ind,Y                |
|    5 | zpg,X                |
|    6 | abs,Y                |
|    7 | abs,X                |

bによってこのように決まっています。また、各展開方法は以下のような感じです。（レジスタ名だけを記述しているところは、レジスタ名の値を表しています）

| 識別子 | 名前                 | アセンブリ | 意味                                                    |
| ------ | -------------------- | ---------- | ------------------------------------------------------- |
| A      | Accumulator          | A          | 値Aがオペランド                                         |
| abs    | absolute             | $LLHH      | *$LLHH*番地がオペランド                                 |
| abs,X  | absolute, X-indexed  | $LLHH,X    | *$LLHH+X*番地がオペランド                               |
| abs,Y  | absolute, Y-indexed  | $LLHH,Y    | *$LLHH+Y*番地がオペランド                               |
| #      | immediate            | #$BB       | 値BBがオペランド                                        |
| impl   | implied              |            | オペランド無し                                          |
| ind    | indirect             | ($LLHH)    | *$LLHH番地の値を***16-bitアドレスとして**オペランドに   |
| X,ind  | X-indexed, indirect  | ($LL,X)    | *$00LL+X番地の値を***16-bitアドレスとして**オペランドに |
| ind,Y  | indirect, Y-indexed  | ($LL),Y    | *$00LL番地の値**+Y**を*16-bitアドレスとしてオペランドに |
| rel    | relative             | $BB        | 分岐先は$BB+PC                                          |
| zpg    | zero page            | $LL        | $00LL番地がオペランド                                   |
| zpg,X  | zero page, X-indexed | $LL,X      | $00LL+X番地がオペランド                                 |
| zpg,Y  | zero page, Y-indexed | $LL,Y      | $00LL+Y番地がオペランド                                 |

ちょっとややこしい部分もありますね。

後留意スべき点としては、

> 
>
>**  The available 16-bit address space is conceived as consisting of pages of 256 bytes each, with  address hi-bytes represententing the page index. An increment with carry may affect the hi-byte  and may thus result in a crossing of page boundaries, adding an extra cycle to the execution.  Increments without carry do not affect the hi-byte of an address and no page transitions do occur.  Generally, increments of 16-bit addresses include a carry, increments of zeropage addresses don't.

表でXやYで足し算をするところの話です。実際のメモリは256byte毎にページという単位で区切られていて、足し算をするとページ境界を超えてしまうことがあります。通常は繰り上がり有りで、ゼロページの場合は繰り上がりなしで実装してください、ということでしょうか。ということで、X,ind、ind,Y、zpg,X、zpg,Yは繰り上がり無しで実装します。

```c
u16 tmppc = PC++;
u16 operand = 0;

#define LLHH(l, h) l + h<<8
#define CUT(x) (x)&0xFF
#define LOAD16(x) LLHH(mem[x],mem[x+1])
switch (b) {
	case 0: operand = LOAD16(mem[CUT(tmppc+X)]); break;       // X,ind
	case 1: operand = mem[tmppc]; break;                      // zpg
	case 2: operand = mem[tmppc]; break;                      // #
	case 3: operand = LLHH(mem[tmppc], mem[PC++]); break;     // abs
	case 4: operand = LOAD16(CUT(mem[tmppc]+Y)); break;       // ind,Y
	case 5: operand = CUT(mem[tmppc]+X); break;               // zpg,X
	case 6: operand = LLHH(mem[tmppc], mem[PC++]) + X; break; // abs,Y
	case 7: operand = LLHH(mem[tmppc], mem[PC++]) + X; break; // abs,X
}
```

慎重にオペランドを展開して、、

```c
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
```

a, cで各命令の処理部分に飛ばします。例外も結構あるのですが、それらは余計な計算をしなくて良いようにアドレス計算の前に個別に飛ばしておきます。

## 各命令の実装で若干迷ったところとか

Carry flagとNegative flagの更新に困りました。

http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt



## 実際に動かしてみたい

命令の実装、サイクル数の打ち込み等ができたので、実際に動かしてみたいわけですが、ちょっと待って下さい、**プログラムって何処に格納されるんですか？**

ちょっと調べてみると、どこでも良いみたい...？

$00-ff = zero page  

$100-1ff = stack  

$FFFA-FFFB - NMI vector  

$FFFC-FFFD - RESET vector  

$FFFE-FFFF - IRQ/BRK vector

どこでも良いならとりあえず、0x0200からコピーして置いていきますか、、、[^2]

あと、テストバイナリが必要です。適当なアセンブリを書いて読ませたいので、まずは6502専用のアセンブラを書こうと思います。



## アセンブラを書く

簡単なものをPythonで書きました。



## Notes

[^1]: 何を言っているかわからないと思います。例が早いと思います。例えば、オペコードが二進数で01101001だった場合は011'010'01で分割してa=011,b=010,c=01とするよ！っていうことです。
[^2]: http://hp.vector.co.jp/authors/VA042397/nes/adrmap.html　ココらへんを見ると$8000って書いてあるからそうしました



**集められた情報** 

・命令はlittle-endianで1-byte

・番地は2-byte

・レジスタ

・A  (8-bit) 計算用

・X, Y (8-bit) インデックス

・SP (8-bit) スタックポインタ

・PC (16-bit) プログラムカウンタ

・P (7-bit) CZIDB\ON

・これらレジスタは$01