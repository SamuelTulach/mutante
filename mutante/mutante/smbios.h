#pragma once

namespace Smbios
{
	char* GetString(SMBIOS_HEADER* header, SMBIOS_STRING string);
	void RandomizeString(char* string);
	NTSTATUS ProcessTable(SMBIOS_HEADER* header);
	NTSTATUS LoopTables(void* mapped, ULONG size);
	NTSTATUS ChangeSmbiosSerials();
}