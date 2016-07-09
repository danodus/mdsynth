/*
	Copyright (c) 1981, 1987, Masataka Ohta, Hiroshi Tezuka.
    Copyright (c) 2011-2012, Meldora Inc.
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

/*
    This is an adaptation of the public domain Micro-C compiler.
*/

#define DEBUG	error(-1)

#include <stdlib.h>
#include <stdio.h>

#define INT         (-1)
#define CHAR	    (-2)
#define UNSIGNED	(-3)
#define POINTER     (-4)
#define ARRAY	    (-5)
#define STRUCT	    (-6)
#define UNION	    (-7)
#define FUNCTION	(-8)
#define EMPTY	    (-9)

#define STATIC	    (-10)
#define GOTO	    (-11)
#define RETURN	    (-12)
#define BREAK	    (-13)
#define CONTINUE	(-14)
#define IF	        (-15)
#define ELSE	    (-16)
#define FOR	        (-17)
#define DO	        (-18)
#define WHILE	    (-19)
#define SWITCH	    (-20)
#define CASE	    (-21)
#define DEFAULT     (-22)
#define RESERVE     (-23)
#define TAG	        (-24)
#define FIELD	    (-25)
#define IDENT	    (-26)
#define STRING	    (-27)
#define MACRO	    (-28)
#define BLABEL	    (-29)
#define FLABEL	    (-30)
#define TYPEDEF     (-31)
#define SIZEOF	    (-32)
#define TYPE	    (-33)
#define LONG	    (-34)
#define SHORT	    (-35)

enum {TOP = 0, GDECL, GSDECL, GUDECL, ADECL, LDECL, LSDECL, LUDECL, STADECL, STAT, GTDECL, LTDECL};

enum { GVAR = 1, RGVAR, CRGVAR, LVAR, RLVAR, CRLVAR, CONST, FNAME, INDIRECT, RINDIRECT, CRINDIRECT,
    ADDRESS, MINUS, LNOT, BNOT, INC, POSTINC, PREINC, CPOSTINC, CPREINC, DEC, CPOSTDEC, CPREDEC,
    MUL, UMUL, DIV, UDIV, MOD, UMOD, ADD, SUB, RSHIFT, URSHIFT, LSHIFT, ULSHIFT, GT, UGT, GE, UGE,
    LT, ULT, LE, ULE, EQ, NEQ, BAND, EOR, BOR, LAND, LOR, COND, ASS, CASS, ASSOP, CASSOP, COMMA,
    LPAR, RPAR, LBRA, RBRA, LC, RC, COLON, SM, PERIOD, ARROW };

#define US          1
#define AS          100

enum {FILERR = 1, DCERR, STERR, EXERR, CNERR, CHERR, GSERR, LSERR, STRERR, LNERR, EOFERR, MCERR,
    INCERR, HPERR, TYERR, LVERR, UDERR, OPTION};

#define GSYMS	450
#define LSYMS	50

#define HEAPSIZE	1000
#define CHEAPSIZE	3000
#define LBUFSIZE	256

#define FILES 3

int sym, ch, chsave, type, mode, gfree, lfree, mflag, lineno, glineno;
int labelno, gpc, lvar, disp;
int symval, args, heap[HEAPSIZE];
int blabel, clabel, dlabel, cslabel, ilabel, control, ac, ac2, lsrc, chk, asmf;

unsigned hash;

char linebuf[LBUFSIZE], cheap[CHEAPSIZE], *chptr, *chptrsave;
char name[9], *cheapp, **av, *sptr, escape();

FILE *obuf;

typedef struct nametable {
	char nm[9];
	int sc,ty,dsp;
} NMTBL;

NMTBL ntable[GSYMS+LSYMS], *nptr, *gnptr, *decl0(), *decl1(), *losearch(), *gsearch();

struct {
    int fd,ln;
    FILE *fcb;
} *filep, filestack[FILES];

//
// Forward declarations
//

int error(int n);
void errmsg();
void checksym(int s);
void init();
struct nametable *decl0();
void newfile();
void reserve(char *s, int d);
void decl();
int typespec();
struct nametable *decl0();
NMTBL *decl1();
void adecl();
void reverse(int t1);
int size(int t);
void def(NMTBL *n);
int sdecl(int s);
void fdecl(NMTBL *n);
void fcheck(NMTBL *n);
void compatible(int t1, int t2);
int scalar(int t);
int integral(int t);
void statement();
void doif();
void dowhile();
void dodo();
void dofor();
void doswitch();
void docomp();
void docase();
void dodefault();
void doreturn();
void return2();
void ret(char *reg);
void unlink2();
void dogoto();
void dolabel();
int expr();
int expr0();
int expr1();
int expr2();
int expr3();
int expr4();
int expr5();
int expr6();
int expr7();
int expr8();
int expr9();
int expr10();
int expr11();
int expr12();
int expr13();
int expr14();
int expr15(int e1);
int expr16(int e1);
int rvalue(int e);
void lcheck(int e);
int indop(int e);
int strop(int e);
int binop(int op, int e1, int e2, int t1, int t2);
int typeid(int s);
int typename();
int ndecl0();
int ndecl1();
void bexpr(int e1, char cond, int l1);
void rexpr(int e1, int l1, char *s);
void jcond(int l, char cond);
void jmp(int l);
int fwdlabel();
void fwddef(int l);
int backdef();
void gexpr(int e1);
void string(int e1);
void function(int e1);
void indirect(int e1);
void machinop(int e1);
void rindirect(int e1);
void assign(int e1);
void assop(int e1);
int simpop(int op);
void oprt(int op, int e1);
void oprt1(int op, char *index, int n);
void dualop(int op, char *index, int n);
void oprtc(int op, int n);
void dualc(int op, int n);
void tosop(int op);
void dualtosop(int op);
void pushd();
void pushx();
void pulld();
void pulx();
void tfrdx();
void tfrxd();
void asld();
void adddim(int n);
void subdim(int n);
void cmpdimm(int n);
void addds();
void subds();
void clrd();
void lddim(int n);
void ldd(int e);
void lddx();
void lddy(int n);
void lddu(int n);
void predecx(char *op, int l);
void std(int e);
void stdx();
void stdy(int n);
void stdu(int n);
void ldbx();
void ldby(int n);
void ldbu(int n);
void postincx(char *op, int l);
void leaxy(int n);
void leaxu(int n);
void leaxpcr(NMTBL *n);
void ldx(int e);
void ldxy(int n);
void ldxu(int n);
void stx(int e);
void stxy(int n);
void stxu(int n);
void sex();
void incx();
void decx();
void opdx(char *op);
void indexx(char *op, int n);
void index_(char *op, int e);
void indexy(char *op, int n);
void indexu(char *op, int n);
void indir(char *op, int e);
void indiry(char *op, int n);
void indiru(char *op, int n);
void sextend(int byte);
void binexpr(int e1);
void library(int op);
int cexpr(int e);
int getsym();
int postequ(int s1, int s2);
int alpha(char c);
int digit(char c);
NMTBL *gsearch();
NMTBL *losearch();
int neqname(char *p);
void copy(char *p);
void getstring();
int skipspc();
int getch();
char escape();
FILE *getfname();
void getl();
int macroeq(char *s);
int car(int e);
int cadr(int e);
int caddr(int e);
int cadddr(int e);
int list2(int e1, int e2);
int list3(int e1, int e2, int e3);
int list4(int e1, int e2, int e3, int e4);
int getfree(int n);
int rplacad(int e, int n);

// ----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    NMTBL *nptr;
    int i;
    char *ccout;
	
    // If no argument is provided, we bail out
    if(argc == 1)
        exit(1);


	lsrc = chk = asmf = 0;
	ccout = "c.out";
	ac = argc;
	av = argv;

    // For each argument
    for (ac2 = 1; (ac2 < ac) && (*av[ac2] == '-'); ++ac2) {
        switch (*(av[ac2] + 1)) {
        case 'S': case 's':
			lsrc = 1;
			 break;
		case 'O': case 'o':
			ccout = av[ac2]+2;
			break;
		case 'C': case 'c':
			chk = 1;
			break;
		default:
			error(OPTION);
			exit(1);
		}
    }

    if (!chk)
		if ((obuf = fopen(ccout,"w")) == NULL)
            error(FILERR);

    // We perform the initialization
	init();

	while(1)
	{
        for (nptr = &ntable[GSYMS], i = LSYMS; i--;)
			(nptr++)->sc = EMPTY;
		mode = TOP;

		while(getsym() == SM);
		mode=GDECL;
		args=0;
		decl();
	}
}

// ----------------------------------------------------------------------------
int error(int n)
{
    if (n == EOFERR) {
		if(filep!=filestack)
		{	lineno=filep->ln;
			fclose(filep->fcb);
			printf("End of inclusion.\n");
			--filep;
			return 0;
		}
		else if(ac2!=ac)
		{	fclose(filep->fcb);
			newfile();
			return 0;
		}
		else if(mode == TOP)
		{	printf("\nCompiled %u lines.\n",glineno-1);
			if (!chk) printf(
				"Total internal labels	: %u.\n",labelno-1);
			printf(
				"Total global variables : %u bytes.\n\n",gpc);
			fprintf(obuf, "_%d\tRTS\n_INITIALIZE\tEQU\t_1\n",ilabel);
			fprintf(obuf, "_GLOBALS\tEQU\t%u\n\tEND\n",gpc);
			fclose(obuf);
			exit(0);
		}
	}

	printf("%5d:%s.\n",lineno,
		(n==FILERR) ? "Can't open specified file" :
		(n==DCERR) ? "Declaration syntax" :
		(n==STERR) ? "Statement syntax" :
		(n==EXERR) ? "Expression syntax" :
		(n==CNERR) ? "Constant required" :
		(n==CHERR) ? "Illegal character" :
		(n==GSERR) ? "Too many global symbols" :
		(n==LSERR) ? "Too many local symbols" :
		(n==STRERR) ? "Too many strings or macros" :
		(n==LNERR) ? "Line too long" :
		(n==EOFERR) ? "Unexpected end of file" :
		(n==MCERR) ? "Macro syntax" :
		(n==INCERR) ? "Include syntax" :
		(n==HPERR) ? "Too long expression" :
		(n==TYERR) ? "Type mismatch" :
		(n==LVERR) ? "Lvalue required" :
		(n==UDERR) ? "Undeclared identifier" :
		(n==OPTION) ? "Illegal option" :
		"Bug of compiler");
	errmsg();
	exit(1);
    return 0;
}

// ----------------------------------------------------------------------------    
void errmsg()
{
    char *p,*lim;
	if (lineno == 0)
        return;

    printf("%s",linebuf);

    lim = (mflag ? chptrsave : chptr);

	for (p = linebuf; p < lim;)
		printf((*p++ == '\t') ? "\t" : " ");

	printf ("^\n");
}

