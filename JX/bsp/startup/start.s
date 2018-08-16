; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        EXTERN  SystemInit        
        PUBLIC  __vector_table

        DATA
__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler             ; Reset Handler
        DCD     Software_Handler          ; NMI Handler
        DCD     PrefetchAbort_Handler     ; Hard Fault Handler
        DCD     DataAbort_Handler         ; MPU Fault Handler
        DCD     0                         ; Bus Fault Handler
        DCD     IRQ_Handler               ; Usage Fault Handler
        DCD     FIQ_Handler               ; Reserved

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:NOROOT(2)
Reset_Handler
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0
        
        PUBWEAK Software_Handler
        SECTION .text:CODE:NOROOT(2)
Software_Handler
        B Software_Handler

        PUBWEAK PrefetchAbort_Handler
        SECTION .text:CODE:NOROOT(2)
PrefetchAbort_Handler
        B PrefetchAbort_Handler

        PUBWEAK DataAbort_Handler
        SECTION .text:CODE:NOROOT(2)
DataAbort_Handler
        B DataAbort_Handler

        PUBWEAK IRQ_Handler
        SECTION .text:CODE:NOROOT(2)
IRQ_Handler
        B IRQ_Handler

        PUBWEAK FIQ_Handler
        SECTION .text:CODE:NOROOT(2)
FIQ_Handler
        B FIQ_Handler
        
        END
