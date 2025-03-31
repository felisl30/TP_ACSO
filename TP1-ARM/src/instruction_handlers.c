#include "instruction_handlers.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

instruction_t op_31_21[15] = {
    {0xD4400000, "HLT", handle_hlt},
    {0x8B200000, "ADD ext", handle_add_register},
    {0xAB000000, "ADDS EXT", handle_adds_register},
    {0xEB000000, "SUBS", handle_subs_register},
    {0xEA000000, "ANDS", handle_ands_register},
    {0xF8000000, "STUR", handle_stur},
    {0x38000000, "STURB", handle_sturb},
    {0x78000000, "STURH", handle_sturh},
    {0x9B000000, "MUL", handle_mul},
    {0xAA000000, "ORR", handle_orr},
    {0xCA000000, "EOR", handle_xor},
    {0xD2800000, "MOVZ", handle_movz},
    {0xF8400000, "LDUR", handle_ldur},
    {0x38400000, "LDURB", handle_ldurb},
    {0x78400000, "LDURH", handle_ldurh},
};

instruction_t op_31_22[2] = {
    {0xd3400000, "LSL/LSR", handle_lsl_lsr},
    {0xf1000000, "CMP/SUBIS", handle_sub_immediate},
};

instruction_t op_31_24[5] = {
    {0xb4000000, "CBZ", handle_cbz},
    {0xb5000000, "CBNZ", handle_cbnz},
    {0x54000000, "B.COND", handle_bcond},
    {0xB1000000, "ADDS IMM", handle_adds_immediate},
    {0x91000000, "ADD IMM", handle_add_immediate},

};

instruction_t btarget = {0x14000000, "B_TARGET", handle_btarget};
instruction_t br = {0xD61F0000, "BR", handle_br};

void handle_adds_immediate(uint32_t instr) {
    uint16_t imm12 = IMM_ADDS(instr);
    uint8_t rd = RD(instr);
    uint8_t rn = RN(instr);
    uint8_t shift = (instr >> 22) & 0x3;

    if (shift == 0x1){
        imm12 = imm12 << 12;
    }

    uint64_t expected = CURRENT_STATE.REGS[rn] + imm12;

    NEXT_STATE.REGS[rd] = expected;
    NEXT_STATE.FLAG_Z = (expected == 0);
    NEXT_STATE.FLAG_N = (expected >> 63) & 1ULL;
}

void handle_add_immediate(uint32_t instr) {
    uint16_t imm12 = IMM_ADDS(instr);
    uint8_t rd = RD(instr);
    uint8_t rn = RN(instr);
    uint8_t shift = (instr >> 22) & 0x3;
    
    if (shift == 0x1){
        imm12 = imm12 << 12;
    }


    uint64_t expected = CURRENT_STATE.REGS[rn] + imm12;

    NEXT_STATE.REGS[rd] = expected;

}


void handle_adds_register(uint32_t instr){
uint8_t rd = RD(instr);
uint8_t rn = RN(instr);
uint8_t rm = RM(instr);

int64_t signed_rn = (int64_t)CURRENT_STATE.REGS[rn];
int64_t signed_rm = (int64_t)CURRENT_STATE.REGS[rm];
int64_t signed_expected = signed_rn + signed_rm;


NEXT_STATE.REGS[rd] = signed_expected;
NEXT_STATE.FLAG_Z = (signed_expected == 0);
NEXT_STATE.FLAG_N = (signed_expected < 0);

}


void handle_add_register(uint32_t instr){
uint8_t rd = RD(instr);
uint8_t rn = RN(instr);
uint8_t rm = RM(instr);

int64_t signed_rn = (int64_t)CURRENT_STATE.REGS[rn];
int64_t signed_rm = (int64_t)CURRENT_STATE.REGS[rm];
int64_t signed_expected = signed_rn + signed_rm;

NEXT_STATE.REGS[rd] = signed_expected;


}



void handle_subs_register(uint32_t instr){
uint8_t rd = RD(instr);
uint8_t rn = RN(instr);
uint8_t rm = RM(instr);

int64_t signed_rn = (int64_t)CURRENT_STATE.REGS[rn];
int64_t signed_rm = (int64_t)CURRENT_STATE.REGS[rm];
int64_t signed_expected = signed_rn - signed_rm;

uint64_t expected = (uint64_t)signed_expected;

NEXT_STATE.FLAG_Z = (signed_expected == 0);

NEXT_STATE.FLAG_N = (signed_expected < 0);
if (rd != 31) { 
    NEXT_STATE.REGS[rd] = expected;
    }
}


void handle_ands_register(uint32_t instr){
uint8_t rd = RD(instr);
uint8_t rn = RN(instr);
uint8_t rm = RM(instr);

uint64_t rm_val = CURRENT_STATE.REGS[rm];

uint64_t expected = CURRENT_STATE.REGS[rn] & rm_val;
NEXT_STATE.REGS[rd] = expected;


NEXT_STATE.FLAG_N = (expected >> 63) & 1ULL;

NEXT_STATE.FLAG_Z = (expected == 0);


}