// ----------------------------------------------------------------------------    
void checksym(int s)
{
    char *p;
	if (sym != s)
	{
        p = (s == RPAR) ? "')'" : (s==RBRA) ? "']'": (s==SM) ? "';'":
		  (s==LPAR) ? "'('": (s==WHILE) ? "'while'":
		  (s==COLON) ? "':'": "Identifier";
		printf("%d:%s expected.\n",lineno,p);
		errmsg();
	}
	else getsym();
}

// ----------------------------------------------------------------------------    
void init()
{
    NMTBL *nptr;
    int i;

	for(nptr = ntable,i = GSYMS; i--;)
		(nptr++)->sc = EMPTY;

	reserve("int",INT);
	reserve("void",INT);
	reserve("char",CHAR);
	reserve("struct",STRUCT);
	reserve("union",UNION);
	reserve("unsigned",UNSIGNED);
	reserve("static",STATIC);
	reserve("goto",GOTO);
	reserve("return",RETURN);
	reserve("break",BREAK);
	reserve("continue",CONTINUE);
	reserve("if",IF);
	reserve("else",ELSE);
	reserve("for",FOR);
	reserve("do",DO);
	reserve("while",WHILE);
	reserve("switch",SWITCH);
	reserve("case",CASE);
	reserve("default",DEFAULT);
	reserve("typedef",TYPEDEF);
	reserve("sizeof",SIZEOF);
	reserve("long",LONG);
	reserve("short",SHORT);

	gpc = glineno = mflag =0;
	gfree = ilabel = 1;
	labelno = 2;
	cheapp = cheap;
	lfree = HEAPSIZE;
	filep = filestack;
	newfile();
	getl();
	getch();
}

// ----------------------------------------------------------------------------    
void newfile()
{	lineno=0;
	printf("%s:\n",av[ac2]);
	if ( (filep->fcb = fopen(av[ac2++],"r")) == NULL )
		error(FILERR);
}

// ----------------------------------------------------------------------------    
void reserve(char *s, int d)
{
    NMTBL *nptr;
    char *t;
	hash=0;
	t=name;
	while((*t++ = *s))
        hash = 7 * (hash + *s++);
	(nptr = gsearch())->sc = RESERVE;
	nptr->dsp = d;
}

// ----------------------------------------------------------------------------    
void decl()
{
    NMTBL *n;
    int t;

	if (sym == STATIC)
		if (mode == LDECL) {
			getsym();
			mode = STADECL;
		}
		else error(DCERR);
	else if (sym == TYPEDEF) {
		if (mode == GDECL) {
			getsym();
			mode=GTDECL;
		} else if (mode==LDECL) {
			getsym();
			mode=LTDECL;
		}
		else error(DCERR);
	}
	
	if ((t=typespec()) == 0)
		return;

	if (sym==SM)
		return;

	type = t;
	n = decl0();
	reverse(t);
	if (args || sym == LC) {
		fdecl(n);
		return;
	}

	def(n);

	while (sym == COMMA) {
		getsym();
		type = t;
		n = decl0();
		reverse(t);
		if (args)
			error(DCERR);
		def(n);
	}

	if (sym != SM)
		error(DCERR);

	if (mode == GTDECL)
		mode=GDECL;

	if (mode == STADECL || mode == LTDECL)
		mode=LDECL;
}

// ----------------------------------------------------------------------------    
int typespec()
{
    int t;
	switch(sym) {
	case INT:
	case CHAR:
		t = sym;
		getsym();
		break;
	case STRUCT:
	case UNION:
		t = sdecl(sym);
		break;
	case UNSIGNED:
		t = UNSIGNED;
		if(getsym() == INT)
			getsym();
		break;
	case SHORT:
		t = CHAR;
		if(getsym() == INT)
			getsym();
		break;
	case LONG:
		t = INT;
		if (getsym() == INT)
			getsym();
		break;
	default:
		if (sym == IDENT) {
			if (nptr->sc == TYPE) {
				t = nptr->ty;
				getsym();
				break;
			}
			else if(nptr->sc == EMPTY && gnptr->sc == TYPE) {
				t = gnptr->ty;
				getsym();
				break;
			}
			if(mode == LDECL)
				return 0;
			t = INT;
		}
	}
	return t;
}

// ----------------------------------------------------------------------------    
struct nametable *decl0()
{
    NMTBL *n;
	if(sym == MUL)
	{	getsym();
		n = decl0();
		type = list2(POINTER,type);
		return n;
	}
	return decl1();
}

// ----------------------------------------------------------------------------    
NMTBL *decl1()
{
    NMTBL *n;
    int i,t;
	if(sym == LPAR) {
		getsym();
		n=decl0();
		checksym(RPAR);
	}
	else if (sym == IDENT) {
		n = nptr;
		getsym();
	}
	else
		error(DCERR);

	while(1) {
		if (sym == LBRA) {
			if(getsym() == RBRA)
			{
				getsym();
				if (mode != ADECL)
					error(DCERR);
				t=type;
				type = list2(POINTER, type);
			}
			else {
				t = type;
				i = cexpr(expr());
				checksym(RBRA);
				type = list3(ARRAY, t, i);
			}
		} else if (sym == LPAR) {
			if (mode == GDECL) {
				mode = ADECL;
				getsym();
				mode = GDECL;
			}
			else getsym();
			if (sym == RPAR) {
				getsym();
			} else {
				n->sc = FUNCTION;
				adecl();
				n->sc = EMPTY;
			}
			type = list2(FUNCTION, type);
		}
		else return n;
	}
}

// ----------------------------------------------------------------------------    
void adecl()
{	
    if (mode != GDECL)
		error(DCERR);

	mode = ADECL;
	args = 2;
	while(1) {
		if(sym != IDENT)
			error(DCERR);
		nptr->ty = INT;
		nptr->sc = LVAR;
		nptr->dsp = (args += 2);
		if(getsym() != COMMA)
			break;
		getsym();
	}
	checksym(RPAR);
	mode = GDECL;
	return;
}

// ----------------------------------------------------------------------------    
void reverse(int t1)
{
    int t2, t3;
	t2 = t1;
	while (type != t1) {
		t3 = cadr(type);
		rplacad(type, t2);
		t2 = type;
		type = t3;
	}
	type = t2;
}

// ----------------------------------------------------------------------------    
int size(int t)
{
	if (t == CHAR)
		return 1;

	if (scalar(t))
		return 2;

	if (car(t) == STRUCT || car(t) == UNION) {
		if(cadr(t) == -1)
			error(DCERR);
		return(cadr(t));
	}

	if (car(t) == ARRAY) {
		return(size(cadr(t)) * caddr(t));
	} else {
		 error(DCERR);
	}

	// Not reached
	return 0;
}

// ----------------------------------------------------------------------------    
void def(NMTBL *n)
{
    int sz,nsc,ndsp,slfree,l,t,e;
	if(car(type)==FUNCTION)
	{	fcheck(n);
		return;
	}
	if (n->sc!=EMPTY &&
	    (mode!=ADECL || n->sc!=LVAR || n->ty!=INT) &&
	    ((mode!=GSDECL && mode!=LSDECL) || n->sc!=FIELD || n->dsp!=disp) &&
	    ((mode!=GUDECL && mode!=LUDECL) || n->sc!=FIELD || n->dsp!=0) )
		 error(DCERR);
	sz = size(n->ty = type);
	switch(mode)
	{case GDECL:
		fprintf(obuf, "%s\tEQU\t%u\n",n->nm,gpc);
	case STADECL:
		nsc = GVAR;
		ndsp = gpc;
		if(sym==ASS)
		{	t=type;
			if(!scalar(t))
				error(TYERR);
			if(mode==STADECL) fprintf(obuf, "\tBRA\t_%d\n",l=fwdlabel());
			fwddef(ilabel);
			getsym();
			slfree=lfree;
			e=expr1();
			if(car(e)==CONST)
			{	lddim(cadr(e));
				indexy(t==CHAR?"STB":"STD",gpc);
			}
			else if(t!=CHAR)
			{	if(car(e)==ADDRESS&&car(cadr(e))==GVAR)
					leaxy(cadr(cadr(e)));
				else if(car(e)==FNAME)
					leaxpcr((NMTBL *)cadr(e));
				else error(TYERR);
				stxy(gpc);
			}
			else error(TYERR);
			lfree=slfree;
			jmp(ilabel=fwdlabel());
			if(mode==STADECL) fwddef(l);
			type=t;
		}
		gpc +=sz;
		break;
	case GSDECL:
		nsc = FIELD;
		ndsp = disp;
		disp += sz;
		break;
	case GUDECL:
		nsc = FIELD;
		ndsp = 0;
		if (disp < sz) disp = sz;
		break;
	case GTDECL:
		nsc = TYPE;
		break;
	case ADECL:
		if(type==CHAR) ++(n->dsp);
		else if (!scalar(type)) error(TYERR);
		return;
	case LDECL:
		nsc = LVAR;
		ndsp = (disp -= sz);
		break;
	case LSDECL:
		nsc = FIELD;
		ndsp = disp;
		disp += sz;
		break;
	case LUDECL:
		nsc = FIELD;
		ndsp = 0;
		if (disp < sz) disp = sz;
		break;
	case LTDECL:
		nsc = TYPE;
		break;
	default:
		error(DCERR);
	}
	n->sc = nsc;
	n->dsp = ndsp;
}

// ----------------------------------------------------------------------------    
int sdecl(int s)
{
    int smode,sdisp,type;
    NMTBL *nptr0;
	smode=mode;
	if (mode==GDECL || mode==GSDECL || mode==GUDECL || mode==GTDECL)
		mode=(s==STRUCT?GSDECL:GUDECL);
	else mode=(s==STRUCT?LSDECL:LUDECL);
	sdisp=disp;
	disp=0;
	if (getsym() == IDENT)
	{	nptr0 = nptr;
		if (getsym() == LC)
		{	if (nptr0->sc != EMPTY) error(DCERR);
			nptr0->sc = TAG;
			nptr0->ty = list2(s,-1);
			while (getsym() != RC) decl();
			getsym();
			rplacad(type = nptr0->ty,disp);
		}
		else
		{	if(nptr0->sc == EMPTY) nptr0=gnptr;
			if(nptr0->sc == EMPTY) error(UDERR);
			if(nptr0->sc != TAG) error(TYERR);
			type = nptr0->ty;
		}
	}
	else if(sym==LC)
	{	while(getsym() != RC) decl();
		getsym();
		type = list2(s,disp);
	}
	else error(DCERR);
	disp=sdisp;
	mode=smode;
	return type;
}

