//  SimpleStoreErp
//  Copyright(C) 2021 殷越
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
//存储用户信息
typedef struct UserDataInfo
{
    //用户名
    PWCHAR Name;
    //密码 SHA-256
    PWCHAR PasswordHash;
    //用户类型 ADMIN/NORMAL
    PWCHAR Type;
} USERDATAINFO;
typedef USERDATAINFO* USERDATAINFO_PTR;
//写入用户信息
bool yinyue200_UserRecordSaveToFile(LPWSTR path, vector* vec);
//读取用户信息
vector* UserRecordLoadToVector(LPWSTR path);
//对宽字符串做 SHA-256 运算
void Hash256LPWSTR(LPWSTR str, wchar_t buf[65]);