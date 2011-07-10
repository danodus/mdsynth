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