// ----------------------------------------------------------------------------    
void fdecl(NMTBL *n)
{
    args=0;
	fcheck(n);
	mode=ADECL;
	lfree= HEAPSIZE;
	while (sym!=LC) {decl(); getsym();}
	disp=0;
	mode=STAT;
	while (typeid(getsym()) || sym==STATIC || sym==TYPEDEF)
	{	mode=LDECL;
		decl();
		mode=STAT;
	}
	control=1;
	fprintf(obuf, "%s\n\tPSHS\tU\n\tLEAU\t,S\n",n->nm);
	if(disp) fprintf(obuf, "\tLEAS\t%d,S\n",disp);
	lvar= -disp;
	while(sym!=RC) statement();
	if (control) return2();
}

// ----------------------------------------------------------------------------    
void fcheck(NMTBL *n)
{
    if(mode!=GDECL||car(type)!=FUNCTION) error(DCERR);
	if(n->sc==FUNCTION) compatible(n->ty,cadr(type));
	else if(n->sc!=EMPTY) error(DCERR);
	n->sc=FUNCTION;
	n->ty=cadr(type);
}

// ----------------------------------------------------------------------------    
void compatible(int t1, int t2)
{
    if(integral(t1))
	{	if(t1!=t2) error(TYERR);
	}
	else if(car(t1)!=car(t2)) error(TYERR);
	else if((car(t1)==STRUCT || car(t1)==UNION) && cadr(t1)!=cadr(t2))
		error(TYERR);
	else if(car(t1)==POINTER || car(t1)==ARRAY ||car(t1)==FUNCTION)
		compatible(cadr(t1),cadr(t2));
}

// ----------------------------------------------------------------------------    
int scalar(int t)
{
    return(integral(t)||car(t)==POINTER);
}

// ----------------------------------------------------------------------------    
int integral(int t)
{
    return(t==INT||t==CHAR||t==UNSIGNED);
}

// ----------------------------------------------------------------------------    
void statement()
{
    int slfree;
	switch(sym)
	{case IF:
		doif();
		return;
	case WHILE:
		dowhile();
		return;
	case DO:
		dodo();
		return;
	case FOR:
		dofor();
		return;
	case SWITCH:
		doswitch();
		return;
	case LC:
		docomp();
		return;
	case BREAK:
		jmp(blabel);
		getsym();
		checksym(SM);
		return;
	case CONTINUE:
		jmp(clabel);
		getsym();
		checksym(SM);
		return;
	case CASE:
		docase();
		statement();
		return;
	case DEFAULT:
		dodefault();
		statement();
		return;
	case RETURN:
		doreturn();
		return;
	case GOTO:
		dogoto();
		return;
	case SM:
		getsym();
		return;
	default:if(sym==IDENT&&skipspc()==':')
		{	dolabel();
			statement();
		}
		else
		{	slfree=lfree;
			gexpr(expr());
			lfree=slfree;
			checksym(SM);
		}
	}
}

// ----------------------------------------------------------------------------    
void doif()
{
    int l1, l2, slfree;
	getsym();
	checksym(LPAR);
	slfree = lfree;
	bexpr(expr(), 0, l1 = fwdlabel());
	lfree = slfree;
	checksym(RPAR);
	statement();
	if(sym == ELSE) {
		if ((l2 = control))
			jmp(l2 = fwdlabel());

		fwddef(l1);
		getsym();
		statement();

		if (l2)
			fwddef(l2);
	} else
		fwddef(l1);
}

// ----------------------------------------------------------------------------    
void dowhile()
{
    int sbreak, scontinue, slfree, e;
	sbreak = blabel;
	scontinue = clabel;
	blabel = fwdlabel();
	clabel = backdef();
	getsym();
	checksym(LPAR);
	slfree = lfree;
	e = expr();
	checksym(RPAR);
	if(sym == SM) {
		bexpr(e, 1, clabel);
		lfree = slfree;
		getsym();
	} else {
		bexpr(e, 0, blabel);
		lfree = slfree;
		statement();
		jmp(clabel);
	}
	fwddef(blabel);
	clabel = scontinue;
	blabel = sbreak;
}

// ----------------------------------------------------------------------------    
void dodo()
{
    int sbreak, scontinue, l, slfree;
	sbreak = blabel;
	scontinue = clabel;
	blabel = fwdlabel();
	clabel = fwdlabel();
	l = backdef();
	getsym();
	statement();
	fwddef(clabel);
	checksym(WHILE);
	checksym(LPAR);
	slfree = lfree;
	bexpr(expr(),1,l);
	lfree = slfree;
	checksym(RPAR);
	checksym(SM);
	fwddef(blabel);
	clabel = scontinue;
	blabel = sbreak;
}

// ----------------------------------------------------------------------------    
void dofor()
{
    int sbreak,scontinue,l,e,slfree;
	sbreak=blabel;
	scontinue=clabel;
	blabel=fwdlabel();
	getsym();
	checksym(LPAR);
	slfree=lfree;
	if(sym!=SM)
	{	gexpr(expr());
		checksym(SM);
	}
	else getsym();
	lfree=slfree;
	l=backdef();
	if(sym!=SM)
	{	bexpr(expr(),0,blabel);
		checksym(SM);
	}
	else getsym();
	lfree=slfree;
	if(sym==RPAR)
	{	clabel=l;
		getsym();
		statement();
	}
	else
	{	clabel=fwdlabel();
		e=expr();
		checksym(RPAR);
		statement();
		fwddef(clabel);
		gexpr(e);
		lfree=slfree;
	}
	jmp(l);
	fwddef(blabel);
	clabel=scontinue;
	blabel=sbreak;
}

// ----------------------------------------------------------------------------    
void doswitch()
{
    int sbreak,scase,sdefault,slfree;
	sbreak=blabel;
	blabel=fwdlabel();
	sdefault=dlabel;
	dlabel=0;
	scase=cslabel;
	getsym();
	checksym(LPAR);
	slfree=lfree;
	gexpr(expr());
	lfree=slfree;
	checksym(RPAR);
	cslabel = control = 0;
	statement();
	if(dlabel) fprintf(obuf, "_%d\tEQU\t_%d\n",cslabel,dlabel);
	else fwddef(cslabel);
	cslabel=scase;
	dlabel=sdefault;
	fwddef(blabel);
	blabel=sbreak;
}

// ----------------------------------------------------------------------------    
void docomp()
{
    getsym();
	while(sym!=RC) statement();
	getsym();
}

// ----------------------------------------------------------------------------    
void docase()
{
    int c,n,l,slfree;
	c=0;
	n=2;
	slfree=lfree;
	while(sym==CASE)
	{	getsym();
		c=list2(cexpr(expr()),c);
		n+=6;
		checksym(COLON);
	}
	l=fwdlabel();
	if (control)
	{	control=0;
		if (n>127) jmp(l);
		else fprintf(obuf, "\tBRA\t_%d\n",l);
	}
	if (cslabel) fwddef(cslabel);
	while(cadr(c))
	{	cmpdimm(car(c));
		if((n-=6)>127) jcond(l,0);
		else fprintf(obuf, "\tBEQ\t_%d\n",l);
		c=cadr(c);
	}
	lfree=slfree;
	cmpdimm(car(c));
	jcond(cslabel=fwdlabel(),1);
	fwddef(l);
}

// ----------------------------------------------------------------------------    
void dodefault()
{	
    getsym();
	checksym(COLON);
	if (dlabel) error(STERR);
	if (!cslabel) jmp(cslabel = fwdlabel());
	dlabel = backdef();
}

// ----------------------------------------------------------------------------    
void doreturn()
{
    int slfree;
	if(getsym()==SM)
	{	getsym();
		return2();
		return;
	}
	slfree=lfree;
	gexpr(expr());
	lfree=slfree;
	checksym(SM);
	control=0;
	switch(lvar)
	{case 0:
		ret("");
		return;
	case 2:
		ret("X,");
		return;
	default:unlink2();
		return;
	}
}

// ----------------------------------------------------------------------------    
void return2()
{	
    control=0;
	switch(lvar)
	{case 0:
		ret("");
		return;
	case 1:
		ret("A,");
		return;
	case 2:
		ret("D,");
		return;
	case 3:
		ret("A,X,");
		return;
	case 4:
		ret("D,X,");
		return;
	default:unlink2();
		return;
	}
}

// ----------------------------------------------------------------------------    
void ret(char *reg)
{
    fprintf(obuf, "\tPULS\t%sU,PC\n",reg);
}

// ----------------------------------------------------------------------------    
void unlink2()
{
    fprintf(obuf, "\tLEAS\t,U\n");
	ret("");
}

// ----------------------------------------------------------------------------    
void dogoto()
{
    NMTBL *nptr0;
	getsym();
	nptr0=nptr;
	checksym(IDENT);
	if (nptr0->sc == BLABEL || nptr0->sc == FLABEL) {
		jmp(nptr0->dsp);
	} else if (nptr0->sc == EMPTY) {
		nptr0->sc = FLABEL;
		jmp(nptr0->dsp = fwdlabel());
	} else
		error(STERR);
	checksym(SM);
}

// ----------------------------------------------------------------------------    
void dolabel()
{
    if (nptr->sc == FLABEL) {
		fwddef(nptr->dsp);
	} else if (nptr->sc != EMPTY)
		error(TYERR);
	nptr->sc = BLABEL;
	nptr->dsp = backdef();
	getsym();
	checksym(COLON);
}

// ----------------------------------------------------------------------------    
int expr()
{
    return(rvalue(expr0()));
}

// ----------------------------------------------------------------------------    
int expr0()
{
    int e;
	e = expr1();
	while(sym == COMMA) {
		getsym();
		e = list3(COMMA,e,rvalue(expr1()));
	}
	return e;
}

// ----------------------------------------------------------------------------    
int expr1()
{
    int e1, e2, t, op;
	e1 = expr2();
	switch (sym) {
	case ASS:
		lcheck(e1);
		t = type;
		getsym();
		e2 = rvalue(expr1());
		if(t == CHAR) {
			type= INT;
			return (list3(CASS,e1,e2));
		}
		type = t;
		return (list3(ASS,e1,e2));
	case ADD+AS:
	case SUB+AS:
	case MUL+AS:
	case DIV+AS:
	case MOD+AS:
	case RSHIFT+AS:
	case LSHIFT+AS:
	case BAND+AS:
	case EOR+AS:
	case BOR+AS:
		op = sym-AS;
		lcheck(e1);
		t = type;
		getsym();
		e2 = rvalue(expr1());
		if(!integral(type))
			error(TYERR);
		if((t == UNSIGNED || type==UNSIGNED) &&
			(op == MUL || op == DIV || op == MOD|| op == RSHIFT || op == LSHIFT))
			op = op + US;
		if(t == CHAR)
		{	type = INT;
			return(list4(CASSOP,e1,e2,op));
		}
		type = t;
		if(integral(t))
			return(list4(ASSOP,e1,e2,op));
		if ((op != ADD && op != SUB) || car(t) != POINTER)
			error(TYERR);
		e2 = binop(MUL, e2, list2(CONST,size(cadr(t))), INT, UNSIGNED);
		type = t;
		return list4(ASSOP, e1, e2, op);
	default:
		return(e1);
	}
}

