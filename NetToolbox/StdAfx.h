#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define PIO_APC_ROUTINE_DEFINED
#include <WinSock2.h>
#include <winternl.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <format>
#include "../DuiLib_Faw/DuiLib/StdAfx.h"
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
