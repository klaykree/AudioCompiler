#pragma once
#include <Windows.h>

struct BYTE_ARRAY InstToOPCode(struct INSTRUCTION_ARRAY* Program, size_t Index);

struct BYTE_ARRAY SetEBP(DWORD RVA);

struct BYTE_ARRAY GetPrintFormats();

int OPCodeSize(short Instruction);