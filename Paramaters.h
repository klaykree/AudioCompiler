#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct _CompileOptions
{
	int SkipNotes;
	char OutName[32];
}
CompileOptions;

CompileOptions ParseParameters(int argc, char** argv)
{
	CompileOptions Options;
	Options.SkipNotes = 0;
	memset(Options.OutName, 0, 32);

	if(argc < 2)
	{
		printf("File required");
	}

	for(int i = 2 ; i < argc ; ++i)
	{
		if(argv[i][0] == '/' || argv[i][0] == '-')
		{
			if(argv[i][1] == 's')
			{
				Options.SkipNotes = atoi(argv[i + 1]);
			}
			else if(argv[i][1] == 'n')
			{
				size_t Length = strnlen(argv[i + 1], 32);
				memcpy(Options.OutName, argv[i + 1], Length);
				memcpy(Options.OutName + Length, ".exe", 5);
				Options.OutName[Length] = 0;

				//int Length = strnlen(Options.OutName, 32);
				//char* Name = malloc(Length + 5);
				//memcpy(Name, Options.OutName, Length);
				//memcpy(Name + Length, ".exe", 5);
			}
		}
	}

	printf("SkipNotes: %i\n", Options.SkipNotes);
	printf("Outname: %s\n", Options.OutName);
	
	return Options;
}