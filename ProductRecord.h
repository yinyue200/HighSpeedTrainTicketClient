#pragma once
//  SimpleStoreErp
//	Copyright(C) 2021 ��Խ
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
//�����ȡ ProductRecord ��Ա�ĺ��������ĺ�
#define YINYUE200_DEFINESIG_GETMEMBERMETHOD(name) void* yinyue200_GetProductRecord##name(void* obj);
//��Ʒ״̬����ʱδʹ�ã�
enum ProductState
{
	PRODUCTSTATE_UNKNOWN=0,
};
//��¼��Ϣ
typedef struct ProductRecord
{
	wchar_t* Name;//0
	int64_t ID;//1
	wchar_t* Type;//3
	wchar_t* State;//4
	int64_t Date;//5
	wchar_t* ProvideBy;//6
	wchar_t* RecievedBy;//6
	wchar_t* ResentBy;//�˻�������//7
	int64_t Count;//8
	double Cost;//����//9
	double Price;//10
	double ResentPrice;//11
	wchar_t* Signer;//12
} PRODUCTRECORD;
typedef  PRODUCTRECORD* PRODUCTRECORD_PTR;
//������¼������������Ȩ�������߸���free��
PRODUCTRECORD_PTR CreateProductRecord();
//��ȡ��¼��Ϣ
vector* ProductRecordLoadToVector(LPWSTR path);
//д���¼���ļ�
bool yinyue200_ProductRecordSaveToFile(LPWSTR path, vector* vec);
//д����ַ������ļ�
bool WritePWSTR(PCWSTR str, HANDLE hFile);

//=======�����ȡ ProductRecord ��Ա�ĺ�������=======
YINYUE200_DEFINESIG_GETMEMBERMETHOD(Name)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(ID)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(Type)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(State)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(Date)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(ProvideBy)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(RecievedBy)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(ResentBy)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(Count)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(Cost)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(Price)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(ResentPrice)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(Signer)
//========