// ----------------------------------------------------------------------------    
int expr2()
{
    int e1, e2, e3, t;
	e1 = expr3();
	if (sym == COND)
	{	
		e1 = rvalue(e1);
		getsym();
		e2 = rvalue(expr2());
		t = type;
		checksym(COLON);
		e3 = rvalue(expr2());
		if (car(e1) == CONST)
			if(cadr(e1)) {
				type=t;
				return e2;
			} else
				return e3;
		if (type==INT || t!= INT && type == UNSIGNED)
			type=t;
		return (list4(COND, e1, e2, e3));
	}
	return(e1);
}

// ----------------------------------------------------------------------------    
int expr3()
{
    int e;
	e=expr4();
	while(sym == LOR) {
		e = rvalue(e);
		getsym();
		e = list3(LOR, e, rvalue(expr4()));
		type = INT;
	}
	return(e);
}

// ----------------------------------------------------------------------------    
int expr4()
{
    int e;
	e=expr5();
	while(sym==LAND)
	{	e=rvalue(e);
		getsym();
		e=list3(LAND,e,rvalue(expr5()));
		type= INT;
	}
	return(e);
}

// ----------------------------------------------------------------------------    
int expr5()
{
    int e1,e2,t;
	e1=expr6();
	while(sym==BOR)
	{	e1=rvalue(e1);
		t=type;
		getsym();
		e2=rvalue(expr6());
		e1=binop(BOR,e1,e2,t,type);
	}
	return(e1);
}

// ----------------------------------------------------------------------------    
int expr6()
{
    int e1,e2,t;
	e1=expr7();
	while(sym==EOR)
	{	e1=rvalue(e1);
		t=type;
		getsym();
		e2=rvalue(expr7());
		e1=binop(EOR,e1,e2,t,type);
	}
	return(e1);
}

// ----------------------------------------------------------------------------    
int expr7()
{
    int e1,e2,t;
	e1=expr8();
	while(sym==BAND)
	{	e1=rvalue(e1);
		t=type;
		getsym();
		e2=rvalue(expr8());
		e1=binop(BAND,e1,e2,t,type);
	}
	return(e1);
}

// ----------------------------------------------------------------------------    
int expr8()
{
    int e,op;
	e=expr9();
	while((op=sym)==EQ||op==NEQ)
	{	e=rvalue(e);
		getsym();
		e=list3(op,e,rvalue(expr9()));
		type= INT;
	}
	return e;
}

// ----------------------------------------------------------------------------    
int expr9()
{
    int e1,e2,t,op;
	e1=expr10();
	while((op=sym)==GT||op==GE||op==LT||op==LE)
	{	e1=rvalue(e1);
		t=type;
		getsym();
		e2=rvalue(expr10());
		if(t==INT&&type==INT) e1=list3(op,e1,e2);
		else e1=list3(op+US,e1,e2);
		type= INT;
	}
	return e1;
}

// ----------------------------------------------------------------------------    
int expr10()
{
    int e1,e2,t,op;
	e1=expr11();
	while((op=sym)==RSHIFT||op==LSHIFT)
	{	e1=rvalue(e1);
		t=type;
		getsym();
		e2=rvalue(expr11());
		e1=binop(op,e1,e2,t,type);
	}
	return e1;
}

// ----------------------------------------------------------------------------    
int expr11()
{
    int e1,e2,t,op;
	e1=expr12();
	while((op=sym)==ADD||op==SUB)
	{	e1=rvalue(e1);
		t=type;
		getsym();
		e2=rvalue(expr12());
		e1=binop(op,e1,e2,t,type);
	}
	return e1;
}

// ----------------------------------------------------------------------------    
int expr12()
{
    int e1,e2,t,op;
	e1=expr13();
	while((op=sym)==MUL||op==DIV||op==MOD)
	{	e1=rvalue(e1);
		t=type;
		getsym();
		e2=rvalue(expr13());
		e1=binop(op,e1,e2,t,type);
	}
	return e1;
}

// ----------------------------------------------------------------------------    
int expr13()
{
    int e,op;
	switch (op = sym)
	{case INC: case DEC:
		getsym();
		lcheck(e=expr13());
		if(type==CHAR)
		{	type= INT;
			return(list2(op==INC?CPREINC:CPREDEC,e));
		}
		if(integral(type))
			return(list3(PREINC,e,op==INC?1:-1));
		if(car(type)!=POINTER) error(TYERR);
		return(list3(PREINC,e,
			op==INC?size(cadr(type)):-size(cadr(type)) ));
	case MUL:
		getsym();
		e=rvalue(expr13());
		return(indop(e));
	case BAND:
		getsym();
		switch(car(e=expr13()))
		{case INDIRECT:
			e=cadr(e);
			break;
		case GVAR:
		case LVAR:
			e=list2(ADDRESS,e);
			break;
		case FNAME:
			return e;
		default:error(LVERR);
		}
		type=list2(POINTER,type);
		return e;
	case SUB:
		getsym();
		e=rvalue(expr13());
		if(!integral(type)) error(TYERR);
		return(car(e)==CONST?list2(CONST,-cadr(e)):list2(MINUS,e));
	case BNOT:
		getsym();
		e=rvalue(expr13());
		if(!integral(type)) error(TYERR);
		return(car(e)==CONST?list2(CONST,~cadr(e)):list2(BNOT,e));
	case LNOT:
		getsym();
		return(list2(LNOT,rvalue(expr13())));
	case SIZEOF:
		if(getsym()==LPAR)
			if(typeid(getsym()))
			{	e=list2(CONST,size(typename()));
				type=INT;
				checksym(RPAR);
				return e;
			}
			else
			{	e=expr0();
				checksym(RPAR);
				expr16(e);
				if(sym==INC||sym==DEC)
				{	getsym();
					if(type==CHAR) type=INT;
					else if(!scalar(type))
						error(TYERR);
				}
			}
		else expr13();
		e=list2(CONST,size(type));
		type=INT;
		return e;
	}
	e=expr14();
	if((op=sym)==INC||op==DEC)
	{	lcheck(e);
		getsym();
		if(type==CHAR)
		{	type= INT;
			return(list2(op==INC?CPOSTINC:CPOSTDEC,e));
		}
		if(integral(type))
			return(list3(POSTINC,e,op==INC?1:-1));
		if(car(type)!=POINTER) error(TYERR);
		return (list3(POSTINC,e,
			op == INC ? size(cadr(type)): -size(cadr(type)) ));
	}
	return e;
}

// ----------------------------------------------------------------------------    
int expr14()
{
    int e1,t;
	switch(sym)
	{case IDENT:
		switch(nptr->sc)
		{case GVAR:
			e1=list2(GVAR,nptr->dsp);
			type=nptr->ty;
			getsym();
			break;
		case LVAR:
			e1=list2(LVAR,nptr->dsp);
			type=nptr->ty;
			getsym();
			break;
		case FUNCTION:
			e1=list2(FNAME,(int)nptr);
			type=list2(FUNCTION,nptr->ty);
			getsym();
			break;
		case EMPTY:
			if(getsym()==LPAR)
			{	nptr->sc = FUNCTION;
				nptr->ty= INT;
				type= list2(FUNCTION,INT);
				e1=expr15(list2(FNAME,(int)nptr));
				break;
			}
		default:error(UDERR);
		}
		break;
	case STRING:
		e1=list3(STRING,(int)sptr,symval);
		type=list3(ARRAY,CHAR,symval);
		getsym();
		break;
	case CONST:
		type= INT;
		e1=list2(CONST,symval);
		getsym();
		break;
	case LPAR:
		if(typeid(getsym()))
		{	t=typename();
			checksym(RPAR);
			e1=expr13();
			type=t;
			return e1;
		}
		e1=expr0();
		checksym(RPAR);
		break;
	default:error(EXERR);
	}
	return expr16(e1);
}

// ----------------------------------------------------------------------------    
expr15(int e1)
{
    int t,args;
	t=type;
	if(integral(t)||car(t)!=FUNCTION)
		error(TYERR);
	t=cadr(t);
	getsym();
	args=0;
	while(sym!=RPAR)
	{	args=list2(rvalue(expr1()),args);
		if(sym!=COMMA) break;
		getsym();
	}
	checksym(RPAR);
	if(t==CHAR) type= INT;else type=t;
	return list3(FUNCTION,e1,args);
}

// ----------------------------------------------------------------------------    
int expr16(int e1)
{
    int e2,t;
	while(1)
		if(sym==LBRA)
		{	e1=rvalue(e1);
			t=type;
			getsym();
			e2=rvalue(expr0());
			checksym(RBRA);
			e1=binop(ADD,e1,e2,t,type);
			e1=indop(e1);
		}
		else if(sym==LPAR) e1=expr15(e1);
		else if(sym==PERIOD) e1=strop(e1);
		else if(sym==ARROW) e1=strop(indop(rvalue(e1)));
		else break;
	if(car(e1)==FNAME) type=list2(POINTER,type);
	return e1;
}

// ----------------------------------------------------------------------------    
int rvalue(int e)
{
    if(type==CHAR)
	{	type= INT;
		switch(car(e))
		{case GVAR:
			return(list2(CRGVAR,cadr(e)));
		case LVAR:
			return(list2(CRLVAR,cadr(e)));
		case INDIRECT:
			return(list2(CRINDIRECT,cadr(e)));
		default:return(e);
		}
	}
	if(!integral(type))
		if(car(type)==ARRAY)
		{	type=list2(POINTER,cadr(type));
			if(car(e)==INDIRECT) return cadr(e);
			return list2(ADDRESS,e);
		}
		else if(car(type)!=POINTER) error(TYERR);
	switch(car(e))
	{case GVAR:
		return(list2(RGVAR,cadr(e)));
	case LVAR:
		return(list2(RLVAR,cadr(e)));
	case INDIRECT:
		return(list2(RINDIRECT,cadr(e)));
	default:return(e);
	}
}

// ----------------------------------------------------------------------------    
void lcheck(int e)
{   
    if(!scalar(type)||car(e)!=GVAR&&car(e)!=LVAR&&car(e)!=INDIRECT)
		error(LVERR);
}

