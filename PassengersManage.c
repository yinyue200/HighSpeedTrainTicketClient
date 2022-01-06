//  HighSpeedTrainTicketClient
//	Copyright(C) 2022 殷越
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
#include "PassengersManage.h"
#include "UserSetting.h"
#include "HashMap.h"
static vector* yinyue200_FullListOfPassengers = NULL;
static HASHMAP* yinyue200_Passengers_OwnerIndexed = NULL;
//caseid是信息列数（0开始） member是成员名称
#define LOADWSTRDATATOVECTOR(member,caseid) case caseid:\
{\
	p->##member = info;\
	break;\
}
vector* LoadPassengerInfoFromFile(PWSTR path)
{
	//FILE SHOULD BE UTF-8 ENCODED
	vector* vec = yinyue200_safemalloc(sizeof(vector));
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
	char* data = yinyue200_safemalloc(FILESIZEINFO.QuadPart);
	DWORD readsize;

	BOOL readfilesucc = ReadFile(hFile, data, FILESIZEINFO.QuadPart, &readsize, NULL);

	if (readfilesucc)
	{
		int laststart = 0;
		int tindex = 0;
		BOOL lastisbl = FALSE;
		YINYUE200_PASSENGERINFO_PTR p = CreatePassengerInfo();
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
							LOADWSTRDATATOVECTOR(IDType,0)
							LOADWSTRDATATOVECTOR(IDNumber, 1)
							LOADWSTRDATATOVECTOR(FullName, 2)
							LOADWSTRDATATOVECTOR(Owner, 3)
							LOADWSTRDATATOVECTOR(PhoneNum, 4)
							LOADWSTRDATATOVECTOR(EmergencyContactPersonFullName, 5)
							LOADWSTRDATATOVECTOR(EmergencyContactPhoneNumber, 6)
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
	CloseHandle(hFile);
	return vec;
}
PWSTR yinyue200_GetOwnerFromIndex(void* index)
{
	return yinyue200_GetPassengerInfoOwner(vector_get(yinyue200_FullListOfPassengers, index));
}
void Yinyue200_InitFullListOfPassengersIfNeed()
{
	if (yinyue200_FullListOfPassengers == NULL)
	{
		yinyue200_FullListOfPassengers = LoadPassengerInfoFromFile(yinyue200_GetPassengerInfoConfigFilePath());

		//构建 owner 索引
		yinyue200_Passengers_OwnerIndexed = yinyue200_safemalloc(sizeof(HASHMAP));
		*yinyue200_Passengers_OwnerIndexed = HashMap_Create(vector_total(yinyue200_FullListOfPassengers), xxHashPWSTR, ComparePWSTR, yinyue200_GetOwnerFromIndex, HashSet_NoFree);
		for (int i = 0; i < vector_total(yinyue200_FullListOfPassengers); i++)
		{
			HashMap_Add(yinyue200_Passengers_OwnerIndexed, i);
		}
	}
}
void Yinyue200_FreeFullListOfPassengers()
{
	if (yinyue200_FullListOfPassengers)
	{
		for (int i = 0; i < vector_total(yinyue200_FullListOfPassengers); i++)
		{
			FreePassengerInfo(vector_get(yinyue200_FullListOfPassengers, i));
		}
		vector_free(yinyue200_FullListOfPassengers);
		yinyue200_FullListOfPassengers = NULL;


		HashMap_Free(yinyue200_Passengers_OwnerIndexed);
		free(yinyue200_Passengers_OwnerIndexed);
		yinyue200_Passengers_OwnerIndexed = NULL;
	}
}
YINYUE200_PASSENGERINFO_PTR CreatePassengerInfo()
{
	YINYUE200_PASSENGERINFO_PTR nn = yinyue200_safemallocandclear(sizeof(YINYUE200_PASSENGERINFO));
	return nn;
}
void FreePassengerInfo(YINYUE200_PASSENGERINFO_PTR record)
{
	free(record->IDType);
	free(record->IDNumber);
	free(record->FullName);
	free(record->Owner);
	free(record->PhoneNum);
	free(record->EmergencyContactPersonFullName);
	free(record->EmergencyContactPhoneNumber);
	free(record);
}
vector* GetFullListOfPassengerInfo()
{
	return yinyue200_FullListOfPassengers;
}

vector CreateFullListOfPassengerInfoRefWithOwner(PWCHAR owner)
{
	size_t maxposs;
	HASHMAPNODE* node = HashMap_GetPointersByKey(yinyue200_Passengers_OwnerIndexed, owner, NULL, &maxposs);
	vector vec;
	vector_initwithcap(&vec, maxposs);
	while (node)
	{
		YINYUE200_PASSENGERINFO_PTR record = vector_get(yinyue200_FullListOfPassengers, node->value);
		if (record->deled == false)
		{
			vector_add(&vec, record);
		}
		node = HashMap_GetPointersByKey(yinyue200_Passengers_OwnerIndexed, owner, node, NULL);//get next node
	}
	return vec;
}

vector AddPassenger(YINYUE200_PASSENGERINFO_PTR newpassenger)
{
	vector_add(&yinyue200_FullListOfPassengers, newpassenger);
	HashMap_Add(yinyue200_Passengers_OwnerIndexed, vector_total(yinyue200_FullListOfPassengers));
}

vector DeletePassenger(YINYUE200_PASSENGERINFO_PTR tobedel)
{
	tobedel->deled = true;
}
