#include "Compiler.h"
#include "DynamicArray.h"
#include "OPCode.h"

const int InstructionCount = 21;
const short DefinedInstructions[][2] = { //Second value is the number of operands for the instruction
	{ AddImm, 3 }, { SubImm, 3 }, { MulImm, 3 }, { DivImm, 3 },
	{ AddVar, 2 }, { SubVar, 2 }, { MulVar, 2 }, { DivVar, 2 },
	{ PrintVar, 1 }, { PrintVarLn, 1 }, { PrintLn, 0 },
	{ CreateVar, 3 }, { AssignFromVariable, 2 }, { AssignFromImmediate, 3 },
	{ IfImmEql, 3 }, { IfVarEql, 2 }, { ForImm, 2 }, { ForVar, 1 }, { EndIf, 0 }, { EndFor, 0 }, { EndProgram, 0 }
};

BOOL ScopeStarter(short Instruction)
{
	if(Instruction == IfImmEql || Instruction == IfVarEql || Instruction == ForImm || Instruction == ForVar)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL ScopeEnder(short Instruction)
{
	if(Instruction == EndIf || Instruction == EndFor)
	{
		return TRUE;
	}

	return FALSE;
}

//Returns size of variable to create
//returns zero if not an instruction to create a variable
char VariableCreator(short Instruction)
{
	if(Instruction == CreateVar)
		return 4;

	return 0;
}

short OperandsInInstruction(short Instruction)
{
	for(int i = 0 ; i < InstructionCount ; ++i)
	{
		if(DefinedInstructions[i][0] == Instruction)
		{
			return DefinedInstructions[i][1];
		}
	}

	return -1;
}

DWORD GetIdentifierRVA(INSTRUCTION_ARRAY* Program, size_t StartIndex, short Identifier)
{
	size_t FoundIndex = -1; //Program array index of variable instruction
	BOOL IgnoreVarCreators = FALSE;

	//Traverse upwards through the program
	for(int i = StartIndex - 1 ; i >= 0 ; --i)
	{
		//If a scope is entered then ignore variable creators
		if(ScopeEnder(Program->Instructions[i].Operands[0]))
		{
			IgnoreVarCreators = TRUE;
			continue;
		}

		//If a scope is exited then look at variable creators
		if(ScopeStarter(Program->Instructions[i].Operands[0]))
		{
			IgnoreVarCreators = FALSE;
			continue;
		}

		if(IgnoreVarCreators == FALSE && VariableCreator(Program->Instructions[i].Operands[0]))
		{
			if(Program->Instructions[i].Operands[1] == Identifier)
			{
				FoundIndex = i;
				break;
			}
		}
	}

	assert("RVA for identifier not found" && FoundIndex != -1);

	//Begin at 9 because of the 9 bytes of print formats at the start of the data section
	DWORD RVA = 9;

	for(size_t i = 0 ; i < FoundIndex ; ++i)
	{
		char VariableSize = VariableCreator(Program->Instructions[i].Operands[0]);
		if(VariableSize)
		{
			RVA += VariableSize;
		}
	}

	return RVA;
}

//Converts an array of shorts representing a program to a more useable array
INSTRUCTION_ARRAY ConvertProgram(short* Program, size_t ProgramLength)
{
	INSTRUCTION_ARRAY Converted;
	Converted.Instructions = calloc(0x100, sizeof(INSTRUCTION));
	Converted.Length = 0;

	size_t ProgramIndex = 0;
	while(ProgramIndex < ProgramLength)
	{
		short Length = OperandsInInstruction(Program[ProgramIndex]) + 1;

		memcpy(Converted.Instructions[Converted.Length].Operands, Program + ProgramIndex, Length * sizeof(short));
		++Converted.Length;

		ProgramIndex += Length;
	}

	return Converted;
}

void CreateDataSection(INSTRUCTION_ARRAY* Program, BYTE* Data)
{
	size_t DataIndex = 0;

	BYTE_ARRAY Format = GetPrintFormats();
	memcpy(Data + DataIndex, Format.Array, Format.Length);
	DataIndex += Format.Length;

	free(Format.Array);

	for(size_t i = 0 ; i < Program->Length ; ++i)
	{
		char VariableSize = VariableCreator(Program->Instructions[i].Operands[0]);
		if(VariableSize != 0)
		{
			BYTE_ARRAY OPCode = InstToOPCode(Program, i);
			memcpy(Data + DataIndex, OPCode.Array, OPCode.Length);
			DataIndex += OPCode.Length;
			free(OPCode.Array);
		}
	}
}

void CreateTextSection(INSTRUCTION_ARRAY* Program, BYTE* Text)
{
	size_t TextIndex = 0;

	BYTE_ARRAY Intialise = SetEBP(0x403000);
	memcpy(Text, Intialise.Array, Intialise.Length);
	TextIndex += Intialise.Length;

	free(Intialise.Array);

	for(size_t i = 0 ; i < Program->Length ; ++i)
	{
		if(VariableCreator(Program->Instructions[i].Operands[0]) == 0)
		{
			BYTE_ARRAY OPCode = InstToOPCode(Program, i);
			memcpy(Text + TextIndex, OPCode.Array, OPCode.Length);
			TextIndex += OPCode.Length;
			free(OPCode.Array);
		}
	}
}

ProgramTextData ParseProgram(short* Program, size_t ProgramLength)
{
	INSTRUCTION_ARRAY Converted = ConvertProgram(Program, ProgramLength);

	ProgramTextData ByteProgram;
	ByteProgram.Text = calloc(0x200, sizeof(BYTE));
	ByteProgram.Data = calloc(0x200, sizeof(BYTE));

	CreateDataSection(&Converted, ByteProgram.Data);
	CreateTextSection(&Converted, ByteProgram.Text);

	free(Converted.Instructions);

	return ByteProgram;
}

//Assumes that StartIndex is an instruction that begins a scope
//Returns distance in bytes
DWORD DistToScopeEnd(INSTRUCTION_ARRAY* Program, size_t StartIndex)
{
	int SkipEnders = 0;
	int ByteDistance = 0;

	//StartIndex + 1 to skip the first instruction because it should be the scope starter
	for(int i = StartIndex + 1 ; i < 0x200 ; ++i)
	{
		ByteDistance += OPCodeSize(Program->Instructions[i].Operands[0]);

		//Another scope was entered
		if(ScopeStarter(Program->Instructions[i].Operands[0]))
		{
			++SkipEnders;
		}
		else if(ScopeEnder(Program->Instructions[i].Operands[0]))
		{
			--SkipEnders;
		}

		if(SkipEnders == -1)
		{
			return ByteDistance;
		}
	}

	return ByteDistance;
}

DWORD DistToScopeStart(INSTRUCTION_ARRAY* Program, size_t StartIndex)
{
	int SkipEnders = 0;
	int ByteDistance = 0;

	for(int i = StartIndex ; i >= 0 ; --i)
	{
		int Size = OPCodeSize(Program->Instructions[i].Operands[0]);
		ByteDistance += Size;

		//Another scope was entered
		if(ScopeEnder(Program->Instructions[i].Operands[0]))
		{
			++SkipEnders;
		}
		else if(ScopeStarter(Program->Instructions[i].Operands[0]))
		{
			--SkipEnders;
		}

		if(SkipEnders == 0)
		{
			return ByteDistance - Size;
		}
	}
	
	return ByteDistance;
}