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

/*
 *      eval --- evaluate expression
 *
 *      an expression is constructed like this:
 *
 *      expr ::=  expr + term |
 *                expr - term ;
 *                expr * term ;
 *                expr / term ;
 *                expr | term ;
 *                expr & term ;
 *                expr % term ;
 *                expr ^ term ;
 *
 *      term ::=  symbol |
 *                * |
 *                constant ;
 *
 *      symbol ::=  string of alphanumerics with non-initial digit
 *
 *      constant ::= hex constant |
 *                   binary constant |
 *                   octal constant |
 *                   decimal constant |
 *                   ascii constant;
 *
 *      hex constant ::= '$' {hex digits};
 *
 *      octal constant ::= '@' {octal digits};
 *
 *      binary constant ::= '%' { 1 | 0 };
 *
 *      decimal constant ::= {decimal digits};
 *
 *      ascii constant ::= ''' any printing char;
 *
 */
eval()
{
        int     left,right;     /* left and right terms for expression */
        char    o;              /* operator character */

#ifdef DEBUG
        printf("Evaluating %s\n",Optr);
#endif
        Force_byte = NO;
        Force_word = NO;
        if(*Optr=='<'){
                Force_byte++;
                Optr++;
                }
        else if(*Optr=='>'){
                Force_word++;
                Optr++;
                }
        left = get_term();         /* pickup first part of expression */

        while( is_op(*Optr)){
                o = *Optr++; /* pickup connector and skip */
                right = get_term();     /* pickup current rightmost side */
                switch(o){
                        case '+': left += right; break;
                        case '-': left -= right; break;
                        case '*': left *= right; break;
                        case '/': left /= right; break;
                        case '|': left |= right; break;
                        case '&': left &= right; break;
                        case '%': left %= right; break;
                        case '^': left = left^right; break;
                        }
                }

        Result= left;
#ifdef DEBUG
        printf("Result=%x\n",Result);
        printf("Force_byte=%d  Force_word=%d\n",Force_byte,Force_word);
#endif
        return(YES);
}

/*
 *      is_op --- is character an expression operator?
 */
is_op(c)
char c;
{
        if( any(c,"+-*/&%|^"))
                return(YES);
        return(NO);
}


/*
 *      get_term --- evaluate a single item in an expression
 */
get_term()
{
        char    hold[MAXBUF];
        char    *tmp;
        int     val = 0;        /* local value being built */
        int     minus;          /* unary minus flag */
        struct nlist *pointer;
        struct link *pnt,*bpnt;

        if( *Optr == '-' ){
                Optr++;
                minus =YES;
                }
        else
                minus = NO;

        while( *Optr == '#' ) Optr++;

        /* look at rest of expression */

        if(*Optr=='%'){ /* binary constant */
                Optr++;
                while( any(*Optr,"01"))
                        val = (val * 2) + ( (*Optr++)-'0');
                }
        else if(*Optr=='@'){ /* octal constant */
                Optr++;
                while( any(*Optr,"01234567"))
                        val = (val * 8) + ((*Optr++)-'0');
                }
        else if(*Optr=='$'){ /* hex constant */
                Optr++;
                while( any(*Optr,"0123456789abcdefABCDEF"))
                        if( *Optr > '9' )
                                val = (val * 16) + 10 + (mapdn(*Optr++)-'a');
                        else
                                val = (val * 16) + ((*Optr++)-'0');
                }
        else if( any(*Optr,"0123456789")){ /* decimal constant */
                while(*Optr >= '0' && *Optr <= '9')
                        val = (val * 10) + ( (*Optr++)-'0');
                }
        else if(*Optr=='*'){    /* current location counter */
                Optr++;
                val = Old_pc;
                }
        else if(*Optr=='\''){   /* character literal */
                Optr++;
                if(*Optr == EOS)
                        val = 0;
                else
                        val = *Optr++;
                }
        else if( alpha(*Optr) ){ /* a symbol */
                tmp = hold;     /* collect symbol name */
                while(alphan(*Optr))
                        *tmp++ = *Optr++;
                *tmp = EOS;
                pointer = lookup(hold);
                  if (pointer != NULL)
                   {
                   if (Pass == 2)
                    {
                       pnt = pointer->L_list;
                        bpnt = NULL;
                        while (pnt != NULL)
                         {
                           bpnt = pnt;
                           pnt = pnt->next;
                         }
                        pnt = (struct link *) alloc(sizeof(struct link));
                      if (bpnt == NULL)
                       pointer->L_list = pnt;
                      else bpnt->next = pnt;
                     pnt->L_num = Line_num;
                    pnt->next = NULL;
                    }
                       val = Last_sym;
                   }
                else{
                        if(Pass==1){    /* forward ref here */
                                fwdmark();
                                if( !Force_byte )
                                        Force_word++;
                                val = 0;
                                }
                        else    /* added ver TER_2.0  2 Jul 89 */
                                error("Symbol undefined Pass 2");
                        }
                if(Pass==2 && Line_num==F_ref && Cfn==Ffn){
                        if( !Force_byte  )
                                Force_word++;
                        fwdnext();
                        }
                }
        else
                /* none of the above */
                val = 0;

        if(minus)
                return(-val);
        else
                return(val);
}
