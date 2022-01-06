//  SimpleStoreErp
//  Copyright(C) 2021 ��Խ
//
//  This program is free software : you can redistribute it and /or modify
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
#include <windows.h>
#include <stdbool.h>
#include "vector.h"
//�洢�û���Ϣ
typedef struct UserDataInfo
{
    //�û���
    PWCHAR Name;
    //���� SHA-256
    PWCHAR PasswordHash;
    //�û����� ADMIN/NORMAL
    PWCHAR Type;
} USERDATAINFO;
typedef USERDATAINFO* USERDATAINFO_PTR;
//д���û���Ϣ
bool yinyue200_UserRecordSaveToFile(LPWSTR path, vector* vec);
//��ȡ�û���Ϣ
vector* UserRecordLoadToVector(LPWSTR path);
//�Կ��ַ����� SHA-256 ����
void Hash256LPWSTR(LPWSTR str, wchar_t buf[65]);