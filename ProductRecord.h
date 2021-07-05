#pragma once
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
#include <Windows.h>
#include <stdint.h>
#include "common.h"
#define YINYUE200_DEFINESIG_GETMEMBERMETHOD(name) void* yinyue200_GetProductRecord##name(void* obj);
enum ProductState
{
	PRODUCTSTATE_UNKNOWN=0,
};
typedef struct ProductRecord
{
	wchar_t* Name;//0
	int64_t ID;//1
	wchar_t* Type;//3
	enum ProductState State;//4
	int64_t Date;//5
	wchar_t* ProvideBy;//6
	wchar_t* RecievedBy;//6
	wchar_t* ResentBy;//退货商名称//7
	int Count;//8
	int64_t Cost;//进价,以分为单位//9
	int64_t Price;//10
	int64_t ResentPrice;//11
	wchar_t* Signer;//12
} PRODUCTRECORD;
typedef  PRODUCTRECORD* PRODUCTRECORD_PTR;
PRODUCTRECORD_PTR CreateProductRecord();
vector* ProductRecordLoadToVector(LPWSTR path);
bool yinyue200_ProductRecordSaveToFile(LPWSTR path, vector* vec);
YINYUE200_DEFINESIG_GETMEMBERMETHOD(Name)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(ID)
