#pragma once

IMAGE_DOS_HEADER CreateDOSHeader()
{
	IMAGE_DOS_HEADER Header;

	Header.e_magic = 0x5A4D;
	Header.e_cblp = 144U;
	Header.e_cp = 3U;
	Header.e_crlc = 0U;
	Header.e_cparhdr = 4U;
	Header.e_minalloc = 0U;
	Header.e_maxalloc = 65535U;
	Header.e_ss = 0U;
	Header.e_sp = 184U;
	Header.e_csum = 0U;
	Header.e_ip = 0U;
	Header.e_cs = 0U;
	Header.e_lfarlc = 64U;
	Header.e_ovno = 0U;
	memset(Header.e_res, 0U, 8);
	Header.e_oemid = 0U;
	Header.e_oeminfo = 0U;
	memset(Header.e_res2, 0U, 20);
	Header.e_lfanew = 128;

	return Header;
}

void CreateDOSProgram(unsigned short* Buf)
{
	unsigned short Temp[32] = { 0x0E1F, 0xBA0E, 0x00B4, 0x09CD, 0x21B8, 0x014C, 0xCD21, 0x5468, 0x6973, 0x2070, 0x726F, 0x6772, 0x616D, 0x2063, 0x616E, 0x6E6F, 0x7420, 0x6265, 0x2072, 0x756E, 0x2069, 0x6E20, 0x444F, 0x5320, 0x6D6F, 0x6465, 0x2E0D, 0x0D0A, 0x2400, 0x0000, 0x0000, 0x0000 };
	memcpy(Buf, Temp, sizeof(Temp));

	//Flip endianness
	for(int i = 0 ; i < 32 ; ++i) {
		Buf[i] = (Buf[i] << 8) | (Buf[i] >> 8);
	}
}

IMAGE_FILE_HEADER CreatePEHeader()
{
	IMAGE_FILE_HEADER Header;

	Header.Machine = 0x014C; //Intel 386
	Header.NumberOfSections = 0x0003;
	Header.TimeDateStamp = 0; //(DWORD)time(NULL);
	Header.PointerToSymbolTable = 0;
	Header.NumberOfSymbols = 0;
	Header.SizeOfOptionalHeader = 0xE0;
	Header.Characteristics = IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_32BIT_MACHINE;

	return Header;
}

IMAGE_OPTIONAL_HEADER32 CreatePEOptionalHeader()
{
	IMAGE_OPTIONAL_HEADER32 Header;

	Header.Magic = IMAGE_NT_OPTIONAL_HDR32_MAGIC;
	Header.MajorLinkerVersion = 0;
	Header.MinorLinkerVersion = 0;
	Header.SizeOfCode = 0; //Temporary, find actual size of .text section
	Header.SizeOfInitializedData = 0; //Temporary, find actual size of .data section
	Header.SizeOfUninitializedData = 0; //Temporary, find actual size of .bss section (if it exists)
	Header.AddressOfEntryPoint = 0x1000;
	Header.BaseOfCode = 0; //Temporary, find actual RVA of code section
	Header.BaseOfData = 0; //Temporary, find actual RVA of data section (the section that follows the code section)

	Header.ImageBase = 0x00400000;
	Header.SectionAlignment = 0x1000;
	Header.FileAlignment = 0x200;
	Header.MajorOperatingSystemVersion = 0;
	Header.MinorOperatingSystemVersion = 0;
	Header.MajorImageVersion = 0;
	Header.MinorImageVersion = 0;
	Header.MajorSubsystemVersion = 4;
	Header.MinorSubsystemVersion = 0;
	Header.Win32VersionValue = 0;
	Header.SizeOfImage = 0x4000; //May be wrong, must be a multiple of SectionAlignment
	Header.SizeOfHeaders = 0x200; //May be wrong, must be a multiple of FileAlignment
	Header.CheckSum = 0;
	Header.Subsystem = 3;
	Header.DllCharacteristics = 0;
	Header.SizeOfStackReserve = 0x1000;
	Header.SizeOfStackCommit = 0x1000;
	Header.SizeOfHeapReserve = 0x10000;
	Header.SizeOfHeapCommit = 0x1000;
	Header.LoaderFlags = 0;
	Header.NumberOfRvaAndSizes = 16;
	for(int i = 0 ; i < 16 ; ++i)
	{
		Header.DataDirectory[i].VirtualAddress = 0;
		Header.DataDirectory[i].Size = 0;
	}

	Header.DataDirectory[1].VirtualAddress = 0x2000;

	return Header;
}

