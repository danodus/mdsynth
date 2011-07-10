* 
***************************************************
*   MEMORY MAP EQUATES                            *
***************************************************
MONIO  EQU $E000  I/O SPACE
       IFD B3SOPT 
MONEXT EQU $F000  START OF EXTENDED COMMANDS
EXTCMD EQU $00    EXTENDED OFFSET
       ENDIF B3SOPT
       IFD S3EOPT
MONRAM EQU $7FC0
       ELSE
MONRAM EQU $DFC0  STACK SPACE
       ENDIF S3EOPT
MONROM EQU $F800  START OF ROM
       IFD S3SOPT 
MONEXT EQU $F000  START OF EXTENDED COMMANDS
EXTCMD EQU $00    EXTENDED OFFSET
       ENDIF S3SOPT
       IFD XESOPT 
MONEXT EQU $F000  START OF EXTENDED COMMANDS
EXTCMD EQU $00    EXTENDED OFFSET
       ENDIF XESOPT
       IFD DE270OPT 
MONEXT EQU $F000  START OF EXTENDED COMMANDS
EXTCMD EQU $00    EXTENDED OFFSET
       ENDIF DE270OPT
**************************************************
**************************************************

        IFD SWTOPT
*
* SOUTH WEST TECHNICAL PRODUCTS COMPUTER
*
ACIAOPT EQU $FF ACIA AT PORT 0
DMAFOPT EQU $FF DMAF2 8" FLOPPY DISK BOOT
MFDCOPT EQU $FF MINIFLOPPY 5.25" DISK BOOT
DATOPT  EQU $FF DYNAMIC ADDRESS TRANSLATION
        ENDIF
*
        IFD ADSOPT
*
* ACKERMAN DIGITAL ADS6809
*
DG640OPT  EQU $FF DG640 VDU AT $E800
*RTCOPT  EQU $FF REAL TIME CLOCK
PRTOPT  EQU $FF PRINTER DRIVERS
MFDCOPT EQU $FF MINIFLOPPY 5.25" DISK BOOT
        ENDIF ADSOPT
*
        IFD B3SOPT
*
* BURCHED SPARTAN 2 B3+
*
ACIAOPT EQU $FF ACIA AT PORT 0
PS2OPT  EQU $FF PS2 KEYBOARD AT $E020
VDUOPT  EQU $FF VDU AT $E030
IDEOPT  EQU $FF IDE AT $E100
DATOPT  EQU $FF DYNAMIC ADDRESS TRANSLATION
HFCOPT  EQU $FF HARDWARE FLOW CONTROL
        ENDIF B3SOPT         
*
        IFD B5XOPT
*
* BURCHED SPARTAN 2 B5-X300
*
ACIAOPT EQU $FF ACIA AT PORT 0
PS2OPT  EQU $FF PS2 KEYBOARD AT $E020
VDUOPT  EQU $FF VDU AT $E030
CF8OPT  EQU $FF COMPACT FLASH AT $E040
DATOPT  EQU $FF DYNAMIC ADDRESS TRANSLATION
HFCOPT  EQU $FF HARDWARE FLOW CONTROL
        ENDIF B5XOPT         
*
        IFD S3SOPT
*
* DIGILENT SPARTAN 3 STARTER
*
ACIAOPT EQU $FF ACIA AT PORT 0
PS2OPT  EQU $FF PS2 KEYBOARD AT $E020
VDUOPT  EQU $FF VDU AT $E030
CF8OPT  EQU $FF COMPACT FLASH AT $E040
DATOPT  EQU $FF DYNAMIC ADDRESS TRANSLATION
EXTOPT  EQU $FF EXTENDED COMMANDS
        ENDIF S3SOPT         
*
        IFD S3EOPT
*
* DIGILENT SPARTAN 3E STARTER
*
ACIAOPT EQU $FF ACIA AT PORT 0
PS2OPT  EQU $FF PS2 KEYBOARD AT $E020
VDUOPT  EQU $FF VDU AT $E030
TRAOPT  EQU $FF PIA TRACE TIMER
        ENDIF S3EOPT         
*
        IFD XESOPT
*
* XESS SPARTAN 3 XSA-3S1000 & XST-3.0
*
ACIAOPT EQU $FF ACIA AT PORT 0
PS2OPT  EQU $FF PS2 KEYBOARD AT $E020
VDUOPT  EQU $FF VDU AT $E030
IDEOPT  EQU $FF IDE AT $E100
DATOPT  EQU $FF DYNAMIC ADDRESS TRANSLATION
HFCOPT  EQU $FF HARDWARE FLOW CONTROL
EXTOPT  EQU $FF EXTENDED COMMANDS
        ENDIF XESOPT         
*
        IFD DE270OPT
