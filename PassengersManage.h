//  HighSpeedTrainTicketClient
//  Copyright(C) 2022 殷越
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
  PWCHAR IDType;//0
  PWCHAR IDNumber;//1
  PWCHAR FullName;//2
  PWCHAR Owner;//3
  PWCHAR PhoneNum;//4
  PWCHAR EmergencyContactPersonFullName;//5
  PWCHAR EmergencyContactPhoneNumber;//6

  bool deled;//不序列化
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
//构造获取 YINYUE200_PASSENGERINFO 成员的函数定义的宏
#define YINYUE200_PASSENGERINFO_DEFINE_GETMEMBERMETHOD(name) inline void* yinyue200_GetPassengerInfo##name(void* obj)\
{\
YINYUE200_PASSENGERINFO_PTR p = obj;\
return p->name;\
}
//构造获取 YINYUE200_PASSENGERINFO 成员的地址的函数定义的宏
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