#pragma once
#include <limits.h>
#include <assert.h>
#include <stdlib.h>
#include <Windows.h>

#define AddImm 4093
#define SubImm 4094
#define MulImm 4095
#define DivImm 4096
#define AddVar 4097
#define SubVar 4098
#define MulVar 4099
#define DivVar 4100

#define PrintVar 8191
#define PrintVarLn 8192
#define PrintLn 8193

#define CreateVar 16383
#define AssignFromImmediate 16384
#define AssignFromVariable 16385

#define IfImmEql 24574
#define IfVarEql 24575
#define ForImm 24576
#define ForVar 24577
#define EndIf 24578
#define EndFor 24579
#define EndProgram 24580

typedef struct INSTRUCTION
{
	short Operands[8];
}INSTRUCTION;

typedef struct INSTRUCTION_ARRAY
{
	INSTRUCTION* Instructions;
	size_t Length;
}INSTRUCTION_ARRAY;

typedef struct BYTE_ARRAY
{
	BYTE* Array;
	size_t Length;
}BYTE_ARRAY;

typedef struct ProgramTextData
{
	BYTE* Text;
	BYTE* Data;
}ProgramTextData;

BOOL ScopeStarter(short Instruction);

BOOL ScopeEnder(short Instruction);

//Returns size of variable to create
//returns zero if not an instruction to create a variable
char VariableCreator(short Instruction);

short OperandsInInstruction(short Instruction);

DWORD GetIdentifierRVA(INSTRUCTION_ARRAY* Program, size_t StartIndex, short Identifier);

INSTRUCTION_ARRAY ConvertProgram(short* Program, size_t ProgramLength);

void CreateDataSection(INSTRUCTION_ARRAY* Instructions, BYTE* Data);

void CreateTextSection(INSTRUCTION_ARRAY* Instructions, BYTE* Text);

ProgramTextData ParseProgram(short* Program, size_t ProgramLength);

DWORD DistToScopeEnd(INSTRUCTION_ARRAY* Program, size_t StartIndex);

DWORD DistToScopeStart(INSTRUCTION_ARRAY* Program, size_t StartIndex);