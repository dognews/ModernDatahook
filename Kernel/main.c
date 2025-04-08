#include "imports.h"
#include "defines.h"
#include "win32.h"

NtUserEnableWindowGDIScaledDpiMessage previous_func = NULL;

bool __fastcall NtUserEnableWindowGDIScaledDpiMessageHook(uint64_t p1, int p2) {
	print("NtUserEnableWindowGDIScaledDpiMessageHook Called\n");

	return previous_func(p1, p2);
}

NTSTATUS HookFunction(uint32_t function_table_offset, uint32_t function_offset, void** previous_function, void* new_function) {
	W32GetSessionState GetW32SessionState = (W32GetSessionState)GetKernelModuleExport(L"win32k.sys", "W32GetSessionState");
	uint64_t function_table = *(uint64_t*)(*(uint64_t*)(GetW32SessionState() + 136) + function_table_offset);
	uint64_t* function_pointer = (uint64_t*)(function_table + function_offset);

	if (!function_pointer || !*function_pointer)
		return STATUS_UNSUCCESSFUL;

	*previous_function = *function_pointer;
	InterlockedExchangePointer(function_pointer, new_function);

	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegistryPath);

	if (!NT_SUCCESS(HookFunction(336LL, 768LL, &previous_func, &NtUserEnableWindowGDIScaledDpiMessageHook))) {
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	return STATUS_SUCCESS;
}
