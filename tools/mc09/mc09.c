
#define DEBUG	error(-1)

/*#include "CCLIB.TXT"
*/
#include <stdlib.h>
#include <stdio.h>

#define INT	(-1)
#define CHAR	(-2)
#define UNSIGNED	(-3)
#define POINTER (-4)
#define ARRAY	(-5)
#define STRUCT	(-6)
#define UNION	(-7)
#define FUNCTION	(-8)
#define EMPTY	(-9)

#define STATIC	(-10)
#define GOTO	(-11)
#define RETURN	(-12)
#define BREAK	(-13)
#define CONTINUE	(-14)
#define IF	(-15)
#define ELSE	(-16)
#define FOR	(-17)
#define DO	(-18)
#define WHILE	(-19)
#define SWITCH	(-20)
#define CASE	(-21)
#define DEFAULT (-22)
#define RESERVE (-23)
#define TAG	(-24)
#define FIELD	(-25)
#define IDENT	(-26)
#define STRING	(-27)
#define MACRO	(-28)
#define BLABEL	(-29)
#define FLABEL	(-30)
#define TYPEDEF (-31)
#define SIZEOF	(-32)
#define TYPE	(-33)
#define LONG	(-34)
#define SHORT	(-35)

#define TOP	0
#define GDECL	1
#define GSDECL	2
#define GUDECL	3
#define ADECL	4
#define LDECL	5
#define LSDECL	6
#define LUDECL	7
#define STADECL 8
#define STAT	9
#define GTDECL	10
#define LTDECL	11

#define GVAR	1
#define RGVAR	2
#define CRGVAR	3
#define LVAR	4
#define RLVAR	5
#define CRLVAR	6
#define CONST	7
#define FNAME	8
#define INDIRECT	9
#define RINDIRECT	10
#define CRINDIRECT	11
#define ADDRESS 12
#define MINUS	13
#define LNOT	14
#define BNOT	15
#define INC	16
#define POSTINC 17
#define PREINC	18
#define CPOSTINC	19
#define CPREINC 20
#define DEC	21
#define CPOSTDEC	22
#define CPREDEC 23
#define MUL	24
#define UMUL	25
#define DIV	26
#define UDIV	27
#define MOD	28
#define UMOD	29
#define ADD	30
#define SUB	31
#define RSHIFT	32
#define URSHIFT 33
#define LSHIFT	34
#define ULSHIFT 35
#define GT	36
#define UGT	37
#define GE	38
#define UGE	39
#define LT	40
#define ULT	41
#define LE	42
#define ULE	43
#define EQ	44
#define NEQ	45
#define BAND	46
#define EOR	47
#define BOR	48
#define LAND	49
#define LOR	50
#define COND	51
#define ASS	52
#define CASS	53
#define ASSOP	54
#define CASSOP	55
#define COMMA	56
#define LPAR	57
#define RPAR	58
#define LBRA	59
#define RBRA	60
#define LC	61
#define RC	62
#define COLON	63
#define SM	64
#define PERIOD	65
#define ARROW	66

#define US	1
#define AS	100

#define FILERR	1
#define DCERR	2
#define STERR	3
#define EXERR	4
#define CNERR	5
#define CHERR	6
#define GSERR	7
#define LSERR	8
#define STRERR	9
#define LNERR	10
#define EOFERR	11
#define MCERR	12
#define INCERR	13
#define HPERR	14
#define TYERR	15
#define LVERR	16
#define UDERR	17
#define OPTION	18

#define GSYMS	450
#define LSYMS	50

#define HEAPSIZE	1000
#define CHEAPSIZE	3000
#define LBUFSIZE	256

#define FILES 3

int sym,ch,chsave,type,mode,gfree,lfree,mflag,lineno,glineno;
int labelno,gpc,lvar,disp;
int symval,args,heap[HEAPSIZE];
int blabel,clabel,dlabel,cslabel,ilabel,control,ac,ac2,lsrc,chk,asmf;

unsigned hash;

char linebuf[LBUFSIZE],cheap[CHEAPSIZE],*chptr,*chptrsave;
char name[9],*cheapp,**av,/*obuf[320],*/*sptr,escape();

FILE *obuf;

typedef struct nametable {
	char nm[9];
	int sc,ty,dsp; } NMTBL;

NMTBL ntable[GSYMS+LSYMS],*nptr,*gnptr,*decl0(),*decl1(),*lsearch(),*gsearch();

struct {int fd,ln;/*char fcb[320]*/FILE *fcb;} *filep,filestack[FILES];

