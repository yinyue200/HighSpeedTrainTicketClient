//  HighSpeedTrainTicketClient
//	Copyright(C) 2022 “Û‘Ω
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
} YINYUE200_PASSENGERINFO;
typedef YINYUE200_PASSENGERINFO* YINYUE200_PASSENGERINFO_PTR;
YINYUE200_PASSENGERINFO_PTR CreatePassengerInfo();
vector* GetFullListOfPassengerInfo();
vector* GetFullListOfPassengerInfoRefWithOwner(PWCHAR owner);
vector AddPassenger(YINYUE200_PASSENGERINFO_PTR newpassenger);
vector DeletePassenger(YINYUE200_PASSENGERINFO_PTR tobedel);
void FreePassengerInfo(YINYUE200_PASSENGERINFO_PTR record);