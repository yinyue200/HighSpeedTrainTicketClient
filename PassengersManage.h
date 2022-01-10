//  HighSpeedTrainTicketClient
//  Copyright(C) 2022 ��Խ
//
//  This program is free software : you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program. If not, see <https://www.gnu.org/licenses/>.
#pragma once
#include "common.h"
typedef struct Yinyue200_PassengerInfo
{
	/// <summary>
	///֤������
	/// </summary>
	PWSTR IDType;//0
	/// <summary>
	/// ֤����
	/// </summary>
	PWSTR IDNumber;//1
	/// <summary>
	/// ����
	/// </summary>
	PWSTR FullName;//2
	/// <summary>
	/// ������
	/// </summary>
	PWSTR Owner;//3
	/// <summary>
	/// �ֻ���
	/// </summary>
	PWSTR PhoneNum;//4
	/// <summary>
	/// ������ϵ������
	/// </summary>
	PWSTR EmergencyContactPersonFullName;//5
	/// <summary>
	/// ������ϵ����ϵ��ʽ
	/// </summary>
	PWSTR EmergencyContactPhoneNumber;//6

	bool deled;//�����л�
} YINYUE200_PASSENGERINFO;
typedef YINYUE200_PASSENGERINFO* YINYUE200_PASSENGERINFO_PTR;
void Yinyue200_InitFullListOfPassengersIfNeed();
YINYUE200_PASSENGERINFO_PTR CreatePassengerInfo();
vector* GetFullListOfPassengerInfo();
vector CreateFullListOfPassengerInfoRefWithOwner(PWCHAR owner);
vector AddPassenger(YINYUE200_PASSENGERINFO_PTR newpassenger);
vector DeletePassenger(YINYUE200_PASSENGERINFO_PTR tobedel);
void FreePassengerInfo(YINYUE200_PASSENGERINFO_PTR record);
bool yinyue200_PassengerInfoSaveToFile(LPWSTR path, vector* vec);
bool yinyue200_MemoryPassengerInfoSaveToFile();
bool CheckIfThereAreAnyPassengerInfoRefWithOwner(PWCHAR owner);
//�����ȡ YINYUE200_PASSENGERINFO ��Ա�ĺ�������ĺ�
#define YINYUE200_PASSENGERINFO_DEFINE_GETMEMBERMETHOD(name) inline void* yinyue200_GetPassengerInfo##name(void* obj)\
{\
YINYUE200_PASSENGERINFO_PTR p = obj;\
return p->name;\
}
//�����ȡ YINYUE200_PASSENGERINFO ��Ա�ĵ�ַ�ĺ�������ĺ�
#define YINYUE200_PASSENGERINFO_DEFINE_GETMEMBERADDRMETHOD(name) inline void* yinyue200_GetPassengerInfo##name(void* obj)\
{\
YINYUE200_PASSENGERINFO_PTR p = obj;\
return &p->name;\
}

YINYUE200_PASSENGERINFO_DEFINE_GETMEMBERMETHOD(IDType)
YINYUE200_PASSENGERINFO_DEFINE_GETMEMBERMETHOD(IDNumber)
YINYUE200_PASSENGERINFO_DEFINE_GETMEMBERMETHOD(FullName)
YINYUE200_PASSENGERINFO_DEFINE_GETMEMBERMETHOD(Owner)
YINYUE200_PASSENGERINFO_DEFINE_GETMEMBERMETHOD(PhoneNum)
YINYUE200_PASSENGERINFO_DEFINE_GETMEMBERMETHOD(EmergencyContactPersonFullName)
YINYUE200_PASSENGERINFO_DEFINE_GETMEMBERMETHOD(EmergencyContactPhoneNumber)