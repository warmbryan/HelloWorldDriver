#pragma once
#include <ntddk.h>
#include <stdio.h>
#include <stdlib.h>

#define MESSAGE_BUFFER_SIZE 100

template<typename... Args>
void log(const char* format, Args... args)
{
	char buffer[MESSAGE_BUFFER_SIZE];

	_snprintf(buffer, MESSAGE_BUFFER_SIZE, format, args...);

	DbgPrintEx(0, 0, "[Warm] %s", buffer);
}