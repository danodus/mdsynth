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


#ifndef __pseudo_h__
#define __pseudo_h__

#include "as.h"

/*
 *      pseudo --- pseudo op processing
 */

#define RMB     0       /* Reserve Memory Bytes         */
#define FCB     1       /* Form Constant Bytes          */
#define FDB     2       /* Form Double Bytes (words)    */
#define FCC     3       /* Form Constant Characters     */
#define ORG     4       /* Origin                       */
#define EQU     5       /* Equate                       */
#define ZMB     6       /* Zero memory bytes            */
#define FILL    7       /* block fill constant bytes    */
#define OPT     8       /* assembler option             */
#define NULL_OP 9       /* null pseudo op               */
#define PAGE    10      /* new page                     */
#define INCLUDE 11      /* include <file> or "file" ver TER_2.0 */
#define END     12      /* include <file> terminator ver TER_2.0 */
#define IFD     13      /* if define <symbol> ver TER_2.0 */
#define IFND    14      /* if not define <symbol> ver TER_2.0 */
#define ELSE    15      /* else (for IF statements) ver TER_2.0 */
#define ENDIF   16      /* endif (for IF statements) ver TER_2.0 */
#define BSS     17      /* block storage segment (RAM) ver TER_2.09 */
#define CODE    18      /* code segment ver TER_2.09 25 Jul 89 */
#define DATA    19      /* data segment ver TER_2.09 25 Jul 89 */
#define AUTO    20      /* data segment ver TER_2.09 25 Jul 89 */

#define NPSE    28
extern struct oper pseudo[NPSE];


#endif // __pseudo_h__
