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

	Disks::ChangeDiskSerials();
	Disks::DisableSmart();
	Smbios::ChangeSmbiosSerials();

	// TODO: add credits
	// - https://www.unknowncheats.me/forum/anti-cheat-bypass/310941-hdd-serial-spoofer-hooking-4.html
	// - https://github.com/btbd/hwid/blob/master/Kernel/main.c
	// - https://github.com/KunYi/DumpSMBIOS/blob/master/DumpSMBIOS/smbios.cpp#L285

	return STATUS_SUCCESS;
}