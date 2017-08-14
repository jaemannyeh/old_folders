

ARM assembler in Raspberry Pi 01 Introduction


ARM assembler in Raspberry Pi 02 Registers and basic arithmetic


ARM assembler in Raspberry Pi 03 Memory, addresses. Load and store.


ARM assembler in Raspberry Pi 04 GDB


ARM assembler in Raspberry Pi 05 Branches


ARM assembler in Raspberry Pi 06 Control structures


ARM assembler in Raspberry Pi 07 Indexing modes


ARM assembler in Raspberry Pi 08 Arrays and structures and more indexing modes.


ARM assembler in Raspberry Pi 09 Functions (I)


ARM assembler in Raspberry Pi 10 Functions (II). The stack


ARM assembler in Raspberry Pi 11 Predication


ARM assembler in Raspberry Pi 12 Loops and the status register


ARM assembler in Raspberry Pi 13 Floating point numbers


ARM assembler in Raspberry Pi 14 Matrix multiply


ARM assembler in Raspberry Pi 15 Integer division


ARM assembler in Raspberry Pi 16 Switch control structure


ARM assembler in Raspberry Pi 17 Passing data to functions


ARM assembler in Raspberry Pi 18 Local data and the frame pointer


ARM assembler in Raspberry Pi 19 The operating system


ARM assembler in Raspberry Pi 20 Indirect calls


ARM assembler in Raspberry Pi 21 Subword data


ARM assembler in Raspberry Pi 22 The Thumb instruction set


ARM assembler in Raspberry Pi 23 Nested functions


ARM assembler in Raspberry Pi 24 Trampolines


ARM assembler in Raspberry Pi 25 Integer SIMD


ARM assembler in Raspberry Pi 26 A primer about linking


    .data:
    var: .word 42
    .text
    func:
        /* ... */
        ldr r0, addr_of_var  /* r0 ← &var */
        ldr r0, [r0]         /* r0 ← *r0 */
        /* ... */
    addr_of_var : .word var

    
    ldr r0, [pc, #offset]
    
    
    /* main.s */
    .data
     
    one_var : .word 42
    another_var : .word 66
     
    .globl result_var             /* mark result_var as global */
    result_var : .word 0
     
    .text
     
    .globl main
    main:
        ldr r0, addr_one_var      /* r0 ← &one_var */
        ldr r0, [r0]              /* r0 ← *r0 */
        ldr r1, addr_another_var  /* r1 ← &another_var */
        ldr r1, [r1]              /* r1 ← *r1 */
        add r0, r0, r1            /* r0 ← r0 + r1 */
        ldr r1, addr_result       /* r1 ← &result */
        str r0, [r1]              /* *r1 ← r0 */
        bl inc_result             /* call to inc_result */
        mov r0, #0                /* r0 ← 0 */
        bx lr                     /* return */
     
     
    addr_one_var  : .word one_var
    addr_another_var  : .word another_var
    addr_result  : .word result_var






