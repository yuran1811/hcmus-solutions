.extern scanf
.extern printf

.data
    sfmt: .asciz "%ld"

    menu: .asciz "\n=========== Menu ===========\n1. Nhap mang\n2. Xuat mang\n3. Liet ke cac so nguyen to\n4. Tim so lon nhat\n5. Tinh trung binh cong\n6. Thoat\n============================\n"
    menu_choice_msg: .asciz "Nhap lua chon cua ban: "
    invalid_choice_msg: .asciz "Lua chon khong hop le\n"

    action1_msg1: .asciz "Nhap so phan tu cua mang: "
    action1_msg2: .asciz "a[%d]: "

    action2_msg1: .asciz "Mang vua nhap la:\n"
    action2_msg2: .asciz "a[%d] = %d\n"

    action3_msg1: .asciz "Danh sach cac so nguyen to:\n"
    action4_msg1: .asciz "Gia tri lon nhat trong mang la: %d\n"
    action5_msg1: .asciz "Trung binh cong cac gia tri trong mang la: %f\n"
    action6_msg1: .asciz "Hen gap lai!\n"

.bss
    menu_choice: .space 8
    arr: .space 160
    n: .space 8

.text
    .global _is_prime
    .global _arr_primes
    .global _arr_scan
    .global _arr_prin
    .global _arr_aver
    .global _arr_max
    .global _menu_render
    .global main

_is_prime:
    sub x28, x28, #8
    stur x30, [x28]
    mov x1, #1

    cmp x0, x1
    b.le _is_prime.not_prime    // n <= 1: not prime

    mov x1, #2
    cmp x0, x1
    b.eq _is_prime.prime        // n == 2: prime

    mov x2, #2                  // i = 2
    _is_prime.check_loop:
        mul x3, x2, x2
        cmp x3, x0
        b.gt _is_prime.prime    // i * i > n: prime

        udiv x3, x0, x2         // n / i
        msub x3, x3, x2, x0     // n - i * (n / i)
        cbz x3, _is_prime.not_prime

        add x2, x2, #1
        b _is_prime.check_loop

    _is_prime.prime:
        ldur x30, [x28]
        add x28, x28, #8
        mov x0, #1
        ret

    _is_prime.not_prime:
        ldur x30, [x28]
        add x28, x28, #8
        mov x0, xzr
        ret

_arr_primes:
    sub x28, x28, #8
    stur x30, [x28]

    adrp x0, action3_msg1
    add x0, x0, #:lo12:action3_msg1
    bl printf

    adrp x0, n
    add x0, x0, #:lo12:n
    ldr x19, [x0]               // x19 = n

    adrp x20, arr
    add x20, x20, :lo12:arr     // x20 = &arr[0]

    mov x21, xzr                // x21 = i = 0
    _arr_primes.loop:
        cmp x21, x19
        b.ge _arr_primes.end

        ldr x0, [x20]
        bl _is_prime

        cbz x0, _arr_primes.next

        adrp x0, action2_msg2
        add x0, x0, #:lo12:action2_msg2
        mov x1, x21
        ldr x2, [x20]        
        bl printf
        
        _arr_primes.next:
            add x20, x20, #8
            add x21, x21, #1
            b _arr_primes.loop
    _arr_primes.end:
        mov x0, xzr

        ldur x30, [x28]
        add x28, x28, #8
        ret

_arr_scan:
    sub x28, x28, #8
    stur x30, [x28]

    adrp x0, action1_msg1
    add x0, x0, #:lo12:action1_msg1
    bl printf

    adrp x0, sfmt
    add x0, x0, #:lo12:sfmt
    adrp x1, n
    add x1, x1, :lo12:n
    bl scanf

    adrp x0, n
    add x0, x0, #:lo12:n
    ldr x19, [x0]               // x19 = n

    adrp x20, arr
    add x20, x20, :lo12:arr     // x20 = &arr[0]

    mov x21, xzr                // x21 = i = 0
    _arr_scan.loop:
        cmp x21, x19
        b.ge _arr_scan.end

        adrp x0, action1_msg2
        add x0, x0, #:lo12:action1_msg2
        mov x1, x21
        bl printf

        adrp x0, sfmt
        add x0, x0, #:lo12:sfmt
        mov x1, x20
        bl scanf
        
        add x20, x20, #8
        add x21, x21, #1
        b _arr_scan.loop
    _arr_scan.end:
        mov x0, xzr

        ldur x30, [x28]
        add x28, x28, #8
        ret

