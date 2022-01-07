#pragma once
//  HighSpeedTrainTicketClient
//  Copyright(C) 2022 ��Խ
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
#include <Windows.h>
#include <stdint.h>
#include "vector.h"
#include "common.h"
//�����ȡ ProductRecord ��Ա�ĺ�������ĺ�
#define YINYUE200_TRAINPLANRECORD_DEFINE_GETMEMBERMETHOD(name) inline void* yinyue200_GetTrainPlanRecord##name(void* obj)\
{\
YINYUE200_TRAINPLANRECORD_PTR p = obj;\
return p->name;\
}
//�����ȡ ProductRecord ��Ա�ĵ�ַ�ĺ�������ĺ�
#define YINYUE200_TRAINPLANRECORD_DEFINE_GETMEMBERADDRMETHOD(name) inline void* yinyue200_GetTrainPlanRecord##name(void* obj)\
{\
YINYUE200_TRAINPLANRECORD_PTR p = obj;\
return &p->name;\
}
//����״̬����ʱδʹ�ã�
enum TrainPlanState
{
    TRAINPLANSTATE_UNKNOWN=0,
};
#define TRAUBPLANSTATEWSTR_NORMAL TEXT("����")
#define TRAUBPLANSTATEWSTR_STOP TEXT("ͣ��")
enum TrainSeatType
{
    TRAINTICKETTYPE_UNKNOWN = 0,
    TRAINTICKETTYPE_FIRSTCLASS = 1,
    TRAINTICKETTYPE_SECONDCLASS = 2,
    TRAINTICKETTYPE_BUSINESSCLASS = 3
};
typedef struct Yinyue200_StationInfo
{
    wchar_t* DisplayName;
} YINYUE200_STATIONINFO;
typedef struct Yinyue200_TrainPlanRecord_RoutePoint
{
    YINYUE200_STATIONINFO Station;
    uint64_t RouteRunTimeSpan;//Ԥ�ƴ����վ����վ��ʱ��
    uint64_t Distance;//���վ����վ��̣�����Ϊ��λ������Ʊ�ۼ���
    uint64_t ResidenceTime;//�ڸ�·�����ͣ��ʱ��
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
    int64_t Repeat;//��ʾÿ�����쿪��һ�α������г�//4
    vector RoutePoints;//��ʾ�г�·�ߣ�YINYUE200_TRAINPLANRECORD_ROUTEPOINT ����//5
    vector TicketCount;//���� YINYUE200_PAIR_OF_int32_t_int32_t//6
    uint64_t StartTimePoint;//�����״ο�������ʱ��//7
    //wchar_t* PricePolicy;//��ʾ�������Ƿ�Ӧ�õ�Զ�ݼ�������Ʊ�ۼ������
                         //ʵ��Ʊ�ۼ����൱���ӣ����ο�����
                         //Ŀǰ�������һ�ּ����
                         //ʵ��Ʊ�ۼ���������й�������·�������޹�˾���йص�λ��ع涨Ϊ׼
                         //�ο����ϣ� �й����������� ��2021�°棩��·�����˼���̱�
                         //�й����������� ȫ����·�����˼���̽���վʾ��ͼ 
} YINYUE200_TRAINPLANRECORD;
typedef YINYUE200_TRAINPLANRECORD* YINYUE200_TRAINPLANRECORD_PTR;
YINYUE200_TRAINPLANRECORD_PTR Yinyue200_GetTrainPlanRecordByTrainID(YINYUE200_PAIR_OF_uint64_t_uint64_t ID);
void Yinyue200_InitTrainPlanRecordIndexs();
YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR Yinyue200_GetTrainPlanRecordRoutePointFromStationDisplayName(YINYUE200_TRAINPLANRECORD_PTR plan, PWSTR station);
void AddTrainPlanRecord(YINYUE200_TRAINPLANRECORD_PTR record);
void RemoveTrainPlanRecord(YINYUE200_TRAINPLANRECORD_PTR record);
//������¼������������Ȩ�������߸���free��
YINYUE200_TRAINPLANRECORD_PTR CreateTrainPlanRecord();
void freeTrainPlanRecord(YINYUE200_TRAINPLANRECORD_PTR record);
void freeTrainPlanRecord_RoutePoints(vector* vec);
// �ٶ� srcvec ��δ��ʼ���� vector
void deepcopy_TrainPlanRecord_RoutePoints(vector* srcvec, vector* orivec);
//��ȡ��¼��Ϣ
vector* ProductRecordLoadToVector(LPWSTR path);
//д���¼���ļ�
bool yinyue200_ProductRecordSaveToFile(LPWSTR path, vector* vec);

