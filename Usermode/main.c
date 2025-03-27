#include "imports.h"
#include "win32.h"

int main() {
	ConvertToWin32Thread();

	NtUserEnableWindowGDIScaledDpiMessage hooked_kernel_function = (NtUserEnableWindowGDIScaledDpiMessage)GetUsermodeModuleExport("win32u.dll", "NtUserEnableWindowGDIScaledDpiMessage");
	if (hooked_kernel_function == NULL) {
		puts("Failed to find function");
		int _ = getchar();
	}

	hooked_kernel_function(4, 2);
	puts("Found and called hooked function");
	int _ = getchar();

	return 0;
}