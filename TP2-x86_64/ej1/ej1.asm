; /** defines bool y puntero **/
%define NULL 0
%define TRUE 1
%define FALSE 0

section .data

section .text

global string_proc_list_create_asm
global string_proc_node_create_asm
global string_proc_list_add_node_asm
global string_proc_list_concat_asm

; FUNCIONES auxiliares que pueden llegar a necesitar:
extern malloc
extern free
extern str_concat


string_proc_list_create_asm:
    ; malloc(sizeof(string_proc_list)) = malloc(16)
    mov rdi, 16
    call malloc
    test rax, rax
    je .return_null

    ; *(string_proc_list *)rax
    ; rax -> list
    ; list->first = NULL
    mov qword [rax], 0
    ; list->last = NULL
    mov qword [rax + 8], 0
    ret

.return_null:
    mov rax, 0
    ret

    

; Entrada:
; rdi = uint8_t type
; rsi = char* hash

string_proc_node_create_asm:
    ; malloc(sizeof(string_proc_node)) = malloc(32)
    mov rdi, 32
    call malloc
    test rax, rax
    je .return_null

    ; rax contiene el puntero al nodo
    ; nodo->type = rdi (original)
    mov [rax], dil              ; dil = parte baja de rdi (uint8_t)
    ; nodo->hash = rsi
    mov [rax + 8], rsi
    ; nodo->previous = NULL
    mov qword [rax + 16], 0
    ; nodo->next = NULL
    mov qword [rax + 24], 0
    ret

.return_null:
    mov rax, 0
    ret

    
    

; rdi = list
; rsi = type
; rdx = hash

string_proc_list_add_node_asm:
    ; llama a string_proc_node_create(type, hash)
    movzx rdi, sil    ; type
    mov rsi, rdx    ; hash
    call string_proc_node_create_asm
    test rax, rax
    je .ret

    ; rax = new_node
    ; rdi = list

    ; new_node->previous = list->last
    mov rcx, [rdi + 8]
    mov [rax + 16], rcx

    ; if (!list->last)
    test rcx, rcx
    je .first_null

    ; list->last->next = new_node
    mov [rcx + 24], rax
    jmp .set_last

.first_null:
    ; list->first = new_node
    mov [rdi], rax

.set_last:
    ; list->last = new_node
    mov [rdi + 8], rax

.ret:
    ret

 

; rdi = list
; rsi = type
; rdx = hash

string_proc_list_concat_asm:
    ; guarda a = hash
    mov r8, rdx        ; r8 = a (resultado actual)
    mov r9b, sil       ; r9b = type
    mov r10, [rdi]     ; r10 = node = list->first

.loop:
    test r10, r10
    je .done

    ; compara node->type == type
    mov al, [r10]
    cmp al, r9b
    jne .next

    ; b = node->hash
    mov rsi, r8        ; a
    mov rdi, [r10 + 8] ; b = node->hash
    call str_concat
    ; resultado en rax
    ; si a != hash original (r8 != rdx), liberar
    cmp r8, rdx
    je .no_free
    mov rdi, r8
    call free

.no_free:
    mov r8, rax        ; a = result

.next:
    ; node = node->next
    mov r10, [r10 + 24]
    jmp .loop

.done:
    mov rax, r8        ; devuelve a
    ret

