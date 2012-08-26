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


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
/*#include <sys/io.h>*/
#include <fcntl.h>

#include "as.h"
#include "util.h"


/*
 *      file I/O version of forward ref handler
 */

#define FILEMODE 0644 /* file creat mode */
#define UPDATE  2 /* file open mode */
#define ABS  0 /* absolute seek */

int Forward =0;  /* temp file's file descriptor */
char Fwd_name[] = { "Fwd_refs" } ;

/*
 *      fwdinit --- initialize forward ref file
 */
fwdinit()
{
 Forward = open(Fwd_name,O_CREAT | O_TRUNC | O_WRONLY, FILEMODE); //creat(Fwd_name,FILEMODE);
 if(Forward <0)
  fatal("Can't create temp file");
 close(Forward); /* close and reopen for reads and writes */
 Forward = open(Fwd_name,UPDATE);
 if(Forward <0)
  fatal("Forward ref file has gone.");
}

/*
 *      fwdreinit --- reinitialize forward ref file
 */
fwdreinit()
{
 F_ref   = 0;
 Ffn     = 0;
 lseek(Forward,0L,ABS);   /* rewind forward refs */
 read(Forward,&Ffn,sizeof(Ffn));
 read(Forward,&F_ref,sizeof(F_ref)); /* read first forward ref into mem */
#ifdef DEBUG
 printf("First fwd ref: %d,%d\n",Ffn,F_ref);
#endif
}

/*
 *      fwdmark --- mark current file/line as containing a forward ref
 */
fwdmark()
{
 write(Forward,&Cfn,sizeof(Cfn));
 write(Forward,&Line_num,sizeof(Line_num));
}

/*
 *      fwdnext --- get next forward ref
 */
fwdnext()
{
 int stat;

 stat = read(Forward,&Ffn,sizeof(Ffn));
#ifdef DEBUG
 printf("Ffn stat=%d ",stat);
#endif
 stat = read(Forward,&F_ref,sizeof(F_ref));
#ifdef DEBUG
 printf("F_ref stat=%d  ",stat);
#endif
 if( stat < 2 ){
  F_ref=0;Ffn=0;
  }
#ifdef DEBUG
 printf("Next Fwd ref: %d,%d\n",Ffn,F_ref);
#endif
}

/*
 *  fwd_done --- closes & deletes forward reference file
 */
fwd_done()
{
	int stat;

	stat = close(Forward); /* Have to close first on MS-DOS or you leave lost clusters. */
#ifndef DEBUG
	if( stat == 0 )        /* If close was successful, delete file */
		unlink(Fwd_name);
#endif
}
