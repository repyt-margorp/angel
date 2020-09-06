	.file	"ok.c"
	.text
	.globl	divide
	.type	divide, @function
divide:
	pushq	%rbp
	movq	%rsp, %rbp
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
	ret
	.size	divide, .-divide
	.ident	"GCC: (Debian 8.3.0-6) 8.3.0"
	.section	.note.GNU-stack,"",@progbits
