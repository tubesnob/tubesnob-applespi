******************************************************************
* SPIASM
* Low-level 65816 routines for fast SPI data transfer
******************************************************************

                case on
                mcopy m16.macros
		mcopy m16.ORCA
                keep tsspilib_asm_spireadb
                
******************************************************************
* spireadb(uint32_t *buffer, uint16_t bytecount);
* 
* reads [bytecount] bytes from SPI device, places 
* them in [buffer] and returns the number of bytes
* read.
* 
* device must be selected via SSEL before calling
* this routine. 
****************************************************


spireadb        start
	        csubroutine (4:buffer,2:bytecount),0
 
initloop	STZ bufidx
                STZ totalbytes
                LDA bytecount
                STA totalbytes
                LDA buffer
                STA storebyte+1
                SEP #$20
                LONGA OFF
                LDA buffer+2
                STA storebyte+3
                REP #$20
                LONGA ON
			
byteloop	LDY bufidx			- load our current buffer index
                CPY totalbytes			- is it the same as our requested bytecount?
                BNE bitloopinit                 - nope - continue
                JMP exit			- yes ... exit the loop

bitloopinit     STZ workbyte
                STZ mask
                LDY #00
                
                SEP #$20                        - turn on 8 bit accumulator
                LONGA OFF
                LDA #$80 
                STA mask

bitloop         LDA >$E0C05A                    - turn sclk OFF
                LDA >$E0C05B                    - turn sclk ON
                LDA >$E0C061                    - get our bit
                BMI gotabit
                BRA rotate

gotabit         TYA
                ORA mask
                TAY

rotate		LSR mask                        - rotate it
                LDA mask
		BEQ savebyte			- are we out of bits? jump to savebyte
		BRA bitloop                     - read more bits

savebyte	TYA
                LDX bufidx
storebyte       STA >buffer,X                 - store the work byte in the buffer
                REP #$20
                LONGA ON

                INC bufidx
		JMP byteloop                    - jump to the top of our byte loop

exit            LDA >$E0C05A                    - turn sclk OFF
                creturn 2:bufidx

bufroot         ds 4
bufidx	        ds 2
totalbytes      ds 2
workbyte	ds 2
mask		ds 2
goodrxcount	ds 2
goodrxstate  	ds 2
                
                end


