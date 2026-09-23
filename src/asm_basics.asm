.code
asm_add PROC
    mov rax, rcx
    add rax, rdx
    ret
asm_add ENDP

asm_maxInAnArray PROC
    xor     eax, eax              
    test    rdx, rdx
    jz      sum_done

    mov     rax, [rcx]
    add     rcx, 8
    dec     rdx                    
    jz      sum_done

sum_loop:
    mov     r8, [rcx]
    cmp     rax, r8
    jge     end_if  
    mov     rax, r8
end_if:
    add     rcx, 8
    dec     rdx
    jnz     sum_loop
sum_done:
    ret
asm_maxInAnArray ENDP

asm_sortAnArray PROC
    
    test    rdx, rdx
    jz      done
    cmp     rdx, 1
    jle     done            

    xor     r9, r9          

outer_loop:
    
    mov     rax, rdx
    dec     rax             
    cmp     r9, rax
    jge     outer_done

  
    mov     r10, r9                    
    mov     r11, [rcx + r9 * 8] 

   
    mov     r12, r9
    inc     r12

inner_loop:
    cmp     r12, rdx
    jge     inner_done

    mov     r13, [rcx + r12 * 8]       
    cmp     r13, r11
    jge     skip_update
    mov     r11, r13               
    mov     r10, r12                   
skip_update:
    inc     r12                        
    jmp     inner_loop

inner_done:
    
    cmp     r10, r9
    je      no_swap

    mov     rax, [rcx + r9 * 8]        
    mov     rbx, [rcx + r10 * 8]    
    mov     [rcx + r9 * 8], rbx     
    mov     [rcx + r10 * 8], rax    

no_swap:
    inc     r9                         
    jmp     outer_loop

outer_done:
done:
    ret
asm_sortAnArray ENDP

asm_CompCharsArrays PROC
    cmp     rdx, r9
    jne     notequal
    xor     r10, r10

COMP_LOOP:
    cmp     r10, rdx
    jge     equalArrays

    movzx   eax, byte ptr [rcx + r10]
    movzx   r11d, byte ptr [r8 + r10]
    cmp     eax, r11d
    jne     notEqual

    inc     r10
    jmp     COMP_LOOP
    
equalArrays:
    mov     rax, 1
    ret

notEqual: 
    xor     rax, rax
    ret
    
asm_CompCharsArrays ENDP

asm_sumFloat PROC
    addss xmm0, xmm1
    ret
asm_sumFloat ENDP

asm_move PROC
    mov rax, [rcx]
    add rax, [rcx + 8 * 2]
    mov [rcx], rax
    mov rax, [rcx + 8]
    add rax, [rcx + 8 * 3]
    mov [rcx + 8], rax
    ret

asm_move ENDP

END




























