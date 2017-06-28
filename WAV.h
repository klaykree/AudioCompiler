#pragma once
#include <stdio.h>
#include <math.h>

#include "ShortArray.h"

//The structure appears to not have any padding but just in case
#pragma pack(1)
typedef struct WAV_HEAD
{
	//RIFF head
	char RIFF[4]; //Big endian
	unsigned int FileSize; //Total size - 8 (does not include RIFF and FileSize)
	char WAVEFormat[4]; //Big endian

	//fmt head/chunk
	char SubChunk1ID[4]; //Big endian
	unsigned int SubChunk1Size;
	unsigned short AudioFormat;
	unsigned short Channels;
	unsigned int SampleRate;
	unsigned int ByteRate;
	unsigned short BlockAlign;
	unsigned short BitsPerSample;

	//data head/chunk
	char SubChunk2ID[4];
	unsigned int SubChunk2Size; //Size - 44
}WAV_HEAD;
#pragma pack()

typedef struct WAV_DATA_16BIT
{
	unsigned short Channels;
	short* Data;
}WAV_DATA_16BIT;

WAV_HEAD CreateWAVHead(size_t DataLength)
{
	WAV_HEAD Head;

	//RIFF chunk
	strncpy_s(Head.RIFF, 5, "RIFF", 4);
	strncpy_s(Head.WAVEFormat, 5, "WAVE", 4);

	//fmt sub-chunk (format section)
	strncpy_s(Head.SubChunk1ID, 5, "fmt ", 4);
	Head.SubChunk1Size = 16;
	Head.AudioFormat = 1; //Wave type PCM
	Head.Channels = 1;
	Head.SampleRate = 44100; //8000
	Head.BitsPerSample = 16;
	Head.ByteRate = Head.SampleRate * Head.Channels * Head.BitsPerSample / 8; //sample rate * channel count * bits per sample / 8
	Head.BlockAlign = Head.Channels * Head.BitsPerSample / 8; //channel count * bits per sample / 8

	//data section start
	strncpy_s(Head.SubChunk2ID, 5, "data", 4);

	int Seconds = 5;
	//size_t FileSize = Head.SampleRate * Head.Channels * Seconds * (Head.BitsPerSample / 8);
	//Head.FileSize = FileSize - 8;
	//Head.SubChunk2Size = FileSize - 44;

	Head.SubChunk2Size = DataLength * 2;
	Head.FileSize = Head.SubChunk2Size + 36;

	return Head;
}

void WriteWAVData(FILE* File, WAV_HEAD* Head, WAV_DATA_16BIT* Data, size_t Size)
{
	fwrite(Head, sizeof(WAV_HEAD), 1, File);
	fwrite(Data->Data, sizeof(short), Size / sizeof(short), File);
}

void CreateWAV(const char* Name, short* Data, size_t DataSize)
{
	FILE* File;
	fopen_s(&File, Name, "wb+");

	WAV_HEAD Head = CreateWAVHead(DataSize);

	WAV_DATA_16BIT WaveData;
	WaveData.Channels = Head.Channels;
	WaveData.Data = malloc(Head.SubChunk2Size);

	for(size_t i = 0 ; i < Head.SubChunk2Size / 2 ; ++i)
	{
		//Change the byte order, WAV is little endian for the data section
		//short LittleEndData = Data[i % DataSize];
		//short LittleEndDataTemp = LittleEndData;
		//LittleEndData <<= 8;
		//LittleEndData ^= (LittleEndDataTemp >> 8) & 0b0000000011111111;
		WaveData.Data[i] = Data[i];
	}

	WriteWAVData(File, &Head, &WaveData, Head.SubChunk2Size);

	free(WaveData.Data);

	fclose(File);
}

void ReadWAV(char* Name, SHORT_ARRAY* Program, short ProgramStop)
{
	FILE* File;
	fopen_s(&File, Name, "rb+");

	WAV_HEAD Head;
	fread(&Head, sizeof(Head), 1, File);

	Program->Data = malloc(Head.SubChunk2Size);
	fread(Program->Data, sizeof(short), Head.SubChunk2Size / sizeof(short), File);

	for(size_t i = 0 ; i < Head.SubChunk2Size / sizeof(short) ; ++i)
	{
		//Change the byte order, WAV is little endian for the data section
		short BigEndData = Program->Data[i];
		short LittleEndDataTemp = BigEndData;
		BigEndData = (BigEndData << 8);
		BigEndData ^= (LittleEndDataTemp >> 8) & 0b0000000011111111;
		Program->Data[i] = BigEndData;

		if(BigEndData == ProgramStop)
		{
			Program->Length = i + 1;
			break;
		}
	}

	fclose(File);
}