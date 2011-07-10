/*
 *      fatal --- fatal error handler
 */
fatal(str)
char    *str;
{
 printf("%s\n",str);

#ifdef IBM      /* changed ver TER_2.0 */
 exit(-1);
#else
 exit(10);      /* Amiga & UNIX prefer positive numbers for error
                        minimal error is 10 (no system prob) */
#endif
}

/*
 *      error --- error in a line
 *                      print line number and error
 */
error(str)
char    *str;
{
/* if(N_files > 1)      commented out test for N_files in rel TER_2.0
                        because a single command line source file
                        (which is what N_files counts) can have multiple
                        include source files. */
 printf("%s\n",str);
}
/*
 *      warn --- trivial error in a line
 *                      print line number and error
 */
warn(str)
char    *str;
{
/* if(N_files > 1)      commented out in rel TER_2.0 same reason as above */
 printf("%s, line no.",Argv[Cfn]);      /* current file name */
 printf("%d: ",Line_num);       /* current line number */
 printf("Warning --- %s\n",str);
 Page_lines++;
}


/*
 *      delim --- check if character is a delimiter
 */
delim(c)
char    c;
{
 if( any(c," \t\n"))
  return(YES);
 return(NO);
}

/*
 *      skip_white --- move pointer to next non-whitespace char
 */
char *skip_white(ptr)
char    *ptr;
{
 while(*ptr==BLANK || *ptr==TAB)
  ptr++;
 return(ptr);
}

/*
 *      eword --- emit a word to code file
 */
eword(wd)
int     wd;
{
 emit(hibyte(wd));
 emit(lobyte(wd));
}

/*
 *      emit --- emit a byte to code file
 */
emit(byte)
{
#ifdef DEBUG
 printf("%2x @ %4x\n",byte,Pc);
#endif
 if(Pass==1){
  Pc++;
  return(YES);
  }
 if(P_total < P_LIMIT)
  P_bytes[P_total++] = byte;
 E_bytes[E_total++] = byte;
 Pc++;
 if(E_total == E_LIMIT)
  f_record();
}

/*
 *      f_record --- flush record out in `S1' format
 */
void f_record() /* made void for ANSI C compat ver TER_2.0 6/18/89 */
{
 int     i;
 int     chksum;

 if(Pass == 1)
  return;
 if(E_total==0){
  E_pc = Pc;
  return;
  }
 F_total += E_total;    /* total bytes in file ver (TER)2.01 19 Jun 89 */
 chksum =  E_total+3;    /* total bytes in this record */
 chksum += lobyte(E_pc);
 chksum += E_pc>>8;
 fprintf(Objfil,"S1");   /* record header preamble */
 hexout(E_total+3);      /* byte count +3 */
 hexout(E_pc>>8);        /* high byte of PC */
 hexout(lobyte(E_pc)); /* low byte of PC */
 for(i=0;i<E_total;i++){
  chksum += lobyte(E_bytes[i]);
  hexout(lobyte(E_bytes[i])); /* data byte */
  }
 chksum =~ chksum;       /* one's complement */
 hexout(lobyte(chksum)); /* checksum */
 if (CRflag == 1)       /* test for CRflag added ver TER_1.1 */
  fprintf(Objfil,"%c\n",CR);  /* print in IBM format for some PROM boxes */
 else
  fprintf(Objfil,"\n"); /* this is original statement */
 E_pc = Pc;
 E_total = 0;
}

char    *hexstr = { "0123456789ABCDEF" } ;

hexout(byte)
int     byte;
{

 byte = lobyte(byte);
 fprintf(Objfil,"%c%c",hexstr[byte>>4],hexstr[byte&017]);
}

/*
 *      print_line --- pretty print input line
 */
print_line()
{
 int     i;
 register char *ptr;

        printf ("%04d ",Line_num);
 if(P_total || P_force)
	printf("%04X",Old_pc);
 else
	printf("    ");

 for(i=0;i<P_total && i<6;i++)
	printf(" %02X",lobyte(P_bytes[i]));
 for(;i<6;i++)
  printf("   ");
 printf("  ");

 if(Cflag){
  if(Cycles)
   printf("[%2d ] ",Cycles);
  else
   printf("      ");
  }
 ptr = Line;
 while( *ptr != '\n' )   /* just echo the line back out */
  putchar(*ptr++);
 for(;i<P_total;i++){
  if( i%6 == 0 )
	 printf("\n         ");               /* force data alignment DWC 1-5-92 */
	printf(" %02X",lobyte(P_bytes[i]));
  }
 if (Pflag50 && (++Page_lines>=50))     /* form feed if flag set */
         NewPage();                     /* ver (TER) 2.02 19 Jun 89 */

 if (Pflag75 && (++Page_lines>=75))     /* form feed if flag set */
         NewPage();                     /* ver (DWC) 2.10 8 Oct 2001 */


printf("\n");
}

/*
 *      any --- does str contain c?
 */
any(c,str)
char    c;
char    *str;
{
 while(*str != EOS)
  if(*str++ == c)
   return(YES);
 return(NO);
}

