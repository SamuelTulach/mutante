#include <ntifs.h>
#include <ntstrsafe.h>
#include "utils.h"
#include "shared.h"
#include "log.h"
#include "disks.h"


/**
 * \brief Null smart mask in raid extension to disable S.M.A.R.T functionality
 * \param extension Pointer to disk's raid unit extension
 */
void Disks::DisableSmartBit(PRAID_UNIT_EXTENSION extension)
{
	extension->_Smart.Telemetry.SmartMask = 0;
}

/**
 * \brief Get pointer to device object of desired raid port of given name
 * \param deviceName Name of the raid port (path ex. \\Device\\RaidPort0)
 * \return Pointer to device object
 */
PDEVICE_OBJECT Disks::GetRaidDevice(const wchar_t* deviceName)
{
	UNICODE_STRING raidPort;
	RtlInitUnicodeString(&raidPort, deviceName);

	PFILE_OBJECT fileObject = nullptr;
	PDEVICE_OBJECT deviceObject = nullptr;
	auto status = IoGetDeviceObjectPointer(&raidPort, FILE_READ_DATA, &fileObject, &deviceObject);
	if (!NT_SUCCESS(status))
	{
		return nullptr;
	}

	return deviceObject->DriverObject->DeviceObject; // not sure about this
}

/**
 * \brief Loop through all devices in the array and change serials of any
 * device of type FILE_DEVICE_DISK
 * \param deviceArray Pointer to first device
 * \param registerInterfaces Function from storport.sys to reset registry entries
 * \return 
 */
NTSTATUS Disks::DiskLoop(PDEVICE_OBJECT deviceArray, RaidUnitRegisterInterfaces registerInterfaces)
{
	auto status = STATUS_NOT_FOUND;
	
	while (deviceArray->NextDevice)
	{
		if (deviceArray->DeviceType == FILE_DEVICE_DISK)
		{
			auto* extension = static_cast<PRAID_UNIT_EXTENSION>(deviceArray->DeviceExtension);
			if (!extension)
				continue;
			
			const auto length = extension->_Identity.Identity.SerialNumber.Length;
			if (!length)
				continue;

			char original[256];
			memcpy(original, extension->_Identity.Identity.SerialNumber.Buffer, length);
			original[length] = '\0';

			auto* buffer = static_cast<char*>(ExAllocatePoolWithTag(NonPagedPool, length, POOL_TAG));
			buffer[length] = '\0';

			Utils::RandomText(buffer, length);
			RtlInitString(&extension->_Identity.Identity.SerialNumber, buffer);

			Log::Print("Changed disk serial %s to %s.\n", original, buffer);

			status = STATUS_SUCCESS;
			ExFreePool(buffer);

			/*
			 * On some devices DiskEnableDisableFailurePrediction will fail
			 * Setting the bits directly will not fail and should work on any device
			 */
			DisableSmartBit(extension);

			registerInterfaces(extension);
		}

		deviceArray = deviceArray->NextDevice;
	}

	return status;
}

/**
 * \brief Change serials of internal disk drives by looping FILE_DEVICE_DISK type devices
 * and changing their identifiers
 * \return Status of the change (returns STATUS_NOT_FOUND if no FILE_DEVICE_DISK was found)
 */
NTSTATUS Disks::ChangeDiskSerials()
{
	auto* base = Utils::GetModuleBase("storport.sys");
	if (!base)
	{
		Log::Print("Failed to find storport.sys base!\n");
		return STATUS_UNSUCCESSFUL;
	}

	const auto registerInterfaces = static_cast<RaidUnitRegisterInterfaces>(Utils::FindPatternImage(base, "\x48\x89\x5C\x24\x00\x55\x56\x57\x48\x83\xEC\x50", "xxxx?xxxxxxx")); // RaidUnitRegisterInterfaces
	if (!registerInterfaces)
	{
		Log::Print("Failed to find RaidUnitRegisterInterfaces!\n");
		return STATUS_UNSUCCESSFUL;
	}

	/* We want to loop through multiple raid ports since on my test systems
	 * and VMs, NVMe drives were always on port 1 and SATA drives on port 0.
	 * Maybe on some systems looping through more ports will be needed,
	 * but I haven't found system that would need it.
	 */
	
	auto status = STATUS_NOT_FOUND;
	for (auto i = 0; i < 2; i++)
	{
		const auto* raidFormat = L"\\Device\\RaidPort%d";
		wchar_t raidBuffer[18];
		RtlStringCbPrintfW(raidBuffer, 18 * sizeof(wchar_t), raidFormat, i);

		auto* device = GetRaidDevice(raidBuffer);
		if (!device)
			continue;

		const auto loopStatus = DiskLoop(device, registerInterfaces);
		if (NT_SUCCESS(loopStatus))
			status = loopStatus;
	}

	return status;
}

/*
 * Object type for driver objects (exported by ntoskrnl, but not in WDK for some reason)
 */
extern "C" POBJECT_TYPE* IoDriverObjectType;

/**
 * \brief Loop through disk driver's device objects and disable
 * S.M.A.R.T functionality on all found drives
 * \return Status of the action (STATUS_SUCCESS if required function and list found, not if actually disabled)
 */
NTSTATUS Disks::DisableSmart()
{
	auto* base = Utils::GetModuleBase("disk.sys");
	if (!base)
	{
		Log::Print("Failed to find disk.sys base!\n");
		return STATUS_UNSUCCESSFUL;
	}

	const auto disableFailurePrediction = static_cast<DiskEnableDisableFailurePrediction>(Utils::FindPatternImage(base, "\x4C\x8B\xDC\x49\x89\x5B\x10\x49\x89\x7B\x18\x55\x49\x8D\x6B\xA1\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x45\x4F", "xxxxxxxxxxxxxxxxxxx????xxx????xxxxxxx")); // DiskEnableDisableFailurePrediction
	if (!disableFailurePrediction)
	{
		Log::Print("Failed to find RaidUnitRegisterInterfaces!\n");
		return STATUS_UNSUCCESSFUL;
	}

	UNICODE_STRING driverDisk;
	RtlInitUnicodeString(&driverDisk, L"\\Driver\\Disk");

	PDRIVER_OBJECT driverObject = nullptr;
	auto status = ObReferenceObjectByName(&driverDisk, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, nullptr, 0, *IoDriverObjectType, KernelMode, nullptr, reinterpret_cast<PVOID*>(&driverObject));
	if (!NT_SUCCESS(status))
	{
		Log::Print("Failed to get disk driver object!\n");
		return STATUS_UNSUCCESSFUL;
	}

	PDEVICE_OBJECT deviceObjectList[64];
	RtlZeroMemory(deviceObjectList, sizeof(deviceObjectList));

	ULONG numberOfDeviceObjects = 0;
	status = IoEnumerateDeviceObjectList(driverObject, deviceObjectList, sizeof(deviceObjectList), &numberOfDeviceObjects);
	if (!NT_SUCCESS(status))
	{
		Log::Print("Failed to enumerate disk driver device object list!\n");
		return STATUS_UNSUCCESSFUL;
	}

	for (ULONG i = 0; i < numberOfDeviceObjects; ++i)
	{
		auto* deviceObject = deviceObjectList[i];
		disableFailurePrediction(deviceObject->DeviceExtension, false);
		ObDereferenceObject(deviceObject);
	}

	ObDereferenceObject(driverObject);
	return STATUS_SUCCESS;
}