*
* TERASIC CYCLONE 2 DE2-70 
*
ACIAOPT EQU $FF ACIA AT PORT 0
PS2OPT  EQU $FF PS2 KEYBOARD AT $E020
VDUOPT  EQU $FF VDU AT $E030
CF8OPT  EQU $FF COMPACT FLASH AT $E040
DATOPT  EQU $FF DYNAMIC ADDRESS TRANSLATION
EXTOPT  EQU $FF EXTENDED COMMANDS
        ENDIF DE270OPT         
*
*
        IFD   ACIAOPT
* 
***************************************************
*   SERIAL PORT                                   *
***************************************************
*
** ACIA SITS ON PORT 0
*
ACIAS   EQU   MONIO+$00   CONTROL PORT 
*
        ENDIF ACIAOPT
        IFD   MFDCOPT
* 
***************************************************
*   MINIFLOPPY DRIVE                              *
***************************************************
*
** FLOPPY DISK CONTROLLER SITS ON PORT 1
*
DRVFDC  EQU   MONIO+$14
CMDFDC  EQU   MONIO+$18
SECFDC  EQU   MONIO+$1A
DATFDC  EQU   MONIO+$1B
        ENDIF MFDCOPT
        IFD PS2OPT
* 
***************************************************
*   VDU8 PS/2 KEYBOARD PORT                       *
***************************************************
*
** KEYBOARD SITS ON PORT 2
*
PS2KBD  EQU   MONIO+$20   PS/2 KEYBOARD PORT 
        ENDIF PS2OPT
        IFD VDUOPT
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
        ENDIF VDUOPT
*
           IFD CF8OPT
* 
***************************************************
*   COMPACT FLASH EQUATES 8 BIT TRANSFER          *
***************************************************
*
** COMPACT FLASH SITS AT PORT 4
*
CF_BASE    EQU MONIO+$40
CF_DATA    EQU CF_BASE+0
CF_ERROR   EQU CF_BASE+1 ; read error
CF_FEATURE EQU CF_BASE+1 ; write feature
CF_SECCNT  EQU CF_BASE+2
CF_SECNUM  EQU CF_BASE+3
CF_CYLLO   EQU CF_BASE+4
CF_CYLHI   EQU CF_BASE+5
CF_HEAD    EQU CF_BASE+6
CF_STATUS  EQU CF_BASE+7 ; read status
CF_COMAND  EQU CF_BASE+7 ; write command
*
* Command Equates
*
CMDREAD    EQU $20 ; Read Single sector
CMDWRITE   EQU $30 ; Write Single sector
CMDFEATURE EQU $EF
FEAT8BIT   EQU $01 ; enable 8 bit transfers
HEADLBA    EQU $E0
*
* Status bit equates
*
BUSY       EQU $80
DRDY       EQU $40
DRQ        EQU $08
ERR        EQU $01
*
           ENDIF CF8OPT
*
           IFD IDEOPT
* 
***************************************************
*   COMPACT FLASH EQUATES 16 BIT TRANSFER (XESS)  *
***************************************************
*
** COMPACT FLASH SITS AT PORT 4
*
CF_BASE    EQU MONIO+$0100
CF_DATA    EQU CF_BASE+0
CF_ERROR   EQU CF_BASE+2 ; read error
CF_FEATURE EQU CF_BASE+2 ; write feature
CF_SECCNT  EQU CF_BASE+4
CF_SECNUM  EQU CF_BASE+6
CF_CYLLO   EQU CF_BASE+8
CF_CYLHI   EQU CF_BASE+10
CF_HEAD    EQU CF_BASE+12
CF_STATUS  EQU CF_BASE+14 ; read status
CF_COMAND  EQU CF_BASE+14 ; write command
CF_AUX     EQU CF_BASE+30
*
* Command Equates
*
CMDREAD    EQU $20 ; Read Single sector
CMDWRITE   EQU $30 ; Write Single sector
AUXRESET   EQU $06 ; Reset IDE
AUXRSTREL  EQU $02 ; Reset release IRQ masked
HEADLBA    EQU $E0
*
* Status bit equates
*
BUSY       EQU $80
DRDY       EQU $40
DRQ        EQU $08
ERR        EQU $01
*
           ENDIF CF8OPT
*
        IFD RTCOPT
