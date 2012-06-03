* 6809
* Demo
*
	NAM GAME
	OPT l
	PAGE

*******************************************************************************
* Equates
*
		
VDU	EQU	$E030
SNDL	EQU	$E080
SNDR	EQU	$E090

*******************************************************************************
* Variables
*

	ORG	$0F00
STACK	EQU	*
CURX	RMB	1		; Current cursor position X
CURY	RMB	1		; Current cursor position Y
		
*******************************************************************************
* Main entry point
*
	ORG	$1000
MAIN	LDS	#STACK

* Hide the cursor

	BSR	HIDECUR

* Clear the screen

	BSR	CLRSCR

* Print a nice message

	CLRA
	STA	CURX
	STA	CURY
	BSR	MOVECUR
	LDX	#MSG
	BSR	PRINTS

* We set the sound chips to DAC direct mode
*	CLRA
*	LDX	#SNDL
*	LDY	#SNDR
*	STA	0,X
*	STA	0,Y

	BRA	*

*******************************************************************************
* Hide the cursor
*
HIDECUR	PSHS	A,Y
	LDY	#VDU
	CLRA
	STA	5,Y
	PULS	A,Y
	RTS

*******************************************************************************
* Move the cursor to CURX, CURY
*
MOVECUR	PSHS	A,Y
	LDY	#VDU
	LDA	CURX
	STA	2,Y
	LDA	CURY
	STA	3,Y
	PULS	A,Y
	RTS

*******************************************************************************
* Clear the screen
*
CLRSCR	PSHS	A,B,Y
	LDY	#VDU
	CLRB		; Clear the line number
CLRSCR0	STB	3,y	; Set the line number
* Clear the line
	PSHS	B	; Push the line number
	CLRB		; Clear the column number
CLRSCR1	STB	2,Y	; Set the column number
	CLRA
	STA	,Y	; Write zero
	LDA	#$C7	; White foreground, blue background
	STA	1,Y
	INCB		; Go to next column
	CMPB	#80
	BLO	CLRSCR1
	PULS	B	; Pull the line number
	INCB		; Go to next line
	CMPB	#25
	BLO	CLRSCR0
	PULS	A,B,Y
	RTS

*******************************************************************************
* Print a character
* A = Character to print
PRINTC	PSHS	A,Y
	LDY	#VDU
	STA	,Y	; Write the character
	LDA	CURX
	INCA
	CMPA	#80
	BLO	PRINTC0
	CLRA
	INC	CURY	; Increase the column
PRINTC0	STA	CURX
	BSR	MOVECUR
	PULS	A,Y
	RTS

*******************************************************************************
* Print a string
*	
PRINTS	PSHS	A,Y
	LDY	#VDU
PRINTS0	LDA	,X+
	CMPA	#0
	BEQ	PRINTS1
	BSR	PRINTC
	BRA	PRINTS0
PRINTS1	PULS	A,Y
	RTS

*******************************************************************************
* Constants
*
MSG	FCB	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
	FCB	0

	END

