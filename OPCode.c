#include "OPCode.h"
#include "Compiler.h"

BYTE_ARRAY InstToOPCode(INSTRUCTION_ARRAY* Program, size_t InstIndex)
{
	INSTRUCTION* Inst = &Program->Instructions[InstIndex];
	short InstructionType = Inst->Operands[0];

	BYTE_ARRAY Instruction;
	Instruction.Length = OPCodeSize(Inst->Operands[0]);
	Instruction.Array = calloc(Instruction.Length, sizeof(BYTE));

	if(InstructionType == AddImm)
	{
		Instruction.Array[0] = 0x81; //ADD
		Instruction.Array[1] = 0x85; //[EBP] + disp32
		DWORD RVA = GetIdentifierRVA(Program, InstIndex, Inst->Operands[1]);
		memcpy(Instruction.Array + 2, &RVA, sizeof(DWORD)); //displacement
		memcpy(Instruction.Array + 6, &Inst->Operands[2], sizeof(int)); //immediate value
	}
	else if(InstructionType == AddVar)
	{
		//Move right hand side variable to EAX
		Instruction.Array[0] = 0x8B; //MOV
		Instruction.Array[1] = 0x85; //[EBP] + disp32, EAX
		DWORD RVA = GetIdentifierRVA(Program, InstIndex, Inst->Operands[2]);
		memcpy(Instruction.Array + 2, &RVA, sizeof(DWORD)); //displacement

		Instruction.Array[6] = 0x01; //ADD
		Instruction.Array[7] = 0x85; //[EBP] + disp32, EAX
		RVA = GetIdentifierRVA(Program, InstIndex, Inst->Operands[1]);
		memcpy(Instruction.Array + 8, &RVA, sizeof(DWORD)); //displacement
	}
	else if(InstructionType == SubImm)
	{
		Instruction.Array[0] = 0x81; //SUB
		Instruction.Array[1] = 0xAD; //[EBP] + disp32
		DWORD RVA = GetIdentifierRVA(Program, InstIndex, Inst->Operands[1]);
		memcpy(Instruction.Array + 2, &RVA, sizeof(DWORD)); //displacement
		memcpy(Instruction.Array + 6, &Inst->Operands[2], sizeof(int)); //immediate value
	}
	else if(InstructionType == SubVar)
	{
		//Move right hand side variable to EAX
		Instruction.Array[0] = 0x8B; //MOV
		Instruction.Array[1] = 0x85; //[EBP] + disp32, EAX
		DWORD RVA = GetIdentifierRVA(Program, InstIndex, Inst->Operands[2]);
		memcpy(Instruction.Array + 2, &RVA, sizeof(DWORD)); //displacement

		Instruction.Array[6] = 0x29; //ADD
		Instruction.Array[7] = 0x85; //[EBP] + disp32, EAX
		RVA = GetIdentifierRVA(Program, InstIndex, Inst->Operands[1]);
		memcpy(Instruction.Array + 8, &RVA, sizeof(DWORD)); //displacement
	}
	else if(InstructionType == MulImm)
	{
		//Move the immediate value to EAX
		Instruction.Array[0] = 0xC7; //MOV
		Instruction.Array[1] = 0xC0; //EAX, immediate
		memcpy(Instruction.Array + 2, &Inst->Operands[2], sizeof(int)); //immediate value

		//Multiply EAX by the variable, the result will be in EAX
		Instruction.Array[6] = 0x0F; //IMUL
		Instruction.Array[7] = 0xAF;
		Instruction.Array[8] = 0x85; //EAX, [EBP] + disp32
		DWORD RVA = GetIdentifierRVA(Program, InstIndex, Inst->Operands[1]);
		memcpy(Instruction.Array + 9, &RVA, sizeof(DWORD)); //displacement

		//Move the result from EAX to the variable
		Instruction.Array[13] = 0x89; //MOV
		Instruction.Array[14] = 0x85; //[EBP] + disp32, EAX
		memcpy(Instruction.Array + 15, &RVA, sizeof(DWORD)); //displacement
	}
	else if(InstructionType == MulVar)
	{
		//Move the left hand side variable to EAX
		Instruction.Array[0] = 0x8B; //MOV
		Instruction.Array[1] = 0x85; //EAX, [EBP] + disp32
		DWORD RVAOne = GetIdentifierRVA(Program, InstIndex, Inst->Operands[1]);
		memcpy(Instruction.Array + 2, &RVAOne, sizeof(DWORD)); //displacement
		
		//Multiply EAX by the variable, the result will be in EAX
		Instruction.Array[6] = 0x0F; //IMUL
		Instruction.Array[7] = 0xAF;
		Instruction.Array[8] = 0x85; //EAX, [EBP] + disp32
		DWORD RVATwo = GetIdentifierRVA(Program, InstIndex, Inst->Operands[2]);
		memcpy(Instruction.Array + 9, &RVATwo, sizeof(DWORD)); //displacement

		//Move the result from EAX to the variable
		Instruction.Array[13] = 0x89; //MOV
		Instruction.Array[14] = 0x85; //[EBP] + disp32, EAX
		memcpy(Instruction.Array + 15, &RVAOne, sizeof(DWORD)); //displacement
	}
	else if(InstructionType == DivImm)
	{
		//Dividend / divisor = quotient + remainder

		//Zero the dividend (remainder result)
		Instruction.Array[0] = 0x33; //XOR
		Instruction.Array[1] = 0xD2; //EDX, EDX

		//Move dividend to EAX
		Instruction.Array[2] = 0x8B; //MOV
		Instruction.Array[3] = 0x85; //EAX, [EBP] + disp32
		DWORD RVA = GetIdentifierRVA(Program, InstIndex, Inst->Operands[1]);
		memcpy(Instruction.Array + 4, &RVA, sizeof(DWORD)); //displacement

		//Move immediate value to ECX
		Instruction.Array[8] = 0xB8 + 0x01; //MOV to ECX
		memcpy(Instruction.Array + 9, &Inst->Operands[2], sizeof(int)); //immediate value

		//Divide EAX by ECX
		Instruction.Array[13] = 0xF7; //IDIV
		Instruction.Array[14] = 0xF9; //ECX
		
		//Move the result from EAX to the variable
		Instruction.Array[15] = 0x89; //MOV
		Instruction.Array[16] = 0x85; //[EBP] + disp32, EAX
		memcpy(Instruction.Array + 17, &RVA, sizeof(DWORD)); //displacement
	}
	else if(InstructionType == DivVar)
	{
		//Dividend / divisor = quotient + remainder

		//Zero the dividend (remainder result)
		Instruction.Array[0] = 0x33; //XOR
		Instruction.Array[1] = 0xD2; //EDX, EDX

		//Move dividend to EAX
		Instruction.Array[2] = 0x8B; //MOV
		Instruction.Array[3] = 0x85; //[EBP] + disp32, EAX
		DWORD RVAOne = GetIdentifierRVA(Program, InstIndex, Inst->Operands[1]);
		memcpy(Instruction.Array + 4, &RVAOne, sizeof(DWORD)); //displacement

		//Divide EAX by [EBP] + disp32 (the variable)
		Instruction.Array[8] = 0xF7; //IDIV
		Instruction.Array[9] = 0xBD; //[EBP] + disp32
		DWORD RVATwo = GetIdentifierRVA(Program, InstIndex, Inst->Operands[2]);
		memcpy(Instruction.Array + 10, &RVATwo, sizeof(DWORD)); //displacement

		//Move the result (quotient) from EAX to the variable
		Instruction.Array[14] = 0x89; //MOV
		Instruction.Array[15] = 0x85; //[EBP] + disp32, EAX
		memcpy(Instruction.Array + 16, &RVAOne, sizeof(DWORD)); //displacement
	}
	else if(InstructionType == CreateVar)
	{
		//Copy just the value of the variable
		memcpy(Instruction.Array, Inst->Operands + 2, sizeof(int));
	}
	else if(InstructionType == AssignFromImmediate)
	{
		Instruction.Array[0] = 0xC7; //MOV
		Instruction.Array[1] = 0x85; //[EBP] + disp32
		DWORD RVA = GetIdentifierRVA(Program, InstIndex, Inst->Operands[1]);
		memcpy(Instruction.Array + 2, &RVA, sizeof(DWORD)); //displacement
		memcpy(Instruction.Array + 6, &Inst->Operands[2], sizeof(int)); //immediate value
	}
	else if(InstructionType == AssignFromVariable)
	{
		Instruction.Array[0] = 0x8B; //MOV
		Instruction.Array[1] = 0x85; //EAX, [EBP] + disp32
		DWORD RVATwo = GetIdentifierRVA(Program, InstIndex, Inst->Operands[2]);
		memcpy(Instruction.Array + 2, &RVATwo, sizeof(DWORD)); //displacement

		Instruction.Array[6] = 0x89; //MOV
		Instruction.Array[7] = 0x85; //[EBP] + disp32, EAX
		DWORD RVAOne = GetIdentifierRVA(Program, InstIndex, Inst->Operands[1]);
		memcpy(Instruction.Array + 8, &RVAOne, sizeof(DWORD)); //displacement
	}
	else if(InstructionType == PrintVar)
	{
		//Push variable value to stack
		Instruction.Array[0] = 0xFF; //PUSH
		DWORD RVA = GetIdentifierRVA(Program, InstIndex, Inst->Operands[1]);
		Instruction.Array[1] = 0x75; //[EBP] + disp8
		Instruction.Array[2] = (BYTE)RVA; //displacement

		Instruction.Array[3] = 0x68; //PUSH immediate 32
		//Address of print format: "%i\0"
		Instruction.Array[4] = 0x00;
		Instruction.Array[5] = 0x30;
		Instruction.Array[6] = 0x40;
		Instruction.Array[7] = 0x00;

		Instruction.Array[8] = 0xFF; //CALL
		Instruction.Array[9] = 0x15;

		Instruction.Array[10] = 0x3B; //Function address of printf
		Instruction.Array[11] = 0x20;
		Instruction.Array[12] = 0x40;
		Instruction.Array[13] = 0x00;

		Instruction.Array[14] = 0x58; //POP to eax (popping the pushed variable)
		Instruction.Array[15] = 0x58;
	}
	else if(InstructionType == PrintVarLn)
	{
		//Push variable value to stack
		Instruction.Array[0] = 0xFF; //PUSH
		DWORD RVA = GetIdentifierRVA(Program, InstIndex, Inst->Operands[1]);
		Instruction.Array[1] = 0x75; //[EBP] + disp8
		Instruction.Array[2] = (BYTE)RVA; //displacement

		Instruction.Array[3] = 0x68; //PUSH immediate 32
		//Address of print format: "%i\n\0"
		Instruction.Array[4] = 0x03;
		Instruction.Array[5] = 0x30;
		Instruction.Array[6] = 0x40;
		Instruction.Array[7] = 0x00;

		Instruction.Array[8] = 0xFF; //CALL
		Instruction.Array[9] = 0x15;

		Instruction.Array[10] = 0x3B; //Function address of printf
		Instruction.Array[11] = 0x20;
		Instruction.Array[12] = 0x40;
		Instruction.Array[13] = 0x00;

		Instruction.Array[14] = 0x58; //POP to eax
		Instruction.Array[15] = 0x58;
	}
	else if(InstructionType == PrintLn)
	{
		Instruction.Array[0] = 0x6A;
		Instruction.Array[1] = 0;

		Instruction.Array[2] = 0x68; //PUSH immediate 32
		//Address of print format: "\n\0"
		Instruction.Array[3] = 0x07;
		Instruction.Array[4] = 0x30;
		Instruction.Array[5] = 0x40;
		Instruction.Array[6] = 0x00;

		Instruction.Array[7] = 0xFF; //CALL
		Instruction.Array[8] = 0x15;

		Instruction.Array[9] = 0x3B; //Function address of printf
		Instruction.Array[10] = 0x20;
		Instruction.Array[11] = 0x40;
		Instruction.Array[12] = 0x00;

		Instruction.Array[13] = 0x58; //POP to eax
		Instruction.Array[14] = 0x58;
	}
	else if(InstructionType == IfImmEql)
	{
		Instruction.Array[0] = 0x81; //CMP
		Instruction.Array[1] = 0xBD; //[EBP] + disp32
		DWORD RVA = GetIdentifierRVA(Program, InstIndex, Inst->Operands[1]);
		memcpy(Instruction.Array + 2, &RVA, sizeof(DWORD)); //displacement
		memcpy(Instruction.Array + 6, &Inst->Operands[2], sizeof(int)); //immediate value

		Instruction.Array[10] = 0x0F; //JNE
		Instruction.Array[11] = 0x85;
		DWORD ScopeSize = DistToScopeEnd(Program, InstIndex);
		memcpy(Instruction.Array + 12, &ScopeSize, sizeof(ScopeSize));
	}
	else if(InstructionType == IfVarEql)
	{
		//Move right hand side to EAX
		Instruction.Array[0] = 0x8B; //MOV
		Instruction.Array[1] = 0x85; //[EBP] + disp32, EAX
		DWORD RVATwo = GetIdentifierRVA(Program, InstIndex, Inst->Operands[2]);
		memcpy(Instruction.Array + 2, &RVATwo, sizeof(DWORD)); //displacement

		Instruction.Array[6] = 0x39; //CMP
		Instruction.Array[7] = 0x85; //[EBP] + disp32, EAX
		DWORD RVAOne = GetIdentifierRVA(Program, InstIndex, Inst->Operands[1]);
		memcpy(Instruction.Array + 8, &RVAOne, sizeof(DWORD)); //displacement

		Instruction.Array[12] = 0x0F; //JNE
		Instruction.Array[13] = 0x85; //relative32
		DWORD ScopeSize = DistToScopeEnd(Program, InstIndex);
		memcpy(Instruction.Array + 14, &ScopeSize, sizeof(ScopeSize));
	}
	else if(InstructionType == ForImm)
	{
		//Move immediate value (loop count) to EDI
		Instruction.Array[0] = 0xB8 + 0x07; //MOV to EDI
		memcpy(Instruction.Array + 1, &Inst->Operands[1], sizeof(int)); //immediate value
	}
	else if(InstructionType == ForVar)
	{
		//Move variable (loop count) to EDI
		Instruction.Array[0] = 0x8B; //MOV
		Instruction.Array[1] = 0xBD; //EDI, [EBP] + disp32
		DWORD RVA = GetIdentifierRVA(Program, InstIndex, Inst->Operands[1]);
		memcpy(Instruction.Array + 2, &RVA, sizeof(DWORD)); //displacement
	}
	else if(InstructionType == EndFor)
	{
		Instruction.Array[0] = 0x48 + 0x07; //DEC EDI

		Instruction.Array[1] = 0x81; //CMP
		Instruction.Array[2] = 0xFF; //EDI
		Instruction.Array[3] = 0x00; //Immediate
		Instruction.Array[4] = 0x00;
		Instruction.Array[5] = 0x00;
		Instruction.Array[6] = 0x00;

		Instruction.Array[7] = 0x0F; //JNE
		Instruction.Array[8] = 0x85;
		int ScopeSize = DistToScopeStart(Program, InstIndex);
		ScopeSize = -ScopeSize;
		memcpy(Instruction.Array + 9, &ScopeSize, sizeof(ScopeSize));
	}
	else if(InstructionType == EndProgram)
	{
		Instruction.Array[0] = 0xC3; //RETN
	}

	return Instruction;
}

