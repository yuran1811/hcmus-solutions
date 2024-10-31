.extern scanf
.extern printf

.data
    sfmt: .asciz "%ld"

    smsg: .asciz "Enter n: "
    prime_msg: .asciz "%d is prime\n"
    nprime_msg: .asciz "%d is not prime\n"

.bss
    n: .space 8

.text
    .global _is_prime
    .global main

_is_prime:
    cmp x0, #1
    b.le _is_prime.not_prime    // n <= 1: not prime
    cmp x0, #2
    b.eq _is_prime.prime        // n == 2: prime

    mov x1, #2                  // i = 2
    _is_prime.check_loop:
        mul x2, x1, x1
        cmp x2, x0
        b.gt _is_prime.prime    // i * i > n: prime

        udiv x2, x0, x1         // n / i
        msub x2, x2, x1, x0     // n - i * (n / i)
        cbz x2, _is_prime.not_prime

        add x1, x1, #1
        b _is_prime.check_loop

    _is_prime.prime:
        mov x0, #1
        ret

    _is_prime.not_prime:
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
    bl _is_prime

    cbz x0, main.nprime_handle
    adrp x0, prime_msg
    add x0, x0, :lo12:prime_msg
    b main.output

    main.nprime_handle:
        adrp x0, nprime_msg
        add x0, x0, :lo12:nprime_msg

    main.output:
        adrp x1, n
        add x1, x1, :lo12:n
        ldr x1, [x1]
        bl printf

    mov x0, xzr
    mov x8, #93
    svc #0
