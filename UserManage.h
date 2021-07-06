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
void Hash256LPWSTR(LPWSTR str, wchar_t buf[33]);