/*
 * Mutante
 * Made by Samuel Tulach
 * https://github.com/SamuelTulach/mutante
 */

#include <ntifs.h>
#include "log.h"
#include "shared.h"

#include "disks.h"
#include "smbios.h"

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