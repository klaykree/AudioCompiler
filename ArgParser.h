#pragma once

#include "ShortArray.h"

//Turn the program instructions in the arguments into a more usable SHORT_ARRAY
SHORT_ARRAY ArgsToShortArray(char** ProgramInstructions, size_t ProgramLength)
{
	SHORT_ARRAY Program;
	Program.Length = ProgramLength;

	Program.Data = malloc(Program.Length * sizeof(short));

	for(size_t i = 0 ; i < Program.Length ; ++i)
	{
		if(sscanf_s(ProgramInstructions[i], "%hi", &Program.Data[i]) != 1) {
			fprintf(stderr, "error - not an integer");
		}
	}

	return Program;
}