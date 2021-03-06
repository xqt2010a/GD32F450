; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-A5 version
;

        MODULE  ?start_jx

        ;; Forward declaration of sections.
        SECTION IRQ_STACK:DATA:NOROOT(3)
        SECTION FIQ_STACK:DATA:NOROOT(3)
        SECTION CSTACK:DATA:NOROOT(3)
        
; The module in this file are included in the libraries, and may be
; replaced by any user-defined modules that define the PUBLIC symbol
; __iar_program_start or a user defined start symbol.
;
; To override the cstartup defined in the library, simply add your
; modified version to the workbench project.

        SECTION .intvec:CODE:NOROOT(2)
        
        PUBLIC  __vector
        PUBLIC  __iar_program_start
        EXTERN  Undefined_Handler
        EXTERN  SWI_Handler
        EXTERN  Prefetch_Handler
        EXTERN  Abort_Handler
        EXTERN  IRQ_Handler
        EXTERN  FIQ_Handler
        EXTERN  SystemInit
        EXTERN  LowLevelInit

        DATA
        
__iar_init$$done:               ; The vector table is not needed
                                ; until after copy initialization is done
          
__vector:
        
        ARM
        
        ; All default exception handlers (except reset) are
        ; defined as weak symbol definitions.
        ; If a handler is defined by the application it will take precedence.
        LDR     PC,Reset_Addr           ; Reset
        LDR     PC,Undefined_Addr       ; Undefined instructions
        LDR     PC,SWI_Addr             ; Sofeware interrupt (SWI/SVC)
        LDR     PC,Prefetch_Addr        ; Prefetch abort
        LDR     PC,Abort_Addr           ; Data Abort 
        DCD     0x3137314E              ; RESERVED
        LDR     PC,IRQ_Addr             ; IRQ
        LDR     PC,FIQ_Addr             ; FIQ
        
        DATA
        
Reset_Addr:     DCD     __iar_program_start
Undefined_Addr: DCD     Undefined_Handler
SWI_Addr:       DCD     SWI_Handler
Prefetch_Addr:  DCD     Prefetch_Handler
Abort_Addr:     DCD     Abort_Handler
IRQ_Addr:       DCD     IRQ_Handler
FIQ_Addr:       DCD     FIQ_Handler


; --------------------------------------------------
; ?cstartup -- low-level system initialization code.
;
; After a reset execution starts here, the mode is ARM, supervisor
; with interrupts disabled.

        SECTION .text:CODE:NOROOT(2)
        
        EXTERN  __cmain
        REQUIRE __vector
        EXTWEAK __iar_init_core
        EXTWEAK __iar_init_vfp
        
        ARM
        
__iar_program_start:
?start_jx:
;smu init
        LDR R0, =0x3FE08100 
        LDR R1, =0xFFFFFFFF
        STR R1, [R0]        ;0x3FE08100
        STR R1, [R0, #4]    ;0x3FE08104
        LDR R0, =0x0190d100
        STR R1, [R0], #4    ;0x0190d100
        STR R1, [R0], #4    ;0x0190d104
        STR R1, [R0], #4    ;0x0190d108
        STR R1, [R0], #4    ;0x0190d10c
        STR R1, [R0], #4    ;0x0190d110
        STR R1, [R0], #12   ;0x0190d114
        STR R1, [R0], #4    ;0x0190d120
        STR R1, [R0]        ;0x0190d124
; Disable Addr translation, D cache and I cache disabled
        MRC     p15,0,R1,C1,C0,0
        LDR     R0,=0xFFFFEFFA
        AND     R1,R1,R0
        ORR     R1,R1,#(1<<12)
        MCR     p15,0,R1,C1,C0,0
        
; Add initialization needed before setup of stackpointers here.        
        
; Initialize the stack pointers.
; The pattern below can be used for any of the exception stacks:
; FIQ, IRQ, SVC, ABT, UND, SYS.
; The USR mode uses the same stack as SYS.
; The stack segments must be defined in the linker command file,
; and be declared above.        
        
; --------------------
; Mode, correspords to bits 0-5 in CPSR

#define MODE_MSK 0x1F            ; Bit mask for mode bits in CPSR

#define USR_MODE 0x10            ; User mode
#define FIQ_MODE 0x11            ; Fast Interrupt Request mode
#define IRQ_MODE 0x12            ; Interrupt Request mode
#define SVC_MODE 0x13            ; Supervisor mode
#define ABT_MODE 0x17            ; Abort mode
#define UND_MODE 0x1B            ; Undefined Instruction mode
#define SYS_MODE 0x1F            ; System mode   

#define FIQ_BIT  0x40
#define IRQ_BIT  0x80
 
        /* Remap */
        /* Set V=0 in CP15 SCTLR register - for VBAR to point to vector */
        mrc	p15, 0, r0, c1, c0, 0   ;	@ Read CP15 SCTLR Register
        bic	r0, r0, #0x00002000     ;	@ clear bits 13 (--V-)  
        mcr	p15, 0, r0, c1, c0, 0   ;	@ Write CP15 SCTLR Register
        /* Set vector address in CP15 VBAR register */
        ldr r0, =0x80000
        mcr	p15, 0, r0, c12, c0, 0  ;	@Set VBAR
        
        ;;ldr r4, = SFE(CSTACK)
        ;;mov sp, r4
        ;;blx r0
        
        ;; Set up the interrupt stack pointer.
        MSR     cpsr_c, #IRQ_MODE ;| IRQ_BIT | FIQ_BIT ;change IRQ mode and disable IRQ & FIQ
        LDR     sp, =SFE(IRQ_STACK)         ; End of IRQ_STACK
        ;BIC     sp,sp,#0x7                  ; Make sure SP is 8 aligned

        ;; Set up the fast interrupt stack pointer.
        MSR     cpsr_c, #FIQ_MODE ;| IRQ_BIT | FIQ_BIT ;change FIQ_MODE and disable IRQ & FIQ
        LDR     sp, =SFE(FIQ_STACK)         ; End of FIQ_STACK
        ;BIC     sp,sp,#0x7                  ; Make sure SP is 8 aligned
        
        MSR     cpsr_c, #SVC_MODE
        LDR     sp, =SFE(CSTACK)
		/* Back to Supervisor mode bfore calling main().  The schduduler should
		be started from Supervisor mode. */
        ;;MSR     cpsr_c, #SVC_MODE | FIQ_BIT              ; Change the mode
        MRS     r0, cpsr                ; Original PSR value
        BIC     r0 ,r0, #MODE_MSK       ; Clear the mode bits
        ORR     r0 ,r0, #SYS_MODE       ; Set System mode bits
        MSR     cpsr_c, r0              ; Change the mode
        LDR     sp, =SFE(CSTACK)        ; End of CSTACK
        BIC     sp,sp,#0x7              ; Make sure SP is 8 aligned
        
        ;; Turn on core features assumed to be enabled.
          FUNCALL __iar_program_start, __iar_init_core
        BL      __iar_init_core

        ;; Initialize VFP (if needed).
          FUNCALL __iar_program_start, __iar_init_vfp
        BL      __iar_init_vfp
        
;;; Add more initialization here        
        ;;LDR     R0, = SystemInit
        ;;BLX     R0
        
;;; Continue to __cmain for C-level initialization.

          FUNCALL __iar_program_start, __cmain
        B       __cmain 
        
        END