// ----------------------------------------------------------------------------    
int indop(int e)
{	if(type!=INT&&type!=UNSIGNED)
		if(car(type)==POINTER) type=cadr(type);
		else error(TYERR);
	else type= CHAR;
	if(car(e)==ADDRESS) return(cadr(e));
	return(list2(INDIRECT,e));
}

// ----------------------------------------------------------------------------    
int strop(e)
{
    getsym();
	if (sym!=IDENT||nptr->sc!=FIELD) error(TYERR);
	if (integral(type)||car(type)!=STRUCT && car(type)!=UNION)
		e=rvalue(e);
	type = nptr->ty;
	switch(car(e))
	{case GVAR:
	case LVAR:
		e=list2(car(e),cadr(e) + nptr->dsp);
		break;
	case INDIRECT:
		if(!nptr->dsp) break;
		e=list2(INDIRECT,list3(ADD,cadr(e),list2(CONST,nptr->dsp)));
		break;
	default:
		e=list2(INDIRECT,list3(ADD,e,list2(CONST,nptr->dsp)));
	}
	getsym();
	return e;
}

// ----------------------------------------------------------------------------    
int binop(int op, int e1, int e2, int t1, int t2)
{
    int e;
	if(car(e1)==CONST&&car(e2)==CONST)
	{	e1=cadr(e1);
		e2=cadr(e2);
		type= INT;
		switch(op)
		{case BOR:
			e=e1|e2;break;
		case EOR:
			e=e1^e2;break;
		case BAND:
			e=e1&e2;break;
		case ADD:
			if(integral(t1))
			{	if(integral(t2))
					e=e1+e2;
				else
				{	if(car(t2)!=POINTER) error(TYERR);
					e=size(cadr(t2))*e1+e2;
					type=t2;
				}
			}
			else
			{	if(car(t1)!=POINTER) error(TYERR);
				e=e1+size(cadr(t1))*e2;
				type=t1;
			}
			break;
		case SUB:
			if(integral(t1))
				e=e1-e2;
			else
			{	if(car(t1)!=POINTER) error(TYERR);
				e=e1-size(cadr(t1))*e2;
				type=t1;
			}
			break;
		case MUL:
			e=e1*e2;break;
		case DIV:
			if(!e2) error(EXERR);e=e1/e2;break;
		case MOD:
			if(!e2) error(EXERR);e=e1%e2;break;
		case RSHIFT:
			e=e1>>e2;break;
		case LSHIFT:
			e=e1<<e2;
		}
		return list2(CONST,e);
	}
	if((op==ADD||op==MUL||op==BOR||op==EOR||op==BAND)&&
		(car(e1)==CONST||car(e2)!=CONST&&
		(car(e1)==RGVAR||car(e1)==RLVAR)))
		{e=e1;e1=e2;e2=e;e=t1;t1=t2;t2=e;}
	if(op==ADD)
	{	if(integral(t1))
		{	if(integral(t2))
			{	if(t1==INT) type=t2;else type=t1;
				return(list3(ADD,e1,e2));
			}
			if(car(t2)!=POINTER) error(TYERR);
			e=binop(MUL,e1,list2(CONST,size(cadr(t2))),t1,INT);
			type=t2;
			return(list3(ADD,e,e2));
		}
		if(car(t1)!=POINTER||!integral(t2)) error(TYERR);
		e=binop(MUL,e2,list2(CONST,size(cadr(t1))),t2,INT);
		type=t1;
		if(car(e1)==ADDRESS&&car(e)==CONST)
			return(list2(ADDRESS,list2(car(cadr(e1)),
				cadr(cadr(e1))+cadr(e))));
		return(list3(ADD,e1,e));
	}
	if(op==SUB)
	{	if(integral(t1))
		{	if(!integral(t2)) error(TYERR);
			if(t1==INT) type=t2;else type=t1;
			return(list3(SUB,e1,e2));
		}
		if(car(t1)!=POINTER) error(TYERR);
		if(integral(t2))
		{	e=binop(MUL,e2,list2(CONST,size(cadr(t1))),t2,INT);
			type=t1;
			return(list3(SUB,e1,e));
		}
		if(car(t2)!=POINTER)
			error(TYERR);
		compatible(t1,t2);
		e=list3(SUB,e1,e2);
		e=binop(DIV,e,list2(CONST,size(cadr(t1))),UNSIGNED,INT);
		type= INT;
		return e;
	}
	if(!integral(t1)||!integral(t2)) error(TYERR);
	if(t1==INT) type=t2;else type=t1;
	if((op==MUL||op==DIV)&&car(e2)==CONST&&cadr(e2)==1) return e1;
	if(op==BOR||op==EOR||op==BAND) return(list3(op,e1,e2));
	return(list3(type==UNSIGNED?op+US:op,e1,e2));
}

// ----------------------------------------------------------------------------    
int typeid(int s)
{	return (integral(s) || s==SHORT || s==LONG || s==STRUCT || s==UNION ||
		(s==IDENT && nptr->sc==TYPE));
}

// ----------------------------------------------------------------------------    
int typename()
{
    int t;
	type=t=typespec();
	ndecl0();
	reverse(t);
	return type;
}

// ----------------------------------------------------------------------------    
int ndecl0()
{
    if(sym==MUL)
	{	getsym();
		return type=list2(POINTER,ndecl0());
	}
	return ndecl1();
}

// ----------------------------------------------------------------------------    
int ndecl1()
{
    int i,t;
	if(sym==LPAR)
		if(getsym()==RPAR) {type=list2(FUNCTION,type); getsym();}
		else
		{	ndecl0();
			checksym(RPAR);
		}
	while(1)
		if(sym==LBRA)
		{	getsym();
			t=type;
			i=cexpr(expr());
			checksym(RBRA);
			type=list3(ARRAY,t,i);
		}
		else if(sym==LPAR)
		{	getsym();
			checksym(RPAR);
			type=list2(FUNCTION,type);
		}
		else return type;
}

// ----------------------------------------------------------------------------    
void bexpr(int e1, char cond, int l1)
{
    int e2,l2;
	if (chk) return;
	e2=cadr(e1);
	switch(car(e1))
	{case LNOT:
		bexpr(e2,!cond,l1);
		return;
	case GT:
		rexpr(e1,l1,cond?"GT":"LE");
		return;
	case UGT:
		rexpr(e1,l1,cond?"HI":"LS");
		return;
	case GE:
		rexpr(e1,l1,cond?"GE":"LT");
		return;
	case UGE:
		rexpr(e1,l1,cond?"HS":"LO");
		return;
	case LT:
		rexpr(e1,l1,cond?"LT":"GE");
		return;
	case ULT:
		rexpr(e1,l1,cond?"LO":"HS");
		return;
	case LE:
		rexpr(e1,l1,cond?"LE":"GT");
		return;
	case ULE:
		rexpr(e1,l1,cond?"LS":"HI");
		return;
	case EQ:
		rexpr(e1,l1,cond?"EQ":"NE");
		return;
	case NEQ:
		rexpr(e1,l1,cond?"NE":"EQ");
		return;
	case LAND:
		bexpr(e2,0,cond?(l2=fwdlabel()):l1);
		bexpr(caddr(e1),cond,l1);
		if(cond) fwddef(l2);
		return;
	case LOR:
		bexpr(e2,1,cond?l1:(l2=fwdlabel()));
		bexpr(caddr(e1),cond,l1);
		if(!cond) fwddef(l2);
		return;
	case CRGVAR:
		ldby(e2);
		jcond(l1,cond);
		return;
	case CRLVAR:
		ldbu(e2);
		jcond(l1,cond);
		return;
	case CONST:
		if(cond&&e2||!cond&&!e2) jmp(l1);
		return;
	case RGVAR:
	case RLVAR:
	case CRINDIRECT:
		gexpr(e1);
		jcond(l1,cond);
		return;
	default:gexpr(e1);
		subdim(0);
		jcond(l1,cond);
		return;
	}
}

// ----------------------------------------------------------------------------    
void rexpr(int e1, int l1, char *s)
{
    gexpr(list3(SUB,cadr(e1),caddr(e1)));
	fprintf(obuf, "\tLB%s\t_%d\n",s,l1);
}

// ----------------------------------------------------------------------------    
void jcond(int l, char cond)
{
    fprintf(obuf, "\tLB%s\t_%d\n",cond?"NE":"EQ",l);
}

// ----------------------------------------------------------------------------    
void jmp(int l)
{
    control=0;
	fprintf(obuf, "\tLBRA\t_%d\n",l);
}

// ----------------------------------------------------------------------------    
int fwdlabel()
{
    return labelno++;
}

// ----------------------------------------------------------------------------    
void fwddef(int l)
{	control=1;
	fprintf(obuf, "_%d\n",l);
}

