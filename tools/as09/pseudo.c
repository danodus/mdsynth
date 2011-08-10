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

#include "pseudo.h"

#include <stdio.h>

struct oper pseudo[NPSE] = {
"=",    PSEUDO, EQU,    0,      /* ver TER_2.09 25 Jul 89 */
"auto", PSEUDO, AUTO,   0,      /* ver TER_2.09 25 Jul 89 */
"bss",  PSEUDO, BSS,    0,      /* ver TER_2.09 25 Jul 89 */
"bsz",  PSEUDO, ZMB,    0,
"code", PSEUDO, CODE,   0,      /* ver TER_2.09 25 Jul 89 */
"data", PSEUDO, DATA,   0,      /* ver TER_2.09 25 Jul 89 */
"else", PSEUDO, ELSE,   0,      /* ver TER_2.0 6/17/89 */
"end",  PSEUDO, END,    0,      /* ver TER_2.0 6/17/89 */
"endif",PSEUDO, ENDIF,  0,      /* ver TER_2.0 6/17/89 */
"equ",  PSEUDO, EQU,    0,
"fcb",  PSEUDO, FCB,    0,
"fcc",  PSEUDO, FCC,    0,
"fdb",  PSEUDO, FDB,    0,
"fill", PSEUDO, FILL,   0,
"ifd",  PSEUDO, IFD,    0,      /* ver TER_2.0 6/17/89 */
"ifnd", PSEUDO, IFND,   0,      /* ver TER_2.0 6/17/89 */
"include", PSEUDO, INCLUDE, 0,  /* ver TER_2.0 6/17/89 */
"nam",  PSEUDO, NULL_OP,0,
"name", PSEUDO, NULL_OP,0,
"opt",  PSEUDO, OPT,    0,
"org",  PSEUDO, ORG,    0,
"pag",  PSEUDO, PAGE,   0,
"page", PSEUDO, PAGE,   0,
"ram",  PSEUDO, BSS,    0,      /* ver TER_2.09 25 Jul 89 */
"rmb",  PSEUDO, RMB,    0,
"spc",  PSEUDO, NULL_OP,0,
"ttl",  PSEUDO, NULL_OP,0,
"zmb",  PSEUDO, ZMB,    0
};

/*
 *      do_pseudo --- do pseudo op processing
 */
