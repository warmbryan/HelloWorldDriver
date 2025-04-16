#include <ntddk.h>

#include "logging.h"

#define DRIVER_TAG 'hwdb'

extern void NTAPI server_thread(void*);

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegistryPath);

	log("Skibidi Toilet loaded in Kernel Land.\n");

	HANDLE thread_handle = NULL;

	const NTSTATUS status = PsCreateSystemThread(
		&thread_handle,
		(ACCESS_MASK)0,
		NULL,
		(PVOID)0,
		NULL,
		server_thread,
		NULL
	);

	// PsTerminateSystemThread(STATUS_SUCCESS);

	if (!NT_SUCCESS(status))
	{
		log("Failed to create timer thread. Status code %X.", status);
		return STATUS_UNSUCCESSFUL;
	}

	return STATUS_SUCCESS;
}