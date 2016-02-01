	 ;Input bytes loaded into addresses x4000 through x4003
	 ;and the shuffle control mask loaded into address x4004 
	 ;store the shuffled bytes into addresses x4005 through x4008

		.ORIG x3000
		
		;initialize registers
		AND R0 R0 #0
		AND R1 R1 #0
		AND R2 R2 #0
		AND R3 R3 #0
		AND R4 R4 #0
		AND R5 R5 #0
		AND R6 R6 #0

		;initialize variables
		LEA R6 SHFMASK
		LDB R0 R6 #0	;R0 = shuffle mask

		LEA R4 INPUT 	;R4 = input addr
		LEA R5 OUTPUT	;R5 = output addr

		;Byte 0
		AND R1 R0 x0003	;R1 = byte 0 control
		ADD R2 R4 R1	;R2 = x4000 + byte0 offset
		LDB R3 R2 #0	;R3 = MEM[x4000 + byte0 offset]
		STB R3 R5 #0	;MEM[x4005] = MEM[x4000 + byte0 offset]

		;Byte 1
		AND R1 R0 x000C	;R1 = byte 1 control
		RSHFL R1 R1 #2
		ADD R2 R4 R1	;R2 = x4000 + byte1 offset
		LDB R3 R2 #0	;R3 = MEM[x4000 + byte1 offset]
		STB R3 R5 #1	;MEM[x4006] = MEM[x4000 + byte1 offset]

		;Byte 2
		LEA R1 MASK2
		LDB R1 R1 #0	;R1 = MEM[MASK2]
		AND R1 R0 R1	;R1 = byte 2 control
		RSHFL R1 R1 #4
		ADD R2 R4 R1	;R2 = x4000 + byte0 offset
		LDB R3 R2 #0	;R3 = MEM[x4000 + byte0 offset]
		STB R3 R5 #2	;MEM[x4007] = MEM[x4000 + byte0 offset]

		;Byte 3
		LEA R1 MASK3
		LDB R1 R1 #0	;R1 = MEM[MASK3]
		AND R1 R0 R1	;R1 = byte 3 control
		RSHFL R1 R1 #6
		ADD R2 R4 R1	;R2 = x4000 + byte0 offset
		LDB R3 R2 #0	;R3 = MEM[x4000 + byte0 offset]
		STB R3 R5 #3	;MEM[x4008] = MEM[x4000 + byte0 offset]

		HALT
MASK2	.FILL x0030
MASK3	.FILL x00C0
ZERO	.FILL #0
INPUT	.FILL x4000
SHFMASK .FILL x4004
OUTPUT	.FILL x4005		
		.END