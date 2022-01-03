//  SimpleStoreErp
//	Copyright(C) 2021 殷越
//
//	This program is free software : you can redistribute it and /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program. If not, see <https://www.gnu.org/licenses/>.
#include "common.h"
#include <stdint.h>
LPWORD lpwAlign(LPWORD lpIn)
{
	ULONG ul;

	ul = (ULONG)lpIn;
	ul++;
	ul >>= 1;
	ul <<= 1;
	return (ULONG)ul;
}
PWCHAR CreateWSTR(size_t length)
{
	return malloc(length * sizeof(wchar_t));
}
PWCHAR CreateWstrForWindowText(HWND hwnd)
{
	size_t size = GetWindowTextLength(hwnd) + 1;
	PWCHAR p = CreateWSTR(size);
	GetWindowText(hwnd, p, size);
	return p;
}
__declspec(noreturn) void UnrecoveryableFailed()
{
	MessageBox(NULL, L"不可恢复的错误", NULL, 0);
	exit(1234);//检测到不可恢复的错误后直接退出程序
}
void CheckIfNoWindowAndQuit()
{
	if (CheckIfNoWindow())
		PostQuitMessage(0);
}
void FailedIfFalse(bool state)
{
	if (state)
	{
		;
	}
	else
	{
		UnrecoveryableFailed();
	}
}
void* yinyue200_safemalloc(size_t size)
{
	void* p = malloc(size);
	if (p)
	{
		return p;
	}
	else
	{
		UnrecoveryableFailed();
		return NULL;
	}
}
void* yinyue200_safemallocandclear(size_t size)
{
	void* p = yinyue200_safemalloc(size);
	memset(p, 0, size);
	return p;
}
GUID Yinyue200_ConvertToGuid(uint64_t high, uint64_t low)
{
	unsigned char low64[8] = {
	(low & 0XFF00000000000000) >> 56,
	(low & 0X00FF000000000000) >> 48,
	(low & 0X0000FF0000000000) >> 40,
	(low & 0X000000FF00000000) >> 32,
	(low & 0X00000000FF000000) >> 24,
	(low & 0X0000000000FF0000) >> 16,
	(low & 0X000000000000FF00) >> 8,
	(low & 0X00000000000000FF) >> 0,
	};
	GUID guid = {
		(high & 0XFFFFFFFF00000000) >> 32,
		(high & 0X00000000FFFF0000) >> 16,
		(high & 0X000000000000FFFF),
		low64
	};
	return guid;
}
YINYUE200_PAIR_OF_uint64_t_uint64_t Yinyue200_ConvertFromGuid(GUID guid)
{
	uint64_t low = 
		((uint64_t)guid.Data4[0]) << 56 |
		((uint64_t)guid.Data4[1]) << 48 |
		((uint64_t)guid.Data4[2]) << 40 |
		((uint64_t)guid.Data4[3]) << 32 |
		((uint64_t)guid.Data4[4]) << 24 |
		((uint64_t)guid.Data4[5]) << 16 |
		((uint64_t)guid.Data4[6]) << 8 |
		((uint64_t)guid.Data4[7]) << 0;
	YINYUE200_PAIR_OF_uint64_t_uint64_t ret = {
		(((uint64_t)guid.Data1)<<32)|
		(((uint64_t)guid.Data2)<<16)| guid.Data3,

	};
	return ret;
}
FILETIME Yinyue200_ConvertToFileTimeFromUINT64(uint64_t time)
{
	ULARGE_INTEGER filetime;
	filetime.QuadPart = time;
	FILETIME real_filetime = { filetime.LowPart, filetime.HighPart };
	return real_filetime;
}
uint64_t Yinyue200_ConvertToUINT64FromFileTime(FILETIME time)
{
	ULARGE_INTEGER integer;
	integer.LowPart = time.dwLowDateTime;
	integer.HighPart = time.dwHighDateTime;
	return integer.QuadPart;
}