#ifndef INSTRUCTION_HANDLERS_H
#define INSTRUCTION_HANDLERS_H

#include <stdint.h>
#include "shell.h"

typedef struct {
    uint32_t op;
    char* name;
    void (*function) (uint32_t instruction);
} instruction_t;

#define RD(instr) (instr & 0x1F)              // bits [4:0]
#define RN(instr) ((instr >> 5) & 0x1F)       // bits [9:5]
#define IMM_ADDS(instr) ((instr >> 10) & 0x3FF) // bits [21:10] (10 bits)
#define RM(instr) ((instr >> 16) & 0x1F)      // bits [20:16]
#define IMM9(instr) ((instr >> 12) & 0x1FF) // bits [20-12]
#define IMM6(instr) ((instr >> 10) & 0x3f) // bits [15-10]
#define IMM16(instr) ((instr >> 5) & 0xFFFF) // bits [20-5]
#define IMM12(instr) ((instr >> 10) & 0xfff) // bits [21-10]
#define IMM19(instr) ((instr >> 5) & 0x7FFFF) // bits [23-5]
#define BCOND_MASK(instr) (instr & 0xf)

#define OPCODE_MASK_31_21 0xFFE00000
#define OPCODE_MASK_31_22 0xFFC00000
#define OPCODE_MASK_31_24 0xFF000000
#define OPCODE_MASK_31_26 0xFC000000
#define OPCODE_MASK_31_10 0xFFFFFC00

void handle_adds_immediate(uint32_t instr);
void handle_add_immediate(uint32_t instr);
void handle_adds_register(uint32_t instr);
void handle_add_register(uint32_t instr);
void handle_subs_register(uint32_t instr);
void handle_ands_register(uint32_t instr);
void handle_stur(uint32_t instr);
void handle_sturb(uint32_t instr);
void handle_sturh(uint32_t instr);
void handle_mul(uint32_t instr);
void handle_orr(uint32_t instr);
void handle_xor(uint32_t instr);
void handle_movz(uint32_t instr);
void handle_ldur(uint32_t instr);
void handle_ldurb(uint32_t instr);
void handle_ldurh(uint32_t instr);
void handle_hlt(uint32_t instr);
void handle_lsl_lsr(uint32_t instr);
void handle_sub_immediate(uint32_t instr);
void handle_cbz(uint32_t instr);
void handle_cbnz(uint32_t instr);
void handle_bcond(uint32_t instr);
void handle_btarget(uint32_t instr);
void handle_br(uint32_t instr);

extern instruction_t op_31_21[15];
extern instruction_t op_31_22[2];
extern instruction_t op_31_24[5];
extern instruction_t btarget;
extern instruction_t br;

#endif