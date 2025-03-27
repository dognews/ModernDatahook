#pragma once
#define print(string) (DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, string));
#define printf(string, other) (DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, string, other));

#include <ntifs.h>
#include <stdint.h>
#include <stdbool.h>
