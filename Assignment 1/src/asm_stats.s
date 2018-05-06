 @ Aditya Agarwal A0161473H
 @ Siddharth Madhavan A0150785Y
 	.syntax unified
 	.cpu cortex-m3
 	.thumb
 	.align 2
 	.global	asm_stats
 	.thumb_func

asm_stats:
@  Write assembly language code here to compute the statistical parameters

// stores the initial values of the registers
push {r4}
push {r5}
push {r6}
push {r8}
push {r9}
push {r12}

mov r12, 100 // holds the number of data

push {r0} // stores the address of the array

mov r4, r12 // holds the counter value for loop

mov r6, #0 // initializes the summation value

// initialises the max and min values
ldr r8, [r0] // min value
ldr r9, [r0] // max value

MAX_MIN_MEAN:
	ldr r5, [r0],0x04
	add r6, r6, r5

	cmp r5, r8
	it lt
	movlt r8, r5

	cmp r5, r9
	it ge
	movge r9, r5

	subs r4, 1
	bne MAX_MIN_MEAN

sdiv r6, r6, r12 // holds the mean value

str r8, [r2] // stores the min value to the address
str r9, [r3] // stores the max value to the address

pop {r0} // restores the address of the initial array

mov r4, r12 // holds the counter value for loop

mov r8, #0 // initializes the summation value for variance calculation
mov r9, #0 // initializes the intermediate value for variance calculation

VARIANCE:
	ldr r5, [r0],0x04
	sub r9, r5, r6
	mul r9, r9, r9
	add r8, r8, r9

	subs r4, 1
	bne VARIANCE

mov r0, r6 // prepares for the return mean value

mov r4, r12 // holds the value of total number of data
sub r4, #1 // computes n-1 and stores in r4

sdiv r8, r8, r4 // computes the variance and stores it in r11

str r8, [r1] // stores the computed variance in the initial variance address

// return back those initial values of the registers
pop {r12}
pop {r9}
pop {r8}
pop {r6}
pop {r5}
pop {r4}

 BX	LR // returns the mean back to C program
