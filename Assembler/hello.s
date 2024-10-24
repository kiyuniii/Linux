.global _start

_start:
    mov w0, #1            // w0에 1(STDOUT) 저장
    adr x1, msg           // msg 문자열의 주소를 x1에 로드
    mov w2, #13           // 문자열 길이 13을 w2에 저장
    mov x8, #64           // sys_write 시스템 호출 번호
    svc #0                // 시스템 호출 실행

    mov w0, #0            // 정상 종료를 위해 w0에 0 저장
    mov x8, #93           // sys_exit 시스템 호출 번호
    svc #0                // 시스템 호출 실행

msg:
    .asciz "Hello, QEMU!"

