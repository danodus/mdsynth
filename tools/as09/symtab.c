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

#include "symtab.h"

#include "as.h"
#include "table09.h"
#include "pseudo.h"

/*
 *      install --- add a symbol to the table
 */
install(str,val)
char    *str;
int     val;
{
        struct link *lp;
 struct nlist *np,*p,*backp;
 int     i;
 char *LastChar();      /* ver TER_2.0 */

 if( !alpha(*str) ){
  error("Illegal Symbol Name");
  return(NO);
  }
 if( (np = lookup(str)) != NULL ){
  if (*LastChar(str) == '@') {  /* redefinable symbol  ver TER_2.0 */
        np->def = val;          /* redefined */
        return(YES);
        }
  if( Pass==2 ){
   np->def2 = 2;        /* defined for this pass=Pass  ver TER_2.0 */
   if( np->def == val )
    return(YES);
   else{
    error("Phasing Error");
    return(NO);
    }
   }
  error("Symbol Redefined");
  return(NO);
  }
 /* enter new symbol */
#ifdef DEBUG
 printf("Installing %s as %d\n",str,val);
#endif
 np = (struct nlist *) alloc(sizeof(struct nlist));
 if( np == (struct nlist *)ERR ){
  error("Symbol table full");
  return(NO);
  }
 np->name = alloc(strlen(str)+1);
 if( np->name == (char *)ERR ){
  error("Symbol table full");
  return(NO);
  }
 strcpy(np->name,str);
 np->def = val;
 np->def2 = 1;  /* defined for this pass=Pass  ver TER_2.0 */
 np->Lnext = NULL;
        np->Rnext = NULL;
           lp = (struct link *) alloc(sizeof(struct link));
           np->L_list = lp;
           lp->L_num = Line_num;
           lp->next = NULL;
        p = root;
          backp = NULL;
           while (p != NULL)
            {
              backp = p;
              i = strcmp (str,p->name);
               if (i<0)
                   p=p->Lnext;
                  else p=p->Rnext;
            }
          if (backp == NULL)
              root = np;
             else if (strcmp(str,backp->name)<0)
                  backp->Lnext = np;
                 else backp->Rnext = np;
          return (YES);
}

/*
 *      lookup --- find string in symbol table
 */
struct nlist *
lookup(char *name)
{
 struct nlist *np;
 int     i;
 char   *c;     /* ver TER_2.0 */

 c = name;      /* copy symbol pointer */
 while(alphan(*c))      /* search for end of symbol */
  c++;  /* next char */
 *c = EOS;      /* disconnect anything after for good compare */
                /* end of mods for ver TER_2.0 */

        np = root;      /* and now go on and look for symbol */
         while (np != NULL)
          {
            i = strcmp(name,np->name);
             if (i == 0)
               {
                 Last_sym = np->def;
                 return (np);
               }
             else if (i < 0)
                 np = np->Lnext;
                else np = np->Rnext;
          }
      Last_sym = 0;
/*      if (Pass == 2)
        error ("symbol Undefined on pass 2"); */
                /* commented out ver TER_2.0 2 Jul 89 */
                /* function used in IFD */
        return (NULL);
}


/*
 *      mne_look --- mnemonic lookup
 *
 *      Return pointer to an oper structure if found.
 *      Searches both the machine mnemonic table and the pseudo table.
 */
struct oper *
mne_look(str)
char    *str;
{
 struct oper *low,*high,*mid;
 int     cond;

 /* Search machine mnemonics first */
 low =  &table[0];
 high = &table[ NMNE-1 ];
 while (low <= high){
  mid = low + (high-low)/2;
  if( ( cond = strcmp(str,mid->mnemonic)) < 0)
   high = mid - 1;
  else if (cond > 0)
   low = mid + 1;
  else
   return(mid);
 }

 /* Check for pseudo ops */
 low =  &pseudo[0];
 high = &pseudo[ NPSE-1 ];
 while (low <= high){
  mid = low + (high-low)/2;
  if( ( cond = strcmp(str,mid->mnemonic)) < 0)
   high = mid - 1;
  else if (cond > 0)
   low = mid + 1;
  else
   return(mid);
 }

 return(NULL);
}
