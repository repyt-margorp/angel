	.file	"ok.c"
	.text
	.globl	divide
	.type	divide, @function
divide:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movl	-20(%rbp), %eax
	cltd
	idivl	-24(%rbp)
	movl	%eax, -8(%rbp)
	movl	-20(%rbp), %eax
	cltd
	idivl	-24(%rbp)
	movl	%edx, %eax
	movl	%eax, -4(%rbp)
	movq	-8(%rbp), %rax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	divide, .-divide
	.ident	"GCC: (Debian 8.3.0-6) 8.3.0"
	.section	.note.GNU-stack,"",@progbits