IMAGE_SECTION_HEADER CreateTextSectionHeader()
{
	IMAGE_SECTION_HEADER Header;

	Header.Name[0] = '.';
	Header.Name[1] = 't';
	Header.Name[2] = 'e';
	Header.Name[3] = 'x';
	Header.Name[4] = 't';
	Header.Name[5] = 0;
	Header.Name[6] = 0;
	Header.Name[7] = 0;
	Header.Misc.VirtualSize = 0x1000; //Size of data, may be different
	Header.VirtualAddress = 0x1000; //May be different
	Header.SizeOfRawData = 0x200;
	Header.PointerToRawData = 0x200;
	Header.PointerToRelocations = 0;
	Header.PointerToLinenumbers = 0;
	Header.NumberOfRelocations = 0;
	Header.NumberOfLinenumbers = 0;
	Header.Characteristics = IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_CODE;

	return Header;
}

IMAGE_SECTION_HEADER CreateImportSectionHeader()
{
	IMAGE_SECTION_HEADER Header;

	Header.Name[0] = '.';
	Header.Name[1] = 'i';
	Header.Name[2] = 'd';
	Header.Name[3] = 'a';
	Header.Name[4] = 't';
	Header.Name[5] = 'a';
	Header.Name[6] = 0;
	Header.Name[7] = 0;
	Header.Misc.VirtualSize = 0x004B; //Size of data, may be different
	Header.VirtualAddress = 0x2000; //May be different
	Header.SizeOfRawData = 0x200;
	Header.PointerToRawData = 0x400;
	Header.PointerToRelocations = 0;
	Header.PointerToLinenumbers = 0;
	Header.NumberOfRelocations = 0;
	Header.NumberOfLinenumbers = 0;
	Header.Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ;

	return Header;
}

IMAGE_SECTION_HEADER CreateDataSectionHeader()
{
	IMAGE_SECTION_HEADER Header;

	Header.Name[0] = '.';
	Header.Name[1] = 'd';
	Header.Name[2] = 'a';
	Header.Name[3] = 't';
	Header.Name[4] = 'a';
	Header.Name[5] = 0;
	Header.Name[6] = 0;
	Header.Name[7] = 0;
	Header.Misc.VirtualSize = 0x1000; //Size of data, may be different
	Header.VirtualAddress = 0x3000; //May be different
	Header.SizeOfRawData = 0x200;
	Header.PointerToRawData = 0x600;
	Header.PointerToRelocations = 0;
	Header.PointerToLinenumbers = 0;
	Header.NumberOfRelocations = 0;
	Header.NumberOfLinenumbers = 0;
	Header.Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;

	return Header;
}

typedef struct DLL_DESCRIPTOR
{
	DWORD AddressToImportNameTable; //INT
	BYTE Padding[8]; //Not padding: 4 bytes of time/date, 4 bytes of index of first forwarder reference
	DWORD AddressToDLLNames;
	DWORD AddressToImportAddressTable; //IAT
} DLL_DESCRIPTOR;

typedef struct IMPORTS_SECTION
{
	DLL_DESCRIPTOR* Descriptors;
	size_t SizeOfDescriptors;

	char** DLLNames;
	size_t SizeOfDLLNames;

	UINT64* ImportNameTable;
	size_t SizeOfNameTable;

	UINT64* ImportAddressTable;
	size_t SizeOfAddressTable;

	UINT16* FunctionHints;
	char** FunctionNames;
	size_t SizeOfFunctionNames;
} IMPORTS_SECTION;

IMPORTS_SECTION CreateImportsSection()
{
	IMPORTS_SECTION Section;

	//Descriptors
	Section.SizeOfDescriptors = 2; //Number of dll descriptors plus one to indicate end of descriptors
	Section.Descriptors = malloc(sizeof(DLL_DESCRIPTOR) * Section.SizeOfDescriptors);
	Section.Descriptors[0].AddressToImportNameTable = 0x2033;
	memset(Section.Descriptors[0].Padding, 0, 8);
	Section.Descriptors[0].AddressToDLLNames = 0x2028;
	Section.Descriptors[0].AddressToImportAddressTable = 0x203B;

	//All zero to indicate end of descriptors
	Section.Descriptors[1].AddressToImportNameTable = 0;
	memset(Section.Descriptors[1].Padding, 0, 8);
	Section.Descriptors[1].AddressToDLLNames = 0;
	Section.Descriptors[1].AddressToImportAddressTable = 0;

	//Dll names
	Section.SizeOfDLLNames = 1;
	Section.DLLNames = malloc(sizeof(char*) * Section.SizeOfDLLNames);
	Section.DLLNames[0] = _strdup("msvcrt.dll");

	//Import name table
	Section.SizeOfNameTable = 1;
	Section.ImportNameTable = malloc(sizeof(UINT64) * Section.SizeOfNameTable);
	Section.ImportNameTable[0] = 0x2043;

	//Import address table
	Section.SizeOfAddressTable = 1;
	Section.ImportAddressTable = malloc(sizeof(UINT64) * Section.SizeOfAddressTable);
	Section.ImportAddressTable[0] = 0x2043;

	//Function names
	Section.SizeOfFunctionNames = 1;
	Section.FunctionHints = calloc(Section.SizeOfFunctionNames, sizeof(UINT16));
	Section.FunctionNames = malloc(sizeof(char*) * Section.SizeOfFunctionNames);
	Section.FunctionNames[0] = _strdup("printf");

	return Section;
}