main(argc,argv)
int argc;
char **argv;
{NMTBL *nptr;
int i;
char *ccout;
	if(argc==1) exit(1);
	lsrc = chk = asmf = 0;
	ccout = "c.out";
	ac=argc;
	av=argv;
	for (ac2=1; (ac2 < ac) && (*av[ac2] == '-'); ++ac2)
		switch (*(av[ac2]+1))
		{case 'S': case 's':
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
	fclose(stdout);
	if (!chk)
		if ( (obuf = fopen(ccout,"w")) == NULL ) error(FILERR);
	init();
	while(1)
	{	for (nptr = &ntable[GSYMS],i=LSYMS; i--;)
			(nptr++)->sc = EMPTY;
		mode=TOP;
		while(getsym()==SM);
		mode=GDECL;
		args=0;
		decl();
	}
}
error(n)
int n;
{	if(n == EOFERR)
		if(filep!=filestack)
		{	lineno=filep->ln;
			fclose(filep->fcb);
			fprintf(stderr,"End of inclusion.\n");
			--filep;
			return;
		}
		else if(ac2!=ac)
		{	fclose(filep->fcb);
			newfile();
			return;
		}
		else if(mode == TOP)
		{	fprintf(stderr,"\nCompiled %u lines.\n",glineno-1);
			if (!chk) fprintf(stderr,
				"Total internal labels	: %u.\n",labelno-1);
			fprintf(stderr,
				"Total global variables : %u bytes.\n\n",gpc);
			printf("_%d\tRTS\n_INITIALIZE\tEQU\t_1\n",ilabel);
			printf("_GLOBALS\tEQU\t%u\n\tEND\n",gpc);
			exit(0);
		}
	fprintf(stderr,"%5d:%s.\n",lineno,
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
}
errmsg()
{char *p,*lim;
	if(lineno==0) return;
	fprintf(stderr,"%s",linebuf);
	lim=(mflag?chptrsave:chptr);
	for (p=linebuf; p < lim;)
		fprintf(stderr,(*p++ == '\t') ? "\t" : " ");
	fprintf (stderr,"^\n");
}
checksym(s)
int s;
{char *p;
	if (sym != s)
	{	p=(s==RPAR) ? "')'": (s==RBRA) ? "']'": (s==SM) ? "';'":
		  (s==LPAR) ? "'('": (s==WHILE) ? "'while'":
		  (s==COLON) ? "':'": "Identifier";
		fprintf(stderr,"%d:%s expected.\n",lineno,p);
		errmsg();
	}
	else getsym();
}
init()
{NMTBL *nptr;
int i;
	for(nptr = ntable,i = GSYMS; i--;) (nptr++)->sc = EMPTY;
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
	gpc=glineno=mflag=0;
	gfree=ilabel=1;
	labelno=2;
	cheapp=cheap;
	lfree=HEAPSIZE;
	filep=filestack;
	newfile();
	getline();
	getch();
}
newfile()
{	lineno=0;
	fprintf(stderr,"%s:\n",av[ac2]);
	if ( (filep->fcb = fopen(av[ac2++],"r")) == NULL ) error(FILERR);
}
reserve(s,d)
char *s;
int d;
{NMTBL *nptr;
char *t;
	hash=0;
	t=name;
	while(*t++ = *s) hash=7*(hash+*s++);
	(nptr = gsearch())->sc = RESERVE;
	nptr->dsp = d;
}

decl()
{NMTBL *n;
int t;
	if(sym==STATIC)
		if(mode==LDECL)
		{	getsym();
			mode=STADECL;
		}
		else error(DCERR);
	else if(sym==TYPEDEF)
		if(mode==GDECL)
		{	getsym();
			mode=GTDECL;
		}
		else if(mode==LDECL)
		{	getsym();
			mode=LTDECL;
		}
		else error(DCERR);
	if((t=typespec())==0) return;
	if(sym==SM) return;
	type=t;
	n=decl0();
	reverse(t);
	if(args||sym==LC) {fdecl(n);return;}
	def(n);
	while(sym==COMMA)
	{	getsym();
		type=t;
		n=decl0();
		reverse(t);
		if(args) error(DCERR);
		def(n);
	}
	if(sym!=SM) error(DCERR);
	if(mode==GTDECL) mode=GDECL;
	if(mode==STADECL||mode==LTDECL) mode=LDECL;
}
typespec()
{int t;
	switch(sym)
	{case INT:
	case CHAR:
		t= sym;
		getsym();
		break;
	case STRUCT:
	case UNION:
		t=sdecl(sym);
		break;
	case UNSIGNED:
		t = UNSIGNED;
		if(getsym()==INT) getsym();
		break;
	case SHORT:
		t=CHAR;
		if(getsym()==INT) getsym();
		break;
	case LONG:
		t=INT;
		if(getsym()==INT) getsym();
		break;
	default:
		if(sym==IDENT)
			if(nptr->sc==TYPE)
			{	t=nptr->ty;
				getsym();
				break;
			}
			else if(nptr->sc==EMPTY && gnptr->sc==TYPE)
			{	t=gnptr->ty;
				getsym();
				break;
			}
		if(mode==LDECL) return 0;
		t= INT;
	}
	return t;
}
struct nametable *decl0()
{NMTBL *n;
	if(sym==MUL)
	{	getsym();
		n=decl0();
		type=list2(POINTER,type);
		return n;
	}
	return decl1();
}
NMTBL *decl1()
{NMTBL *n;
int i,t;
	if(sym==LPAR)
	{	getsym();
		n=decl0();
		checksym(RPAR);
	}
	else if (sym == IDENT)
	{	n=nptr;
		getsym();
	}
	else error(DCERR);
	while(1)
		if(sym==LBRA)
			if(getsym()==RBRA)
			{	getsym();
				if(mode!=ADECL) error(DCERR);
				t=type;
				type=list2(POINTER,type);
			}
			else
			{	t=type;
				i=cexpr(expr());
				checksym(RBRA);
				type=list3(ARRAY,t,i);
			}
		else if(sym==LPAR)
		{	if(mode==GDECL) {mode=ADECL;getsym();mode=GDECL;}
			else getsym();
			if(sym==RPAR) getsym();
			else
			{	n->sc=FUNCTION;
				adecl();
				n->sc=EMPTY;
			}
			type=list2(FUNCTION,type);
		}
		else return n;
}
adecl()
{	if(mode!=GDECL) error(DCERR);
	mode=ADECL;
	args= 2;
	while(1)
	{	if(sym!=IDENT) error(DCERR);
		nptr->ty = INT;
		nptr->sc = LVAR;
		nptr->dsp = (args += 2);
		if(getsym()!=COMMA) break;
		getsym();
	}
	checksym(RPAR);
	mode=GDECL;
	return;
}
reverse(t1)
int t1;
{int t2,t3;
	t2=t1;
	while(type!=t1)
	{	t3=cadr(type);
		rplacad(type,t2);
		t2=type;
		type=t3;
	}
	type=t2;
}
size(t)
int t;
{	if(t==CHAR) return 1;
	if(scalar(t)) return 2;
	if(car(t)==STRUCT||car(t)==UNION)
	{	if(cadr(t)==-1) error(DCERR);
		return(cadr(t));
	}
	if(car(t)==ARRAY) return(size(cadr(t))*caddr(t));
	else error(DCERR);
	/*NOTREACHED*/
}
def(n)
NMTBL *n;
{int sz,nsc,ndsp,slfree,l,t,e;
	if(car(type)==FUNCTION)
	{	fcheck(n);
		return;
	}
	if (n->sc!=EMPTY &&
	    (mode!=ADECL || n->sc!=LVAR || n->ty!=INT) &&
	    (mode!=GSDECL&&mode!=LSDECL || n->sc!=FIELD || n->dsp!=disp) &&
	    (mode!=GUDECL&&mode!=LUDECL || n->sc!=FIELD || n->dsp!=0) )
		 error(DCERR);
	sz = size(n->ty = type);
	switch(mode)
	{case GDECL:
		printf("%s\tEQU\t%u\n",n->nm,gpc);
	case STADECL:
		nsc = GVAR;
		ndsp = gpc;
		if(sym==ASS)
		{	t=type;
			if(!scalar(t))
				error(TYERR);
			if(mode==STADECL) printf("\tBRA\t_%d\n",l=fwdlabel());
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
sdecl(s)
int s;
{int smode,sdisp,type;
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
fdecl(n)
NMTBL *n;
{	args=0;
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
	printf("%s\n\tPSHS\tU\n\tLEAU\t,S\n",n->nm);
	if(disp) printf("\tLEAS\t%d,S\n",disp);
	lvar= -disp;
	while(sym!=RC) statement();
	if (control) return2();
}
fcheck(n)
NMTBL *n;
{	if(mode!=GDECL||car(type)!=FUNCTION) error(DCERR);
	if(n->sc==FUNCTION) compatible(n->ty,cadr(type));
	else if(n->sc!=EMPTY) error(DCERR);
	n->sc=FUNCTION;
	n->ty=cadr(type);
}
compatible(t1,t2)
int t1,t2;
{	if(integral(t1))
	{	if(t1!=t2) error(TYERR);
	}
	else if(car(t1)!=car(t2)) error(TYERR);
	else if((car(t1)==STRUCT || car(t1)==UNION) && cadr(t1)!=cadr(t2))
		error(TYERR);
	else if(car(t1)==POINTER || car(t1)==ARRAY ||car(t1)==FUNCTION)
		compatible(cadr(t1),cadr(t2));
}
scalar(t)
int t;
{	return(integral(t)||car(t)==POINTER);
}
integral(t)
int t;
{	return(t==INT||t==CHAR||t==UNSIGNED);
}

statement()
{int slfree;
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
doif()
{int l1,l2,slfree;
	getsym();
	checksym(LPAR);
	slfree=lfree;
	bexpr(expr(),0,l1=fwdlabel());
	lfree=slfree;
	checksym(RPAR);
	statement();
	if(sym==ELSE)
	{	if (l2 = control) jmp(l2=fwdlabel());
		fwddef(l1);
		getsym();
		statement();
		if (l2) fwddef(l2);
	}
	else fwddef(l1);
}
dowhile()
{int sbreak,scontinue,slfree,e;
	sbreak=blabel;
	scontinue=clabel;
	blabel=fwdlabel();
	clabel=backdef();
	getsym();
	checksym(LPAR);
	slfree=lfree;
	e=expr();
	checksym(RPAR);
	if(sym==SM)
	{	bexpr(e,1,clabel);
		lfree=slfree;
		getsym();
	}
	else
	{	bexpr(e,0,blabel);
		lfree=slfree;
		statement();
		jmp(clabel);
	}
	fwddef(blabel);
	clabel=scontinue;
	blabel=sbreak;
}
dodo()
{int sbreak,scontinue,l,slfree;
	sbreak=blabel;
	scontinue=clabel;
	blabel=fwdlabel();
	clabel=fwdlabel();
	l=backdef();
	getsym();
	statement();
	fwddef(clabel);
	checksym(WHILE);
	checksym(LPAR);
	slfree=lfree;
	bexpr(expr(),1,l);
	lfree=slfree;
	checksym(RPAR);
	checksym(SM);
	fwddef(blabel);
	clabel=scontinue;
	blabel=sbreak;
}
dofor()
{int sbreak,scontinue,l,e,slfree;
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
doswitch()
{int sbreak,scase,sdefault,slfree;
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
	if(dlabel) printf("_%d\tEQU\t_%d\n",cslabel,dlabel);
	else fwddef(cslabel);
	cslabel=scase;
	dlabel=sdefault;
	fwddef(blabel);
	blabel=sbreak;
}
docomp()
{	getsym();
	while(sym!=RC) statement();
	getsym();
}
docase()
{int c,n,l,slfree;
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
		else printf("\tBRA\t_%d\n",l);
	}
	if (cslabel) fwddef(cslabel);
	while(cadr(c))
	{	cmpdimm(car(c));
		if((n-=6)>127) jcond(l,0);
		else printf("\tBEQ\t_%d\n",l);
		c=cadr(c);
	}
	lfree=slfree;
	cmpdimm(car(c));
	jcond(cslabel=fwdlabel(),1);
	fwddef(l);
}
dodefault()
{	getsym();
	checksym(COLON);
	if (dlabel) error(STERR);
	if (!cslabel) jmp(cslabel = fwdlabel());
	dlabel = backdef();
}
doreturn()
{int slfree;
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
return2()
{	control=0;
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
ret(reg)
char *reg;
{	printf("\tPULS\t%sU,PC\n",reg);
}
unlink2()
{	printf("\tLEAS\t,U\n");
	ret("");
}
dogoto()
{NMTBL *nptr0;
	getsym();
	nptr0=nptr;
	checksym(IDENT);
	if(nptr0->sc == BLABEL || nptr0->sc == FLABEL) jmp(nptr0->dsp);
	else if(nptr0->sc == EMPTY)
	{	nptr0->sc = FLABEL;
		jmp(nptr0->dsp = fwdlabel());
	}
	else error(STERR);
	checksym(SM);
}
dolabel()
{	if(nptr->sc == FLABEL) fwddef(nptr->dsp);
	else if(nptr->sc != EMPTY) error(TYERR);
	nptr->sc = BLABEL;
	nptr->dsp = backdef();
	getsym();
	checksym(COLON);
}

expr()
{	return(rvalue(expr0()));
}
expr0()
{int e;
	e=expr1();
	while(sym==COMMA) {getsym();e=list3(COMMA,e,rvalue(expr1()));}
	return e;
}
expr1()
{int e1,e2,t,op;
	e1=expr2();
	switch (sym)
	{case ASS:
		lcheck(e1);
		t=type;
		getsym();
		e2=rvalue(expr1());
		if(t==CHAR) {type= INT;return(list3(CASS,e1,e2));}
		type=t;
		return(list3(ASS,e1,e2));
	case ADD+AS: case SUB+AS: case MUL+AS: case DIV+AS: case MOD+AS:
	case RSHIFT+AS: case LSHIFT+AS: case BAND+AS: case EOR+AS: case BOR+AS:
		op = sym-AS;
		lcheck(e1);
		t=type;
		getsym();
		e2=rvalue(expr1());
		if(!integral(type)) error(TYERR);
		if((t==UNSIGNED||type==UNSIGNED)&&
			(op==MUL||op==DIV||op==MOD||op==RSHIFT||op==LSHIFT))
			op=op+US;
		if(t==CHAR)
		{	type= INT;
			return(list4(CASSOP,e1,e2,op));
		}
		type=t;
		if(integral(t)) return(list4(ASSOP,e1,e2,op));
		if((op!=ADD&&op!=SUB)||car(t)!=POINTER) error(TYERR);
		e2=binop(MUL,e2,list2(CONST,size(cadr(t))),INT,UNSIGNED);
		type=t;
		return list4(ASSOP,e1,e2,op);
	default:
		return(e1);
	}
}
expr2()
{int e1,e2,e3,t;
	e1=expr3();
	if(sym==COND)
	{	e1=rvalue(e1);
		getsym();
		e2=rvalue(expr2());
		t=type;
		checksym(COLON);
		e3=rvalue(expr2());
		if(car(e1)==CONST)
			if(cadr(e1)) {type=t;return e2;}
			else return e3;
		if(type==INT||t!=INT&&type==UNSIGNED) type=t;
		return(list4(COND,e1,e2,e3));
	}
	return(e1);
}
expr3()
{int e;
	e=expr4();
	while(sym==LOR)
	{	e=rvalue(e);
		getsym();
		e=list3(LOR,e,rvalue(expr4()));
		type= INT;
	}
	return(e);
}
expr4()
{int e;
	e=expr5();
	while(sym==LAND)
	{	e=rvalue(e);
		getsym();
		e=list3(LAND,e,rvalue(expr5()));
		type= INT;
	}
	return(e);
}
expr5()
{int e1,e2,t;
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
expr6()
{int e1,e2,t;
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
expr7()
{int e1,e2,t;
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
expr8()
{int e,op;
	e=expr9();
	while((op=sym)==EQ||op==NEQ)
	{	e=rvalue(e);
		getsym();
		e=list3(op,e,rvalue(expr9()));
		type= INT;
	}
	return e;
}
expr9()
{int e1,e2,t,op;
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
expr10()
{int e1,e2,t,op;
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
expr11()
{int e1,e2,t,op;
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
expr12()
{int e1,e2,t,op;
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
expr13()
{int e,op;
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
expr14()
{int e1,t;
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
expr16(e1)
int e1;
{int e2,t;
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
rvalue(e)
int e;
{	if(type==CHAR)
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
lcheck(e)
int e;
{	if(!scalar(type)||car(e)!=GVAR&&car(e)!=LVAR&&car(e)!=INDIRECT)
		error(LVERR);
}
indop(e)
int e;
{	if(type!=INT&&type!=UNSIGNED)
		if(car(type)==POINTER) type=cadr(type);
		else error(TYERR);
	else type= CHAR;
	if(car(e)==ADDRESS) return(cadr(e));
	return(list2(INDIRECT,e));
}
strop(e)
{	getsym();
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
binop(op,e1,e2,t1,t2)
int op,e1,e2,t1,t2;
{int e;
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
expr15(e1)
int e1;
{int t,args;
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
typeid(s)
int s;
{	return (integral(s) || s==SHORT || s==LONG || s==STRUCT || s==UNION ||
		(s==IDENT && nptr->sc==TYPE));
}
typename()
{int t;
	type=t=typespec();
	ndecl0();
	reverse(t);
	return type;
}
ndecl0()
{	if(sym==MUL)
	{	getsym();
		return type=list2(POINTER,ndecl0());
	}
	return ndecl1();
}
ndecl1()
{int i,t;
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

bexpr(e1,cond,l1)
int e1,l1;
char cond;
{int e2,l2;
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
rexpr(e1,l1,s)
int e1,l1;
char *s;
{	gexpr(list3(SUB,cadr(e1),caddr(e1)));
	printf("\tLB%s\t_%d\n",s,l1);
}
jcond(l,cond)
int l;
char cond;
{	printf("\tLB%s\t_%d\n",cond?"NE":"EQ",l);
}
jmp(l)
int l;
{	control=0;
	printf("\tLBRA\t_%d\n",l);
}
fwdlabel()
{	return labelno++;
}
fwddef(l)
int l;
{	control=1;
	printf("_%d\n",l);
}
backdef()
{	control=1;
	printf("_%d\n",labelno);
	return labelno++;
}

gexpr(e1)
int e1;
{int e2,e3;
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
		printf("\tNEGA\n\tNEGB\n\tSBCA\t#0\n");
		return;
	case BNOT:
		gexpr(e2);
		printf("\tCOMA\n\tCOMB\n");
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
		printf("\tBRA\t*+5\n");
		fwddef(e2);
		lddim(1);
	}
}
string(e1)
int e1;
{char *s;
int i,l,lb;
	s=(char *)cadr(e1);
	lb=fwdlabel();
	if ((l = caddr(e1)) < 128)
		printf("\tLEAX\t2,PC\n\tBRA\t_%d\n",lb);
	else
		printf("\tLEAX\t3,PC\n\tLBRA\t_%d\n",lb);
	do
	{	printf("\tFCB\t%d",*s++);
		for (i=8; --l && --i;) printf(",%d",*s++);
		printf("\n");
	}
	while (l);
	fwddef(lb);
}
function(e1)
int e1;
{int e2,e3,e4,e5,nargs;
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
		printf("\tLBSR\t%s\n",n->nm);
	}
	else
	{	gexpr(e2);
		printf("\tJSR\t,X\n");
	}
	if (nargs) printf("\tLEAS\t%d,S\n",2*nargs);
}
indirect(e1)
int e1;
{int e2,e3,e4;
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

machinop(e1)
int e1;
{int e2,e3;
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

rindirect(e1)
int e1;
{char *op;
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
assign(e1)
int e1;
{char *op;
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
assop(e1)
int e1;
{int e2,e3,byte,op;
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
simpop(op)
int op;
{	return (op == ADD || op == SUB ||
		op == BAND || op == EOR || op == BOR);
}
oprt(op,e1)
int op,e1;
{int e2;
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
oprt1(op,index,n)
int op,n;
char *index;
{	switch (op)
	{case ADD:
		printf("\tADDD\t%d,%s\n",n,index);
		return;
	case SUB:
		printf("\tSUBD\t%d,%s\n",n,index);
		return;
	case BAND: case EOR: case BOR:
		dualop(op,index,n);
		return;
	}
}
dualop(op,index,n)
int op;
char *index;
int n;
{char *ops;
	ops =  ((op == BAND) ? "AND" :
		(op == EOR)  ? "EOR" :
		(op == BOR)  ? "OR"  : (char *)DEBUG);
	printf("\t%sA\t%d,%s\n\t%sB\t%d+1,%s\n",ops,n,index,ops,n,index);
}

oprtc(op,n)
int op,n;
{	switch (op)
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
dualc(op,n)
int op;
int n;
{char *ops;
	ops =  ((op == BAND) ? "AND" :
		(op == EOR)  ? "EOR" :
		(op == BOR)  ? "OR"  : (char *)DEBUG);
	printf("\t%sA\t#%d\n\t%sB\t#%d\n",ops,(n >> 8) & 0xff,ops,n & 0xff);
}
tosop(op)
int op;
{	switch (op)
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
dualtosop(op)
int op;
{char *ops;
	ops =  ((op == BAND) ? "AND" :
		(op == EOR)  ? "EOR" :
		(op == BOR)  ? "OR"  : (char *)DEBUG);
	printf("\t%sA\t,S+\n\t%sB\t,S+\n",ops,ops);
}
pushd()
{	printf("\tPSHS\tD\n");
}
pushx()
{	printf("\tPSHS\tX\n");
}
pulld()
{	printf("\tPULS\tD\n");
}
pulx()
{	printf("\tPULS\tX\n");
}
tfrdx()
{	printf("\tTFR\tD,X\n");
}
tfrxd()
{	printf("\tTFR\tX,D\n");
}
/*
exgdx()
{	printf("\tEXG\tD,X\n");
}
*/
asld()
{	printf("\tASLB\n\tROLA\n");
}
adddim(n)
{	printf("\tADDD\t#%d\n",n);
}
subdim(n)
{	printf("\tSUBD\t#%d\n",n);
}
cmpdimm(n)
int n;
{	printf("\tCMPD\t#%d\n",n);
}
addds()
{	printf("\tADDD\t,S++\n");
}
subds()
{	printf("\tSUBD\t,S++\n");
}
clrd()
{	printf("\tCLRA\n\tCLRB\n");
}
lddim(n)
int n;
{	printf("\tLDD\t#%d\n",n);
}

ldd(e)
int e;
{	switch (car(e))
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

lddx()
{	printf("\tLDD\t,X\n");
}
lddy(n)
int n;
{	printf("\tLDD\t%d,Y\n",n);
}
lddu(n)
int n;
{	printf("\tLDD\t%d,U\n",n);
}

std(e)
int e;
{	switch (car(e))
	{case GVAR:
		stdy(cadr(e));
		return;
	case LVAR:
		stdu(cadr(e));
		return;
	default:
		DEBUG;
	}
}
stdx()
{	printf("\tSTD\t,X\n");
}
stdy(n)
int n;
{	printf("\tSTD\t%d,Y\n",n);
}
stdu(n)
int n;
{	printf("\tSTD\t%d,U\n",n);
}

ldbx()
{	printf("\tLDB\t,X\n");
}
/*
stbx()
{	printf("\tSTB\t,X\n");
}
*/
ldby(n)
int n;
{	printf("\tLDB\t%d,Y\n",n);
}
ldbu(n)
int n;
{	printf("\tLDB\t%d,U\n",n);
}
predecx(op,l)
char *op;
int l;
{	printf("\t%s\t,%sX\n",op,(l == -1 ? "-" : "--"));
}
postincx(op,l)
char *op;
int l;
{	printf("\t%s\t,X%s\n",op,(l == 1 ? "+" : "++"));
}
leaxy(n)
int n;
{	printf("\tLEAX\t%d,Y\n",n);
}
leaxu(n)
int n;
{	printf("\tLEAX\t%d,U\n",n);
}
leaxpcr(n)
NMTBL *n;
{	printf("\tLEAX\t%s,PCR\n",n->nm);
}

ldx(e)
int e;
{	switch (car(e))
	{case GVAR: case RGVAR:
		ldxy(cadr(e));
		return;
	case LVAR: case RLVAR:
		ldxu(cadr(e));
		return;
	default:
		DEBUG;
	}
}

ldxy(n)
int n;
{	printf("\tLDX\t%d,Y\n",n);
}
ldxu(n)
int n;
{	printf("\tLDX\t%d,U\n",n);
}
/*
ldxi(n)
int n;
{	printf("\tLDX\t#%d\n",n);
}
*/
stx(e)
int e;
{	switch (car(e))
	{case GVAR:
		stxy(cadr(e));
		return;
	case LVAR:
		stxu(cadr(e));
		return;
	default:
		DEBUG;
	}
}

stxy(n)
int n;
{	printf("\tSTX\t%d,Y\n",n);
}
stxu(n)
int n;
{	printf("\tSTX\t%d,U\n",n);
}

sex()
{	printf("\tSEX\n");
}
incx()
{	printf("\tINC\t,X\n");
}
decx()
{	printf("\tDEC\t,X\n");
}
opdx(op)
char *op;
{	printf("\t%s\tD,X\n",op);
}
indexx(op,n)
char *op;
int n;
{	printf("\t%s\t%d,X\n",op,n);
}

index_(op,e)
char *op;
int e;
{	switch (car(e))
	{case GVAR:
		indexy(op,cadr(e));
		return;
	case LVAR:
		indexu(op,cadr(e));
		return;
	default:
		DEBUG;
	}
}

indexy(op,n)
char *op;
int n;
{	printf("\t%s\t%d,Y\n",op,n);
}
indexu(op,n)
char *op;
int n;
{	printf("\t%s\t%d,U\n",op,n);
}


indir(op,e)
char *op;
int e;
{	switch (car(e))
	{case RGVAR:
		indiry(op,cadr(e));
		return;
	case RLVAR:
		indiru(op,cadr(e));
		return;
	default:
		DEBUG;
	}
}

indiry(op,n)
char *op;
int n;
{	printf("\t%s\t[%d,Y]\n",op,n);
}
indiru(op,n)
char *op;
int n;
{	printf("\t%s\t[%d,U]\n",op,n);
}
sextend(byte)
int byte;
{	if (byte) sex();
}
binexpr(e1)
int e1;
{	gexpr(caddr(e1));
	pushd();
	gexpr(cadr(e1));
	pulx();
	library(car(e1));
}
library(op)
int op;
{	printf("\tLBSR\t_0000%d\n",
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
cexpr(e)
int e;
{	if (car(e) != CONST) error(CNERR);
	return (cadr(e));
}

getsym()
{NMTBL *nptr0,*nptr1;
int i;
char c;
	if (alpha(skipspc()))
	{	i = hash = 0;
		while (alpha(ch) || digit(ch))
		{	if (i <= 7) hash=7*(hash+(name[i++]=ch));
			getch();
		}
		name[i] = '\0';
		nptr0 = gsearch();
		if (nptr0->sc == RESERVE) return sym = nptr0->dsp;
		if (nptr0->sc == MACRO && !mflag)
		{	mflag++;
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
		nptr1=lsearch();
		if (mode==STAT)
			if (nptr1->sc == EMPTY) return sym;
			else { nptr=nptr1; return sym;}
		nptr=nptr1;
		return sym;
	}
	else if (digit(ch))
	{	symval=0;
		if (ch == '0')
		{	if (getch() == 'x' || ch == 'X')
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
postequ(s1,s2)
int s1,s2;
{	if(ch=='=') {getch();return sym=s2;}
	return sym=s1;
}
alpha(c)
char c;
{	return('a'<=c&&c<='z'||'A'<=c&&c<='Z'||c=='_');
}
digit(c)
char c;
{	return('0'<=c&&c<='9');
}
NMTBL *gsearch()
{NMTBL *nptr,*iptr;
	iptr=nptr= &ntable[hash % GSYMS];
	while(nptr->sc!=EMPTY && neqname(nptr->nm))
	{	if (++nptr== &ntable[GSYMS]) nptr=ntable;
		if (nptr==iptr) error(GSERR);
	}
	if (nptr->sc == EMPTY) copy(nptr->nm);
	return nptr;
}
NMTBL *lsearch()
{NMTBL *nptr,*iptr;
	iptr=nptr= &ntable[hash%LSYMS+GSYMS];
	while(nptr->sc!=EMPTY && neqname(nptr->nm))
	{	if (++nptr== &ntable[LSYMS+GSYMS]) nptr= &ntable[GSYMS];
		if (nptr==iptr) error(LSERR);
	}
	if (nptr->sc == EMPTY) copy(nptr->nm);
	return nptr;
}
neqname(p)
char *p;
{char *q;
	q=name;
	while(*p) if(*p++ != *q++) return 1;
	return *q!=0;
}
copy(p)
char *p;
{char *q;
	q=name;
	while(*p++= *q++);
}
getstring()
{	getch();
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
skipspc()
{	while(ch=='\t'||ch=='\n'||ch==' '||ch=='\r') getch();
	return ch;
}
getch()
{	if(*chptr) return ch= *chptr++;
	if(mflag) {mflag=0;chptr=chptrsave;return ch=chsave;}
	getline();
	return getch();
}
char escape()
{char c;
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
FILE *getfname()
{int i;
char name[LBUFSIZE];
	getch();
	if(skipspc()!='"') error(INCERR);
	for(i=0;(getch()!='"' && ch!='\n');)
		if(i<LBUFSIZE-1) name[i++]=ch;
	if(ch=='\n') error(INCERR);
	name[i]=0;
	return ( (filep+1)->fcb = fopen(name,"r") );
}
getline()
{int i;
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
	if (lsrc && !asmf) printf("* %s",linebuf);
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
		{	fprintf(stderr,"%s",linebuf);
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
			getline();
			while (asmf)
			{	printf("%s",linebuf);
				getline();
			}
		}
		else if (macroeq("endasm"))
		{	if (!asmf) error(MCERR);
			asmf = 0;
		}
		else if (macroeq(" "))
			getline();
		else error(MCERR);
	}
}

macroeq(s)
char *s;
{char *p;
	for (p = chptr; *s;) if (*s++ != *p++) return 0;
	chptr = p;
	return 1;
}

car(e)
int e;
{	return heap[e];
}
cadr(e)
int e;
{	return heap[e+1];
}
caddr(e)
int e;
{	return heap[e+2];
}
cadddr(e)
int e;
{	return heap[e+3];
}
list2(e1,e2)
int e1,e2;
{int e;
	e=getfree(2);
	heap[e]=e1;
	heap[e+1]=e2;
	return e;
}
list3(e1,e2,e3)
int e1,e2,e3;
{int e;
	e=getfree(3);
	heap[e]=e1;
	heap[e+1]=e2;
	heap[e+2]=e3;
	return e;
}
list4(e1,e2,e3,e4)
int e1,e2,e3,e4;
{int e;
	e=getfree(4);
	heap[e]=e1;
	heap[e+1]=e2;
	heap[e+2]=e3;
	heap[e+3]=e4;
	return e;
}
getfree(n)
int n;
{int e;
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
rplacad(e,n)
int e,n;
{	heap[e+1]=n;
	return e;
}
