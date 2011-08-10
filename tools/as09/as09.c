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

/* This is an adaptation of the public domain tool as09 from System09. */ 

#include "as.h"

#include <stdlib.h>
#include <string.h>

char mapdn();
char *alloc();
/*
* as --- cross assembler main program

Note: Compile with define DEBUG for function module debug
statements.  Compile with define DEBUG2 for version 2 debug
statements only.  Compile with define IBM to use original, non-
Amiga, non-MAC, non-UNIX fgets() function.  Amiga version will
accept IBM generated source code but not the other way around.
Note added version (TER) 2.01 19 June 1989.

Additional modifications to cleanup output listing made by
Donald W. Coates 1/5/92. Version changed to 2.10
*/
main(argc,argv)
int argc;
char **argv;
{
	char **np;
	char *i;
	FILE *fopen();
	int j = 0;

	if(argc < 2){
		printf("Usage: %s [files]\n",argv[j]);
		exit(1);
	}
	Argv = argv;
	initialize();
	while ((j<argc) && (*argv[j] != '-'))
		j++;
	N_files = j-1;
	if (j < argc )
	{
		argv[j]++;
		while (j<argc)
		{
			for (i = argv[j]; *i != 0; i++)
				if ((*i <= 'Z') && (*i >= 'A'))
					*i = *i + 32;
			if (strcmp(argv[j],"l")==0)
				Lflag = 1;
			else if (strcmp(argv[j],"nol")==0)
				Lflag = 0;
			else if (strcmp(argv[j],"c")==0)
				Cflag = 1;
			else if (strcmp(argv[j],"noc")==0)
				Cflag = 0;
			else if (strcmp(argv[j],"s")==0)
				Sflag = 1;
			else if (strcmp(argv[j],"cre")==0)
				CREflag = 1;
			else if (strcmp(argv[j],"crlf")==0) /* test for crlf option */
				CRflag = 1;                       /* add <CR><LF> to S record */
			/* ver TER_1.1 June 3, 1989 */
			else if (strcmp(argv[j],"nnf")==0)  /* test for nnf option */
				nfFlag=0;                         /* nfFlag=1 means number INCLUDE */
			/* separately. ver TER_2.0 6/17/89 */
			else if (strcmp(argv[j],"p50")==0){ /* page every 50 lines */
				Pflag50 = 1;                      /* ver (TER) 2.01 19 Jun 89 */
				Pflag75 = 0;
			}									/* separately. ver TER_2.0 6/17/89 */
			else if (strcmp(argv[j],"p75")==0){ /* page every 75 lines */
				Pflag50 = 0;                      /* ver (DWC) 2.10 8 Oct 2001 */
				Pflag75 = 1;
			}
			j++;
		}
	}
	root = NULL;

	Cfn = 0;
	np = argv;
	Line_num = 0; /* reset line number */
	while( ++Cfn <= N_files )
		if((Fd = fopen(*++np,"r")) == NULL)
			printf("as: can't open %s\n",*np);
		else{
			make_pass();
			fclose(Fd);
		}
		if( Err_count == 0 ){
			Pass++;
			re_init();
			Cfn = 0;
			np = argv;
			Line_num = 0;
			FdCount=0;            /* Resets INCLUDE file nesting ver TER_2.0 6/17/89 */
			while( ++Cfn <= N_files)
				if((Fd = fopen(*++np,"r")) != NULL)
				{
					make_pass();
					fclose(Fd);
				}
				printf ("Program + Init Data = %d bytes\n",F_total); /* print total bytes */
				printf ("Error count = %d\n",Err_count);     /* rel TER_2.0 */
				if (Sflag == 1)
				{
					printf ("\n\f\n");
					stable (root);
				}
				if (CREflag == 1)
				{
					printf ("\n\f\n");
					cross (root);
				}
				if (CRflag == 1)
					fprintf(Objfil,"S9031000FC%c\n",CR); /* ver TER_1.1 print w <CR> */
				else
					fprintf(Objfil,"S9031000FC\n"); /* at least give a decent ending */
				fclose(Objfil);                    /* close file correctly ver TER_1.1 */
		}
		else                                /* just note errors, TER_2.0 */
		{
			printf ("Program + Init Data = %d bytes\n",F_total); /* print total bytes */
			printf ("Error count = %d\n",Err_count);
		}
		fwd_done();
		exit(Err_count);        /* neat for UNIX cuz can test return value in script
								but on other systems like Amiga, mostly just makes
								further script statements fail if >10, else
								nothing.  Therefore, printed out byte count
								and error level. ver (TER) 2.02 19 Jun 89 */
}

