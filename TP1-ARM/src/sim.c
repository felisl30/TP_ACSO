#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "shell.h"
#include "instruction_handlers.h"

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. 
     */
    uint32_t instr = mem_read_32(CURRENT_STATE.PC);

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    uint32_t opcode = instr & OPCODE_MASK_31_21;

    for (int i = 0; i<14; i++){
        if (opcode == op_31_21[i].op){
            op_31_21[i].function(instr);
            return;
        }
    }

    opcode = OPCODE_MASK_31_22 & instr;

    for (int i = 0; i<2; i++){
        if (opcode == op_31_22[i].op){
            op_31_22[i].function(instr);
            return;
        }
    }

    opcode = OPCODE_MASK_31_24 & instr;

    for (int i = 0; i<5; i++){
        if (opcode == op_31_24[i].op){
            op_31_24[i].function(instr);
            return;
        }
    }

    opcode = OPCODE_MASK_31_26 & instr;
    if (opcode == btarget.op){
        btarget.function(instr);
        return;
    }

    opcode = OPCODE_MASK_31_10 & instr;
    if (opcode == br.op){
        br.function(instr);
        return;
    }
}