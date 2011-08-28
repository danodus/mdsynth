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

#include "output.h"
#include "as.h"

/*
 *  stable --- prints the symbol table in alphabetical order
 */
stable(ptr)

struct nlist *ptr;
{
  if (ptr != NULL)
    {
      stable (ptr->Lnext);
        printf ("%-10s %04X\n",ptr->name,ptr->def);
      stable (ptr->Rnext);
    }
}
/*
 *  cross  --  prints the cross reference table
 */
cross(point)

struct nlist *point;
{
struct link *tp;
int i = 1;
  if (point != NULL)
    {
      cross (point->Lnext);
        printf ("%-10s %04X *",point->name,point->def);
         tp = point->L_list;
          while (tp != NULL)
           {
             if (i++>10)
              {
               i=1;
               printf("\n                      ");
              }
              printf ("%04d ",tp->L_num);
               tp = tp->next;
           }
         printf ("\n");
      cross (point->Rnext);
    }
}
