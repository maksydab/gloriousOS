.code32

.global idt_flush
.global irq0_stub
.global irq1_stub
.global exc_stub_table

.extern exception_dispatch

idt_flush:
    movl 4(%esp), %eax
    lidt (%eax)
    ret


isr_common:
    cli
    pusha
    push %ds
    push %es
    push %fs
    push %gs

    mov $0x18, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    push %esp
    call exception_dispatch
    add  $4, %esp

    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add $8, %esp /* drop err + vector */
    iret

irq0_stub:
    push $0 /* dummy error code */
    push $32
    jmp isr_common

irq1_stub:
    push $0
    push $33
    jmp isr_common

.macro EXC_NOERR n
.global exc_stub_\n
exc_stub_\n:
    push $0
    push $\n
    jmp isr_common
.endm

.macro EXC_ERR n
.global exc_stub_\n
exc_stub_\n:
    push $\n
    jmp isr_common
.endm

EXC_NOERR 0
EXC_NOERR 1
EXC_NOERR 2
EXC_NOERR 3
EXC_NOERR 4
EXC_NOERR 5
EXC_NOERR 6
EXC_NOERR 7
EXC_ERR   8
EXC_NOERR 9
EXC_ERR   10
EXC_ERR   11
EXC_ERR   12
EXC_ERR   13
EXC_ERR   14
EXC_NOERR 15
EXC_NOERR 16
EXC_ERR   17
EXC_NOERR 18
EXC_NOERR 19
EXC_NOERR 20
EXC_ERR   21
EXC_NOERR 22
EXC_NOERR 23
EXC_NOERR 24
EXC_NOERR 25
EXC_NOERR 26
EXC_NOERR 27
EXC_NOERR 28
EXC_NOERR 29
EXC_ERR   30
EXC_NOERR 31

.data
.global exc_stub_table
exc_stub_table:
    .long exc_stub_0,  exc_stub_1,  exc_stub_2,  exc_stub_3
    .long exc_stub_4,  exc_stub_5,  exc_stub_6,  exc_stub_7
    .long exc_stub_8,  exc_stub_9,  exc_stub_10, exc_stub_11
    .long exc_stub_12, exc_stub_13, exc_stub_14, exc_stub_15
    .long exc_stub_16, exc_stub_17, exc_stub_18, exc_stub_19
    .long exc_stub_20, exc_stub_21, exc_stub_22, exc_stub_23
    .long exc_stub_24, exc_stub_25, exc_stub_26, exc_stub_27
    .long exc_stub_28, exc_stub_29, exc_stub_30, exc_stub_31