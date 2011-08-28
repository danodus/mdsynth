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


#include "as.h"
#include "util.h"
#include "ifd.h"

/* IfMachine() --- This function implements the IFD & IFND conditional
                assembly machine.
                version 1.0 made for release TER_2.0 cross assembler 27 Jun 89
*/

#define FALSE_BLOCK     0       /* values for state variable */
#define TRUE_BLOCK      1
#define POP_TEST        2
#define ELSE_TEST       3
#define FALSE           0
#define TRUE            1

void IfMachine(Token)
int Token;      /* input token from calling machine */
                /* See file as.h for definition (globals) */
{
  static int State = TRUE_BLOCK, StackPt = 0, IfStack[MAXIFD];
                /* State variable, pointer to "IF stack pointer" and "Stack" */
  int Hiatus;   /* flag to break out of FSM & resume normal processing */

  Hiatus=FALSE; /* infinite loop to operate machine
                                 until time to break out */
  do    /* test at end */
   {

#ifdef DEBUG3
  printf("IfMachine state=%u , token=%u\n",State,Token);
#endif

   if (State == TRUE_BLOCK)     /* a block of statements processed normally */
     switch(Token) {
        case IF_TRUE:
                IfStack[StackPt]=TRUE;
                if (++StackPt > MAXIFD) {       /* check for over flow */
                        StackPt = MAXIFD;
                        error("Error:IFD/IFND nested too deep");
                        }
                /* print_line() will be done in normal processing */
                Hiatus = TRUE;  /* resume normal line processing */
                break;
        case IF_FALSE:
                IfStack[StackPt]=TRUE;
                if (++StackPt > MAXIFD) {       /* check for over flow */
                        StackPt = MAXIFD;
                        error("Error:IFD/IFND nested too deep");
                        }
                if (Pass == 2 && Lflag && !N_page)      /* need to print here */
                        print_line(); /* cuz will not return to normal */
                Token = GetToken(); /* get next line & examine for IF */
                State = FALSE_BLOCK; /* change state */
                break;
        case IF_ELSE:
                if (StackPt == 0) /* bad IF nesting */
                        error("Error: ELSE without IF");
                if (Pass == 2 && Lflag && !N_page)
                        print_line();
                Token = GetToken(); /* get next line & examine for IF */
                State = FALSE_BLOCK;
                break;
        case IF_ENDIF:
                if (StackPt == 0)       /* bad IF nesting */
                        error("Error: ENDIF without IF");
                else
                        StackPt--; /* popped state must be TRUE */
                 Hiatus = TRUE;
                 break;
        /* case NORMAL is not implemented as it represents normal line
                processing. */
        case IF_END:    /* file ended with improperly nested IFD */
                        /* this code can't actually be reached at present
                           in a TRUE_BLOCK */
                fatal("Error: file ended before IFD/IFND/ELSE/ENDIF");
                break;
        default:        /* This code can't be reached at the present.
                           Logically would happen if EOF but handled
                           else where */
                fatal("Can't get here from there.");
                break;
      }
   else if (State == FALSE_BLOCK)       /* statements not processed */
     switch(Token) {
        case IF_TRUE:   /* doesn't make any diff. Just nest IFs */
        case IF_FALSE:
                IfStack[StackPt]=FALSE;
                if (++StackPt > MAXIFD) {
                        StackPt = MAXIFD;
                        error("Error:IFD/IFND nested too deep");
                        }
                if (Pass == 2 && Lflag && !N_page)
                        print_line();
                Token = GetToken();
                break;
        case IF_ELSE:
             /* if (Pass == 2 && Lflag && !N_page) */
             /*         print_line();              */
                State = ELSE_TEST;
                break;
        case IF_ENDIF:
                State = POP_TEST;
                break;
        case IF_END:    /* file ended with improperly nested IFD */
                fatal("Fatal Error: file ended before last ENDIF");
                        /* Fatal will exit assembler.  Things are too
                           messed up.  Include file handling is else where
                           and don't want to duplicate here. */
                break;
        case IF_NORMAL: /* normal line in a FALSE BLOCK */
                if (Pass == 2 && Lflag && !N_page)
                        print_line();
                Token = GetToken();
                break;
        default:
                fatal("Fatal Error: file ended before last ENDIF");
                        /* must be EOF or something else terrible */
                break;
     }
   else if (State == POP_TEST) {        /* pop up outside nest state */
     if (StackPt == 0) {        /* bad IF nesting */
        error("Error: ENDIF without IF");
        if (Pass == 2 && Lflag && !N_page)
                        print_line();
        State = TRUE;
        }
     else {
        StackPt--;      /* pop stack */
        if (IfStack[StackPt] == TRUE) { /* back to normal */
                /* had to come from FALSE block cuz TRUE block cannot
                        be inside FALSE block */
                if (Pass == 2 && Lflag && !N_page)
                        print_line();
                State = TRUE;
                Hiatus = TRUE;  /* sleep for normal line processing */
                }
        else {  /* gotta be that stack == FALSE, still inside FALSE nest */
                if (Pass == 2 && Lflag && !N_page)
                        print_line();
                State = FALSE;
                Token = GetToken();
                }
          }
     }
   else if (State == ELSE_TEST) {       /* change state */
        if (IfStack[StackPt-1] == TRUE) {
                State = TRUE_BLOCK;
                Hiatus = TRUE;
                }
        else
                State = FALSE_BLOCK;
      }
   }
while (Hiatus == FALSE);        /* loop if not exit */
}


