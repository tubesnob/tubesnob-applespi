******************************************************************
* SPIASM
* Low-level 65816 routines for fast SPI data transfer
******************************************************************

                case on
                mcopy m16.macros
		mcopy m16.ORCA
                keep tsspilib_asm_spisendb

******************************************************************
* spisendb(uint32_t *buffer, uint16_t bytecount);
* 
* sends [bytecount] bytes to SPI device from the buffer
* returns the number of bytes sent
* 
* device must be selected via SSEL before calling
* this routine. 
******************************************************************

spisendb        start
	        csubroutine (4:buffer,2:bytecount),0
                
loopinit        LDY #0000
                STZ bufidx
                STZ mask
                jsr clr_sclk                    - turn off sclk
                
byteloop        LDY bufidx
                CPY bytecount
                BEQ exit
                LDY #$80                        - load the initial mask
                STY mask                        - store it for safekeeping
                LDY bufidx                      - load our buffer index
                LDA [buffer],Y                  - load the next byte from the buffer into A                 
                AND #$00FF                      - 16 bit loads... so truncate the high byte
                TAX                             - save the value in X

bitloop         JSR clr_sclk                    - turn off sclk
                TXA                             - transfer X back into A
                AND mask                        - bitwise and with our current mask against A
                BEQ nobit
                JSR set_mosi                    - set mosi....
                BRA tickle                      - jump to the clock tickle... err toggle
nobit           jsr clr_mosi                    - no bit... clear mosi
tickle          jsr set_sclk                    - turn on sclk

rotate          LDA mask                        - load the mask
                LSR A                           - rotate it right (div by 2)
                BEQ nextbyte                    - YES, let's go to the next byte
                STA mask                        - store it back for safekeeping
                BRA bitloop                     - nope... got more bits to send. jump back

nextbyte        INC bufidx                      - load up our buffer index
                BRA byteloop                     - go back to the byte loop 

exit            jsr clr_sclk
                creturn 2:bufidx

clr_sclk        PHA
                LONGA OFF
                SEP #$20
                LDA #$A0
clr_sclk_addr   STA >$E0C05A
                REP #$20
                LONGA ON
                PLA
                RTS

set_sclk        PHA
                LONGA OFF
                SEP #$20
                LDA #$A0
set_sclk_addr   STA >$E0C05B
                REP #$20
                LONGA ON
                PLA
                RTS

clr_mosi        PHA
                LONGA OFF
                SEP #$20
                LDA #$A0
clr_mosi_addr   STA >$E0C05E
                REP #$20
                LONGA ON                        
                PLA
                RTS

set_mosi        PHA
                LONGA OFF
                SEP #$20
                LDA #$A0
set_mosi_addr   STA >$E0C05F                       
                REP #$20
                LONGA ON                        
                PLA
                RTS

bufidx          ds      2
mask            ds      2
cmdidx          ds      2

                end


