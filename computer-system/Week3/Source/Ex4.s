.extern scanf
.extern printf

.data
    sfmt: .asciz "%ld"

    smsg: .asciz "Enter n: "
    palin_msg: .asciz "%d is palindrome\n"
    npalin_msg: .asciz "%d is not palindrome\n"

    ten: .word 10

.bss
    n: .space 8

.text
    .global _is_palin
    .global main

_is_palin:
    cmp x0, #10
    b.lt _is_palin.true

    mov x1, x0  // x1 = n
    mov x2, xzr // x2 = n_rev = 0

    adrp x4, ten
    add x4, x4, :lo12:ten
    ldr w4, [x4] // x4 = 10

    _is_palin.loop:
        mov x3, xzr         // x3 = 0
        udiv x3, x1, x4     // x3 = n / 10
        mul x3, x3, x4      // x3 *= 10
        sub x3, x1, x3      // x3 = n - x3

        mul x2, x2, x4      // x2 *= 10
        add x2, x2, x3      // x2 += x3

        udiv x1, x1, x4     // n /= 10

        cbnz x1, _is_palin.loop

    cmp x0, x2
    b.eq _is_palin.true
    mov x0, xzr
    ret

    _is_palin.true:
        mov x0, #1
        ret

main:
    adrp x0, smsg
    add x0, x0, :lo12:smsg
    bl printf

    adrp x0, sfmt
    add x0, x0, :lo12:sfmt
    adrp x1, n
    add x1, x1, :lo12:n
    bl scanf

    adrp x0, n
    add x0, x0, :lo12:n
    ldr w0, [x0]
    bl _is_palin

    cbnz x0, main.true
    adrp x0, npalin_msg
    add x0, x0, :lo12:npalin_msg
    b main.output

    main.true:
        adrp x0, palin_msg
        add x0, x0, :lo12:palin_msg

    main.output:
        adrp x1, n
        add x1, x1, :lo12:n
        ldr x1, [x1]
        bl printf
    
    mov x0, xzr
    mov x8, #93
    svc #0