/* GetToken() --- get another line from within False Block and extract token
                as would be done in pseudo.c.  Returns token id:
                        IF_TRUE         IFD/IFND evaluated to TRUE
                        IF_FALSE        IFD/IFND evaluated to FALSE
                        IF_ELSE         ELSE pseudo op
                        IF_ENDIF        ENDIF pseudo op
                        IF_END          END pseudo op
                        IF_NORMAL       none of the above, perhaps assy code
                        IF_EOF          encountered end of file
                This function exists because conditional assembly was added
                as pseudo op rather than with key character ("%") and did
                not want to disturb original software topology */

int GetToken()  /* get another line and find token in it */
{               /* similar to make_pass() except one line at a time */
                /* most variables & constants are global */

#ifndef IBM
  if (FGETS(Line,MAXBUF-1,Fd) == (char *)NULL)
#else
  if (fgets(Line,MAXBUF,Fd) == (char *)NULL)
#endif
        return(IF_EOF); /* banged into eof */
  Line_num++;
  P_force=0;
  N_page=0;
  if (!parse_line())
        return(IF_NORMAL);      /* comment line */
  if (*Op==EOS)                 /* skipping over label, probably. */
        return(IF_NORMAL);      /* strcmp() returns 0 if arg1 is NULL string */

  if (strcmp(Op,"ifd")==0)
        return(eval_ifd());
  else if (strcmp(Op,"ifnd")==0)
        return(eval_ifnd());
  else if (strcmp(Op,"else")==0)
        return(IF_ELSE);
  else if (strcmp(Op,"endif")==0)
        return(IF_ENDIF);
  else if (strcmp(Op,"end")==0)
        return(IF_END);
  else
        return(IF_NORMAL);      /* or might be garbage...but who cares?
                                   This is FALSE BLOCK */
}


/*
*  eval_ifd() --- evaluate an if define statement for TRUE or FALSE
*/

int
eval_ifd()
         {
  struct nlist *np;     /* symbol structure */

        if (*Operand==EOS) {
                warn("No symbol for IFD");
                return(IF_FALSE);       /* nothing to check */
                }
        if ((np=lookup(Operand)) == NULL)
                return(IF_FALSE);       /* not defined at all...yet */
        else
                if(np->def2 == Pass)
                        return(IF_TRUE); /* defined for this pass */

        return(IF_FALSE);       /* not defined this pass */
        }

/*
*  eval_ifnd() --- evaluate an if not define statement for TRUE or FALSE
*/

int
eval_ifnd()
 {
  struct nlist *np;     /* symbol structure */

        if (*Operand==EOS) {
                warn("No symbol for IFND");
                return(IF_TRUE);        /* nothing to check */
                }
        if ((np=lookup(Operand)) == NULL)
                return(IF_TRUE);        /* not defined at all...yet */
        else
                if(np->def2 == Pass)
                        return(IF_FALSE); /* defined for this pass */

        return(IF_TRUE);        /* not defined this pass */
 }
