#include "Compiler.h"
//#include "OPCode.h"

const int InstructionCount = 13;
const short DefinedInstructions[][2] = {
	{ Add, 3 },{ Sub, 3 },{ Mul, 3 },{ Div, 3 },
	{ Push, 1 },{ Pop, 1 },
	{ Create16Bit, 2 },{ Create32Bit, 3 },{ Assign16Bit, 2 },{ Assign32Bit, 3 },
	{ If, 2 },{ For, 1 },{ End, 0 }
};

BOOL ScopeStarter(short Instruction)
{
	return Instruction == If || Instruction == For;
}

BOOL ScopeEnder(short Instruction)
{
	return Instruction == End;
}

//Returns size of variable to create
//returns zero if not an instruction to create a variable
char VariableCreator(short Instruction)
{
	if(Instruction == Create16Bit)
		return 2;
	else if(Instruction == Create32Bit)
		return 4;

	return 0;
}

BOOL VariableIdentifierAvailableRec(SCOPE* Scope, short Identifier)
{
	for(size_t i = 0 ; i < Scope->Instructions.Length ; ++i)
	{
		if(VariableCreator(Scope->Instructions.Array[i].Inst) > 0)
		{
			if(Scope->Instructions.Array[i].OPs.Array[0] == Identifier)
			{
				return FALSE;
			}
		}
	}

	SCOPE* CurrentScope = Scope;
	if(CurrentScope->ParentScope)
	{
		return VariableIdentifierAvailableRec(CurrentScope->ParentScope, Identifier);
	}

	return TRUE;
}

//Returns the availability of an identifier
//Instructions and previous variables will disallow availability
BOOL VariableIdentifierAvailable(SCOPE* Scope, short Identifier)
{
	for(int i = 0 ; i < InstructionCount ; ++i)
	{
		if(DefinedInstructions[i][0] == Identifier)
		{
			return FALSE;
		}
	}

	return VariableIdentifierAvailableRec(Scope, Identifier);
}

short OPCount(short Instruction)
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

INSTRUCTION* ParseInstruction(short InstructionData[8])
{
	INSTRUCTION* Instruction = (INSTRUCTION*)calloc(1, sizeof(INSTRUCTION));
	Instruction->Inst = InstructionData[0];
	OPERAND_ARRAY* OPs = (OPERAND_ARRAY*)calloc(1, sizeof(OPERAND_ARRAY));
	OPs->Length = OPCount(Instruction->Inst);
	OPs->Array = (short*)calloc(OPs->Length, sizeof(short));
	for(size_t i = 0 ; i < OPs->Length ; ++i)
	{
		OPs->Array[i] = InstructionData[i + 1];
	}
	Instruction->OPs = *OPs;

	return Instruction;
}

//Returns the program with meta data, returns the top most scope
void Parse(short* Program, size_t ProgramLength, SCOPE* GlobalScope, PINST_ARRAY* SequentialInstructions)
{
	//SCOPE* Global = calloc(1, sizeof(SCOPE)); //Create the top level scope
	SCOPE* Current = GlobalScope; //Begin at the top scope

	DWORD CurrentDataRVA = 0x3000;

	//PINST_ARRAY SequentialInstructions;
	SequentialInstructions->Array = 0;
	SequentialInstructions->Length = 0;

	size_t Index = 0;
	while(Index < ProgramLength)
	{
		size_t IncrementIndexes = 1;

		short Instructions[MaxOperands]; //Instruction at index 0 and operands following
		for(int i = 0 ; i < MaxOperands ; ++i)
		{
			if(Index + i < ProgramLength)
				Instructions[i] = Program[Index + i];
			else
				Instructions[i] = 0;
		}

		INSTRUCTION* NewInstruction = ParseInstruction(Instructions);

		//Create variable data for instruction if applicable
		if(NewInstruction->Inst == Create16Bit)
		{
			VARIABLE NewVariable;
			NewVariable.Identifier = NewInstruction->OPs.Array[0];
			assert(VariableIdentifierAvailable(GlobalScope, NewVariable.Identifier) && "Variable identifier unavailable");
			NewVariable.Size = 2;
			NewVariable.InitalValue[0] = NewInstruction->OPs.Array[1];
			NewVariable.InitalValue[1] = 0;
			NewVariable.RVA = CurrentDataRVA;
			CurrentDataRVA += NewVariable.Size;

			Current->Variables.Array = (VARIABLE*)ArrayPush(Current->Variables.Array, &Current->Variables.Length, &NewVariable, sizeof(VARIABLE));
		}

		Current->Instructions.Array = (INSTRUCTION*)ArrayPush(Current->Instructions.Array, &Current->Instructions.Length, &NewInstruction, sizeof(INSTRUCTION));

		SequentialInstructions->Array = (INSTRUCTION**)ArrayPush(SequentialInstructions->Array, &SequentialInstructions->Length, &NewInstruction, sizeof(INSTRUCTION*));
		printf("%i\n", SequentialInstructions->Array[SequentialInstructions->Length - 1]->Inst);

		IncrementIndexes += NewInstruction->OPs.Length;

		if(ScopeStarter(Program[Index]))
		{
			Current->SubScopes.Array = (SCOPE*)ArrayPush(Current->SubScopes.Array, &Current->SubScopes.Length, NULL, sizeof(SCOPE));

			Current->SubScopes.Array[Current->SubScopes.Length - 1].ParentScope = Current; //Set parent of new child scope
			Current = &Current->SubScopes.Array[Current->SubScopes.Length - 1]; //Use the new child scope as the current scope
		}

		if(ScopeEnder(Program[Index]))
		{
			if(Current != GlobalScope)
			{
				Current = Current->ParentScope;
			}
			else //Program end when current and global scopes are the same with a scope ender
			{
				return;
			}
		}

		Index += IncrementIndexes;
	}

	return;
}

void GenerateDataSectionRec(SCOPE* Scope, BYTE* Data)
{
	for(size_t i = 0 ; i < Scope->Variables.Length ; ++i)
	{
		memcpy(Data + Scope->Variables.Array[i].RVA - 0x3000, Scope->Variables.Array[i].InitalValue, Scope->Variables.Array[i].Size);
	}

	for(size_t i = 0 ; i < Scope->SubScopes.Length ; ++i)
	{
		GenerateDataSectionRec(&Scope->SubScopes.Array[i], Data);
	}
}

BYTE* GenerateDataSection(SCOPE* Scope, size_t Size)
{
	BYTE* Data = (BYTE*)calloc(Size, sizeof(BYTE));
	SCOPE* CurrentScope = Scope;

	GenerateDataSectionRec(CurrentScope, Data);

	return Data;
}

BYTE* GenerateTextSection(SCOPE* Scope, PINST_ARRAY* SequentialInstructions)
{
	BYTE* Text = (BYTE*)calloc(0x200, sizeof(BYTE));

	for(size_t i = 0 ; i < SequentialInstructions->Length ; ++i)
	{

	}

	return Text;
}

void ParseProgram(short* Program, size_t ProgramLength)
{
	BYTE* Text; //.text section
	BYTE* Data; //.data section

	SCOPE* GlobalScope = (SCOPE*)calloc(1, sizeof(SCOPE));
	PINST_ARRAY* SequentialInstructions = (PINST_ARRAY*)calloc(1, sizeof(PINST_ARRAY));
	Parse(Program, ProgramLength, GlobalScope, SequentialInstructions);

	Text = GenerateTextSection(GlobalScope, SequentialInstructions);
	Data = GenerateDataSection(GlobalScope, 0x200);
}