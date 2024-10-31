.extern scanf
.extern printf

.data
    sfmt: .asciz "%ld"

    smsg: .asciz "Enter n: "
    square_msg: .asciz "%d is square\n"
    nsquare_msg: .asciz "%d is not square\n"

.bss
    n: .space 8

.text
    .global _is_square
    .global main

_is_square:
    cmp x0, xzr             // n < 0: not square number
    b.lt _is_square.false
    cmp x0, #1              // 0 <= n <= 1: square number
    b.le _is_square.true

    mov x1, #2              // i = 2
    _is_square.loop:
        mov x2, xzr
        mul x2, x1, x1
        cmp x2, x0
        b.ge _is_square.end // i * i > n: goto end

        add x1, x1, #1
        b _is_square.loop
    _is_square.end:
        mov x2, xzr
        mul x2, x1, x1
        cmp x2, x0
        b.eq _is_square.true
        b _is_square.false

    _is_square.true:
        mov x0, #1
        ret
    _is_square.false:
        mov x0, xzr
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
    ldr x0, [x0]
    bl _is_square

    cbz x0, main.nsquare_handle
    adrp x0, square_msg
    add x0, x0, :lo12:square_msg
    b main.output

    main.nsquare_handle:
        adrp x0, nsquare_msg
        add x0, x0, :lo12:nsquare_msg

    main.output:
        adrp x1, n
        add x1, x1, :lo12:n
        ldr x1, [x1]
        bl printf
    
    mov x0, xzr
    mov x8, #93
    svc #0