void handle_stur(uint32_t instr){
uint8_t rt = RD(instr);  
uint8_t rn = RN(instr);  
int16_t imm9 = IMM9(instr); 


if (imm9 & 0x100) { 
    imm9 |= ~((1 << 9) - 1); 
}


uint64_t address = CURRENT_STATE.REGS[rn] + imm9;


uint64_t value_to_store = CURRENT_STATE.REGS[rt];
mem_write_32(address, value_to_store & 0xFFFFFFFF);        
mem_write_32(address + 4, (value_to_store >> 32) & 0xFFFFFFFF); 
}


void handle_sturb(uint32_t instr){
uint8_t rt = RD(instr);  
uint8_t rn = RN(instr);  
int16_t imm9 = IMM9(instr); 


if (imm9 & 0x100) {
    imm9 |= ~((1 << 9) - 1);
}


uint64_t address = CURRENT_STATE.REGS[rn] + imm9;

    
uint32_t word = mem_read_32(address & ~0x3);
uint32_t byte_pos = address & 0x3;
uint32_t byte_mask = 0xFF << (byte_pos * 8);
uint8_t byte_to_store = CURRENT_STATE.REGS[rt] & 0xFF;
word = (word & ~byte_mask) | (byte_to_store << (byte_pos * 8));

mem_write_32(address & ~0x3, word);

}


void handle_sturh(uint32_t instr){
uint8_t wt = RD(instr);  
uint8_t xn = RN(instr); 

int16_t imm9 = IMM9(instr); 

if (imm9 & 0x100) { 
    imm9 |= ~((1 << 9) - 1); 
}


uint64_t address = CURRENT_STATE.REGS[xn] + imm9;


uint16_t halfword_to_store = CURRENT_STATE.REGS[wt] & 0xFFFF;


uint32_t existing_word = mem_read_32(address & ~0x3); 
uint32_t aligned_offset = address & 0x3; 
uint32_t mask = 0xFFFF << (aligned_offset * 8);
uint32_t new_word = (existing_word & ~mask) | (halfword_to_store << (aligned_offset * 8));
mem_write_32(address & ~0x3, new_word);

}


void handle_mul(uint32_t instr){
uint8_t rd = RD(instr); 
uint8_t rn = RN(instr); 
uint8_t rm = RM(instr); 


uint64_t result = CURRENT_STATE.REGS[rn] * CURRENT_STATE.REGS[rm];


NEXT_STATE.REGS[rd] = result;

}


void handle_orr(uint32_t instr){
uint8_t rd = RD(instr);
uint8_t rn = RN(instr);
uint8_t rm = RM(instr);
uint8_t imm6 = IMM6(instr);


uint64_t expected = CURRENT_STATE.REGS[rn] | (CURRENT_STATE.REGS[rm] << imm6);


NEXT_STATE.REGS[rd] = expected;
}

void handle_xor(uint32_t instr){
uint8_t rd = RD(instr);
uint8_t rn = RN(instr);
uint8_t rm = RM(instr);


uint64_t rm_val = CURRENT_STATE.REGS[rm];

uint64_t expected = CURRENT_STATE.REGS[rn] ^ rm_val;

NEXT_STATE.REGS[rd] = expected;
}


void handle_movz(uint32_t instr){
uint8_t rd = RD(instr);
uint16_t imm16 = IMM16(instr);

uint64_t expected = ((uint64_t)imm16);

NEXT_STATE.REGS[rd] = expected;


}


void handle_ldur(uint32_t instr){
uint8_t rn = RN(instr);
uint64_t base = CURRENT_STATE.REGS[rn];
uint64_t offset = (instr >> 12) & 0x1ff;
uint64_t effective_addr = base + offset;

uint32_t lower = mem_read_32(effective_addr);
uint32_t upper = mem_read_32(effective_addr + 4);


NEXT_STATE.REGS[RD(instr)] = ((uint64_t)upper << 32) | lower;

}

void handle_ldurb(uint32_t instr){
uint8_t rt = RD(instr);
uint8_t rn = RN(instr);
uint16_t imm9 = IMM9(instr);

if (imm9 & 0x100) {
    imm9 |= ~((1 << 9) - 1); 
}

uint64_t address = CURRENT_STATE.REGS[rn] + imm9;
    

uint32_t word = mem_read_32(address & ~0x3); 
uint8_t byte_value = (word >> ((address & 0x3) * 8)) & 0xFF; 

NEXT_STATE.REGS[rt] = (uint64_t)byte_value;
}


void handle_ldurh(uint32_t instr){
uint8_t rt = RD(instr);
uint8_t rn = RN(instr);
uint16_t imm9 = IMM9(instr); 

if (imm9 & 0x100) {
    imm9 |= ~((1 << 9) - 1); 
}

uint64_t address = CURRENT_STATE.REGS[rn] + imm9;

uint32_t word = mem_read_32(address & ~0x3); 
uint16_t halfword_value = (word >> ((address & 0x3) * 8)) & 0xFFFF; 

NEXT_STATE.REGS[rt] = (uint64_t)halfword_value;

}


void handle_hlt(uint32_t instr){
RUN_BIT = 0; 
}