do_pseudo(op)
int op; /* which op */
{
        char    fccdelim, *strsave();
        int     fill;
        int c;  /*test variable  ver TER_2.0 6/18/89 */
        char    *skip_white(), *savept; /* savept is pointer to string save */
        FILE    *FdTemp, *fopen();      /* ver TER_2.0 6/17/89 */
        void    IfMachine(); /* rel TER_2.0 6/18/89 */
        void    PC_Exchange();  /* ver TER_2.09 25 Jul 89 */

        if( op != EQU && *Label )
                install(Label,Pc);

        P_force++;

#ifdef DEBUG3
    printf("%s, line no. ",Argv[Cfn]);  /* current file name */
    printf("%d: ",Line_num);    /* current line number */
    printf(" Pseudo Op=%u\n",op);
#endif

        switch(op){
                case RMB:                       /* reserve memory bytes */
                        if( eval() ){
                                Pc +=  Result;
                                f_record();     /* flush out bytes */
                                }
                        else
                                error("Undefined Operand during Pass One");
                        break;
                case ZMB:                       /* zero memory bytes */
                        if( eval() )
                                while( Result-- )
                                        emit(0);
                        else
                                error("Undefined Operand during Pass One");
                        break;
                case FILL:                      /* fill memory with constant */
                        eval();
                        fill = Result;
                        if( *Optr++ != ',' )
                                error("Bad fill");
                        else{
                                Optr = skip_white(Optr);
                                eval();
                                while( Result-- )
                                        emit(fill);
                                }
                        break;
                case FCB:                       /* form constant byte(s) */
                        do{
                                Optr = skip_white(Optr);
                                eval();
                                if( Result > 0xFF ){
                                        if(!Force_byte)
                                                warn("Value truncated");
                                        Result = lobyte(Result);
                                        }
                                emit(Result);
                        }while( *Optr++ == ',' );
                        break;
                case FDB:                       /* form double byte(s) */
                        do{
                                Optr = skip_white(Optr);
                                eval();
                                eword(Result);
                        }while( *Optr++ == ',' );
                        break;
                case FCC:                       /* form constant characters */
                        if(*Operand==EOS)
                                break;
                        fccdelim = *Optr++;
                        while( *Optr != EOS && *Optr != fccdelim)
                                emit(*Optr++);
                        if(*Optr == fccdelim)
                                Optr++;
                        else
                                error("Missing Delimiter");
                        break;
                case ORG:                       /* origin */
                        if( eval() ){
                                Old_pc = Pc = Result;
                                f_record();     /* flush out any bytes */
                                }
                        else
                                error("Undefined Operand during Pass One");
                        break;
                case EQU:                       /* equate */
                        if(*Label==EOS){
                                error("EQU requires label");
                                break;
                                }
                        if( eval() ){
                                install(Label,Result);
                                Old_pc = Result;        /* override normal */
                                }
                        else
                                error("Undefined Operand during Pass One");
                        break;
                case OPT:                       /* assembler option */
                        P_force=0;
                        if( head(Operand,"l") )
                                Lflag=1;
                        else if (head(Operand,"nol"))
                                Lflag=0;
                        else if (head(Operand,"c")){
                                Cflag=1;
                                Ctotal=0;
                                }
                        else if (head(Operand,"noc"))
                                Cflag=0;
                        else if (head(Operand,"contc")){
                                Cflag=1;
                                }
                        else if (head(Operand,"s"))
                                Sflag = 1;
                        else if (head(Operand,"cre"))
                                CREflag = 1;
                        else if (head(Operand,"p50")){  /* turn on 50 lines/page flag */
                                Pflag50 = 1;
                                Pflag75 = 0;
                                }																	/* separately. ver TER_2.0 6/17/89 */
                        else if (head(Operand,"p75")){  /* turn on 75 lines/page flag */
                                Pflag50 = 0;
                                Pflag75 = 1;
                                }																	/* separately. ver TER_2.0 6/17/89 */
                        else if (head(Operand,"crlf"))  /* add <CR> <LF> to  */
				CRflag = 1;             /* S record ver TER_2.08 */
                        else if (head(Operand,"nnf"))   /* no extra line no. */
				nfFlag = 0;             /* w include files ver TER_2.08 */
                        else
                                error("Unrecognized OPT");
                        break;
                case PAGE:                      /* go to a new page */
                        P_force=0;
                        N_page = 1;
                        if (Pass == 2 )
                         if (Lflag) NewPage();
                        break;
                case NULL_OP:                   /* ignored psuedo ops */
                        P_force=0;
                        break;
                case INCLUDE:   /* case INCLUDE added ver TER_2.0 6/17/89 */
                        P_force=0;      /* no PC in printed output */
                        if ((c=FNameGet(InclFName))==0)
                          error("Improper INCLUDE statement");
                        else
                        {
                        if (FdCount > MAXINCFILES)
                          error("too many INCLUDE files");
                        else
                         {
                          if ((FdTemp = fopen(InclFName,"r"))==0)
                           {
                            printf("%s, line no. ",Argv[Cfn]);
                                        /* current file name */
                            printf("%d: ",Line_num); /* current line number */
                            Page_lines++;       /* increment lines per page */
                printf("warning:can't open INCLUDE file %s\n",InclFName);
                           }
                          else
                           {
                            if ((savept=strsave(InclFName))==0)
                              error("out of memory for INCLUDE file name");
                            else
                            {
                            InclFiles[FdCount].fp=Fd;   /* save current fp */
                            if (nfFlag)
                             {
                              InclFiles[FdCount].line_num=Line_num;
                                        /* save current line count */
                              Line_num=0;       /* reset for new file */
                             }
                            InclFiles[FdCount].name=Argv[Cfn];
                                /* save pointer to current name */
                            Argv[Cfn]=savept;
                                /* now replace pointer to current name with
                                   pointer to name of Include file */
                            Fd=FdTemp;  /* now replace current file with
                                           INCLUDE fp */
                            FdCount++;  /* and increment "stack" */
#ifdef DEBUG2
        printf("pseudo INCLUDE: FdCount=%d\n",FdCount);
        printf("  new input file pointer=%d\n",Fd);
        printf("  new file name=%s\n",Argv[Cfn]);
#endif
                            }
                           }
                         }
                        }
                        break;
                case END:
                        P_force=0;
                        if (FdCount>0)  /* skip END statements in files
                                           received from CLI arguments */
                         {
                          fclose(Fd);   /* close file from this level nest */
                          FdCount--;    /* "pop stack" */
                          Fd=InclFiles[FdCount].fp;     /* restore fp from
                                        nested stack */
                          if(nfFlag) Line_num=InclFiles[FdCount].line_num;
                          Argv[Cfn]=InclFiles[FdCount].name;
                                /* restore file name pointer */
#ifdef DEBUG2
        printf("pseudo END: FdCount=%d\n",FdCount);
        printf("  new input file pointer=%d\n",Fd);
        printf("  new file name=%s\n",Argv[Cfn]);
#endif
                         }
                        break;
                case IFD:
#ifdef DEBUG3
                        printf("case IFD: in pseudo\n");
#endif
                        P_force=0;
                        c=eval_ifd();
                        IfMachine(c);
                        break;
                case IFND:
                        P_force=0;
                        c=eval_ifnd();
                        IfMachine(c);
                        break;
                case ELSE:
                        P_force=0;
                        IfMachine(IF_ELSE);
                        break;
                case ENDIF:
                        P_force=0;
                        IfMachine(IF_ENDIF);
                        break;
                case CODE:      /* CODE,DATA,BSS,AUTO ver TER_2.09 */
                        PC_Exchange(0);
                        break;
                case DATA:
                        PC_Exchange(1);
                        break;
                case BSS:
                        PC_Exchange(2);
                        break;
                case AUTO:
                        PC_Exchange(3);
                        break;
                default:
                        fatal("Pseudo error");
                        break;
                }
}


/*
*       PC_Exchange --- Save current PC and recover requested one
                added ver TER_2.09
*/
void PC_Exchange(PC_ptr_new)
int PC_ptr_new; /* request 0=CODE,1=DATA,2=BSS */
  {
  P_force = 0;  /* no PC in output cuz wrong first time (?) */
  PC_Save[PC_ptr] = Pc; /* save current PC */
  PC_ptr = PC_ptr_new;  /* remember which one we're using */
  Old_pc = Pc = PC_Save[PC_ptr];  /* recover the one requested */
  f_record();   /* flush out any bytes, this is really an ORG */
  }
