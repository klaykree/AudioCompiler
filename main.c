#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <stdio.h>
#include <windows.h>
#include <time.h>

#include "PEHeader.h"
#include "WAV.h"
#include "Compiler.h"
#include "ArgParser.h"

void WriteZeros(FILE* File, size_t Length)
{
	BYTE Zero = 0;
	for(size_t i = 0 ; i < Length ; ++i)
	{
		fwrite(&Zero, sizeof(BYTE), 1, File);
	}
}

void AddToProgram(short* ProgramData, size_t* ProgramIndex, short Value)
{
	ProgramData[*ProgramIndex] = Value;
	++(*ProgramIndex);
}

void MakeWAV(size_t ProgramLength, char** ProgramData, char* WavName)
{
	SHORT_ARRAY ProgramInstructions = ArgsToShortArray(ProgramData, ProgramLength);

	CreateWAV(WavName, ProgramInstructions.Data, ProgramInstructions.Length);

	free(ProgramInstructions.Data);
}

void MakePE(SHORT_ARRAY* ProgramInstructions, char* OutputName)
{
	FILE* File;
	fopen_s(&File, OutputName, "wb+");

	CreateAndWriteHeaders(File);

	ProgramTextData Program = ParseProgram((*ProgramInstructions).Data, (*ProgramInstructions).Length);

	//Write the text (code) portion to the new file
	fwrite(Program.Text, sizeof(BYTE), 0x200, File);

	IMPORTS_SECTION ImportsSection = CreateImportsSection();
	WriteImports(File, &ImportsSection);
	FreeImportsSection(&ImportsSection);

	fwrite(Program.Data, sizeof(BYTE), 0x200, File);

	free(Program.Data);
	free(Program.Text);

	free((*ProgramInstructions).Data);

	fclose(File);
}

int main(int argc, char** argv)
{
	if(argc > 1)
	{
		//argv[1] == -wav or -compile or -wavcompile
		//argv[2] == output file name
		//argv[3] == program data or input file name or output file name
		//argv[4] == program data
		
		//Argument examples:
		//Create wav from data: "-wav", wav output name, wav data
		//Compile exe from wav file: "-compile", exe output name, wav input name
		//Compile exe from data: "-compile", exe output name, program data
		//Create wav and compile exe: "-wavcompile", wav output name, exe output name, wav/exe data

		BOOL WavAndCompile = FALSE;
		size_t ProgramLength = argc - 3; //Discount 3 arguments to get program data length
		size_t WavOutputNameOffset = 2;
		size_t PEOutputNameOffset = 2;
		size_t ProgramInstructionsOffset = 3;

		if(strcmp(argv[1], "-wavcompile") == 0)
		{
			WavAndCompile = TRUE;
			WavOutputNameOffset = 3;
			ProgramInstructionsOffset = 4;
			ProgramLength -= 1; //Minus one due to one more expected argument (a second file output name)
		}

		if(WavAndCompile || strcmp(argv[1], "-wav") == 0)
		{
			MakeWAV(ProgramLength, argv + ProgramInstructionsOffset, *(argv + WavOutputNameOffset));
		}

		if(WavAndCompile || strcmp(argv[1], "-compile") == 0)
		{
			SHORT_ARRAY ProgramInstructions;

			if(argc == 4) //Compile from wav file
			{
				const int WavInputNameOffset = 3;
				ReadWAV(*(argv + WavInputNameOffset), &ProgramInstructions, EndProgram);
			}
			else //Compile from instruction array
			{
				ProgramInstructions = ArgsToShortArray(argv + ProgramInstructionsOffset, ProgramLength);
			}

			MakePE(&ProgramInstructions, *(argv + PEOutputNameOffset));
		}
	}

	/*SHORT_ARRAY ProgramInstructions;
	ReadWAV("test.wav", &ProgramInstructions, EndProgram);
	MakePE(&ProgramInstructions, "lolo.exe");*/

	/*FILE* File;
	fopen_s(&File, "program.exe", "wb+");

	CreateAndWriteHeaders(File);

	short ProgramData[100];
	memset(ProgramData, 0, 100 * sizeof(short));
	size_t ProgramIndex = 0;

	AddToProgram(ProgramData, &ProgramIndex, CreateVar);
	AddToProgram(ProgramData, &ProgramIndex, 0); //Variable identifier
	AddToProgram(ProgramData, &ProgramIndex, 47); //Value, low 2 bytes
	AddToProgram(ProgramData, &ProgramIndex, 0); //Value, high 2 bytes
	AddToProgram(ProgramData, &ProgramIndex, CreateVar);
	AddToProgram(ProgramData, &ProgramIndex, 1);
	AddToProgram(ProgramData, &ProgramIndex, 2);
	AddToProgram(ProgramData, &ProgramIndex, 0);

	AddToProgram(ProgramData, &ProgramIndex, PrintVarLn);
	AddToProgram(ProgramData, &ProgramIndex, 0);
	AddToProgram(ProgramData, &ProgramIndex, PrintVarLn);
	AddToProgram(ProgramData, &ProgramIndex, 1);

	AddToProgram(ProgramData, &ProgramIndex, AddVar);
	AddToProgram(ProgramData, &ProgramIndex, 0);
	AddToProgram(ProgramData, &ProgramIndex, 1);

	AddToProgram(ProgramData, &ProgramIndex, PrintLn);
	AddToProgram(ProgramData, &ProgramIndex, PrintVarLn);
	AddToProgram(ProgramData, &ProgramIndex, 0);
	AddToProgram(ProgramData, &ProgramIndex, PrintVarLn);
	AddToProgram(ProgramData, &ProgramIndex, 1);

	AddToProgram(ProgramData, &ProgramIndex, EndProgram);

	SHORT_ARRAY PData;
	//ReadWAV("program.wav", &PData, EndProgram);

	ProgramTextData Program = ParseProgram(ProgramData, ProgramIndex);
	//ProgramTextData Program = ParseProgram(PData.Data, PData.Length);

	fwrite(Program.Text, sizeof(BYTE), 0x200, File);

	IMPORTS_SECTION ImportsSection = CreateImportsSection();
	WriteImports(File, &ImportsSection);
	FreeImportsSection(&ImportsSection);

	WriteZeros(File, 16 * 27 + 4); //Fill the rest of the imports section with zeros

	fwrite(Program.Data, sizeof(BYTE), 0x200, File);

	free(Program.Data);
	free(Program.Text);

	fclose(File);*/

#if _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}