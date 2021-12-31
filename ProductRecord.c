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
 

//构造获取 ProductRecord 成员的函数定义的宏
#define DEFINE_GETMEMBERMETHOD(name) void* yinyue200_GetTrainPlanRecord##name(void* obj)\
{\
TRAINPLANRECORD_PTR p = obj;\
return p->name;\
}
//构造获取 ProductRecord 成员的地址的函数定义的宏
#define DEFINE_GETMEMBERADDRMETHOD(name) void* yinyue200_GetTrainPlanRecord##name(void* obj)\
{\
TRAINPLANRECORD_PTR p = obj;\
return &p->name;\
}
#include "ProductRecord.h"

TRAINPLANRECORD_PTR CreateTrainPlanRecord()
{
	TRAINPLANRECORD_PTR PT = malloc(sizeof(TRAINPLANRECORD));
	if (PT == NULL)
		return PT;
	memset(PT, 0, sizeof(TRAINPLANRECORD));
	return PT;
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
//caseid是信息列数（0开始） member是成员名称
#define LOADINTDATATOVECTOR(member,caseid) case caseid:\
{\
	int64_t id;\
	if (swscanf(info, L"%lld", &id) == 1)\
	{\
		p->##member = id;\
	}\
	free(info);\
	break;\
}
//caseid是信息列数（0开始） member是成员名称
#define LOADPRICEDATATOVECTOR(member,caseid) case caseid:\
{\
	double id;\
	if (swscanf(info, L"%lf", &id) == 1)\
	{\
		p->##member = id;\
	}\
	free(info);\
	break;\
}
//caseid是信息列数（0开始） member是成员名称
#define LOADWSTRDATATOVECTOR(member,caseid) case caseid:\
{\
	p->##member = info;\
	break;\
}
//member是成员名称
#define SAVEWSTRDATATOVECTOR(member) \
do{\
	FailedIfFalse(WritePWSTR(record->##member, hFile));\
	FailedIfFalse(WritePWSTR(L"\t", hFile));\
}while(0)
//member是成员名称
#define SAVEINTDATATOVECTOR(member) \
do{\
	wchar_t idbuffer[30];\
	swprintf_s(idbuffer, 30, L"%lld", record->##member);\
	FailedIfFalse(WritePWSTR(idbuffer, hFile));\
	FailedIfFalse(WritePWSTR(L"\t", hFile));\
}while(0)
#define SAVEPAIROFUINT64DATATOVECTOR(member) \
do{\
	wchar_t idbuffer[100];\
	swprintf_s(idbuffer, 100, L"%llu;%llu", record->##member.Item1,record->##member.Item2);\
	FailedIfFalse(WritePWSTR(idbuffer, hFile));\
	FailedIfFalse(WritePWSTR(L"\t", hFile));\
}while(0)
//member是成员名称
#define SAVEDOUBLEDATATOVECTOR(member) \
do{\
	wchar_t idbuffer[30];\
	swprintf_s(idbuffer, 30, L"%lf", record->##member);\
	FailedIfFalse(WritePWSTR(idbuffer, hFile));\
	FailedIfFalse(WritePWSTR(L"\t", hFile));\
}while(0)
bool yinyue200_ProductRecordSaveToFile(LPWSTR path, vector* vec)
{
	HANDLE hFile = CreateFile(path,               // file to open
		GENERIC_READ|GENERIC_WRITE,          // open for reading
		0,       // share for reading
		NULL,                  // default security
		CREATE_ALWAYS, //create file always
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
		TRAINPLANRECORD_PTR record = VECTOR_GET(*vec, TRAINPLANRECORD_PTR, i);
		SAVEWSTRDATATOVECTOR(Name);
		SAVEPAIROFUINT64DATATOVECTOR(ID);
		SAVEWSTRDATATOVECTOR(Type);
		SAVEWSTRDATATOVECTOR(State);
		SAVEDOUBLEDATATOVECTOR(Price);


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
				BOOL lastisbl = FALSE;
				TRAINPLANRECORD_PTR p = CreateTrainPlanRecord();
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
								PWCHAR info = malloc(size * 2 + 2);
								if (info == NULL)
								{
									UnrecoveryableFailed();
									return NULL;
								}
								int sizechars = MultiByteToWideChar(CP_UTF8, 0, &data[laststart], size, info, size);
								info[sizechars] = 0;
								switch (tindex)
								{
								case 0:
									p->Name = info;
									break;
								case 1:
								{
									uint64_t id_high;
									uint64_t id_low;
									if (swscanf(info, L"%llu;%llu", &id_high,&id_low) == 1)
									{
										YINYUE200_PAIR_OF_uint64_t_uint64_t pair = { id_high,id_low };
										p->ID = pair;
									}
									free(info);
									break;
								}
									LOADWSTRDATATOVECTOR(Type, 2)
									LOADWSTRDATATOVECTOR(State, 3)
									LOADPRICEDATATOVECTOR(Price, 5)
								default:
									break;
								}
							}
							tindex++;
							laststart = i + 1;
						}
						if (one == '\r' || one == '\n')
						{
							laststart = i + 1;
							tindex = 0;
							if (!lastisbl)
							{			
								VECTOR_ADD(*vec, p);
								p = CreateTrainPlanRecord();
								if (p == NULL)
								{
									UnrecoveryableFailed();
								}
							}
							lastisbl = true;
						}
						else
						{
							lastisbl = false;
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
DEFINE_GETMEMBERMETHOD(Type)
DEFINE_GETMEMBERMETHOD(State)
DEFINE_GETMEMBERADDRMETHOD(Price)