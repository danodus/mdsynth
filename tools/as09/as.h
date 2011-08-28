/*
    Copyright (c) 2011, Meldora Inc.
	Copyright (c) 2004, Motorola Inc.
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following
    conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer
      in the documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
    OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
    OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef __as_h__
#define __as_h__

/*
 *      machine independent definitions and global variables
 */

#include <stdio.h>

#define YES     1
#define NO      0
#define ERR     (-1)

#define MAXBUF  128
#define MAXOP   10      /* longest mnemonic */
#define MAXLAB  16
#define E_LIMIT 32
#define P_LIMIT 64
#define MAXINCFILES  30 /* nestinf levels for  INCLUDE files at one time */
                        /* ver TER_2.0 6/17/89 */
#define MAXIFD    30    /* max nesting levels for IFD/IFND statements */
                        /* ver TER_2.0 2 Jul 89 */
#define IF_FALSE  0     /* tokens for IfMachine (conditional assembly) */
#define IF_TRUE   1     /* added ver TER_2.0 27Jun89 */
#define IF_ELSE   2
#define IF_ENDIF  3
#define IF_NORMAL 4
#define IF_END    5
#define IF_EOF    6     /* end tokens */

/*      Character Constants     */
#define NEWLINE '\n'
#define CR      0x0D    /* <CR> or ^M ver TER_1.1 June 3, 1989 */
#define TAB     '\t'
#define BLANK   ' '
#define EOS     '\0'

/*      Opcode Classes          */
#define INH     0       /* Inherent                     */
#define GEN     1       /* General Addressing           */
#define IMM     2       /* Immediate only               */
#define REL     3       /* Short Relative               */
#define P2REL   4       /* Long Relative                */
#define P1REL   5       /* Long Relative (LBRA and LBSR)*/
#define NOIMM   6       /* General except for Immediate */
#define P2GEN   7       /* Page 2 General               */
#define P3GEN   8       /* Page 3 General               */
#define RTOR    9       /* Register To Register         */
#define INDEXED 10      /* Indexed only                 */
#define RLIST   11      /* Register List                */
#define P2NOIMM 12      /* Page 2 No Immediate          */
#define P2INH   13      /* Page 2 Inherent              */
#define P3INH   14      /* Page 3 Inherent              */
#define GRP2    15      /* Group 2 (Read/Modify/Write)  */
#define LONGIMM 16      /* Immediate mode takes 2 bytes */
#define BTB     17      /* Bit test and branch          */
#define SETCLR  18      /* Bit set or clear             */
#define CPD     19      /* compare d               6811 */
#define XLIMM   20      /* LONGIMM for X           6811 */
#define XNOIMM  21      /* NOIMM for X             6811 */
#define YLIMM   22      /* LONGIMM for Y           6811 */
#define YNOIMM  23      /* NOIMM for Y             6811 */
#define FAKE    24      /* convenience mnemonics   6804 */
#define APOST   25      /* A accum after opcode    6804 */
#define BPM     26      /* branch reg plus/minus   6804 */
#define CLRX    27      /* mvi x,0                 6804 */
#define CLRY    28      /* mvi y,0                 6804 */
#define LDX     29      /* mvi x,expr              6804 */
#define LDY     30      /* mvi y,expr              6804 */
#define MVI     31      /* mvi                     6804 */
#define EXT     32      /* extended                6804 */
#define BIT     33      /* bit manipulation        6301 */
#define SYS     34      /* syscalls (really swi)        */
#define PSEUDO  35      /* Pseudo ops                   */

/* global variables */
extern int     Line_num;            /* current line number          */
extern int     Err_count;           /* total number of errors       */
extern char    Line[MAXBUF];     /* input line buffer            */
extern char    Label[MAXLAB];    /* label on current line        */
extern char    Op[MAXOP];        /* opcode mnemonic on current line      */
extern char    Operand[MAXBUF];  /* remainder of line after op           */
                                /* (up to ';' rel TER_2.0) */
extern char    *Optr;               /* pointer into current Operand field   */
extern int     Result;              /* result of expression evaluation      */
extern int     Force_word;          /* Result should be a word when set     */
extern int     Force_byte;          /* Result should be a byte when set     */
extern int     Pc;                  /* Program Counter              */
extern int     Old_pc;              /* Program Counter at beginning */

