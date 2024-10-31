.extern scanf
.extern printf

.data
    sfmt: .asciz "%ld"

    smsg: .asciz "Enter n: "
    perfect_msg: .asciz "%d is perfect\n"
    nperfect_msg: .asciz "%d is not perfect\n"

.bss
    n: .space 8

.text
    .global _is_perfect
    .global main

_is_perfect:
    cmp x0, #6              // n < 6: not perfect
    b.lt _is_perfect.false

    mov x1, xzr             // x1 = sum = 0
    mov x2, #1              // x2 = i = 1
    _is_perfect.loop:
        cmp x2, x0
        b.ge _is_perfect.end

        mov x3, x0          // x3 = n
        sdiv x4, x3, x2     // x4 = n / i
        mul x4, x4, x2      // x4 = i * (n / i)
        cmp x3, x4
        b.eq _is_perfect.accumulate
        b _is_perfect.next

        _is_perfect.accumulate:
            add x1, x1, x2

        _is_perfect.next:
            add x2, x2, #1
            b _is_perfect.loop
    _is_perfect.end:
        cmp x1, x0
        b.eq _is_perfect.true
        b _is_perfect.false

    _is_perfect.true:
        mov x0, #1
        ret
    _is_perfect.false:
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
    bl _is_perfect

    cbz x0, main.nperfect_handle
    adrp x0, perfect_msg
    add x0, x0, :lo12:perfect_msg
    b main.output

    main.nperfect_handle:
        adrp x0, nperfect_msg
        add x0, x0, :lo12:nperfect_msg

    main.output:
        adrp x1, n
        add x1, x1, :lo12:n
        ldr x1, [x1]
        bl printf

    mov x0, xzr
    mov x8, #93
    svc #0