*
**************************************************
* MM58167A REAL TIME CLOCK MEMORY MAP:
**************************************************
*
** REAL TIME CLOCK SITS ON PORT 4 AND PORT 5
*
CLOCK  EQU MONIO+$40 CLOCK BASE ADDRESS AND REGISTERS
*
* COUNTER AND COMPARITOR REGISTERS:
*
* Both the Clock Counter and Clock Comparitor
* consist of 8 registers for holding the time.
* The register offsets from the Counter and
* Comparitor registers are listed above.
*
COUNTR EQU CLOCK+0
CMPRAM EQU CLOCK+8 COMPARITOR REGISTERS
*
* CLOCK REGISTER OFFSETS:
* These register offsets are used for the CLOCK
* and comparitor ram CMPRAM.
*
S10000 EQU 0 TEN THOUNSANDTHS OF SECONDS
S100   EQU 1 HUNDRETHS AND TENTHS OF SECONDS
SECOND EQU 2
MINUIT EQU 3
HOUR   EQU 4
WKDAY  EQU 5
MTHDAY EQU 6
MONTH  EQU 7
*
* INTERRUPT OUTPUT REGISTERS:
*
* An interrupt output may be generated at the
* following rates by setting the appropriate bit
* in the Interrupt Control Register (CINTCR).
* The Interrupt Status Register (CINTSR) must be
* read to clear the interrupt and will return
* the source of the interrupt.
*
* 1/Month      Bit 7
* 1/Week       Bit 6
* 1/Day        Bit 5
* 1/Hour       Bit 4
* 1/Minuite    Bit 3
* 1/Second     Bit 2
* 10/Second    Bit 1
* Comparitor   Bit 0
*
CINTSR  EQU CLOCK+16 INTERRUPT STATUS REGISTER
CINTCR  EQU CLOCK+17 INTERRUPT CONTROL REGISTER
*
* COUNTER AND RAM RESETS; GO COMMAND.
*
* The counter and comparitor may be reset
* by writing $FF into CTRRES and CMPRES
* respectivly.
* A write to the Go command register (GOCMND)
* will reset the 1/1000ths, 1/100ths and 1/10ths
* of a second counter.
*
CTRRES EQU CLOCK+18 COUNTER RESET
CMPRES EQU CLOCK+19 COMPARITOR RAM RESET
GOCMND EQU CLOCK+21 GO COMMAND
*
* CLOCK STATUS REGISTER.
*
* The counter takes 61 usec. to rollover for
* every 1KHz clock pulse. If the Status bit is
* set after reading the counter, the counter
* should be re-read to ensure the time is correct.
*
CLKSTA EQU CLOCK+20 STATUS BIT
SBYINT EQU CLOCK+22 STANDBY INTERRUPT
TSTMOD EQU CLOCK+31 TEST MODE REGISTER
       ENDIF RTCOPT
* 
       IFD TRAOPT
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
       ENDIF TRAOPT
       IFD ADSOPT
* 
***************************************************
*   SERIAL PORT FOR DG640                         *
***************************************************
*
** SET UP FOR ACKERMAN DIGITAL ADS6809
** THE ADS6809 S100 BOAD HAS AN ON BOARD ACIA
*
ACIAS   EQU   MONIO+$400   CONTROL PORT 
*
        ENDIF ADSOPT
        IFD PRTOPT
*
***************************************************
* PRINTER INTERFACE                               *
***************************************************
*
PADATA EQU MONIO+$404
PACTRL EQU MONIO+$405
PBDATA EQU MONIO+$406
PBCTRL EQU MONIO+$407
*
** CB1  ACK.  I/P
** CB2  STB.  O/P
** PB0 - PB7   DATA 1 - 8   O/P
** PORT A BIT ASSIGNMENT
*
PBUSY  EQU $80 I/P
PEMPTY EQU $40 I/P
SELECT EQU $20 I/P
PERROR EQU $10 I/P
PRESET EQU %00000100 O/P PA3 = 0
AUTOFD EQU %00001000 O/P PA2 = 0
DIRMSK EQU %00001100
       ENDIF PRTOPT
       IFD DG640OPT
*
***************************************************
*   DG640 MEMORY MAPPED DISPLAY DRIVER VARIABLES  *
***************************************************
*
** VIDEO DISPLAY DEFINITIONS
*
SCREEN  EQU   MONIO+$0800 START OF SCREEN MEMORY
LINLEN  EQU   64          LENGTH OF A LINE
NUMLIN  EQU   16          NUMBER OF LINES
SCNLEN  EQU   $400        LENGTH OF SCREEN
        ENDIF DG640OPT
* 
        IFD   DMAFOPT
* 
***************************************************
*   DMAF2 8" DRIVE                                *
***************************************************
*
ADDREG  EQU   $F000  ADDRESS REGISTER 
CNTREG  EQU   $F002  COUNT REGISTER 
CCREG   EQU   $F010  CHANNEL CONTROL REGISTER 
PRIREG  EQU   $F014  DMA PRIORITY REGISTER 
AAAREG  EQU   $F015  ??? 
BBBREG  EQU   $F016  ??? 
COMREG  EQU   $F020  1791 COMMAND REGISTER 
SECREG  EQU   $F022  SECTOR REGISTER 
DRVREG  EQU   $F024  DRIVE SELECT LATCH 
CCCREG  EQU   $F040  ??? 
        ENDIF DMAFOPT
        IFD DATOPT
**************************************************
* DYNAMIC ADDRESS TRANSLATION REGISTERS          *
**************************************************
*
IC11    EQU  $FFF0  DAT RAM CHIP 
TSTPAT  EQU  $55AA  TEST PATTERN 
        ENDIF DATOPT
*
        END