BYTE_ARRAY SetEBP(DWORD RVA)
{
	BYTE_ARRAY OPCode;

	OPCode.Length = 6;
	OPCode.Array = calloc(OPCode.Length, sizeof(BYTE));
	
	OPCode.Array[0] = 0xC7; //MOV imm32
	OPCode.Array[1] = 0xC5; //ModR/M = 11|000|101 = C5 = EBP
	memcpy(OPCode.Array + 2, &RVA, sizeof(RVA));

	return OPCode;
}

BYTE_ARRAY GetPrintFormats()
{
	BYTE_ARRAY Format;

	Format.Length = 9;
	Format.Array = calloc(Format.Length, sizeof(BYTE));

	Format.Array[0] = '%';
	Format.Array[1] = 'i';
	Format.Array[2] = 0;

	Format.Array[3] = '%';
	Format.Array[4] = 'i';
	Format.Array[5] = '\n';
	Format.Array[6] = 0;

	Format.Array[7] = '\n';
	Format.Array[8] = 0;

	return Format;
}

//Returns the number of bytes that the instruction will occupy
int OPCodeSize(short Inst)
{
	if(Inst == AddImm)
	{
		return 10;
	}
	if(Inst == AddVar)
	{
		return 12;
	}
	if(Inst == SubImm)
	{
		return 10;
	}
	if(Inst == SubVar)
	{
		return 12;
	}
	if(Inst == MulImm)
	{
		return 19;
	}
	if(Inst == MulVar)
	{
		return 19;
	}
	if(Inst == DivImm)
	{
		return 21;
	}
	if(Inst == DivVar)
	{
		return 20;
	}

	if(Inst == PrintVar)
	{
		return 16;
	}
	if(Inst == PrintVarLn)
	{
		return 16;
	}
	if(Inst == PrintLn)
	{
		return 15;
	}
	if(Inst == CreateVar)
	{
		return 4;
	}
	if(Inst == AssignFromImmediate)
	{
		return 10;
	}
	if(Inst == AssignFromVariable)
	{
		return 12;
	}

	if(Inst == IfImmEql)
	{
		return 16;
	}
	if(Inst == IfVarEql)
	{
		return 18;
	}
	if(Inst == ForImm)
	{
		return 5;
	}
	if(Inst == ForVar)
	{
		return 6;
	}
	if(Inst == EndFor)
	{
		return 13;
	}

	if(Inst == EndProgram)
	{
		return 1;
	}

	return 0;
}