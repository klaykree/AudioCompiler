#pragma once
#include <stdio.h>
#include <string.h>

//Resizes and zeroes new memory or allocates array if not yet allocated
//updates length argument
void* ArrayPush(void* Array, size_t* Length, void* Value, size_t ValueSize)
{
	(*Length)++;
	if((*Length) == 1)
	{
		Array = calloc(1, ValueSize);
	}
	else
	{
		Array = realloc(Array, (*Length) * ValueSize);
	}

	if(Value != NULL)
		memcpy(((char*)Array) + ((*Length) - 1) * ValueSize, Value, ValueSize);
	else
		memset(((char*)Array) + ((*Length) - 1) * ValueSize, 0, ValueSize);

	return Array;
}