void FreeImportsSection(IMPORTS_SECTION* Section)
{
	free(Section->Descriptors);
	free(Section->DLLNames[0]);
	free(Section->DLLNames);
	free(Section->ImportNameTable);
	free(Section->ImportAddressTable);
	free(Section->FunctionHints);
	free(Section->FunctionNames[0]);
	free(Section->FunctionNames);
}

void WriteImports(FILE* File, IMPORTS_SECTION* Section)
{
	size_t BytesWritten = 0;

	//Write descriptors
	fwrite(Section->Descriptors, sizeof(DLL_DESCRIPTOR), Section->SizeOfDescriptors, File);
	BytesWritten += sizeof(DLL_DESCRIPTOR) * Section->SizeOfDescriptors;

	//Write DLL names
	for(size_t i = 0 ; i < Section->SizeOfDLLNames ; ++i)
	{
		size_t DLLNameSize = strlen(Section->DLLNames[i]) + 1;
		fwrite(Section->DLLNames[i], DLLNameSize, 1, File);
		BytesWritten += DLLNameSize * 1;
	}

	//Write number of names
	fwrite(Section->ImportNameTable, sizeof(UINT64), Section->SizeOfNameTable, File);
	BytesWritten += sizeof(UINT64) * Section->SizeOfNameTable;

	//Write number of addresses
	fwrite(Section->ImportAddressTable, sizeof(UINT64), Section->SizeOfAddressTable, File);
	BytesWritten += sizeof(UINT64) * Section->SizeOfAddressTable;

	//Write function hints and function names
	for(size_t i = 0 ; i < Section->SizeOfNameTable ; ++i)
	{
		fwrite(&Section->FunctionHints[i], sizeof(UINT16), 1, File);
		BytesWritten += sizeof(UINT16) * 1;

		size_t FunctionNameSize = strlen(Section->FunctionNames[i]) + 1;
		fwrite(Section->FunctionNames[i], FunctionNameSize, Section->SizeOfFunctionNames, File);
		BytesWritten += FunctionNameSize * Section->SizeOfFunctionNames;
	}

	BYTE Zero = 0;
	for(size_t i = 0 ; i < 0x200 - BytesWritten ; ++i)
		fwrite(&Zero, sizeof(BYTE), 1, File);
}

void CreateAndWriteHeaders(FILE* File)
{
	IMAGE_DOS_HEADER MZHead = CreateDOSHeader();
	fwrite(&MZHead, sizeof(MZHead), 1, File);

	unsigned short DOSProgram[32];
	CreateDOSProgram(DOSProgram);
	fwrite(DOSProgram, 2, 32, File);

	char PEMagic[4] = { 'P', 'E', 0, 0 };
	fwrite(PEMagic, sizeof(char), 4, File);

	IMAGE_FILE_HEADER PEHead = CreatePEHeader();
	fwrite(&PEHead, sizeof(PEHead), 1, File);

	IMAGE_OPTIONAL_HEADER32 PEOptHead = CreatePEOptionalHeader();
	fwrite(&PEOptHead, sizeof(PEOptHead), 1, File);

	IMAGE_SECTION_HEADER TextHead = CreateTextSectionHeader();
	fwrite(&TextHead, sizeof(TextHead), 1, File);

	IMAGE_SECTION_HEADER IDataHead = CreateImportSectionHeader();
	fwrite(&IDataHead, sizeof(IDataHead), 1, File);

	IMAGE_SECTION_HEADER DataHead = CreateDataSectionHeader();
	fwrite(&DataHead, sizeof(DataHead), 1, File);

	//Fill this section with zeros to meet the file alignment
	BYTE Zero = 0;
	for(size_t i = 0 ; i < 16 ; ++i)
	{
		fwrite(&Zero, sizeof(BYTE), 1, File);
	}
}