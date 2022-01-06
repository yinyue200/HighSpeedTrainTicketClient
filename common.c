//  SimpleStoreErp
//	Copyright(C) 2021 殷越
//
//	This program is free software : you can redistribute it and/or modify
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
//创建一个字符串
//length: 要创建的字符串长度，包括末尾0
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
//将传入的字符串复制一次
//str：要复制的字符串
wchar_t* CreateWstrFromWstr(wchar_t* str)
{
	size_t strlen = wcslen(str);//要创建的字符串长度
	size_t size = strlen * sizeof(wchar_t);//byte size
	wchar_t* ptr = yinyue200_safemalloc(size + sizeof(wchar_t));//需要加一个 sizeof(wchar_t) 因为需要放末尾的 0
	memcpy(ptr, str, size);
	ptr[strlen] = 0;
	return ptr;
}
//一种原创的转义算法
wchar_t* Yinyue200_TsvEncode(wchar_t* str)
{
	vector ret = { 0 };
	int len = wcslen(str);
	vector_initwithcap_wchar_t(&ret, max(4, len * 2));
	int i;
	for (i = 0; i < len; i++)
	{
		wchar_t one = str[i];
		if (iswcntrl(one) || one == L'&' || iswspace(one))
		{
			wchar_t buffer[10];
			swprintf(buffer, 10, L"&%X;", one);
			for (int j = 0; buffer[j] != 0; j++)
			{
				vector_add_wchar_t(&ret, buffer[j]);
			}
		}
		else
		{
			vector_add_wchar_t(&ret, one);
		}
	}
	vector_add_wchar_t(&ret, 0);
	return (wchar_t*)ret.items;
}
wchar_t* Yinyue200_TsvDecode(wchar_t* str)
{
	vector ret;
	int len = wcslen(str);
	vector_initwithcap_wchar_t(&ret, len + 1);
	int i;
	for (i = 0; i < len; i++)
	{
		wchar_t one = str[i];
		if (one == L'&')
		{
			int c;
			int innret = swscanf(str + i + 1, L"%X;", &c);
			if (innret == 1)
			{
				vector_add_wchar_t(&ret, c);
				while (str[i]!=L';')
				{
					i++;
				}
			}
			else
			{
				UnrecoveryableFailed();
			}
		}
		else
		{
			vector_add_wchar_t(&ret, one);
		}
	}
	((wchar_t*)ret.items)[vector_total_wchar_t(&ret)] = 0;
	return (wchar_t*)ret.items;
}
//无法恢复的错误，直接退出程序
__declspec(noreturn) void UnrecoveryableFailed()
{
	MessageBox(NULL, L"不可恢复的错误", NULL, 0);
	exit(1234);//检测到不可恢复的错误后直接退出程序
}
//检查当前是否没有打开的窗口，如果没有则退出程序
void CheckIfNoWindowAndQuit()
{
	if (CheckIfNoWindow())
		PostQuitMessage(0);
}
//检查 hresult,当 hresult 值代表失败时退出程序
void CheckHResult(HRESULT hresult)
{
	FailedIfFalse(SUCCEEDED(hresult));
}
//当 state 为 false 时调用 UnrecoveryableFailed()
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
//当 malloc 失败时调用 UnrecoveryableFailed()
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
//调用yinyue200_safemalloc并将内存区域清0
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
YINYUE200_PAIR_OF_uint64_t_uint64_t Yinyue200_ConvertToUint64PairFromGuid(GUID guid)
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
		low
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
//将百纳秒转换成秒
double Yinyue200_ConvertToTotalSecondFromUINT64(uint64_t time)
{
	double ret = time;
	ret /= 10000000;
	return ret;
}
//将秒转换成百纳秒
uint64_t Yinyue200_ConvertToUINT64FromTotalSecond(double time)
{
	uint64_t ret = time;
	ret *= 10000000;
	return ret;
}
//将vector中的内容序列化到字符串
//func: 将 vector 的每一项序列化的函数
PWSTR Yinyue200_ConvertVectorToString(vector* vec, PWSTR(*func)(void* ptr))
{
	int total = vector_total(vec);
	bool init = false;
	vector ret;
	for (int i = 0; i < total; i++)
	{
		void* one = vector_get(vec, i);
		PWSTR oonestr = func(one);
		PWSTR onestr = Yinyue200_TsvEncode(oonestr);
		free(oonestr);
		size_t onelen = wcslen(onestr);
		if (i == 0)
		{
			vector_initwithcap_wchar_t(&ret, onelen * total);
			init = true;
		}
		for (int j = 0; j < onelen; j++)
		{
			vector_add_wchar_t(&ret, onestr[j]);
		}
		free(onestr);
		vector_add_wchar_t(&ret, L' ');
	}
	if (init == false)
	{
		vector_init_wchar_t(&ret);
		vector_add_wchar_t(&ret, L' ');
	}
	vector_set_wchar_t(&ret, vector_total_wchar_t(&ret)-1, 0);
	return ret.items;
 }
