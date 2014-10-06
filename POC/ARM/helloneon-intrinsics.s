	.syntax unified
	.arch armv7-a
	.eabi_attribute 27, 3
	.fpu neon
	.eabi_attribute 23, 1
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 2
	.eabi_attribute 18, 4
	.thumb
	.file	"helloneon-intrinsics.c"
	.section	.text.neon_interlace,"ax",%progbits
	.align	2
	.global	neon_interlace
	.thumb
	.thumb_func
	.type	neon_interlace, %function
neon_interlace:
	.fnstart
.LFB1871:
	@ args = 8, pretend = 0, frame = 104
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	push	{r4, r5, r6, r7, r8, r9, sl, fp}
	.save {r4, r5, r6, r7, r8, r9, sl, fp}
.LCFI0:
	.pad #104
	sub	sp, sp, #104
.LCFI1:
	mov	ip, r0
	str	r2, [sp, #24]
	str	r3, [sp, #28]
	ldr	r2, [sp, #136]
	ldr	r3, [sp, #140]
	str	r1, [sp, #20]
	mul	r3, r2, r3
	adds	r2, r3, #7
	bics	r3, r3, r3, asr #32
	it	cs
	movcs	r3, r2
	asrs	r3, r3, #3
	str	r3, [sp, #16]
	cmp	r3, #0
	ble	.L4
	movs	r6, #0
	add	r9, sp, #56
	add	r7, sp, #32
	mov	r8, r6
	mov	r5, r6
	mov	r4, r7
	mov	fp, r9
	mov	sl, r9
	add	r2, sp, #80
	str	r7, [sp, #4]
	str	r2, [sp, #12]
.L3:
	ldr	r7, [sp, #20]
	add	r8, r8, #1
	ldr	r2, [sp, #24]
	adds	r3, r7, r6
	ldr	r7, [sp, #28]
	vld1.8	{d18}, [r3]
	adds	r3, r2, r6
	vld1.8	{d17}, [r3]
	adds	r3, r7, r6
	ldr	r7, [sp, #12]
	vld1.8	{d16}, [r3]
	fstd	d18, [sp, #80]
	fstd	d17, [sp, #88]
	fstd	d16, [sp, #96]
	adds	r6, r6, #8
	ldmia	r7!, {r0, r1, r2, r3}
	mov	r9, r7
	mov	r7, sl
	mov	sl, fp
	stmia	r7!, {r0, r1, r2, r3}
	ldmia	r9, {r0, r1}
	ldr	r3, [sp, #4]
	ldr	r2, [sp, #4]
	stmia	r7, {r0, r1}
	mov	r7, fp
	str	r5, [r3], #4
	adds	r3, r3, #4
	str	r5, [r2, #4]
	str	r5, [r3], #4
	str	r5, [r3], #4
	str	r5, [r3], #4
	str	r5, [r3, #0]
	ldmia	r7!, {r0, r1, r2, r3}
	mov	r9, r7
	mov	r7, r4
	stmia	r7!, {r0, r1, r2, r3}
	ldmia	r9, {r0, r1}
	ldr	r2, [sp, #16]
	str	r4, [sp, #4]
	cmp	r8, r2
	stmia	r7, {r0, r1}
	vldmia	r4, {d16-d18}
	vst3.8	{d16-d18}, [ip]
	add	ip, ip, #24
	bne	.L3
.L4:
	add	sp, sp, #104
	pop	{r4, r5, r6, r7, r8, r9, sl, fp}
	bx	lr
.LFE1871:
	.fnend
	.size	neon_interlace, .-neon_interlace
	.section	.text.neon_deinterlace,"ax",%progbits
	.align	2
	.global	neon_deinterlace
	.thumb
	.thumb_func
	.type	neon_deinterlace, %function
neon_deinterlace:
	.fnstart
.LFB1870:
	@ args = 8, pretend = 0, frame = 64
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	push	{r4, r5, r6, r7, r8, r9, sl, fp}
	.save {r4, r5, r6, r7, r8, r9, sl, fp}
.LCFI2:
	.pad #64
	sub	sp, sp, #64
.LCFI3:
	mov	sl, r2
	mov	r9, r3
	ldr	r2, [sp, #96]
	mov	r6, r0
	ldr	r3, [sp, #100]
	mov	fp, r1
	mul	r8, r2, r3
	add	r3, r8, #7
	bics	r8, r8, r8, asr #32
	it	cs
	movcs	r8, r3
	asr	r8, r8, #3
	cmp	r8, #0
	ble	.L10
	movs	r5, #0
	add	r4, sp, #16
	mov	r7, r5
	add	r3, sp, #40
	str	r4, [sp, #4]
	str	r3, [sp, #12]
.L9:
	ldr	r4, [sp, #4]
	vld3.8	{d16-d18}, [r6]
	adds	r7, r7, #1
	adds	r6, r6, #24
	vstmia	r4, {d16-d18}
	ldmia	r4!, {r0, r1, r2, r3}
	mov	ip, r4
	ldr	r4, [sp, #12]
	stmia	r4!, {r0, r1, r2, r3}
	fldd	d16, [sp, #40]
	ldmia	ip, {r0, r1}
	add	r3, fp, r5
	stmia	r4, {r0, r1}
	vst1.8	{d16}, [r3]
	add	r3, sl, r5
	fldd	d16, [sp, #48]
	vst1.8	{d16}, [r3]
	add	r3, r9, r5
	adds	r5, r5, #8
	cmp	r7, r8
	fldd	d16, [sp, #56]
	vst1.8	{d16}, [r3]
	bne	.L9
.L10:
	add	sp, sp, #64
	pop	{r4, r5, r6, r7, r8, r9, sl, fp}
	bx	lr
.LFE1870:
	.fnend
	.size	neon_deinterlace, .-neon_deinterlace
	.ident	"GCC: (GNU) 4.4.3"
	.section	.note.GNU-stack,"",%progbits
