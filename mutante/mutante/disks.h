#pragma once

namespace Disks
{
	PDEVICE_OBJECT GetRaidDevice(const wchar_t* deviceName);
	NTSTATUS DiskLoop(PDEVICE_OBJECT deviceArray, RaidUnitRegisterInterfaces registerInterfaces);
	NTSTATUS ChangeDiskSerials();
	NTSTATUS DisableSmart();
	void DisableSmartBit(PRAID_UNIT_EXTENSION extension);
}