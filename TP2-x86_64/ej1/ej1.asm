; /** defines bool y puntero **/
%define NULL 0
%define TRUE 1
%define FALSE 0
%define SIZE_LIST  16
%define SIZE_NODE  32

section .data

section .text

global string_proc_list_create_asm
global string_proc_node_create_asm
global string_proc_list_add_node_asm
global string_proc_list_concat_asm
extern string_proc_list_concat

; FUNCIONES auxiliares que pueden llegar a necesitar:
extern malloc
extern free
extern str_concat


string_proc_list_create_asm:
    mov     edi, SIZE_LIST
    call    malloc
    cmp     rax, 0
    jnz     .inicializar_lista   
    jmp     .fin_lista           

.inicializar_lista:
    mov     qword [rax+8], 0     ; list->last  = NULL
    mov     qword [rax],   0     ; list->first = NULL

.fin_lista:
    ret

string_proc_node_create_asm:
    movzx   edx, dil            ; edx = tipo
    mov     rcx, rsi            ; rcx = ptr hash
    push    rcx                 
    push    rdx                 
    mov     edi, SIZE_NODE
    call    malloc
    pop     rdx                 
    pop     rcx                 
    test    rax, rax
    je      .fin_nodo

    xor     r8, r8              ; r8 = 0
    mov     [rax+8], r8         ; primero node->prev = NULL
    mov     [rax],   r8         ; luego node->next = NULL
    mov     byte [rax+16], dl   ; node->type = edx
    mov     [rax+24], rcx       ; node->hash = hash ptr

.fin_nodo:
    ret

string_proc_list_add_node_asm:
    push    rdi                 ; guarda lista
    movzx   edi, sil            ; edi = type
    mov     rsi, rdx            ; rsi = hash ptr
    call    string_proc_node_create_asm
    pop     r8                  ; r8 = lista ptr

    test    rax, rax
    je      .fin_agregar        ; si NULL, fin

    mov     rcx, [r8+8]         ; rcx = lista->last
    test    rcx, rcx
    je      .lista_vacia        ; si lista vacía, ir a vacía

    mov     [rcx],    rax       ; last->next = new
    mov     [rax+8],  rcx       ; new->prev  = last
    mov     [r8+8],  rax        ; lista->last = new
    jmp     .fin_agregar

.lista_vacia:
    mov     [r8+8],  rax        ; lista->last  = new
    mov     [r8],    rax        ; lista->first = new

.fin_agregar:
    ret

string_proc_list_concat_asm: ; no la pude hacer andar, probe con jmp string_proc_list_concat y funciona
    push    rbp
    mov     rbp, rsp
    push    rbx
    push    r12
    push    r13
    push    r14
    push    r15

    ; Parámetros:
    ; rdi = list
    ; sil = type
    ; rdx = hash

    mov     r12, rdi            ; r12 = list
    movzx   r13, sil            ; r13 = type (ampliado a 64 bits)
    mov     r14, rdx            ; r14 = hash inicial (a = hash)
    
    mov     r15, [r12]          ; r15 = node = list->first

.bucle:
    test    r15, r15            ; Comprobar si node == NULL
    jz      .fin                ; Si es NULL, terminar

    movzx   rbx, byte [r15+16]  ; rbx = node->type
    cmp     rbx, r13            ; Comparar node->type con type
    jne     .siguiente_nodo     ; Si no coincide, pasar al siguiente nodo

    ; Si coincide el tipo, concatenar los hashes
    mov     rdi, r14            ; rdi = a (primer argumento para str_concat)
    mov     rsi, [r15+24]       ; rsi = node->hash (segundo argumento)
    
    ; Verificar que node->hash no sea NULL
    test    rsi, rsi
    jz      .siguiente_nodo
    
    call    str_concat          ; Llamar a str_concat(a, b)
    
    ; Guardar temporalmente el resultado
    mov     rbx, rax            
    
    ; Verificar si debemos liberar a
    cmp     r14, rdx            
    je      .actualizar_a       ; Si son iguales, no liberar

    ; Verificar que r14 no sea NULL antes de liberar
    test    r14, r14
    jz      .actualizar_a
    
    ; Liberar la memoria de r14 solo si es diferente a rdx
    mov     rdi, r14            ; rdi = a (puntero a liberar)
    call    free                ; Liberar memoria anterior

.actualizar_a:
    mov     r14, rbx            ; a = result (desde el valor guardado en rbx)

.siguiente_nodo:
    mov     r15, [r15]          ; node = node->next
    jmp     .bucle              ; Siguiente iteración

.fin:
    mov     rax, r14            
    
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     rbx
    pop     rbp
    ret
