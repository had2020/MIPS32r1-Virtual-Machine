#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
struct Registers {
  uint32_t zero;
  uint32_t at; // reserved (assembler)

  uint32_t fn_result0;
  uint32_t fn_result1;

  uint32_t fn_arg0;
  uint32_t fn_arg1;
  uint32_t fn_arg2;
  uint32_t fn_arg3;

  uint32_t temp0;
  uint32_t temp1;
  uint32_t temp2;
  uint32_t temp3;
  uint32_t temp4;
  uint32_t temp5;
  uint32_t temp6;
  uint32_t temp7;

  uint32_t stemp0;
  uint32_t stemp1;
  uint32_t stemp2;
  uint32_t stemp3;
  uint32_t stemp4;
  uint32_t stemp5;
  uint32_t stemp6;
  uint32_t stemp7;

  uint32_t temp8;
  uint32_t temp9;

  uint32_t kernel0;
  uint32_t kernel1;

  uint32_t globalptr;
  uint32_t stackptr;
  uint32_t stackframeptr;
  uint32_t returnaddr;
}; */

enum Opcode {
  // ALU
  ADD = 0,   // ADDSUB
  SUB = 1,   // ADDSUB
  MUL = 2,   // MUL
  MULT = 3,  // MUL
  MULTU = 4, // MUL
  DIV = 5,   // DIVREM
  DIVU = 6,  // DIVREM
  MOD = 7,   // DIVREM
  MODU = 8,  // DIVREM
  SLL = 9,   // SLL
  SRL = 10,  // SR
  SRA = 11,  // SR
  ROR = 12,  // SR
  SLT = 13,  // LT
  SLTU = 14, // LT
  AND = 15,  // BITWISE
  OR = 16,   // BITWISE
  XOR = 17,  // BITWISE
  NOR = 18,  // BITWISE
  CLZ = 19,  // CLO_CLZ
  CLO = 20,  // CLO_CLZ
  // Control FLow
  BEQ = 21,        // BRANCH
  BGEZ = 22,       // BRANCH
  BGTZ = 23,       // BRANCH
  BLEZ = 24,       // BRANCH
  BLTZ = 25,       // BRANCH
  BNE = 26,        // BRANCH
  Jump = 27,       // JUMP
  Jumpi = 28,      // JUMP
  JumpDirect = 29, // JUMP
  SYSCALL = 30,    // SYSCALL
  // Memory Op
  LB = 31,  // LOAD
  LBU = 32, // LOAD
  LH = 33,  // LOAD
  LHU = 34, // LOAD
  LW = 35,  // LOAD
  LWL = 36, // LOAD
  LWR = 37, // LOAD
  LL = 38,  // LOAD
  SB = 39,  // STORE
  SH = 40,  // STORE
  SW = 41,  // STORE
  SWL = 42, // STORE
  SWR = 43, // STORE
  SC = 44,  // STORE
  // Misc
  INS = 45,   // INS
  MADDU = 46, // MADDSUB
  MSUBU = 47, // MADDSUB
  MADD = 48,  // MADDSUB
  MSUB = 49,  // MADDSUB
  MEQ = 50,   // MOVCOND
  MNE = 51,   // MOVCOND
  WSBH = 52,  // WSBH
  EXT = 53,   // EXT
  TEQ = 54,   // TEQ
  SEXT = 55,  // SEXT

  // Syscall
  UNIMPL = 0xff,
};

enum FuncCodes {
  // R-Type / Special (Opcode 0) Function Codes
  OP_SLL = 0,
  OP_SRL = 2,
  OP_SRA = 3,
  OP_SLLV = 4,
  OP_SRLV = 6,
  OP_SRAV = 7,
  OP_JR = 8,
  OP_JALR = 9,
  OP_MOVZ = 10,
  OP_MOVN = 11,
  OP_SYSCALL = 12,
  OP_BREAK = 13,
  OP_SYNC = 15,
  OP_MFHI = 16,
  OP_MTHI = 17,
  OP_MFLO = 18,
  OP_MTLO = 19,
  OP_MULT = 24,
  OP_MULTU = 25,
  OP_DIV = 26,
  OP_DIVU = 27,
  OP_ADD = 32,
  OP_ADDU = 33,
  OP_SUB = 34,
  OP_SUBU = 35,
  OP_AND = 36,
  OP_OR = 37,
  OP_XOR = 38,
  OP_NOR = 39,
  OP_SLT = 42,
  OP_SLTU = 43,
  OP_TGE = 48,
  OP_TGEU = 49,
  OP_TLT = 50,
  OP_TLTU = 51,
  OP_TEQ = 52,
  OP_TNE = 54
};

