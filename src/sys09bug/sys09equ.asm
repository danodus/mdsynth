* 
***************************************************
*   MEMORY MAP EQUATES                            *
***************************************************
MONIO  EQU $E000  I/O SPACE
MONRAM EQU $7FC0
MONROM EQU $F800  START OF ROM
**************************************************
**************************************************
*
***************************************************
*   SERIAL PORT                                   *
***************************************************
*
** ACIA SITS ON PORT 0
*
ACIAS   EQU   MONIO+$00   CONTROL PORT 
*
* 
***************************************************
*   VDU8 PS/2 KEYBOARD PORT                       *
***************************************************
*
** KEYBOARD SITS ON PORT 2
*
PS2KBD  EQU   MONIO+$20   PS/2 KEYBOARD PORT 
* 
***************************************************
*   VDU8 DISPLAY DRIVER EQUATES                   *
***************************************************
*
** VDU8 DISPLAY SITS ON PORT 3
*
VDU     EQU   MONIO+$30
VDUCHR  EQU   0        CHARACTER REGISTER
VDUATT  EQU   1        ATTRIBUTE REGISTER
VDUCOL  EQU   2        CURSOR COLUMN
VDUROW  EQU   3        CURSOR ROW
VDUOFF  EQU   4        ROW OFFSET
*
LINLEN  EQU   80       LENGTH OF A LINE
NUMLIN  EQU   25       NUMBER OF LINES
*
**************************************************
* PIA INTERRUPT TIMER
**************************************************
*
** PIA INTERRUPT TIMER SITS ON PORT 7 
*
** PIA TIMER FOR SINGLE STEP / TRACE
*
* TADATA = Output = Timer preset register
* TACTRL - CA1 = input = rising edge = NMI
*        - CA2 = Output = Timer Reset (Active High)
* TBDATA = Input = Timer read back register
* TBCTRL - CB1 = input = rising edge = FIRQ
*        - CB2 = output = strobe low on write to TBDATA = Timer Preset
*
* CRA0 = 0 CA1 IRQ DISAB, CRA0 = 1 CA1 IRQ ENAB
* CRA1 = 1 CA1 Rising edge IRQ
* CRA2 = 0 TADATA = Data Direction, CRA2 = 1 TADATA = I/O Register
* CRA3 = 0 CA2 = 0 output, CRA3 = 1 CA2 = 1
* CRA4 = 1 ] CA2 = Set/Reset output
* CRA5 = 1 ]
* CRA6 = X CA2 Input Interrupt Flag
* CRA7 = X CA1 Interrupt Flag
*
* CRB0 = 0 CB1 IRQ DISAB, CRB0 = 1 CA1 IRQ ENAB
* CRB1 = 1 CB1 Rising edge IRQ
* CRB2 = 0 TBDATA = Data Direction, CRB2 = 1 TBDATA = I/O Register
* CRB3 = 0 CB2 = 0 output, CRB3 = 1 CB2 = 1
* CRB4 = 1 ] CB2 = Set/Reset output
* CRB5 = 1 ]
* CRB6 = X CB2 Input Interrupt Flag
* CRB7 = X CB1 Interrupt Flag
*
* DDRA = 0 TADATA = Input, DDRA = 1 TADATA = Output
* DDRB = 0 TBDATA = Input, DDRB = 1 TBDATA = Output
*
TADATA EQU MONIO+$70 Timer preset port
TACTRL EQU MONIO+$71
TBDATA EQU MONIO+$72 Timer read back port
TBCTRL EQU MONIO+$73
*
TRADEL EQU 13  Number of E cycles for RTI (May need to be fudged)
*
       END