vector Yinyue200_ConvertStringToVector(PWSTR str, void* (*func)(PWSTR str))
{
	size_t len = wcslen(str);
	PWSTR buffer = yinyue200_safemalloc(len * sizeof(wchar_t));
	vector ret;
	vector_init(&ret);
	int i = 0;
	wchar_t* tokencontext = NULL;
	wchar_t* p = wcstok(str, L" ", &tokencontext);
	while (p)
	{
		PWSTR decodeline = Yinyue200_TsvDecode(p);
		vector_add(&ret, func(decodeline));
		free(decodeline);
		p = wcstok(NULL, L" ", &tokencontext);
	}
	free(buffer);
	return ret;
}
//输入的是本地时间
FILETIME ConvertDateToLocalFILETIME(int year, int month, int day)
{
	FILETIME localfiletime;
	SYSTEMTIME systime = { 0 };
	systime.wYear = year;
	systime.wMonth = month;
	systime.wDay = day;
	SystemTimeToFileTime(&systime, &localfiletime);
	return localfiletime;
}
//输入的是本地时间
FILETIME ConvertDateToUTCFILETIME(int year, int month, int day)
{
	FILETIME localfiletime = ConvertDateToLocalFILETIME(year, month, day);
	FILETIME utcfiletime;
	LocalFileTimeToFileTime(&localfiletime, &utcfiletime);
	return utcfiletime;
}
uint64_t ConvertTimeToUINT64(UINT hour, UINT minute, UINT second)
{
	return Yinyue200_ConvertToTotalSecondFromUINT64(hour * 3600llu + minute * 60llu + second);
}
uint64_t GetTimePartUINT64OFUINT64(uint64_t time)
{
	FILETIME filetime = Yinyue200_ConvertToFileTimeFromUINT64(time);
	SYSTEMTIME systime;
	FileTimeToSystemTime(&filetime, &systime);
	return ConvertTimeToUINT64(systime.wHour, systime.wMinute, systime.wSecond) + systime.wMilliseconds * 10000;
}
vector SplitStringToVectorOfString(PWSTR str, PWSTR spl)
{
	VECTOR_INIT(vec);
	wchar_t* context = NULL;
	PWSTR p = wcstok(str, spl, &context);
	while (p)
	{
		vector_add(&vec, CreateWstrFromWstr(p));
		p = wcstok(NULL, spl, &context);
	}
	return vec;
}
void FreeVectorOfString(vector *vec)
{
	for (int i = 0; i < vector_total(vec); i++)
	{
		PWSTR one = vector_get(vec, i);
		free(one);
	}
	vector_free(vec);
}
bool WritePWSTR(PCWSTR str, HANDLE hFile)
{
	if (str == NULL)
		return true;
	size_t len = wcslen(str);
	size_t utf8len = len * sizeof(wchar_t) * 2;//假定一个 UTF-16 编码单元在 UTF-8 中最多使用四个字节表示
	char* utf8bytes = malloc(utf8len);
	if (utf8bytes)
	{
		utf8len = WideCharToMultiByte(CP_UTF8, 0, str, len, utf8bytes, utf8len, NULL, NULL);

		DWORD written;
		if (WriteFile(hFile, utf8bytes, utf8len, &written, NULL))
		{
			free(utf8bytes);
			return true;
		}
		int reason = GetLastError();//错误原因，仅供调试
		free(utf8bytes);
		return false;
	}
	else
	{
		return false;
	}
}