// ----------------------------------------------------------------------------    
int backdef()
{	
    control=1;
	fprintf(obuf, "_%d\n",labelno);
	return labelno++;
}
// ----------------------------------------------------------------------------    
void gexpr(int e1)
{
    int e2,e3;
	if (chk) return;
	e2 = cadr(e1);
	switch (car(e1))
	{case GVAR:
		leaxy(e2);
		return;
	case RGVAR:
		lddy(e2);
		return;
	case CRGVAR:
		ldby(e2);
		sex();
		return;
	case LVAR:
		leaxu(e2);
		return;
	case RLVAR:
		lddu(e2);
		return;
	case CRLVAR:
		ldbu(e2);
		sex();
		return;
	case FNAME:
		leaxpcr((NMTBL *)e2);
		tfrxd();
		return;
	case CONST:
		if (e2) lddim(e2);
		else clrd();
		return;
	case STRING:
		string(e1);
		return;
	case FUNCTION:
		function(e1);
		return;
	case INDIRECT:
		indirect(e1);
		return;
	case RINDIRECT: case CRINDIRECT:
		rindirect(e1);
		return;
	case ADDRESS:
		gexpr(e2);
		tfrxd();
		return;
	case MINUS:
		gexpr(e2);
		fprintf(obuf, "\tNEGA\n\tNEGB\n\tSBCA\t#0\n");
		return;
	case BNOT:
		gexpr(e2);
		fprintf(obuf, "\tCOMA\n\tCOMB\n");
		return;
	case PREINC:
		switch (car(e2))
		{case GVAR: case LVAR:
			ldd(e2);
			adddim(caddr(e1));
			std(e2);
			return;
		default:
			gexpr(e2);
			lddx();
			adddim(caddr(e1));
			stdx();
			return;
		}
	case POSTINC:
		switch (car(e2))
		{case GVAR: case LVAR:
			ldd(e2);
			adddim(e3 = caddr(e1));
			std(e2);
			subdim(e3);
			return;
		default:
			gexpr(e2);
			lddx();
			adddim(e3=caddr(e1));
			stdx();
			subdim(e3);
			return;
		}
	case CPOSTINC:
		gexpr(e2);
		ldbx();
		incx();
		sex();
		return;
	case CPREINC:
		gexpr(e2);
		incx();
		ldbx();
		sex();
		return;
	case CPOSTDEC:
		gexpr(e2);
		ldbx();
		decx();
		sex();
		return;
	case CPREDEC:
		gexpr(e2);
		decx();
		ldbx();
		sex();
		return;
	case MUL: case UMUL:
		if (car(e3=caddr(e1)) == CONST)
		{	if (0 < (e3 = cadr(e3)) && e3 <= 10)
			{	gexpr(e2);
				switch (e3)
				{case 8:
					asld();
				case 4:
					asld();
				case 2:
					asld();
				case 1:
					return;
				case 10:
					asld();
				case 5:
					pushd();
					asld();
					asld();
					addds();
					return;
				case 6:
					asld();
				case 3:
					pushd();
					asld();
					addds();
					return;
				case 9: case 7:
					pushd();
					asld();
					asld();
					asld();
					if (e3 == 9) addds(); else subds();
					return;
				}
			}
		}
	case DIV:    case UDIV:	   case MOD:	case UMOD:
	case LSHIFT: case ULSHIFT: case RSHIFT: case URSHIFT:
		binexpr(e1);
		return;
	case ADD: case SUB: case BAND: case EOR: case BOR:
		machinop(e1);
		return;
	case COND:
		e2=fwdlabel();
		bexpr(cadr(e1),0,e2);
		gexpr(caddr(e1));
		jmp(e3=fwdlabel());
		fwddef(e2);
		gexpr(cadddr(e1));
		fwddef(e3);
		return;
	case ASS: case CASS:
		assign(e1);
		return;
	case ASSOP: case CASSOP:
		assop(e1);
		return;
	case COMMA:
		gexpr(e2);
		gexpr(caddr(e1));
		return;
	default:
		bexpr(e1,1,e2=fwdlabel());
		clrd();
		fprintf(obuf, "\tBRA\t*+5\n");
		fwddef(e2);
		lddim(1);
	}
}

// ----------------------------------------------------------------------------    
void string(int e1)
{
    char *s;
    int i,l,lb;
	s=(char *)cadr(e1);
	lb=fwdlabel();
	if ((l = caddr(e1)) < 128)
		fprintf(obuf, "\tLEAX\t2,PC\n\tBRA\t_%d\n",lb);
	else
		fprintf(obuf, "\tLEAX\t3,PC\n\tLBRA\t_%d\n",lb);
	do
	{	fprintf(obuf, "\tFCB\t%d",*s++);
		for (i=8; --l && --i;) fprintf(obuf, ",%d",*s++);
		fprintf(obuf, "\n");
	}
	while (l);
	fwddef(lb);
}

// ----------------------------------------------------------------------------    
void function(int e1)
{
    int e2,e3,e4,e5,nargs;
    NMTBL *n;
	e2 = cadr(e1);
	nargs = 0;
	for (e3 = caddr(e1); e3; e3 = cadr(e3))
	{	n=(NMTBL *)(e5=(cadr(e4 = car(e3))));
		switch(car(e4))
		{case FNAME:
			leaxpcr(n);
			pushx();
			break;
		case ADDRESS:
			gexpr(e5);
			pushx();
			break;
		default:gexpr(e4);
			pushd();
		}
		++nargs;
	}
	if (car(e2) == FNAME)
	{	n=(NMTBL *)cadr(e2);
		fprintf(obuf, "\tLBSR\t%s\n",n->nm);
	}
	else
	{	gexpr(e2);
		fprintf(obuf, "\tJSR\t,X\n");
	}
	if (nargs) fprintf(obuf, "\tLEAS\t%d,S\n",2*nargs);
}

// ----------------------------------------------------------------------------    
void indirect(int e1)
{
    int e2,e3,e4;
	e3 = cadr(e2 = cadr(e1));
	switch(car(e2))
	{case RGVAR: case RLVAR:
		ldx(e2);
		return;
	case ADD:
		if(car(e3)==ADDRESS)
		{	gexpr(caddr(e2));
			gexpr(cadr(e3));
			opdx("LEAX");
			return;
		}
		switch(car(e4 = caddr(e2)))
		{case RGVAR: case RLVAR:
			gexpr(e3);
			ldx(e4);
			opdx("LEAX");
			return;
		}
	default:
		gexpr(e2);
		tfrdx();
	}
}

// ----------------------------------------------------------------------------    
void machinop(int e1)
{
    int e2,e3;
	e2 = cadr(e1);
	switch (car(e3 = caddr(e1)))
	{case RGVAR: case RLVAR: case CONST:
		gexpr(e2);
		oprt(car(e1),e3);
		return;
	default:
		gexpr(e3);
		pushd();
		gexpr(e2);
		tosop(car(e1));
		return;
	}
}

// ----------------------------------------------------------------------------    
void rindirect(int e1)
{
    char *op;
    int e2,e3,e4,byte,l;
	op = ((byte = (car(e1) == CRINDIRECT)) ? "LDB" : "LDD");
	e3 = cadr(e2 = cadr(e1));
	switch (car(e2))
	{case RGVAR: case RLVAR:
		indir(op,e2);
		sextend(byte);
		return;
	case ADD:
		if(car(e3)==ADDRESS)
		{	gexpr(caddr(e2));
			gexpr(cadr(e3));
			opdx(op);
			sextend(byte);
			return;
		}
		switch(car(e4=caddr(e2)))
		{case RGVAR: case RLVAR:
			gexpr(e3);
			ldx(e4);
			opdx(op);
			sextend(byte);
			return;
		case CONST:
			switch (car(e3))
			{case RGVAR: case RLVAR:
				ldx(e3);
				indexx(op,cadr(e4));
				sextend(byte);
				return;
			}
		default:
			gexpr(e3);
			pushd();
			gexpr(e4);
			pulx();
			opdx(op);
			sextend(byte);
			return;
		}
	case PREINC:
		if ((l = caddr(e2)) == -1 || l == -2)
			switch (car(e3))
			{case GVAR: case LVAR:
				ldx(e3);
				predecx(op,l);
				stx(e3);
				sextend(byte);
				return;
			}
		break;
	case POSTINC:
		if ((l = caddr(e2)) == 1 || l == 2)
			switch (car(e3))
			{case GVAR: case LVAR:
				ldx(e3);
				postincx(op,l);
				stx(e3);
				sextend(byte);
				return;
			}
		break;
	}
	gexpr(e2);
	tfrdx();
	indexx(op,0);
	sextend(byte);
}

// ----------------------------------------------------------------------------    
void assign(int e1)
{
    char *op;
    int e2,e3,e4,e5,l;
	op = (car(e1) == CASS ? "STB" : "STD");
	e3 = cadr(e2 = cadr(e1));
	e4 = caddr(e1);
	switch(car(e2))
	{case GVAR: case LVAR:
		gexpr(e4);
		index_(op,e2);
		return;
	case INDIRECT:
		switch(car(e3))
		{case RGVAR: case RLVAR:
			gexpr(e4);
			indir(op,e3);
			return;
		case ADD:
			if (car(caddr(e3)) == CONST)
				switch (car(e5=cadr(e3)))
				{case RGVAR: case RLVAR:
					gexpr(e4);
					ldx(e5);
					indexx(op,cadr(caddr(e3)));
					return;
				}
			break;
		case PREINC:
			if ((l = caddr(e3)) == -1 || l == -2)
				switch (car(e5=cadr(e3)))
				{case GVAR: case LVAR:
					gexpr(e4);
					ldx(e5);
					predecx(op,l);
					stx(e5);
					return;
				}
			break;
		case POSTINC:
			if ((l = caddr(e3)) == 1 || l == 2)
				switch (car(e5=cadr(e3)))
				{case GVAR: case LVAR:
					gexpr(e4);
					ldx(e5);
					postincx(op,l);
					stx(e5);
					return;
				}
			break;
		}
	}
	switch (car(e4))
	{case RGVAR: case CRGVAR: case RLVAR: case CRLVAR: case CONST:
		gexpr(e2);
		gexpr(e4);
		break;
	default:
		gexpr(e4);
		pushd();
		gexpr(e2);
		pulld();
	}
	indexx(op,0);
	return;
}

// ----------------------------------------------------------------------------    
void assop(int e1)
{
    int e2,e3,byte,op;
    char *ldop,*stop;
	ldop = ((byte = (car(e1) == CASSOP)) ? "LDB" : "LDD");
	stop = (byte ? "STB" : "STD");
	e2 = cadr(e1);
	e3 = caddr(e1);
	op = cadddr(e1);
	switch (car(e2))
	{case GVAR: case LVAR:
		switch (car(e3))
		{case RGVAR: case RLVAR: case CONST:
			if (simpop(op))
			{	index_(ldop,e2);
				sextend(byte);
				oprt(op,e3);
				index_(stop,e2);
				return;
			}
		default:
			gexpr(e3);
			pushd();
			index_(ldop,e2);
			sextend(byte);
			tosop(op);
			index_(stop,e2);
			return;
		}
	default:
		switch (car(e3))
		{case RGVAR: case RLVAR: case CONST:
			if (simpop(op))
			{	gexpr(e2);
				indexx(ldop,0);
				sextend(byte);
				oprt(op,e3);
				indexx(stop,0);
				return;
			}
		default:
			gexpr(e3);
			pushd();
			gexpr(e2);
			indexx(ldop,0);
			sextend(byte);
			tosop(op);
			indexx(stop,0);
			return;
		}
	}
}

// ----------------------------------------------------------------------------    
int simpop(int op)
{	return (op == ADD || op == SUB ||
		op == BAND || op == EOR || op == BOR);
}

// ----------------------------------------------------------------------------    
void oprt(int op, int e1)
{
    int e2;
	e2 = cadr(e1);
	switch (car(e1))
	{case RGVAR:
		oprt1(op,"Y",e2);
		return;
	case RLVAR:
		oprt1(op,"U",e2);
		return;
	case CONST:
		oprtc(op,e2);
		return;
	}
}

// ----------------------------------------------------------------------------    
void oprt1(int op, char *index, int n)
{	
    switch (op)
	{case ADD:
		fprintf(obuf, "\tADDD\t%d,%s\n",n,index);
		return;
	case SUB:
		fprintf(obuf, "\tSUBD\t%d,%s\n",n,index);
		return;
	case BAND: case EOR: case BOR:
		dualop(op,index,n);
		return;
	}
}

