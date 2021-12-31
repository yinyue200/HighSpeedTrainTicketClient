#pragma once
//  SimpleStoreErp
//	Copyright(C) 2021 ��Խ
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
#include <Windows.h>
#include <stdint.h>
#include "common.h"
//�����ȡ ProductRecord ��Ա�ĺ��������ĺ�
#define YINYUE200_DEFINESIG_GETMEMBERMETHOD(name) void* yinyue200_GetTrainPlanRecord##name(void* obj);
//����״̬����ʱδʹ�ã�
enum TrainPlanState
{
	TRAINPLANSTATE_UNKNOWN=0,
};
//������Ϣ��Ϣ
typedef struct TrainPlanRecord
{
	//������Ϣ
	wchar_t* Name;//0
	//ΨһID
	YINYUE200_PAIR_OF_uint64_t_uint64_t ID;//1
	wchar_t* Type;//2
	wchar_t* State;//3
	double Price;//5
	int64_t Repeat;//6
	vector Sations;
	vector TicketCount;

} TRAINPLANRECORD;
typedef TRAINPLANRECORD* TRAINPLANRECORD_PTR;
//������¼������������Ȩ�������߸���free��
TRAINPLANRECORD_PTR CreateTrainPlanRecord();
//��ȡ��¼��Ϣ
vector* ProductRecordLoadToVector(LPWSTR path);
//д���¼���ļ�
bool yinyue200_ProductRecordSaveToFile(LPWSTR path, vector* vec);
//д����ַ������ļ�
bool WritePWSTR(PCWSTR str, HANDLE hFile);

//=======�����ȡ TrainPlanRecord ��Ա�ĺ�������=======
YINYUE200_DEFINESIG_GETMEMBERMETHOD(Name)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(ID)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(Type)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(State)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(Price)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(Sations)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(TicketCount)
//========

