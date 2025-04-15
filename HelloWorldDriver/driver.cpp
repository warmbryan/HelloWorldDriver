#include <ntddk.h>

#include "logging.h"

#define DRIVER_TAG 'hwdb'

UNICODE_STRING g_RegPath;

PVOID g_WorkerThreadObject = NULL;
HANDLE g_WorkerThreadHandle = NULL;
KTIMER g_Timer;
KDPC g_TimerDpc;
KEVENT g_TimerEvent;
BOOLEAN g_StopWorkerThread = FALSE;

//// Function to be executed when the timer expires (DPC routine)
//VOID
//TimerDpcRoutine(
//	_In_ PKDPC Dpc,
//	_In_opt_ PVOID DeferredContext,
//	_In_opt_ PVOID SystemArgument1,
//	_In_opt_ PVOID SystemArgument2
//)
//{
//	UNREFERENCED_PARAMETER(Dpc);
//	UNREFERENCED_PARAMETER(DeferredContext);
//	UNREFERENCED_PARAMETER(SystemArgument1);
//	UNREFERENCED_PARAMETER(SystemArgument2);
//
//	// Signal the worker thread that the timer has expired
//	KeSetEvent(&g_TimerEvent, IO_NO_INCREMENT, FALSE);
//}
//
//// Worker thread function
//VOID
//WorkerThread(
//	_In_ PVOID Context
//)
//{
//	UNREFERENCED_PARAMETER(Context);
//
//	LARGE_INTEGER dueTime;
//	dueTime.QuadPart = -10 * 1000 * 1000; // Relative time in 100-nanosecond intervals (1 second)
//
//	while (!g_StopWorkerThread) {
//		// Set the timer to expire
//		KeSetTimer(&g_Timer, dueTime, &g_TimerDpc);
//
//		// Wait for the timer to expire or the stop event
//		NTSTATUS status = KeWaitForSingleObject(&g_TimerEvent, Executive, KernelMode, FALSE, NULL);
//
//		if (status == STATUS_SUCCESS) {
//			// "Print" something using KdPrint
//			log("MyKernelDriver: Timer ticked! %ul\n", g_Timer.DueTime);
//
//			// Optionally, write to a kernel buffer here
//		}
//		else if (status == STATUS_ALERTED) {
//			// Thread was alerted, likely due to driver unload
//			break;
//		}
//
//		// Reset the event
//		KeClearEvent(&g_TimerEvent);
//	}
//
//	PsTerminateSystemThread(STATUS_SUCCESS);
//}

extern void NTAPI server_thread(void*);

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegistryPath);

	log("Skibidi Toilet loaded in Kernel Land.\n");

	HANDLE thread_handle = NULL;

	// Initialize the timer and DPC
	/*KeInitializeTimer(&g_Timer);
	KeInitializeDpc(&g_TimerDpc, TimerDpcRoutine, NULL);
	KeInitializeEvent(&g_TimerEvent, SynchronizationEvent, FALSE);*/

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