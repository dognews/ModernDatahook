#pragma once

void* GetKernelModuleExport(LPCWSTR module_name, LPCSTR routine_name) {
	UNICODE_STRING name;
	RtlInitUnicodeString(&name, L"PsLoadedModuleList");
	PLIST_ENTRY module_list = (PLIST_ENTRY)MmGetSystemRoutineAddress(&name);

	if (!module_list)
		return NULL;

	for (PLIST_ENTRY link = module_list; link != module_list->Blink; link = link->Flink) {
		LDR_DATA_TABLE_ENTRY* entry = CONTAINING_RECORD(link, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

		UNICODE_STRING name;
		RtlInitUnicodeString(&name, module_name);

		if (RtlEqualUnicodeString(&entry->BaseDllName, &name, TRUE)) {
			return (entry->DllBase) ? RtlFindExportedRoutineByName(entry->DllBase, routine_name) : NULL;
		}
	}
}