void handle_lsl_lsr(uint32_t instr){

uint8_t rd = RD(instr);
uint8_t rn = RN(instr);
uint8_t immr = (instr >> 16) & 0x3f;
uint8_t imms = (instr >> 10) & 0x3f;

uint8_t shift_amount = (64 - immr) % 64;

uint64_t result;
if(imms != 0x3f){
    result = CURRENT_STATE.REGS[rn] << shift_amount;
}
else{
    result = CURRENT_STATE.REGS[rn] >> immr;
}

NEXT_STATE.REGS[rd] = result;
}


void handle_sub_immediate(uint32_t instr){
uint8_t rd = RD(instr);
uint8_t rn = RN(instr);
uint16_t imm12 = IMM12(instr);

int64_t signed_rn = (int64_t)CURRENT_STATE.REGS[rn];
int64_t signed_expected = signed_rn - imm12;

uint64_t expected = (uint64_t)signed_expected;

NEXT_STATE.FLAG_Z = (signed_expected == 0);
NEXT_STATE.FLAG_N = (signed_expected < 0);    


if (rd != 31) {
NEXT_STATE.REGS[rd] = (uint64_t)expected;
}
}


void handle_cbz(uint32_t instr){
uint8_t rt = RD(instr);
uint32_t imm19 = IMM19(instr);


if (imm19 & (1 << 18)) { 
    imm19 |= ~((1 << 19) - 1); 
}

int64_t offset = imm19 << 2;


if (CURRENT_STATE.REGS[rt] == 0) {
    NEXT_STATE.PC = CURRENT_STATE.PC + offset;
}
}


void handle_cbnz(uint32_t instr){
uint8_t rt = RD(instr);
uint8_t imm19 = IMM19(instr);

if (imm19 & (1 << 18)) { 
    imm19 |= ~((1 << 19) - 1); 
}

int64_t offset = imm19 << 2;

if (CURRENT_STATE.REGS[rt] != 0) {
    NEXT_STATE.PC = CURRENT_STATE.PC + offset;
}    
}

void handle_bcond(uint32_t instr){
uint8_t btype = BCOND_MASK(instr);
switch (btype){
    case 0xb: //b.lt
    {
        int32_t imm19 = IMM19(instr);
        if (imm19 & (1 << 18)) {  
            imm19 |= ~((1 << 19) - 1); 
        }

        int64_t offset = imm19 << 2;

        if (CURRENT_STATE.FLAG_N) {
            NEXT_STATE.PC = CURRENT_STATE.PC + offset;
        }
        break;
    }

case 0x1: //bne
    {
        int32_t imm19 = IMM19(instr);

        if (imm19 & (1 << 18)) {
            imm19 |= ~((1 << 19) - 1);
        }

        int64_t offset = imm19 << 2;

        if (!CURRENT_STATE.FLAG_Z) {
            NEXT_STATE.PC = CURRENT_STATE.PC + offset;
        }
        break;
    }

case 0xc: //bgt
    {
        int32_t imm19 = IMM19(instr);

        if (imm19 & (1 << 18)) {
            imm19 |= ~((1 << 19) - 1);
        }

        int64_t offset = imm19 << 2;

        if (!CURRENT_STATE.FLAG_Z && !CURRENT_STATE.FLAG_N) { 
            NEXT_STATE.PC = CURRENT_STATE.PC + offset;
        }
        break;
    }
case 0xa: //bge
    {
        int32_t imm19 = IMM19(instr);

        if (imm19 & (1 << 18)) {
            imm19 |= ~((1 << 19) - 1);
        }

        int64_t offset = imm19 << 2;
        
        if (!CURRENT_STATE.FLAG_N) { 
            NEXT_STATE.PC = CURRENT_STATE.PC + offset;
        }
        break;
    }
case 0xd: //ble
    {
        int32_t imm19 = IMM19(instr);

        if (imm19 & (1 << 18)) {
            imm19 |= ~((1 << 19) - 1);
        }

        int64_t offset = imm19 << 2;
        
        if ( CURRENT_STATE.FLAG_Z || CURRENT_STATE.FLAG_N) { 
            NEXT_STATE.PC = CURRENT_STATE.PC + offset;
        }
        break;
    }
case 0x0: // beq
    {
        int32_t imm19 = IMM19(instr);

    
        if (imm19 & (1 << 18)) {  
            imm19 |= ~((1 << 19) - 1); 
        }
    
        int64_t offset = imm19 << 2;

        if (CURRENT_STATE.FLAG_Z) {
            NEXT_STATE.PC = CURRENT_STATE.PC + offset;
        }
        break;

    }
}
}


void handle_btarget(uint32_t instr){
int32_t imm19 = IMM19(instr); 

if (imm19 & (1 << 18)) {  
    imm19 |= ~((1 << 19) - 1); 
}

int64_t offset = imm19 << 2;

NEXT_STATE.PC = CURRENT_STATE.PC + offset;
}


void handle_br(uint32_t instr){
uint8_t rn = RN(instr); 

NEXT_STATE.PC = CURRENT_STATE.REGS[rn];
}
