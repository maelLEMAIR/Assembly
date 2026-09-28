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
    ; rcx = tableau int64, rdx = n
    cmp     rdx, 1
    jle     done

    xor     r8, r8                  ; i

outer_loop:
    lea     rax, [rdx - 1]
    cmp     r8, rax
    jge     done

    mov     r10, r8                 ; indice du min
    mov     r11, [rcx + r8 * 8]     ; valeur du min
    lea     r9, [r8 + 1]            ; j

inner_loop:
    cmp     r9, rdx
    jge     inner_done
    mov     rax, [rcx + r9 * 8]
    cmp     rax, r11
    jge     skip_update
    mov     r11, rax
    mov     r10, r9
skip_update:
    inc     r9
    jmp     inner_loop

inner_done:
    cmp     r10, r8
    je      no_swap
    mov     rax, [rcx + r8 * 8]     ; ancien a[i]
    mov     [rcx + r10 * 8], rax    ; a[min] = ancien a[i]
    mov     [rcx + r8 * 8], r11     ; a[i]   = min
no_swap:
    inc     r8
    jmp     outer_loop

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
    ; rcx = entity : pos += vel (2 x int32)
    movq    xmm0, qword ptr [rcx]
    movq    xmm1, qword ptr [rcx + 16]
    paddd   xmm0, xmm1
    movq    qword ptr [rcx], xmm0
    ret
asm_move ENDP

asm_checkCell PROC
    ; rcx = grid, rdx = pos (int32[2]), r8b = c, r9 = stride
    mov     r10, r9
    sub     r10, 24
    movsxd  r11, dword ptr [rdx]        ; x
    movsxd  rdx, dword ptr [rdx + 4]    ; y
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
    xor     eax, eax
done:
    ret
asm_checkCell ENDP

asm_attack PROC
    ; rcx = player, rdx = monster
    mov     eax,  [rcx + 36]    ; PV joueur
    mov     r8d,  [rcx + 40]    ; AT joueur
    mov     r9d,  [rdx + 36]    ; PV monstre
    mov     r10d, [rdx + 40]    ; AT monstre
    sub     r9d, r8d
    sub     eax, r10d
    mov     [rcx + 36], eax
    mov     [rdx + 36], r9d
    ret
asm_attack ENDP

asm_getEntity PROC
    ; rcx = entity, rdx = pos (int32[2])
    mov     eax, [rcx]
    mov     r8d, [rcx + 4]
    cmp     eax, [rdx]
    jne     not_equal
    cmp     r8d, [rdx + 4]
    jne     not_equal
    mov     eax, 1
    ret
not_equal:
    xor     eax, eax
    ret
asm_getEntity ENDP

asm_isDead PROC
    cmp     dword ptr [rcx + 36], 0
    jg      alive
    mov     eax, 1
    ret
alive:
    xor     eax, eax
    ret
asm_isDead ENDP

asm_checkDoor PROC
    ; rcx = Door**, edx = count, r8 = Entity*, r9 = stride
    mov     r10d, [r8]          ; player x (int32)
    mov     r11d, [r8 + 4]      ; player y (int32)
    xor     eax, eax          

COMP_loop:
    cmp     eax, edx            
    jae     not_found

    mov     r8d, eax
    imul    r8, r9              
    mov     r8, [rcx + r8]      

    cmp     r10d, [r8]          
    jne     next
    cmp     r11d, [r8 + 4]    
    je      done                

next:
    inc     eax
    jmp     COMP_loop

not_found:
    mov     eax, -1
done:
    ret
asm_checkDoor ENDP

asm_heal PROC
    ; rcx = player, edx = numberOfHeal
    mov     eax, [rcx + 36]
    add     eax, edx
    mov     r8d, [rcx + 32]     ; PV max
    cmp     eax, r8d
    jle     done
    mov     eax, r8d
done:
    mov     [rcx + 36], eax
    ret
asm_heal ENDP
END