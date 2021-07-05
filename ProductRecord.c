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
#define DEFINE_GETMEMBERMETHOD(name) void* yinyue200_GetProductRecord##name(void* obj)\
{\
PRODUCTRECORD_PTR p = obj;\
return p->name;\
}
#define DEFINE_GETMEMBERADDRMETHOD(name) void* yinyue200_GetProductRecord##name(void* obj)\
{\
PRODUCTRECORD_PTR p = obj;\
return &p->name;\
}
#include "ProductRecord.h"

PRODUCTRECORD_PTR CreateProductRecord()
{
	PRODUCTRECORD_PTR PT = malloc(sizeof(PRODUCTRECORD));
	if (PT == NULL)
		return PT;
	memset(PT, 0, sizeof(PRODUCTRECORD));
	return PT;
}
bool WritePWSTR(PWSTR str, HANDLE hFile)
{
	size_t len = wcslen(str);
	size_t utf8len = len * sizeof(wchar_t) * 2;
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
		int reason = GetLastError();
		free(utf8bytes);
		return false;
	}
	else
	{
		return false;
	}

}
bool yinyue200_ProductRecordSaveToFile(LPWSTR path, vector* vec)
{
	HANDLE hFile = CreateFile(path,               // file to open
		GENERIC_READ|GENERIC_WRITE,          // open for reading
		0,       // share for reading
		NULL,                  // default security
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, // normal file
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		int errcod = GetLastError();
		MessageBox(NULL, L"Save Failed!", NULL, 0);
		return false;
	}
	size_t length = VECTOR_TOTAL(*vec);
	for (size_t i = 0; i < length; i++)
	{
		PRODUCTRECORD_PTR record = VECTOR_GET(*vec, PRODUCTRECORD_PTR, i);
		FailedIfFalse(WritePWSTR(record->Name, hFile));
		FailedIfFalse(WritePWSTR(L"\t",hFile));
		wchar_t idbuffer[30];
		swprintf_s(idbuffer, 30, L"%lld", record->ID);
		FailedIfFalse(WritePWSTR(idbuffer, hFile));
		FailedIfFalse(WritePWSTR(L"\t", hFile));



		FailedIfFalse(WritePWSTR(L"\n", hFile));
	}
	CloseHandle(hFile);
}
vector* ProductRecordLoadToVector(LPWSTR path)
{
	//FILE SHOULD BE UTF-8 ENCODED
	vector* vec = malloc(sizeof(vector));
	if (vec)
	{
		vector_init(vec);

		HANDLE hFile = CreateFile(path,               // file to open
			GENERIC_READ,          // open for reading
			FILE_SHARE_READ,       // share for reading
			NULL,                  // default security
			OPEN_EXISTING,         // existing file only
			FILE_ATTRIBUTE_NORMAL, // normal file
			NULL);                 // no attr. template
		if (hFile == INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			return vec;//找不到文件，返回空 vector
		}

		LARGE_INTEGER FILESIZEINFO;
		GetFileSizeEx(hFile, &FILESIZEINFO);
		if (FILESIZEINFO.QuadPart == 0)
		{
			CloseHandle(hFile);
			return vec;
		}
		char* data = malloc(FILESIZEINFO.QuadPart);
		if (data)
		{
			DWORD readsize;

			BOOL readfilesucc = ReadFile(hFile, data, FILESIZEINFO.QuadPart, &readsize, NULL);

			if (readfilesucc)
			{
				int laststart = 0;
				int tindex = 0;
				PRODUCTRECORD_PTR p = CreateProductRecord();
				if (p)
				{
					for (size_t i = 0; i < FILESIZEINFO.QuadPart; i++)
					{
						char one = data[i];
						size_t size = i - laststart;
						if (one == '\t' || one == '\r' || one == '\n')
						{
							if (size > 0)
							{
								PWCHAR info = malloc(size * 2);
								if (info == NULL)
								{
									UnrecoveryableFailed();
									return NULL;
								}
								memset(info, 0, size * 2);
								MultiByteToWideChar(CP_UTF8, 0, &data[laststart], size, info, size);
								switch (tindex)
								{
								case 0:
									p->Name = info;
								case 1:
								{
									int64_t id;
									if (swscanf(info, L"%lld", &id) == 1)
									{
										p->ID = id;
									}
								}
									//p->ID = info;
								default:
									break;
								}
							}
							tindex++;
							laststart = i;
						}
						if (one == '\r' || one == '\n')
						{
							laststart = i;
							if (size > 0)
							{
								tindex = 0;
								VECTOR_ADD(*vec, p);
								p = CreateProductRecord();
								if (p == NULL)
								{
									UnrecoveryableFailed();
								}
							}
						}
					}
				}
				else
				{
					UnrecoveryableFailed();
				}
				free(p);
			}
			else
			{
				UnrecoveryableFailed();
			}
			free(data);
		}
		else
		{
			UnrecoveryableFailed();
		}
		CloseHandle(hFile);
	}
	else
	{
		UnrecoveryableFailed();
	}
	return vec;
}
DEFINE_GETMEMBERMETHOD(Name);
DEFINE_GETMEMBERADDRMETHOD(ID);