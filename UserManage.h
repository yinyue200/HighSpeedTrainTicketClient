//  SimpleStoreErp
//	Copyright(C) 2021 “Û‘Ω
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
#pragma once
#include <windows.h>
#include <stdbool.h>
#include "vector.h"
typedef struct UserDataInfo
{
	PWCHAR Name;
	PWCHAR PasswordHash;
	PWCHAR Type;
} USERDATAINFO;
typedef USERDATAINFO* USERDATAINFO_PTR;
bool yinyue200_UserRecordSaveToFile(LPWSTR path, vector* vec);
vector* UserRecordLoadToVector(LPWSTR path);
void Hash256LPWSTR(LPWSTR str, wchar_t buf[65]);