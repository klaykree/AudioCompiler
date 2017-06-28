#pragma once
#include <stdlib.h>

typedef struct DArray
{
	unsigned int Length;
	unsigned int Max;
	size_t ElementSize;
	void** Contents;
}DArray;

DArray* ArrayCreate(size_t ElementSize)
{
	DArray* Array = malloc(sizeof(DArray));
	Array->ElementSize = ElementSize;
	Array->Length = 0;
	Array->Max = 64;
	Array->Contents = malloc(Array->Max * Array->ElementSize);

	return Array;
}

void ArrayPush(DArray* Array, void* Value)
{
	Array->Contents[Array->Length] = Value;
	Array->Length++;

	if(Array->Length >= Array->Max)
	{
		Array->Max += 64;
		realloc(Array->Contents, Array->Max * Array->ElementSize);
	}
}

void ArraySet(DArray* Array, void* Value, unsigned int Index)
{
	Array->Contents[Index] = Value;
}

void* ArrayGet(DArray* Array, unsigned int Index)
{
	return Array->Contents[Index];
}