/*
 *      mapdn --- convert A-Z to a-z
 */
char mapdn(c)
char c;
{
 if( c >= 'A' && c <= 'Z')
  return((char)(c+040)); /* cast value to char for ANSI C, ver TER_2.0 */
 return(c);
}

/*
 *      lobyte --- return low byte of an int
 */
lobyte(i)
int i;
{
 return(i&0xFF);
}
/*
 *      hibyte --- return high byte of an int
 */
hibyte(i)
int i;
{
 return((i>>8)&0xFF);
}

/*
 *      head --- is str2 the head of str1?
 */
head(str1,str2)
char *str1,*str2;
{
 while( *str1 != EOS && *str2 != EOS){
  if( *str1 != *str2 )break;
  str1++;
  str2++;
  }
 if(*str1 == *str2)return(YES);
 if(*str2==EOS)
  if( any(*str1," \t\n,+-];*") )return(YES);
 return(NO);
}

/*
 *      alpha --- is character a legal letter
 */
alpha(c)
char c;
{
 if( c<= 'z' && c>= 'a' )return(YES);
 if( c<= 'Z' && c>= 'A' )return(YES);
 if( c== '_' )return(YES);
 if( c== '.' )return(YES);
 return(NO);
}
/*
 *      alphan --- is character a legal letter or digit
 */
alphan(c)
char c;
{
 if( alpha(c) )return(YES);
 if( c<= '9' && c>= '0' )return(YES);
 if( c == '$' )return(YES);      /* allow imbedded $ */
 if( c == '@' )return(YES);     /* allow imbedded @, added ver TER_2.0
                                   added to permit redefinable variables */
 return(NO);
}

/*
 * white --- is character whitespace?
 */
white(c)
char c;
{
 if( c == TAB || c == BLANK || c == '\n' )return(YES);
 return(NO);
}

/*
 * alloc --- allocate memory
 */
char *
alloc(nbytes)
int nbytes;
{
 return(malloc(nbytes));
}

/*
 * FNameGet --- Find a file name <file> or "file" in the Operand string
        added ver TER_2.0 6/17/89
        note: this routine will return a file name with white
        space if between delimeters.  This is permitted in
        AmigaDOS.  Other DOS may hiccup or just use name up to white space
 */

int
FNameGet(NameString)
char *NameString;       /* pointer to output string */
{
 char *frompoint;       /* pointers to input string, don't bash Operand */
 char *topoint;         /* pointer to output string, don't bash NameString */

 frompoint=Operand;     /* include file name is in parsed string Operand */
 topoint=NameString;    /* copy of pointer to increment in copying */
 if (*frompoint != '<' && *frompoint != '"') return(0); /* bad syntax */
 frompoint++;   /* skip < or " */

 while (*frompoint != '>' && *frompoint != '"') /* look for delimeter */
  {
   if (*frompoint == EOS) return(0);    /* missing delimeter */
   *topoint++ = *frompoint++;   /* copy path & file name for DOS */
  }

 *topoint=EOS;  /* terminate file name */
#ifdef DEBUG2
 printf("FNameGet: file name=%s\n",NameString);
#endif
 return(1);     /* proper syntax anyway */
}

/*
 * --- strsave()  find a place to save a string & return pointer to it
                added ver TER_2.0 6/18/89  function taken from
                Kernighan & Ritchie 78
 */
char *strsave(s)
char *s;
{
 char *p;

 if ((p = alloc(strlen(s)+1)) != NULL)
        strcpy(p,s);
 return(p);
}

/*
 * pouterror() ---- print out standard error header
                        added rel TER_2.0 6/18/89
 */

void pouterror()
{
    printf("%s, line no. ",Argv[Cfn]);  /* current file name */
    printf("%d: ",Line_num);    /* current line number */
        /* NOTE: THERE IS NO \n ! Calling procedure supplies suffixing
           error message and \n. viz. file pseudo.c procedure do_pseudo
           in case INCLUDE. Note also that error count is incremented.  */
    Err_count++;
    Page_lines++;       /* increment lines per page */
}

/*
 * New Page() --- form feed a new page, print heading & inc page number
                Moved here from do_pseudo (pseudo.c) in ver (TER) 2.02
                19 Jun 89 so that can call from print_line() as well
                for p50 option.
 */

void NewPage()
{
 Page_lines = 0;        /* ver TER_2.08 so that OPT PAGE works */
 printf ("\n\f\n");
 printf ("%-10s",Argv[Cfn]);
 printf ("                                   ");
 printf ("                                   ");
 printf ("page %3d\n",Page_num++);
}


/*
* LastChar() ----- return a pointer to the last character in a string
                Exception: will return garbage if NULL string
*/

char *LastChar(strpt)
char *strpt;    /* pointer to string to be examined */
{
 char *c;

 c=strpt;       /* don't zap original */
 while (*c != EOS)      /* search for end */
        c++;
 return(--c);   /* back up one to last character */
}
