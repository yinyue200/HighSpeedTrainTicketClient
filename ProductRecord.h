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
typedef struct Yinyue200_StationInfo
{
	wchar_t* DisplayName;
} YINYUE200_STATIONINFO;
typedef struct Yinyue200_TrainPlanRecord_RoutePoint
{
	YINYUE200_STATIONINFO Station;
	uint64_t RouteRunTimeSpan;//Ԥ�ƴ����վ����վ��ʱ��
	double Distance;//��һվ����վ��̣���ǧ��Ϊ��λ�����ڵ�Զ�ݼ�������Ʊ�ۼ������ļ���
} YINYUE200_TRAINPLANRECORD_ROUTEPOINT;
typedef YINYUE200_TRAINPLANRECORD_ROUTEPOINT* YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR;
//������Ϣ��Ϣ
typedef struct Yinyue200_TrainPlanRecord
{
	//������Ϣ
	wchar_t* Name;//0
	//ΨһID
	YINYUE200_PAIR_OF_uint64_t_uint64_t ID;//1
	wchar_t* Type;//2
	wchar_t* State;//��ʾ���ε�ǰ״̬��������ͣ����//3
	int64_t Repeat;//��ʾÿ�����쿪��һ�α������г�//6
	vector RoutePoints;//��ʾ�г�·�ߣ�YINYUE200_TRAINPLANRECORD_ROUTEPOINT ����
	vector TicketCount;//���� YINYUE200_PAIR_OF_wchar_t_int32_t
	uint64_t StartTimePoint;//�����״ο�������ʱ��
	wchar_t* PricePolicy;//��ʾ�������Ƿ�Ӧ�õ�Զ�ݼ�������Ʊ�ۼ������
						 //ʵ��Ʊ�ۼ����൱���ӣ����ο�����
						 //Ŀǰ������Ǿ�������ʹ����������ڲ��޸ĳ������ǰ�������κ�Ʊ�ۼ�������¹���
						 //ʵ��Ʊ�ۼ���������й�������·�������޹�˾���йص�λ��ع涨Ϊ׼
						 //�ο����ϣ� �й����������� ��2021�°棩��·�����˼���̱�
						 //�й����������� ȫ����·�����˼���̽���վʾ��ͼ 
} YINYUE200_TRAINPLANRECORD;
typedef YINYUE200_TRAINPLANRECORD* YINYUE200_TRAINPLANRECORD_PTR;
//������¼������������Ȩ�������߸���free��
YINYUE200_TRAINPLANRECORD_PTR CreateTrainPlanRecord();
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
YINYUE200_DEFINESIG_GETMEMBERMETHOD(RoutePoints)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(TicketCount)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(StartTimePoint)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(PricePolicy)
YINYUE200_DEFINESIG_GETMEMBERMETHOD(Repeat)

//========