initialize()
{
	FILE *fopen();
	int i = 0;

#ifdef DEBUG
	printf("Initializing\n");
#endif
	Err_count = 0;
	Pc   = 0;
	Pass   = 1;
	Lflag   = 0;
	Cflag   = 0;
	Ctotal   = 0;
	Sflag   = 0;
	CREflag   = 0;
	N_page   = 0;
	Line[MAXBUF-1] = NEWLINE;

	strcpy(Obj_name,Argv[1]); /* copy first file name into array */
	do {
		if (Obj_name[i]=='.')
			Obj_name[i]=0;
	}
	while (Obj_name[i++] != 0);
	strcat(Obj_name,".s19");  /* append .out to file name. */
	if( (Objfil = fopen(Obj_name,"w")) == NULL)
		fatal("Can't create object file");
	fwdinit();                /* forward ref init */
	localinit();              /* target machine specific init. */
}

re_init()
{
#ifdef DEBUG
	printf("Reinitializing\n");
#endif
	Pc = 0;
	E_total = 0;
	P_total = 0;
	Ctotal = 0;
	N_page = 0;
	fwdreinit();
}

make_pass()
{
#ifdef IBM
	char *fgets();         /* the original line */
#else
	char *FGETS();         /* use own FGETS which is rewrite of lib function */
	/* such that it discards <CR> so can read code */
	/* generated on IBM */
	/* June 3, 1989 rev TER_1.1 */
#endif

#ifdef DEBUG
	printf("Pass %d\n",Pass);
#endif

#ifndef IBM
	while( FGETS(Line,MAXBUF-1,Fd) != (char *)NULL ){  /* changed to FGETS */
		/* which does not pass on <CR>  June 3, 1989 */
		/* rev TER_1.1 */
#else
	while( fgets(Line,MAXBUF,Fd) != (char *)NULL ){

#endif

		Line_num++;
		P_force = 0;          /* No force unless bytes emitted */
		N_page = 0;
		if(parse_line())
			process();
		if(Pass == 2 && Lflag && !N_page)
			print_line();
		P_total = 0;          /* reset byte count */
		Cycles = 0;           /* and per instruction cycle count */
	}
	f_record();
}


/*
* parse_line --- split input line into label, op and operand
*/
parse_line()
{
	register char *ptrfrm = Line;
	register char *ptrto = Label;
	char *skip_white();

	if( *ptrfrm == '*' || *ptrfrm == '\n' || *ptrfrm == ';' )
		/* added check for ';' ver TER_1.1 */
		/* June 3, 1989 */
		return(0);                    /* a comment line */

	while( delim(*ptrfrm)== NO )   /* parsing Label */
		*ptrto++ = *ptrfrm++;
	if(*--ptrto != ':')ptrto++;    /* allow trailing : */
	*ptrto = EOS;

	ptrfrm = skip_white(ptrfrm);
	if (*ptrfrm == ';') {          /* intercept comment after label, ver TER_2.0 */
		*Op = *Operand = EOS;   /* comment, no Opcode or Operand */
		return(1); }
	ptrto = Op;

	while( delim(*ptrfrm) == NO)   /* parsing Opcode */
		*ptrto++ = mapdn(*ptrfrm++);
	*ptrto = EOS;

	ptrfrm = skip_white(ptrfrm);
	if (*ptrfrm == ';') {          /* intercept comment, ver TER_2.0 */
		*Operand = EOS;         /* comment, no Operand */
		return(1); }

	ptrto = Operand;
	while( (*ptrfrm != NEWLINE) && (*ptrfrm != ';')) /* ver TER_2.0 */
		*ptrto++ = *ptrfrm++; /* kill comments */
	*ptrto = EOS;

#ifdef DEBUG
	printf("Label-%s-\n",Label);
	printf("Op----%s-\n",Op);
	printf("Operand-%s-\n",Operand);
#endif
	return(1);
}

/*
* process --- determine mnemonic class and act on it
*/
process()
{
	register struct oper *i;
	struct oper *mne_look();

	Old_pc = Pc;                   /* setup `old' program counter */
	Optr = Operand;                /* point to beginning of operand field */

	if(*Op==EOS){                  /* no mnemonic */
		if(*Label != EOS)
			install(Label,Pc);
	}
	else if( (i = mne_look(Op))== NULL)
		error("Unrecognized Mnemonic");
	else if( i->class == PSEUDO )
		do_pseudo(i->opcode);
	else{
		if( *Label )install(Label,Pc);
		if(Cflag)Cycles = i->cycles;
		do_op(i->opcode,i->class);
		if(Cflag)Ctotal += Cycles;
	}
}

#ifndef IBM
char *FGETS(s, n, iop)  /* get at most n chars from iop */
/* Added rev TER_1.1 June 3, 1989 */
/* Adapted from Kernighan & Ritchie */
/* An fgets() that is IBM proof. Not needed if
this IS an IBM */

char *s;
int n;
register FILE *iop;
{
	register int c;
	register char *cs;

	cs=s;
	while (--n > 0 && (c = getc(iop)) != EOF) /* read chars if not file end */
	{
		if ((*cs = c) != CR) cs++; /* incr buffer pointer if not CR */
		/* If CR, leave to be written over */
		if (c == '\n')
			break;
	}
	*cs = '\0';   /* replace NEWLINE with NULL as in standard fgets() */
	return((c == EOF && cs == s) ? NULL : s);  /* return NULL if this is EOF */
}
#endif
