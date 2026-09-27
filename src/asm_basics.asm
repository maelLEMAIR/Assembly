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
    ; rcx = Entity
    ; int
    mov rax, [rcx]
    add rax, [rcx + 8 * 2]
    mov [rcx], rax
    mov rax, [rcx + 8]
    add rax, [rcx + 8 * 3]
    mov [rcx + 8], rax
    ret

asm_move ENDP

asm_movef PROC
    ; rcx = Entity
    ; float
    movss xmm0, DWORD PTR [rcx]
    addss xmm0, DWORD PTR [rcx + 8]
    movss DWORD PTR [rcx], xmm0

    movss xmm0, DWORD PTR [rcx + 4]
    addss xmm0, DWORD PTR [rcx + 12]
    movss DWORD PTR [rcx + 4], xmm0

    ret
asm_movef ENDP

asm_checkCell PROC
    ; rcx = grid, rdx = pos, r8b = c, r9 = stride
    mov     r10, r9
    sub     r10, 24                 
    mov     r11, [rdx]             
    mov     rdx, [rdx + 8]          
    mov     rax, [rcx + r10]       
    imul    rdx, r9                
    add     rax, rdx          
    mov     rax, [rax + r10]
    movzx   r10, byte ptr [rax + r11]
    cmp     r10b, r8b
    jne     not_equal
    mov     rax, 1
    jmp     done

not_equal:
    mov     rax, 0

done:
    ret
asm_checkCell ENDP

asm_attack PROC
    ; rcx = player, rdx = monster

    mov rax, [rcx + 8 * 5]  ;   <--- PV joueur
    mov r8, [rcx + 8 * 6]   ;   <--- AT joueur
    
    mov r9, [rdx + 8 * 5]   ;   <--- PV monster
    mov r10, [rdx + 8 * 6]  ;   <--- AT monster

    sub r9, r8
    sub rax, r10

    mov [rcx + 8 * 5], rax
    mov [rdx + 8 * 5], r9
    ret

asm_attack ENDP

asm_getEntity PROC
    ; rcx = entity, rdx = pos
    mov rax, [rcx]
    mov r8, [rcx + 8]
    
    mov r9, [rdx]
    mov r10, [rdx + 8]

    cmp rax, r9
    jne not_equal
    cmp r8, r10
    jne not_equal
    mov rax, 1 
    jmp done

not_equal:
    mov rax, 0

done:
    ret
asm_getEntity ENDP

asm_isDead PROC
    ; rcx = entity
    mov rax, [rcx + 8 * 5]
    cmp rax, 0
    jg  alive              
    mov rax, 1           
    jmp done

alive:
    xor rax, rax

done:
    ret
asm_isDead ENDP

asm_checkDoor PROC
    ; rcx = doors (Door**), rdx = count, r8 = pPlayer, r9 = stride

    mov r10, [r8]        
    mov r11, [r8 + 8]     

    xor rax, rax         

COMP_loop:
    cmp rax, rdx
    jae not_found

    mov r8, rax
    imul r8, r9   
    mov r8, [rcx + r8]

    cmp r10d, [r8]      
    jne not_equal
    cmp r11d, [r8 + 4]
    je equal

not_equal:
    inc rax
    jmp COMP_loop

equal:
    jmp done             

not_found:
    mov rax, -1

done:
    ret
asm_checkDoor ENDP

END