extern int     Last_sym;            /* result of last lookup        */

extern int     Pass;                /* Current pass #               */
extern int     N_files;             /* Number of files to assemble  */
extern FILE    *Fd;                 /* Current input file structure */
extern int     Cfn;                 /* Current file number 1...n    */
extern int     Ffn;                 /* forward ref file #           */
extern int     F_ref;               /* next line with forward ref   */
extern char    **Argv;              /* pointer to file names        */

extern int     E_total;             /* total # bytes for one line   */
extern int     E_bytes[E_LIMIT]; /* Emitted held bytes           */
extern int     E_pc;                /* Pc at beginning of collection*/

extern int     Lflag;              /* listing flag 0=nolist, 1=list*/

extern int     P_force;            /* force listing line to include Old_pc */
extern int     P_total;             /* current number of bytes collected    */
extern int     P_bytes[P_LIMIT]; /* Bytes collected for listing  */

extern int     Cflag;              /* cycle count flag */
extern int     Cycles;             /* # of cycles per instruction  */
extern long    Ctotal;             /* # of cycles seen so far */
extern int     Sflag;              /* symbol table flag, 0=no symbol */
extern int     N_page;             /* new page flag */
extern int     Page_num;           /* page number */
extern int     CREflag;            /* cross reference table flag */
extern int     CRflag;             /* flag to add <CR><LF> to S record */
                                /* added ver TER_1.1 June 3, 1989 */
extern int     nfFlag;             /* if=1 number INCLUDE files separate */
                                /* ver TER_2.0 June 17, 1989 */
extern int     FdCount;            /* INCLUDE files "stack" pointer */
                                /* ver TER_2.0 June 17, 1989 */
extern char    InclFName[MAXBUF];  /* current INCLUDE file name */
extern int     F_total;            /* total bytes emitted in S file */
                                /* ver (TER) 2.02 19 Jun 89 */
extern int     Page_lines;         /* total lines this page */
                                /* ver (TER) 2.02 19 Jun 89 */
extern int     Pflag50;            /* if 1 then form feed every 50 lines */
                                /* ver (TER) 2.02 19 Jun 89 */
extern int     Pflag75;            /* if 1 then form feed every 75 lines */
                                /* ver (DWC) 2.10 8 Oct 2001 */
extern int     PC_Save[4]; /* saved contents of CODE,DATA,BSS,AUTO PCs */
                                /* ver TER_2.09 25 July 89 */
extern int     PC_ptr;             /* index or pointer to current PC */
                        /* initialized to CODE ver TER_2.09 25 July 89 */

struct link { /* linked list to hold line numbers */
       int L_num; /* line number */
       struct link *next; /* pointer to next node */
};

struct nlist { /* basic symbol table entry */
        char    *name;
        int     def;    /* value of symbol, assigned by user */
        int     def2;   /* value assign by assembler, e.g. defined this pass? */
                        /* added ver TER_2.0 4 Jul 89 */
        struct nlist *Lnext ; /* left node of the tree leaf */
        struct nlist *Rnext; /* right node of the tree leaf */
        struct link *L_list; /* pointer to linked list of line numbers */
};

struct oper {   /* an entry in the mnemonic table */
        char    *mnemonic;      /* its name */
        char    class;          /* its class */
        int     opcode;         /* its base opcode */
        char    cycles;         /* its base # of cycles */
};

extern struct  nlist *root;            /* root node of the tree */

extern FILE    *Objfil;             /* object file's file descriptor*/
extern char    Obj_name[];

struct  InclFile {      /* The INCLUDE files nesting "stack" */
                        /* added ver TER_2.0 6/17/89 */
        FILE    *fp;            /* saved file pointer (next level up) */
        int     line_num;       /* saved line number count */
        char    *name;  /* saved file name */
                  };

struct  InclFile InclFiles[MAXINCFILES];        /* the nesting stack itself */

/* defined in doXX.c */
void do_op(int opcode,int class);

/* defined in symtab.c */
struct nlist * lookup(char *name);

/* defined in util.c */
void pouterror();
void f_record();
void NewPage();

initialize();
re_init();
make_pass();
char *FGETS(s, n, iop);
parse_line();
process();

#endif // __as_h__