_arr_prin:
    sub x28, x28, #8
    stur x30, [x28]

    adrp x0, action2_msg1
    add x0, x0, #:lo12:action2_msg1
    bl printf

    adrp x0, n
    add x0, x0, #:lo12:n
    ldr x19, [x0]               // x19 = n

    adrp x20, arr
    add x20, x20, :lo12:arr     // x20 = &arr[0]

    mov x21, xzr                // x21 = i = 0
    _arr_prin.loop:
        cmp x21, x19
        b.ge _arr_prin.end

        adrp x0, action2_msg2
        add x0, x0, #:lo12:action2_msg2
        mov x1, x21
        ldr x2, [x20]
        bl printf
        
        add x20, x20, #8
        add x21, x21, #1
        b _arr_prin.loop
    _arr_prin.end:
        mov x0, xzr

        ldur x30, [x28]
        add x28, x28, #8
        ret

_arr_aver:
    sub x28, x28, #8
    stur x30, [x28]

    adrp x0, n
    add x0, x0, #:lo12:n
    ldr x19, [x0]               // x19 = n

    adrp x20, arr
    add x20, x20, :lo12:arr     // x20 = &arr[0]

    mov x21, xzr                // x21 = i = 0
    mov x22, xzr                // x22 = sum = 0
    _arr_aver.loop:
        cmp x21, x19
        b.ge _arr_aver.end

        ldr x0, [x20]
        add x22, x22, x0
        
        _arr_aver.next:
            add x20, x20, #8
            add x21, x21, #1
            b _arr_aver.loop
    _arr_aver.end:
        adrp x0, action5_msg1
        add x0, x0, #:lo12:action5_msg1

        scvtf d0, x22
        scvtf d1, x19
        fdiv d0, d0, d1
        fmov x1, d0
        bl printf

        mov x0, xzr

        ldur x30, [x28]
        add x28, x28, #8
        ret

_arr_max:
    sub x28, x28, #8
    stur x30, [x28]

    adrp x0, n
    add x0, x0, #:lo12:n
    ldr x19, [x0]               // x19 = n

    adrp x20, arr
    add x20, x20, :lo12:arr     // x20 = &arr[0]

    mov x21, xzr                // x21 = i = 0
    mov x22, xzr                // x22 = max = 0
    _arr_max.loop:
        cmp x21, x19
        b.ge _arr_max.end

        ldr x0, [x20]
        cmp x0, x22
        b.lt _arr_max.next

        mov x22, x0
        
        _arr_max.next:
            add x20, x20, #8
            add x21, x21, #1
            b _arr_max.loop
    _arr_max.end:
        adrp x0, action4_msg1
        add x0, x0, #:lo12:action4_msg1
        mov x1, x22
        bl printf

        mov x0, xzr

        ldur x30, [x28]
        add x28, x28, #8
        ret

_menu_render:
    sub x28, x28, #8
    stur x30, [x28]

    adrp x0, menu
    add x0, x0, #:lo12:menu
    bl printf

    adrp x0, menu_choice_msg
    add x0, x0, #:lo12:menu_choice_msg
    bl printf

    adrp x0, sfmt
    add x0, x0, #:lo12:sfmt
    adrp x1, menu_choice
    add x1, x1, #:lo12:menu_choice
    bl scanf
    
    ldur x30, [x28]
    add x28, x28, #8
    ret

main:
    main.menu:
        bl _menu_render

        adrp x0, menu_choice
        add x0, x0, #:lo12:menu_choice
        ldr x0, [x0]

        cmp x0, #1
        b.eq main.action1
        cmp x0, #2
        b.eq main.action2
        cmp x0, #3
        b.eq main.action3
        cmp x0, #4
        b.eq main.action4
        cmp x0, #5
        b.eq main.action5
        cmp x0, #6
        b.eq main.action6

        adrp x0, invalid_choice_msg
        add x0, x0, #:lo12:invalid_choice_msg
        bl printf
        b main.menu

        main.action1:
            bl _arr_scan
            b main.menu

        main.action2:
            bl _arr_prin
            b main.menu

        main.action3:
            bl _arr_primes
            b main.menu
        
        main.action4:
            bl _arr_max
            b main.menu

        main.action5:
            bl _arr_aver
            b main.menu
        
        main.action6:
            adrp x0, action6_msg1
            add x0, x0, #:lo12:action6_msg1
            bl printf
    
    mov x0, xzr
    mov x8, #93
    svc #0
