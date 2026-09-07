#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct registers {
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
};

int main() {
  printf("test");

  uint8_t *sram = malloc(1000000);

  return 0;
}