int main() {
  /*
  uint8_t *sram = malloc(1000000);
  if (sram == NULL) {
    printf("Memory Allocation Failure!");
    return 1;
  }
  */

  // struct Registers regs = {0};

  static uint8_t sram[1000000] = {0};

  uint32_t regs[32] = {0};
  uint32_t epc; // TODO: exception return addr with interrupts every 1000 cycles

  uint32_t pc = 0; // set for program

  bool running = true;

  while (running) {

    // Note: I need to handle for three different types R,I, and J.

    uint32_t fetched; // fetched instruction
    memcpy(&fetched, &sram[pc], sizeof(uint32_t));

    // decode
    uint8_t opcode = fetched >> 26;
    uint8_t rs = (fetched & 0x3E00000) >> 21; // first source register operand
    uint8_t rt = (fetched & 0x1F0000) >> 16;  // second source register operand
    uint8_t rd = (fetched & 0xF800) >> 11;    // destination register operand
    uint8_t shamt = (fetched & 0x7C0) >> 6;   // shift amount in SLL SRL SRA
    uint8_t funct = (fetched & 0x3F);         // function code when R-type
    uint8_t imm = (fetched & 0xFFFF);         // 16 bit constanst imm
    uint8_t addr = (fetched & 0x3FFFFFF); // target instruction for direct jumps
    int16_t simm = (int16_t)imm;

    // execute
    switch (opcode) {
    // ALU
    case ADD: {

      break;
    }
    case SUB: {
      break;
    }
    case MUL: {
      break;
    }
    case MULT: {
      break;
    }
    case MULTU: {
      break;
    }
    case DIV: {
      break;
    }
    case DIVU: {
      break;
    }
    case MOD: {
      break;
    }
    case MODU: {
      break;
    }
    case SLL: {
      break;
    }
    case SRL: {
      break;
    }
    case SRA: {
      break;
    }
    case ROR: {
      break;
    }
    case SLT: {
      break;
    }
    case SLTU: {
      break;
    }
    case AND: {
      break;
    }
    case OR: {
      break;
    }
    case XOR: {
      break;
    }
    case NOR: {
      break;
    }
    case CLZ: {
      break;
    }
    case CLO: {
      break;
    }

    // Control Flow
    case BEQ: {
      break;
    }
    case BGEZ: {
      break;
    }
    case BGTZ: {
      break;
    }
    case BLEZ: {
      break;
    }
    case BLTZ: {
      break;
    }
    case BNE: {
      break;
    }
    case Jump: {
      break;
    }
    case Jumpi: {
      break;
    }
    case JumpDirect: {
      break;
    }
    case SYSCALL: {
      break;
    }

    // Memory Op
    case LB: {
      break;
    }
    case LBU: {
      break;
    }
    case LH: {
      break;
    }
    case LHU: {
      break;
    }
    case LW: {
      break;
    }
    case LWL: {
      break;
    }
    case LWR: {
      break;
    }
    case LL: {
      break;
    }
    case SB: {
      break;
    }
    case SH: {
      break;
    }
    case SW: {
      break;
    }
    case SWL: {
      break;
    }
    case SWR: {
      break;
    }
    case SC: {
      break;
    }

    // Misc
    case INS: {
      break;
    }
    case MADDU: {
      break;
    }
    case MSUBU: {
      break;
    }
    case MADD: {
      break;
    }
    case MSUB: {
      break;
    }
    case MEQ: {
      break;
    }
    case MNE: {
      break;
    }
    case WSBH: {
      break;
    }
    case EXT: {
      break;
    }
    case TEQ: {
      break;
    }
    case SEXT: {
      break;
    }

    // Syscall
    case UNIMPL: {
      break;
    }

    default: {
      running = false;
      printf("Stopped running on unknown instruction!");
    }
    }

    // ALU

    pc += 4;
    regs[0] = 0; //

    printf("PC: %u", pc);
    break; // for debugging just one cpu cycle.
  }

  // free(sram);
  return 0;
}