PWSTR ConvertToStringFrom_Yinyue200_TrainPlanRecord_RoutePoint(YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR routepoint);
PWSTR ConvertToStringFrom_YINYUE200_PAIR_OF_int32_t_int32_t(YINYUE200_PAIR_OF_int32_t_int32_t *routepoint);
YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR ConvertStringToYinyue200_TrainPlanRecord_RoutePoint(PWSTR str);
YINYUE200_PAIR_OF_int32_t_int32_t* ConvertStringToYINYUE200_PAIR_OF_int32_t_int32_t(PWSTR str);
//�ú������ָ�������Ƿ���ָ������
//�����ʱ���Ǳ���ʱ��
bool Yinyue200_CheckTrainPlanRecordDate(YINYUE200_TRAINPLANRECORD_PTR record, uint64_t time);

int32_t* Yinyue200_GetTrainPlanRecordSeatCountPointer(YINYUE200_TRAINPLANRECORD_PTR record, enum TrainSeatType type);
int32_t Yinyue200_GetTrainPlanRecordSeatCount(YINYUE200_TRAINPLANRECORD_PTR record, enum TrainSeatType type);
void Yinyue200_SetTrainPlanRecordSeatCount(YINYUE200_TRAINPLANRECORD_PTR record, enum TrainSeatType type, int32_t value);
int32_t GetSeatCountOfAllTypeOfSeat(YINYUE200_TRAINPLANRECORD_PTR record);
uint64_t Yinyue200_GetLocalTrainStartTimePoint(YINYUE200_TRAINPLANRECORD_PTR train);
/// <summary>
/// �ú�����ȡ���ο�����ָ������������������
/// </summary>
/// <param name="record"></param>
/// <param name="filetime">�����ʱ���Ǳ���ʱ��</param>
/// <returns></returns>
int Yinyue200_GetTrainPlanRecordCreatedTotalDateFromLocalUINT64(YINYUE200_TRAINPLANRECORD_PTR record, uint64_t filetime);

inline uint64_t GetTrainPlanRecordRoutePointStartedTime(YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR record)
{
    return record->RouteRunTimeSpan + record->ResidenceTime;
}

//=======�����ȡ TrainPlanRecord ��Ա�ĺ�������=======
YINYUE200_TRAINPLANRECORD_DEFINE_GETMEMBERMETHOD(Name)
YINYUE200_TRAINPLANRECORD_DEFINE_GETMEMBERADDRMETHOD(ID)
YINYUE200_TRAINPLANRECORD_DEFINE_GETMEMBERMETHOD(Type)
YINYUE200_TRAINPLANRECORD_DEFINE_GETMEMBERMETHOD(State)
YINYUE200_TRAINPLANRECORD_DEFINE_GETMEMBERADDRMETHOD(RoutePoints)
YINYUE200_TRAINPLANRECORD_DEFINE_GETMEMBERADDRMETHOD(TicketCount)
YINYUE200_TRAINPLANRECORD_DEFINE_GETMEMBERADDRMETHOD(StartTimePoint)
YINYUE200_TRAINPLANRECORD_DEFINE_GETMEMBERADDRMETHOD(Repeat)

inline PWSTR yinyue200_GetTrainPlanRecordStartStationForDisplay(YINYUE200_TRAINPLANRECORD_PTR obj)
{
    vector* vec = &obj->RoutePoints;
    int total = vector_total(vec);
    if (total > 1)
    {
        YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR routepoint = vector_get(vec, 0);
        return routepoint->Station.DisplayName;
    }
    else
    {
        return L"NULL";
    }
}

inline PWSTR yinyue200_GetTrainPlanRecordEndStationForDisplay(YINYUE200_TRAINPLANRECORD_PTR obj)
{
    vector* vec = &obj->RoutePoints;
    int total = vector_total(vec);
    if (total > 1)
    {
        YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR routepoint = vector_get(vec, total - 1);
        return routepoint->Station.DisplayName;
    }
    else
    {
        return L"NULL";
    }
}

//========

