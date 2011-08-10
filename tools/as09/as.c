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

/* Adaptation of the as09 tool from the System09 project. */

#include "as.h"

/* global variables */
int     Line_num =0;            /* current line number          */
int     Err_count =0;           /* total number of errors       */
char    Line[MAXBUF] = {0};     /* input line buffer            */
char    Label[MAXLAB] = {0};    /* label on current line        */
char    Op[MAXOP] = {0};        /* opcode mnemonic on current line      */
char    Operand[MAXBUF] = {0};  /* remainder of line after op           */
                                /* (up to ';' rel TER_2.0) */
char    *Optr =0;               /* pointer into current Operand field   */
int     Result =0;              /* result of expression evaluation      */
int     Force_word =0;          /* Result should be a word when set     */
int     Force_byte =0;          /* Result should be a byte when set     */
int     Pc =0;                  /* Program Counter              */
int     Old_pc =0;              /* Program Counter at beginning */

int     Last_sym =0;            /* result of last lookup        */

int     Pass =0;                /* Current pass #               */
int     N_files =0;             /* Number of files to assemble  */
FILE    *Fd =0;                 /* Current input file structure */
int     Cfn =0;                 /* Current file number 1...n    */
int     Ffn =0;                 /* forward ref file #           */
int     F_ref =0;               /* next line with forward ref   */
char    **Argv =0;              /* pointer to file names        */

int     E_total =0;             /* total # bytes for one line   */
int     E_bytes[E_LIMIT] = {0}; /* Emitted held bytes           */
int     E_pc =0;                /* Pc at beginning of collection*/

int     Lflag = 0;              /* listing flag 0=nolist, 1=list*/

int     P_force = 0;            /* force listing line to include Old_pc */
int     P_total =0;             /* current number of bytes collected    */
int     P_bytes[P_LIMIT] = {0}; /* Bytes collected for listing  */

int     Cflag = 0;              /* cycle count flag */
int     Cycles = 0;             /* # of cycles per instruction  */
long    Ctotal = 0;             /* # of cycles seen so far */
int     Sflag = 0;              /* symbol table flag, 0=no symbol */
int     N_page = 0;             /* new page flag */
int     Page_num = 2;           /* page number */
int     CREflag = 0;            /* cross reference table flag */
int     CRflag = 0;             /* flag to add <CR><LF> to S record */
                                /* added ver TER_1.1 June 3, 1989 */
int     nfFlag = 1;             /* if=1 number INCLUDE files separate */
                                /* ver TER_2.0 June 17, 1989 */
int     FdCount = 0;            /* INCLUDE files "stack" pointer */
                                /* ver TER_2.0 June 17, 1989 */
char    InclFName[MAXBUF]={0};  /* current INCLUDE file name */
int     F_total = 0;            /* total bytes emitted in S file */
                                /* ver (TER) 2.02 19 Jun 89 */
int     Page_lines = 0;         /* total lines this page */
                                /* ver (TER) 2.02 19 Jun 89 */
int     Pflag50 = 0;            /* if 1 then form feed every 50 lines */
                                /* ver (TER) 2.02 19 Jun 89 */
int     Pflag75 = 0;            /* if 1 then form feed every 75 lines */
                                /* ver (DWC) 2.10 8 Oct 2001 */
int     PC_Save[4] = {0,0,0,0}; /* saved contents of CODE,DATA,BSS,AUTO PCs */
                                /* ver TER_2.09 25 July 89 */
int     PC_ptr = 0;             /* index or pointer to current PC */
                        /* initialized to CODE ver TER_2.09 25 July 89 */

struct  nlist *root;            /* root node of the tree */

FILE    *Objfil =0;             /* object file's file descriptor*/
char    Obj_name[] = {"             "};
