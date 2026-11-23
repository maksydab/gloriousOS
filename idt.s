.code32
.global idt_flush
.global irq0_stub
.global irq1_stub
.extern pit_irq_handler
.extern keyboard_irq_handler

idt_flush:
    movl 4(%esp), %eax
    lidt (%eax)
    ret

irq0_stub:
    pushal
    call pit_irq_handler
    popal
    iret

irq1_stub:
    pushal
    call keyboard_irq_handler
    popal
    iret