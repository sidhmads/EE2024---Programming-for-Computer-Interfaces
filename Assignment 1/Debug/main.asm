   1              		.syntax unified
   2              		.cpu cortex-m3
   3              		.fpu softvfp
   4              		.eabi_attribute 20, 1
   5              		.eabi_attribute 21, 1
   6              		.eabi_attribute 23, 3
   7              		.eabi_attribute 24, 1
   8              		.eabi_attribute 25, 1
   9              		.eabi_attribute 26, 1
  10              		.eabi_attribute 30, 6
  11              		.eabi_attribute 34, 1
  12              		.eabi_attribute 18, 4
  13              		.thumb
  14              		.file	"main.c"
  15              		.text
  16              	.Ltext0:
  17              		.cfi_sections	.debug_frame
  18              		.section	.text.compute_stats,"ax",%progbits
  19              		.align	2
  20              		.global	compute_stats
  21              		.thumb
  22              		.thumb_func
  24              	compute_stats:
  25              	.LFB29:
  26              		.file 1 "../src/main.c"
   1:../src/main.c **** #include "LPC17xx.h"
   2:../src/main.c **** #include "stdio.h"
   3:../src/main.c **** 
   4:../src/main.c **** #define TOTAL_DATA 100
   5:../src/main.c **** 
   6:../src/main.c **** // EE2024 Assignment 1 skeleton code
   7:../src/main.c **** // (C) CK Tham, ECE, NUS, 2018
   8:../src/main.c **** 
   9:../src/main.c ****  // Do not modify the following function prototype
  10:../src/main.c **** extern int asm_stats(int* px, int* pvar, int* pmin, int* pmax);
  11:../src/main.c **** 
  12:../src/main.c **** typedef struct data
  13:../src/main.c **** {
  14:../src/main.c ****  	int x[TOTAL_DATA];
  15:../src/main.c ****  	int mean;
  16:../src/main.c ****  	int variance;
  17:../src/main.c **** 	int min;
  18:../src/main.c ****  	int max;
  19:../src/main.c **** } dataset;
  20:../src/main.c **** 
  21:../src/main.c **** // Do not modify the following function prototype
  22:../src/main.c **** void compute_stats(dataset* dptr)
  23:../src/main.c **** {
  27              		.loc 1 23 0
  28              		.cfi_startproc
  29              		@ args = 0, pretend = 0, frame = 8
  30              		@ frame_needed = 1, uses_anonymous_args = 0
  31 0000 80B5     		push	{r7, lr}
  32              	.LCFI0:
  33              		.cfi_def_cfa_offset 8
  34              		.cfi_offset 14, -4
  35              		.cfi_offset 7, -8
  36 0002 82B0     		sub	sp, sp, #8
  37              	.LCFI1:
  38              		.cfi_def_cfa_offset 16
  39 0004 00AF     		add	r7, sp, #0
  40              	.LCFI2:
  41              		.cfi_def_cfa_register 7
  42 0006 7860     		str	r0, [r7, #4]
  24:../src/main.c **** 	//  Write the code to call asm_stats() function
  25:../src/main.c **** 	//  in an appropriate manner to compute the statistical parameters
  26:../src/main.c **** 	//  ... asm_stats( ... )
  27:../src/main.c **** 	dptr-> mean = asm_stats(&dptr->x, &dptr->variance, &dptr->min, &dptr->max);
  43              		.loc 1 27 0
  44 0008 7868     		ldr	r0, [r7, #4]
  45 000a 7B68     		ldr	r3, [r7, #4]
  46 000c 03F5CA71 		add	r1, r3, #404
  47 0010 7B68     		ldr	r3, [r7, #4]
  48 0012 03F5CC72 		add	r2, r3, #408
  49 0016 7B68     		ldr	r3, [r7, #4]
  50 0018 03F5CE73 		add	r3, r3, #412
  51 001c FFF7FEFF 		bl	asm_stats
  52 0020 0246     		mov	r2, r0
  53 0022 7B68     		ldr	r3, [r7, #4]
  54 0024 C3F89021 		str	r2, [r3, #400]
  28:../src/main.c **** }
  55              		.loc 1 28 0
  56 0028 07F10807 		add	r7, r7, #8
  57 002c BD46     		mov	sp, r7
  58 002e 80BD     		pop	{r7, pc}
  59              		.cfi_endproc
  60              	.LFE29:
  62              		.section	.rodata
  63              		.align	2
  64              	.LC0:
  65 0000 785B2564 		.ascii	"x[%d]: %d\012\000"
  65      5D3A2025 
  65      640A00
  66 000b 00       		.align	2
  67              	.LC1:
  68 000c 6D65616E 		.ascii	"mean: %d\012\000"
  68      3A202564 
  68      0A00
  69 0016 0000     		.align	2
  70              	.LC2:
  71 0018 76617269 		.ascii	"variance: %d\012\000"
  71      616E6365 
  71      3A202564 
  71      0A00
  72 0026 0000     		.align	2
  73              	.LC3:
  74 0028 6D696E3A 		.ascii	"min: %d\012\000"
  74      2025640A 
  74      00
  75 0031 000000   		.align	2
  76              	.LC4:
  77 0034 6D61783A 		.ascii	"max: %d\012\000"
  77      2025640A 
  77      00
  78 003d 000000   		.section	.text.main,"ax",%progbits
  79              		.align	2
  80              		.global	main
  81              		.thumb
  82              		.thumb_func
  84              	main:
  85              	.LFB30:
  29:../src/main.c **** 
  30:../src/main.c **** int main(void)
  31:../src/main.c **** {
  86              		.loc 1 31 0
  87              		.cfi_startproc
  88              		@ args = 0, pretend = 0, frame = 424
  89              		@ frame_needed = 1, uses_anonymous_args = 0
  90 0000 80B5     		push	{r7, lr}
  91              	.LCFI3:
  92              		.cfi_def_cfa_offset 8
  93              		.cfi_offset 14, -4
  94              		.cfi_offset 7, -8
  95 0002 EAB0     		sub	sp, sp, #424
  96              	.LCFI4:
  97              		.cfi_def_cfa_offset 432
  98 0004 00AF     		add	r7, sp, #0
  99              	.LCFI5:
 100              		.cfi_def_cfa_register 7
  32:../src/main.c **** 	int i;
  33:../src/main.c **** 	//  Instantiate a dataset object X
  34:../src/main.c **** 	dataset X;
  35:../src/main.c **** 
  36:../src/main.c **** 	//  Initialize the dataset object X
  37:../src/main.c **** 	for (i=0;i<TOTAL_DATA;i++)
 101              		.loc 1 37 0
 102 0006 4FF00003 		mov	r3, #0
 103 000a C7F8A431 		str	r3, [r7, #420]
 104 000e 0FE0     		b	.L3
 105              	.L4:
  38:../src/main.c **** 		X.x[i] = i - 6;
 106              		.loc 1 38 0 discriminator 2
 107 0010 D7F8A431 		ldr	r3, [r7, #420]
 108 0014 A3F10601 		sub	r1, r3, #6
 109 0018 07F10403 		add	r3, r7, #4
 110 001c D7F8A421 		ldr	r2, [r7, #420]
 111 0020 43F82210 		str	r1, [r3, r2, lsl #2]
  37:../src/main.c **** 	for (i=0;i<TOTAL_DATA;i++)
 112              		.loc 1 37 0 discriminator 2
 113 0024 D7F8A431 		ldr	r3, [r7, #420]
 114 0028 03F10103 		add	r3, r3, #1
 115 002c C7F8A431 		str	r3, [r7, #420]
 116              	.L3:
  37:../src/main.c **** 	for (i=0;i<TOTAL_DATA;i++)
 117              		.loc 1 37 0 is_stmt 0 discriminator 1
 118 0030 D7F8A431 		ldr	r3, [r7, #420]
 119 0034 632B     		cmp	r3, #99
 120 0036 EBDD     		ble	.L4
  39:../src/main.c **** 	X.mean = X.variance = X.min = X.max = 0;
 121              		.loc 1 39 0 is_stmt 1
 122 0038 07F10403 		add	r3, r7, #4
 123 003c 4FF00002 		mov	r2, #0
 124 0040 C3F89C21 		str	r2, [r3, #412]
 125 0044 07F10403 		add	r3, r7, #4
 126 0048 D3F89C21 		ldr	r2, [r3, #412]
 127 004c 07F10403 		add	r3, r7, #4
 128 0050 C3F89821 		str	r2, [r3, #408]
 129 0054 07F10403 		add	r3, r7, #4
 130 0058 D3F89821 		ldr	r2, [r3, #408]
 131 005c 07F10403 		add	r3, r7, #4
 132 0060 C3F89421 		str	r2, [r3, #404]
 133 0064 07F10403 		add	r3, r7, #4
 134 0068 D3F89421 		ldr	r2, [r3, #404]
 135 006c 07F10403 		add	r3, r7, #4
 136 0070 C3F89021 		str	r2, [r3, #400]
  40:../src/main.c **** 	X.x[5] = 101;
 137              		.loc 1 40 0
 138 0074 07F10403 		add	r3, r7, #4
 139 0078 4FF06502 		mov	r2, #101
 140 007c 5A61     		str	r2, [r3, #20]
  41:../src/main.c **** 	X.x[6] = -50;
 141              		.loc 1 41 0
 142 007e 07F10403 		add	r3, r7, #4
 143 0082 6FF03102 		mvn	r2, #49
 144 0086 9A61     		str	r2, [r3, #24]
  42:../src/main.c **** 	//  Call compute_stats() function in an appropriate manner to process the dataset object X
  43:../src/main.c **** 	//  ... compute_stats( ... )
  44:../src/main.c **** 	compute_stats(&X);
 145              		.loc 1 44 0
 146 0088 07F10403 		add	r3, r7, #4
 147 008c 1846     		mov	r0, r3
 148 008e FFF7FEFF 		bl	compute_stats
  45:../src/main.c **** 
  46:../src/main.c **** 	//  Print out the contents of the dataset object X
  47:../src/main.c **** 	for (i=0;i<TOTAL_DATA;i++)
 149              		.loc 1 47 0
 150 0092 4FF00003 		mov	r3, #0
 151 0096 C7F8A431 		str	r3, [r7, #420]
 152 009a 14E0     		b	.L5
 153              	.L6:
  48:../src/main.c **** 		printf("x[%d]: %d\n", i, X.x[i]);
 154              		.loc 1 48 0 discriminator 2
 155 009c 40F20003 		movw	r3, #:lower16:.LC0
 156 00a0 C0F20003 		movt	r3, #:upper16:.LC0
 157 00a4 07F10402 		add	r2, r7, #4
 158 00a8 D7F8A411 		ldr	r1, [r7, #420]
 159 00ac 52F82120 		ldr	r2, [r2, r1, lsl #2]
 160 00b0 1846     		mov	r0, r3
 161 00b2 D7F8A411 		ldr	r1, [r7, #420]
 162 00b6 FFF7FEFF 		bl	printf
  47:../src/main.c **** 	for (i=0;i<TOTAL_DATA;i++)
 163              		.loc 1 47 0 discriminator 2
 164 00ba D7F8A431 		ldr	r3, [r7, #420]
 165 00be 03F10103 		add	r3, r3, #1
 166 00c2 C7F8A431 		str	r3, [r7, #420]
 167              	.L5:
  47:../src/main.c **** 	for (i=0;i<TOTAL_DATA;i++)
 168              		.loc 1 47 0 is_stmt 0 discriminator 1
 169 00c6 D7F8A431 		ldr	r3, [r7, #420]
 170 00ca 632B     		cmp	r3, #99
 171 00cc E6DD     		ble	.L6
  49:../src/main.c **** 	printf("mean: %d\n", X.mean);
 172              		.loc 1 49 0 is_stmt 1
 173 00ce 40F20003 		movw	r3, #:lower16:.LC1
 174 00d2 C0F20003 		movt	r3, #:upper16:.LC1
 175 00d6 07F10402 		add	r2, r7, #4
 176 00da D2F89021 		ldr	r2, [r2, #400]
 177 00de 1846     		mov	r0, r3
 178 00e0 1146     		mov	r1, r2
 179 00e2 FFF7FEFF 		bl	printf
  50:../src/main.c **** 	printf("variance: %d\n", X.variance);
 180              		.loc 1 50 0
 181 00e6 40F20003 		movw	r3, #:lower16:.LC2
 182 00ea C0F20003 		movt	r3, #:upper16:.LC2
 183 00ee 07F10402 		add	r2, r7, #4
 184 00f2 D2F89421 		ldr	r2, [r2, #404]
 185 00f6 1846     		mov	r0, r3
 186 00f8 1146     		mov	r1, r2
 187 00fa FFF7FEFF 		bl	printf
  51:../src/main.c **** 	printf("min: %d\n", X.min);
 188              		.loc 1 51 0
 189 00fe 40F20003 		movw	r3, #:lower16:.LC3
 190 0102 C0F20003 		movt	r3, #:upper16:.LC3
 191 0106 07F10402 		add	r2, r7, #4
 192 010a D2F89821 		ldr	r2, [r2, #408]
 193 010e 1846     		mov	r0, r3
 194 0110 1146     		mov	r1, r2
 195 0112 FFF7FEFF 		bl	printf
  52:../src/main.c **** 	printf("max: %d\n", X.max);
 196              		.loc 1 52 0
 197 0116 40F20003 		movw	r3, #:lower16:.LC4
 198 011a C0F20003 		movt	r3, #:upper16:.LC4
 199 011e 07F10402 		add	r2, r7, #4
 200 0122 D2F89C21 		ldr	r2, [r2, #412]
 201 0126 1846     		mov	r0, r3
 202 0128 1146     		mov	r1, r2
 203 012a FFF7FEFF 		bl	printf
 204              	.L7:
  53:../src/main.c **** 
  54:../src/main.c **** 	// Enter an infinite loop, just incrementing a counter.
  55:../src/main.c **** 	// Do not modify the code below. It enables values or variables and registers to be inspected befo
  56:../src/main.c **** 	volatile static int loop = 0;
  57:../src/main.c **** 	while (1) {
  58:../src/main.c **** 		loop++;
 205              		.loc 1 58 0 discriminator 1
 206 012e 40F20003 		movw	r3, #:lower16:loop.4859
 207 0132 C0F20003 		movt	r3, #:upper16:loop.4859
 208 0136 1B68     		ldr	r3, [r3, #0]
 209 0138 03F10102 		add	r2, r3, #1
 210 013c 40F20003 		movw	r3, #:lower16:loop.4859
 211 0140 C0F20003 		movt	r3, #:upper16:loop.4859
 212 0144 1A60     		str	r2, [r3, #0]
  59:../src/main.c **** 	}
 213              		.loc 1 59 0 discriminator 1
 214 0146 F2E7     		b	.L7
 215              		.cfi_endproc
 216              	.LFE30:
 218              		.bss
 219              		.align	2
 220              	loop.4859:
 221 0000 00000000 		.space	4
 222              		.text
 223              	.Letext0:
 224              		.file 2 "C:\\Users\\siddh\\Documents\\LPCXpresso_6.1.4_194\\workspace\\Lib_CMSISv1p30_LPC17xx\\inc
DEFINED SYMBOLS
                            *ABS*:00000000 main.c
C:\Users\siddh\AppData\Local\Temp\cctcMZqe.s:19     .text.compute_stats:00000000 $t
C:\Users\siddh\AppData\Local\Temp\cctcMZqe.s:24     .text.compute_stats:00000000 compute_stats
C:\Users\siddh\AppData\Local\Temp\cctcMZqe.s:63     .rodata:00000000 $d
C:\Users\siddh\AppData\Local\Temp\cctcMZqe.s:64     .rodata:00000000 .LC0
C:\Users\siddh\AppData\Local\Temp\cctcMZqe.s:67     .rodata:0000000c .LC1
C:\Users\siddh\AppData\Local\Temp\cctcMZqe.s:70     .rodata:00000018 .LC2
C:\Users\siddh\AppData\Local\Temp\cctcMZqe.s:73     .rodata:00000028 .LC3
C:\Users\siddh\AppData\Local\Temp\cctcMZqe.s:76     .rodata:00000034 .LC4
C:\Users\siddh\AppData\Local\Temp\cctcMZqe.s:79     .text.main:00000000 $t
C:\Users\siddh\AppData\Local\Temp\cctcMZqe.s:84     .text.main:00000000 main
C:\Users\siddh\AppData\Local\Temp\cctcMZqe.s:220    .bss:00000000 loop.4859
C:\Users\siddh\AppData\Local\Temp\cctcMZqe.s:219    .bss:00000000 $d
                     .debug_frame:00000010 $d

UNDEFINED SYMBOLS
asm_stats
printf
