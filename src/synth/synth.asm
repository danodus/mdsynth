* 6809
* Synth
*
		NAM SYNTH
		OPT l
		PAGE
		
KEYB	EQU	$E020
VDU		EQU	$E030
TIMER	EQU	$E050		
SNDL	EQU	$E080
SNDR	EQU	$E090

******************************************************************************
* Variables
******************************************************************************

		ORG	$0F00
STACK	EQU	*
CURX	RMB	1
CURY	RMB	1
MPITCH	RMB	1
CPITCH	RMB	1
MODE	RMB	1
		
******************************************************************************
* Main
******************************************************************************
		
		ORG	$1000
		
		LDS	#STACK

		LDA	#0
		STA	MODE		; We set the initial mode to message pitch
		STA	MPITCH		; We set the initial message pitch
		LDA	#20
		STA	CPITCH		; We set the initial carrier pitch
		
* We clear the screen
		JSR	CLEARS		; We clear the screen
		
* We print the title		
		LDA	#0
		STA	CURX		; We set the cursor position to the top left
		STA	CURY
		
		LDX	#MSG_TITLE
		JSR	PRINTS		; We print the title
		
		
* We play a sequence of sounds

MAIN0	JSR	GETKEY		; We get the current key
		JSR	STOP_SND
		CMPA	#0		; If no key is pressed...
		BEQ	MAIN0		; ...we loop
		CMPA	#'1'	; Check for key '1'
		BNE	MAIN1
* Message pitch mode		
		LDX	#MSG_MESSAGE
		LDA	#1
		STA	CURY
		LDA	#0
		STA	CURX
		JSR	PRINTS
		LDA	#0
		STA	MODE
		BRA	MAIN0
MAIN1	CMPA	#'2'	
		BNE	MAIN2
* Carrier pitch mode
		LDX	#MSG_CARRIER
		LDA	#1
		STA	CURY
		LDA	#0
		STA	CURX
		JSR	PRINTS
		LDA	#1
		STA	MODE
		BRA	MAIN0
MAIN2	LDB	MODE
		CMPB	#1
		BEQ	MAIN3
		STA	MPITCH
		BRA	MAIN4
MAIN3	STA	CPITCH
MAIN4	JSR	PLAY_SND
		
		JMP MAIN0
			
		
******************************************************************************
* Wait subroutines for 20us, 1ms and 100ms
******************************************************************************
		
WAIT10US	PSHS	Y,A	
		LDY	#TIMER
		LDA	#250
		STA 0,Y
		LDA	#1
		STA 1,Y
WLOOP 	LDA	0,Y
		BNE	WLOOP
		PULS	Y,A
		RTS

WAIT1MS	PSHS	B
		LDB	#100
W2LOOP	BSR WAIT10US
		DECB
		BNE	W2LOOP
		PULS	B
		RTS

WAIT100MS	PSHS	B
		LDB	#100
W3LOOP	BSR WAIT1MS
		DECB
		BNE	W3LOOP
		PULS	B
		RTS

******************************************************************************
* Print a string to the VDU
* Parameters:
*	X: Address of the null-terminated string		
******************************************************************************
		
PRINTS	PSHS	Y,A,B
		LDY	#VDU
		LDA	CURY		; Get current line
		STA	3,Y			; Set the line to VDU
		LDA	CURX		; A = column		
PRINTS0	LDB	,X+			; B = the next character
		CMPB	#0		; Is it null?
		BEQ	PRINTS1		; If yes, stop
		STA	2,Y			; Set the column to VDU
		STB	,Y			; We write the character
		INCA			; We go to the next column
		BRA PRINTS0		; We continue
PRINTS1	STA	CURX		; We write the current column
		STA	2,Y			; Set the column to VDU
		PULS	Y,A,B
		RTS

******************************************************************************
* Clear the screen
******************************************************************************
		
CLEARS	PSHS	Y,A,B
		LDY		#VDU
		LDB		#32		; Character to write
		LDA		#0		; Start at the first line
CLEARS0	STA		3,Y		; Set the line to VDU
		PSHS	A		; Preserve the line value
		LDA		#0		; Start at the first column
CLEARS1	STA		2,Y		; Set the column to VDU
		STB		,Y		; Write character to VDU
		INCA			; Increment the column
		CMPA	#80		; Is the last column?
		BNE		CLEARS1	; Continue if not the last column
		PULS	A		; Restore the line value
		INCA			; Increment the line
		CMPA	#25		; Is the last line?
		BNE		CLEARS0	; Continue if not the last line
		PULS	Y,A,B
		RTS
		
		
******************************************************************************
* Get currently pressed key
* Returns:
*	A: Key (0 if none)
******************************************************************************

GETKEY	PSHS	Y
	LDY	#KEYB
GETKEY0	LDA	,Y
	BITA	#1			; Test for key pressed
	BNE	GETKEY1			; Branch if a key is pressed
	LDA	#0				; No key is pressed
	BRA	GETKEY2
GETKEY1	LDA	1,Y			; Get the key value
GETKEY2	PULS	Y
	RTS


******************************************************************************
* Play a sound
******************************************************************************
		
PLAY_SND	PSHS	A,B,X,Y

		LDX	#SNDL
		LDY #SNDR

* We set the message pitch
		LDA	MPITCH
		STA	1,X
		STA 1,Y
		
* We set the carrier pitch
		LDA	CPITCH
		STA	2,X
		STA	2,Y
		
* We enable the phase-modulated wave
		LDA	#4
		STA ,X
		STA ,Y
		
* We wait
PLAY0	LBSR WAIT100MS
		DECB
		BNE	PLAY0
		
* We disable the sine wave
		LDA	#0
		STA ,X
		STA ,Y
		PULS	A,B,X,Y
		RTS
		

******************************************************************************
* Stop sound
******************************************************************************
		
STOP_SND	PSHS	A,X,Y

		LDX	#SNDL
		LDY #SNDR
		
* We disable the sine wave
		LDA	#0
		STA ,X
		STA ,Y
		PULS	A,X,Y
		RTS
		
		
******************************************************************************
* Constants
******************************************************************************

MSG_TITLE	FCC	'MD-Synthesizer'
			FCB	0
MSG_MESSAGE	FCC 'Message Pitch'
			FCB	0
MSG_CARRIER	FCC	'Carrier Pitch'
			FCB	0
		END
