/*
 * Mutante
 * Made by Samuel Tulach
 * https://github.com/SamuelTulach/mutante
 */

#include <ntifs.h>
#include <ntstrsafe.h>
#include <ntddk.h>
#include <ntifs.h>
#include <wdm.h>

#define _NO_CRT_STDIO_INLINE

//#include <shared.h>

#include "log.h"
#include "shared.h"

#include "disks.h"
#include "smbios.h"
#include "utils.cpp"
#include "smbios.cpp"
#include "log.cpp"
#include "disks.cpp"

unsigned long _pei386_runtime_relocator = NULL;

/**
 * \brief Driver's main entry point
 * \param object Pointer to driver object (invalid when manual mapping)
 * \param registry Registry path (invalid when manual mapping)
 * \return Status of the driver execution
 */
extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT object, PUNICODE_STRING registry)
{
	UNREFERENCED_PARAMETER(object);
	UNREFERENCED_PARAMETER(registry);

	Log::Print("Driver loaded. Build on %s.", __DATE__);

	Disks::DisableSmart();
	Disks::ChangeDiskSerials();
	Smbios::ChangeSmbiosSerials();

	return STATUS_SUCCESS;
}