// ----------------------------------------------------------------------------    
void dualop(int op, char *index, int n)
{
    char *ops;
	ops =  ((op == BAND) ? "AND" :
		(op == EOR)  ? "EOR" :
		(op == BOR)  ? "OR"  : (char *)DEBUG);
	fprintf(obuf, "\t%sA\t%d,%s\n\t%sB\t%d+1,%s\n",ops,n,index,ops,n,index);
}

// ----------------------------------------------------------------------------    
void oprtc(int op, int n)
{
    switch (op)
	{case ADD:
		adddim(n);
		return;
	case SUB:
		subdim(n);
		return;
	case BAND: case EOR: case BOR:
		dualc(op,n);
		return;
	}
}

// ----------------------------------------------------------------------------    
void dualc(int op, int n)
{
    char *ops;
	ops =  ((op == BAND) ? "AND" :
		(op == EOR)  ? "EOR" :
		(op == BOR)  ? "OR"  : (char *)DEBUG);
	fprintf(obuf, "\t%sA\t#%d\n\t%sB\t#%d\n",ops,(n >> 8) & 0xff,ops,n & 0xff);
}

// ----------------------------------------------------------------------------    
void tosop(int op)
{	
    switch (op)
	{case ADD:
		addds();
		return;
	case SUB:
		subds();
		return;
	case BAND: case EOR: case BOR:
		dualtosop(op);
		return;
	default:
		pulx();
		library(op);
	}
}

// ----------------------------------------------------------------------------    
void dualtosop(int op)
{
    char *ops;
	ops =  ((op == BAND) ? "AND" :
		(op == EOR)  ? "EOR" :
		(op == BOR)  ? "OR"  : (char *)DEBUG);
	fprintf(obuf, "\t%sA\t,S+\n\t%sB\t,S+\n",ops,ops);
}

// ----------------------------------------------------------------------------    
void pushd()
{
    fprintf(obuf, "\tPSHS\tD\n");
}

// ----------------------------------------------------------------------------    
void pushx()
{
    fprintf(obuf, "\tPSHS\tX\n");
}

// ----------------------------------------------------------------------------    
void pulld()
{
    fprintf(obuf, "\tPULS\tD\n");
}

// ----------------------------------------------------------------------------    
void pulx()
{
    fprintf(obuf, "\tPULS\tX\n");
}

// ----------------------------------------------------------------------------    
void tfrdx()
{
    fprintf(obuf, "\tTFR\tD,X\n");
}

// ----------------------------------------------------------------------------    
void tfrxd()
{
    fprintf(obuf, "\tTFR\tX,D\n");
}

// ----------------------------------------------------------------------------    
void asld()
{	
    fprintf(obuf, "\tASLB\n\tROLA\n");
}

// ----------------------------------------------------------------------------    
void adddim(n)
{	
    fprintf(obuf, "\tADDD\t#%d\n",n);
}

// ----------------------------------------------------------------------------    
void subdim(n)
{	
    fprintf(obuf, "\tSUBD\t#%d\n",n);
}

// ----------------------------------------------------------------------------    
void cmpdimm(int n)
{	
    fprintf(obuf, "\tCMPD\t#%d\n",n);
}

// ----------------------------------------------------------------------------    
void addds()
{
    fprintf(obuf, "\tADDD\t,S++\n");
}

// ----------------------------------------------------------------------------    
void subds()
{
    fprintf(obuf, "\tSUBD\t,S++\n");
}

// ----------------------------------------------------------------------------    
void clrd()
{	
    fprintf(obuf, "\tCLRA\n\tCLRB\n");
}

// ----------------------------------------------------------------------------    
void lddim(int n)
{
    fprintf(obuf, "\tLDD\t#%d\n",n);
}

// ----------------------------------------------------------------------------    
void ldd(int e)
{
    switch (car(e))
	{case GVAR:
		lddy(cadr(e));
		return;
	case LVAR:
		lddu(cadr(e));
		return;
	default:
		DEBUG;
	}
}

// ----------------------------------------------------------------------------    
void lddx()
{	
    fprintf(obuf, "\tLDD\t,X\n");
}

// ----------------------------------------------------------------------------    
void lddy(int n)
{
    fprintf(obuf, "\tLDD\t%d,Y\n",n);
}

// ----------------------------------------------------------------------------    
void lddu(int n)
{	
    fprintf(obuf, "\tLDD\t%d,U\n",n);
}

// ----------------------------------------------------------------------------    
void std(int e)
{
    switch (car(e)) {
	case GVAR:
		stdy(cadr(e));
		return;
	case LVAR:
		stdu(cadr(e));
		return;
	default:
		DEBUG;
	}
}

// ----------------------------------------------------------------------------    
void stdx()
{
    fprintf(obuf, "\tSTD\t,X\n");
}

// ----------------------------------------------------------------------------    
void stdy(int n)
{
    fprintf(obuf, "\tSTD\t%d,Y\n",n);
}

// ----------------------------------------------------------------------------    
void stdu(int n)
{
    fprintf(obuf, "\tSTD\t%d,U\n",n);
}

// ----------------------------------------------------------------------------    
void ldbx()
{
    fprintf(obuf, "\tLDB\t,X\n");
}

// ----------------------------------------------------------------------------    
void ldby(int n)
{
    fprintf(obuf, "\tLDB\t%d,Y\n",n);
}

// ----------------------------------------------------------------------------    
void ldbu(int n)
{
    fprintf(obuf, "\tLDB\t%d,U\n",n);
}

// ----------------------------------------------------------------------------    
void predecx(char *op, int l)
{
    fprintf(obuf, "\t%s\t,%sX\n",op,(l == -1 ? "-" : "--"));
}

// ----------------------------------------------------------------------------    
void postincx(char *op, int l)
{
    fprintf(obuf, "\t%s\t,X%s\n",op,(l == 1 ? "+" : "++"));
}

// ----------------------------------------------------------------------------    
void leaxy(int n)
{
    fprintf(obuf, "\tLEAX\t%d,Y\n",n);
}

// ----------------------------------------------------------------------------    
void leaxu(int n)
{	
    fprintf(obuf, "\tLEAX\t%d,U\n",n);
}

// ----------------------------------------------------------------------------    
void leaxpcr(NMTBL *n)
{
    fprintf(obuf, "\tLEAX\t%s,PCR\n",n->nm);
}

// ----------------------------------------------------------------------------    
void ldx(int e)
{	
    switch (car(e)) {
	case GVAR: case RGVAR:
		ldxy(cadr(e));
		return;
	case LVAR: case RLVAR:
		ldxu(cadr(e));
		return;
	default:
		DEBUG;
	}
}

// ----------------------------------------------------------------------------    
void ldxy(int n)
{
    fprintf(obuf, "\tLDX\t%d,Y\n",n);
}

// ----------------------------------------------------------------------------    
void ldxu(int n)
{
    fprintf(obuf, "\tLDX\t%d,U\n",n);
}

// ----------------------------------------------------------------------------    
void stx(int e)
{
    switch (car(e)) {
	case GVAR:
		stxy(cadr(e));
		return;
	case LVAR:
		stxu(cadr(e));
		return;
	default:
		DEBUG;
	}
}

// ----------------------------------------------------------------------------    
void stxy(int n)
{	
    fprintf(obuf, "\tSTX\t%d,Y\n",n);
}

// ----------------------------------------------------------------------------    
void stxu(int n)
{	
    fprintf(obuf, "\tSTX\t%d,U\n",n);
}

// ----------------------------------------------------------------------------    
void sex()
{	
    fprintf(obuf, "\tSEX\n");
}

// ----------------------------------------------------------------------------    
void incx()
{	
    fprintf(obuf, "\tINC\t,X\n");
}

// ----------------------------------------------------------------------------    
void decx()
{	
    fprintf(obuf, "\tDEC\t,X\n");
}

// ----------------------------------------------------------------------------    
void opdx(char *op)
{
    fprintf(obuf, "\t%s\tD,X\n",op);
}

// ----------------------------------------------------------------------------    
void indexx(char *op, int n)
{
    fprintf(obuf, "\t%s\t%d,X\n",op,n);
}

// ----------------------------------------------------------------------------    
void index_(char *op, int e)
{
    switch (car(e)) {
	case GVAR:
		indexy(op,cadr(e));
		return;
	case LVAR:
		indexu(op,cadr(e));
		return;
	default:
		DEBUG;
	}
}

// ----------------------------------------------------------------------------    
void indexy(char *op, int n)
{
    fprintf(obuf, "\t%s\t%d,Y\n",op,n);
}

// ----------------------------------------------------------------------------    
void indexu(char *op, int n)
{
    fprintf(obuf, "\t%s\t%d,U\n",op,n);
}

// ----------------------------------------------------------------------------    
void indir(char *op, int e)
{
    switch (car(e)) {
	case RGVAR:
		indiry(op,cadr(e));
		return;
	case RLVAR:
		indiru(op,cadr(e));
		return;
	default:
		DEBUG;
	}
}

// ----------------------------------------------------------------------------    
void indiry(char *op, int n)
{
    fprintf(obuf, "\t%s\t[%d,Y]\n",op,n);
}

// ----------------------------------------------------------------------------    
void indiru(char *op, int n)
{
    fprintf(obuf, "\t%s\t[%d,U]\n",op,n);
}

// ----------------------------------------------------------------------------    
void sextend(int byte)
{
    if (byte)
		sex();
}

// ----------------------------------------------------------------------------    
void binexpr(int e1)
{	
    gexpr(caddr(e1));
	pushd();
	gexpr(cadr(e1));
	pulx();
	library(car(e1));
}

// ----------------------------------------------------------------------------    
void library(int op)
{
    fprintf(obuf, "\tLBSR\t_0000%d\n",
	       ((op == MUL || op == UMUL) ? 1 :
		(op == DIV)	? 2 :
		(op == UDIV)	? 3 :
		(op == MOD)	? 4 :
		(op == UMOD)	? 5 :
		(op == LSHIFT)	? 6 :
		(op == ULSHIFT) ? 7 :
		(op == RSHIFT)	? 8 :
		(op == URSHIFT) ? 9 : DEBUG));
}

// ----------------------------------------------------------------------------    
int cexpr(e)
{
    if (car(e) != CONST) error(CNERR);
	return (cadr(e));
}

// ----------------------------------------------------------------------------    
int getsym()
{
    NMTBL *nptr0,*nptr1;
    int i;
    char c;
	if (alpha(skipspc())) {
		i = hash = 0;
		while (alpha(ch) || digit(ch)) {
			if (i <= 7) 
				hash = 7*(hash+(name[i++] = ch));
			getch();
		}
		name[i] = '\0';
		nptr0 = gsearch();
		if (nptr0->sc == RESERVE)
			return sym = nptr0->dsp;
		if (nptr0->sc == MACRO && !mflag) {
			mflag++;
			chsave = ch;
			chptrsave = chptr;
			chptr = (char *)nptr0->dsp;
			getch();
			return getsym();
		}
		sym = IDENT;
		gnptr=nptr=nptr0;
		if (mode==GDECL || mode==GSDECL || mode==GUDECL ||
		    mode==GTDECL || mode==TOP)
			return sym;
		nptr1=losearch();
		if (mode==STAT) {
			if (nptr1->sc == EMPTY) {
				return sym;
			} else {
				nptr=nptr1;
				return sym;
			}
		}
		nptr=nptr1;
		return sym;
	}
	else if (digit(ch)) {
		symval=0;
		if (ch == '0') {
			if (getch() == 'x' || ch == 'X')
				while(1)
					if(digit(getch()))
						symval=symval*16+ch-'0';
					else if('a'<=ch&&ch<='f')
						symval=symval*16+ch-'a'+10;
					else if('A'<=ch&&ch<='F')
						symval=symval*16+ch-'A'+10;
					else break;
			else while (digit(ch)) {symval=symval*8+ch-'0';getch();}
		}
		else while(digit(ch)) {symval=symval*10+ch-'0';getch();}
		return sym=CONST;
	}
	else if(ch=='\'')
	{	getch();
		symval=escape();
		if(ch!='\'') error(CHERR);
		getch();
		return sym=CONST;
	}
	else if(ch=='"')
	{	getstring();
		return sym= STRING;
	}
	c=ch;
	getch();
	switch(c)
	{case '*':
		return postequ(MUL,MUL+AS);
	case '&':
		if(ch=='&') {getch();return sym=LAND;}
		return postequ(BAND,BAND+AS);
	case '-':
		if(ch=='>') {getch();return sym=ARROW;}
		if(ch=='-') {getch();return sym=DEC;}
		return postequ(SUB,SUB+AS);
	case '!':
		return postequ(LNOT,NEQ);
	case '~':
		return sym=BNOT;
	case '+':
		if(ch=='+') {getch();return sym=INC;}
		return postequ(ADD,ADD+AS);
	case '%':
		return postequ(MOD,MOD+AS);
	case '^':
		return postequ(EOR,EOR+AS);
	case '|':
		if(ch=='|') {getch();return sym=LOR;}
		return postequ(BOR,BOR+AS);
	case '=':
		return postequ(ASS,EQ);
	case '>':
		if(ch=='>') {getch();return postequ(RSHIFT,RSHIFT+AS);}
		return postequ(GT,GE);
	case '<':
		if(ch=='<') {getch();return postequ(LSHIFT,LSHIFT+AS);}
		return postequ(LT,LE);
	case '(':
		return sym=LPAR;
	case ')':
		return sym=RPAR;
	case '[':
		return sym=LBRA;
	case ']':
		return sym=RBRA;
	case '{':
		return sym=LC;
	case '}':
		return sym=RC;
	case ',':
		return sym=COMMA;
	case ';':
		return sym=SM;
	case ':':
		return sym=COLON;
	case '?':
		return sym=COND;
	case '.':
		return sym=PERIOD;
	case '/':
		if(ch!='*') return postequ(DIV,DIV+AS);
		getch();
		while(ch=='*'?getch()!='/':getch());
		getch();
		return getsym();
	default:
		error(CHERR);
		return getsym();
	}
}

// ----------------------------------------------------------------------------    
int postequ(int s1, int s2)
{
    if(ch=='=') {getch();return sym=s2;}
	return sym=s1;
}
// ----------------------------------------------------------------------------    
int alpha(char c)
{
    return('a'<=c&&c<='z'||'A'<=c&&c<='Z'||c=='_');
}

// ----------------------------------------------------------------------------    
int digit(char c)
{
    return('0'<=c&&c<='9');
}

// ----------------------------------------------------------------------------    
NMTBL *gsearch()
{
    NMTBL *nptr,*iptr;
	iptr=nptr= &ntable[hash % GSYMS];
	while(nptr->sc!=EMPTY && neqname(nptr->nm))
	{	if (++nptr== &ntable[GSYMS]) nptr=ntable;
		if (nptr==iptr) error(GSERR);
	}
	if (nptr->sc == EMPTY) copy(nptr->nm);
	return nptr;
}

// ----------------------------------------------------------------------------    
NMTBL *losearch()
{
    NMTBL *nptr,*iptr;
	iptr=nptr= &ntable[hash%LSYMS+GSYMS];
	while(nptr->sc!=EMPTY && neqname(nptr->nm))
	{	if (++nptr== &ntable[LSYMS+GSYMS]) nptr= &ntable[GSYMS];
		if (nptr==iptr) error(LSERR);
	}
	if (nptr->sc == EMPTY) copy(nptr->nm);
	return nptr;
}

// ----------------------------------------------------------------------------    
int neqname(char *p)
{
    char *q;
	q=name;
	while(*p) if(*p++ != *q++) return 1;
	return *q!=0;
}

// ----------------------------------------------------------------------------    
void copy(char *p)
{
    char *q;
	q=name;
	while(*p++= *q++);
}

// ----------------------------------------------------------------------------    
void getstring()
{	
    getch();
	symval = 0;
	sptr = cheapp;
	while (ch != '"')
	{	*cheapp++ = escape();
		symval++;
		if (cheapp >= cheap+CHEAPSIZE) error(STRERR);
	}
	getch();
	*cheapp++ = '\0';
	symval++;
}

// ----------------------------------------------------------------------------    
int skipspc()
{	
    while(ch=='\t'||ch=='\n'||ch==' '||ch=='\r') getch();
	return ch;
}

// ----------------------------------------------------------------------------    
int getch()
{	
    if(*chptr) return ch= *chptr++;
	if(mflag) {mflag=0;chptr=chptrsave;return ch=chsave;}
	getl();
	return getch();
}

// ----------------------------------------------------------------------------    
char escape()
{
    char c;
	if ((c=ch) == '\\')
	{	if (digit(c=getch()))
		{	c = ch-'0';
			if (digit(getch()))
			{	c = c*8+ch-'0';
				if (digit(getch())) {c=c*8+ch-'0';getch();}
			}
			return c;
		}
		getch();
		switch(c)
		{case 'n':
			return '\n';
		case 't':
			return '\t';
		case 'b':
			return '\b';
		case 'r':
			return '\r';
		case 'f':
			return '\f';
		case '\n':
			return escape();
		default:
			return c;
		}
	}
	if (c == '\n') error(EXERR);
	getch();
	return c;
}

// ----------------------------------------------------------------------------    
FILE *getfname()
{
    int i;
    char name[LBUFSIZE];
	getch();
	if(skipspc()!='"') error(INCERR);
	for(i=0;(getch()!='"' && ch!='\n');)
		if(i<LBUFSIZE-1) name[i++]=ch;
	if(ch=='\n') error(INCERR);
	name[i]=0;
	return ( (filep+1)->fcb = fopen(name,"r") );
}

// ----------------------------------------------------------------------------    
void getl()
{
    int i;
    int c;
	lineno++;
	glineno++;
	chptr=linebuf;
	i=0;
	while ((*chptr++ = c = getc(filep->fcb)) != '\n')
	{	if (++i > LBUFSIZE-2) error(LNERR);
		if (c==EOF)
		{	error(EOFERR);
			--chptr;
		}
	}
	*chptr = '\0';
	if (lsrc && !asmf) fprintf(obuf, "* %s",linebuf);
	if (*(chptr = linebuf) == '#')
	{	++chptr;
		if (macroeq("define"))
		{	i=mode;
			mode=GDECL;
			ch= *chptr;
			if (getsym() == IDENT)
			{	if (nptr->sc == EMPTY)
				{	nptr->sc = MACRO;
					nptr->dsp = (int)cheapp;
					while ((*cheapp++ = c = *chptr++)
							&& c != '\n');
					*cheapp++ = '\0';
					if (cheapp >= cheap+CHEAPSIZE)
						error(STRERR);
					if (!c) error(EOFERR);
				}
				else error(MCERR);
			}
			else error(MCERR);
			mode=i;
			*(chptr = linebuf) = '\0';
		}
		else if (macroeq("include"))
		{	printf("%s",linebuf);
			if(filep+1 >= filestack + FILES) error(FILERR);
			if ( ((filep+1)->fcb=getfname()) == NULL) error(FILERR);
			(filep+1)->ln=lineno;
			lineno=0;
			++filep;
			*(chptr = linebuf) = '\0';
		}
		else if (macroeq("asm"))
		{	if (asmf) error(MCERR);
			asmf = 1;
			getl();
			while (asmf)
			{	fprintf(obuf, "%s",linebuf);
				getl();
			}
		}
		else if (macroeq("endasm"))
		{	if (!asmf) error(MCERR);
			asmf = 0;
		}
		else if (macroeq(" "))
			getl();
		else error(MCERR);
	}
}

// ----------------------------------------------------------------------------    
int macroeq(char *s)
{
    char *p;
	for (p = chptr; *s;) if (*s++ != *p++) return 0;
	chptr = p;
	return 1;
}

// ----------------------------------------------------------------------------    
int car(int e)
{
    return heap[e];
}

// ----------------------------------------------------------------------------    
int cadr(int e)
{
    return heap[e+1];
}

// ----------------------------------------------------------------------------    
int caddr(int e)
{
    return heap[e+2];
}

// ----------------------------------------------------------------------------    
int cadddr(int e)
{	return heap[e+3];
}

// ----------------------------------------------------------------------------    
int list2(int e1, int e2)
{
    int e;
	e=getfree(2);
	heap[e]=e1;
	heap[e+1]=e2;
	return e;
}

// ----------------------------------------------------------------------------    
int list3(int e1, int e2, int e3)
{
    int e;
	e=getfree(3);
	heap[e]=e1;
	heap[e+1]=e2;
	heap[e+2]=e3;
	return e;
}

// ----------------------------------------------------------------------------    
int list4(int e1, int e2, int e3, int e4)
{
    int e;
	e=getfree(4);
	heap[e]=e1;
	heap[e+1]=e2;
	heap[e+2]=e3;
	heap[e+3]=e4;
	return e;
}

// ----------------------------------------------------------------------------    
int getfree(int n)
{
    int e;
	switch (mode)
	{case GDECL: case GSDECL: case GUDECL: case GTDECL:
		e=gfree;
		gfree+=n;
		break;
	default:
		lfree-=n;
		e=lfree;
	}
	if(lfree<gfree) error(HPERR);
	return e;
}

// ----------------------------------------------------------------------------    
int rplacad(int e, int n)
{
    heap[e+1]=n;
	return e;
}
