#pragma once
//  HighSpeedTrainTicketClient
//  Copyright(C) 2022 殷越
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
//构造获取 TrainPlanRecord 成员的函数定义的宏
#define YINYUE200_TRAINPLANRECORD_DEFINE_GETMEMBERMETHOD(name) inline void* yinyue200_GetTrainPlanRecord##name(void* obj)\
{\
YINYUE200_TRAINPLANRECORD_PTR p = obj;\
return p->name;\
}
//构造获取 TrainPlanRecord 成员的地址的函数定义的宏
#define YINYUE200_TRAINPLANRECORD_DEFINE_GETMEMBERADDRMETHOD(name) inline void* yinyue200_GetTrainPlanRecord##name(void* obj)\
{\
YINYUE200_TRAINPLANRECORD_PTR p = obj;\
return &p->name;\
}
//车次状态（暂时未使用）
enum TrainPlanState
{
    TRAINPLANSTATE_UNKNOWN=0,
};
#define TRAUBPLANSTATEWSTR_NORMAL TEXT("正常")
#define TRAUBPLANSTATEWSTR_STOP TEXT("停运")
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
    uint64_t RouteRunTimeSpan;//预计从起点站到本站的时间
    uint64_t Distance;//起点站至本站里程，以米为单位，用于票价计算
    uint64_t ResidenceTime;//在该路径点的停留时间
} YINYUE200_TRAINPLANRECORD_ROUTEPOINT;
typedef YINYUE200_TRAINPLANRECORD_ROUTEPOINT* YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR;
//车次信息信息
typedef struct Yinyue200_TrainPlanRecord
{
    //车次信息
    wchar_t* Name;//0
    //唯一ID
    YINYUE200_PAIR_OF_uint64_t_uint64_t ID;//1
    wchar_t* Type;//2
    wchar_t* State;//表示车次当前状态（正常或停开）//3
    int64_t Repeat;//表示每多少天开行一次本车次列车//4
    vector RoutePoints;//表示行车路线，YINYUE200_TRAINPLANRECORD_ROUTEPOINT 类型//5
    vector TicketCount;//类型 YINYUE200_PAIR_OF_int32_t_int32_t//6
    uint64_t StartTimePoint;//车次首次开行日期时间//7
    //wchar_t* PricePolicy;//表示本车次是否应用递远递减等特殊票价计算规则
                         //实际票价计算相当复杂，见参考资料
                         //目前的设计是一种简化设计
                         //实际票价计算规则以中国国家铁路集团有限公司及有关单位相关规定为准
                         //参考资料： 中国铁道出版社 （2021新版）铁路客运运价里程表
                         //中国铁道出版社 全国铁路客运运价里程接算站示意图 
} YINYUE200_TRAINPLANRECORD;
typedef YINYUE200_TRAINPLANRECORD* YINYUE200_TRAINPLANRECORD_PTR;
YINYUE200_TRAINPLANRECORD_PTR Yinyue200_GetTrainPlanRecordByTrainID(YINYUE200_PAIR_OF_uint64_t_uint64_t ID);
void Yinyue200_InitTrainPlanRecordIndexs();
YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR Yinyue200_GetTrainPlanRecordRoutePointFromStationDisplayName(YINYUE200_TRAINPLANRECORD_PTR plan, PWSTR station);
void AddTrainPlanRecord(YINYUE200_TRAINPLANRECORD_PTR record);
void RemoveTrainPlanRecord(YINYUE200_TRAINPLANRECORD_PTR record);
//创建记录，并传递所有权（调用者负责free）
YINYUE200_TRAINPLANRECORD_PTR CreateTrainPlanRecord();
void freeTrainPlanRecord(YINYUE200_TRAINPLANRECORD_PTR record);
void freeTrainPlanRecord_RoutePoints(vector* vec);
// 假定 srcvec 是未初始化的 vector
void deepcopy_TrainPlanRecord_RoutePoints(vector* srcvec, vector* orivec);
//读取记录信息
vector* TrainPlanRecordLoadToVector(LPWSTR path);
//写入记录到文件
bool yinyue200_TrainPlanRecordSaveToFile(LPWSTR path, vector* vec);

PWSTR ConvertToStringFrom_Yinyue200_TrainPlanRecord_RoutePoint(YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR routepoint);
PWSTR ConvertToStringFrom_YINYUE200_PAIR_OF_int32_t_int32_t(YINYUE200_PAIR_OF_int32_t_int32_t *routepoint);
YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR ConvertStringToYinyue200_TrainPlanRecord_RoutePoint(PWSTR str);
YINYUE200_PAIR_OF_int32_t_int32_t* ConvertStringToYINYUE200_PAIR_OF_int32_t_int32_t(PWSTR str);
//该函数检查指定日期是否开行指定车次
//传入的时间是本地时间
bool Yinyue200_CheckTrainPlanRecordDate(YINYUE200_TRAINPLANRECORD_PTR record, uint64_t time);

int32_t* Yinyue200_GetTrainPlanRecordSeatCountPointer(YINYUE200_TRAINPLANRECORD_PTR record, enum TrainSeatType type);
int32_t Yinyue200_GetTrainPlanRecordSeatCount(YINYUE200_TRAINPLANRECORD_PTR record, enum TrainSeatType type);
void Yinyue200_SetTrainPlanRecordSeatCount(YINYUE200_TRAINPLANRECORD_PTR record, enum TrainSeatType type, int32_t value);
int32_t GetSeatCountOfAllTypeOfSeat(YINYUE200_TRAINPLANRECORD_PTR record);
uint64_t Yinyue200_GetLocalTrainStartTimePoint(YINYUE200_TRAINPLANRECORD_PTR train);
/// <summary>
/// 该函数获取车次开行至指定日期所经过的天数
/// </summary>
/// <param name="record"></param>
/// <param name="filetime">传入的时间是本地时间</param>
/// <returns></returns>
int Yinyue200_GetTrainPlanRecordCreatedTotalDateFromLocalUINT64(YINYUE200_TRAINPLANRECORD_PTR record, uint64_t filetime);

inline uint64_t GetTrainPlanRecordRoutePointStartedTime(YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR record)
{
    return record->RouteRunTimeSpan + record->ResidenceTime;
}

//=======构造获取 TrainPlanRecord 成员的函数声明=======
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

inline uint64_t yinyue200_GetTrainPlanRecordTotalDistance(YINYUE200_TRAINPLANRECORD_PTR obj)
{
    vector* vec = &obj->RoutePoints;
    int total = vector_total(vec);
    if (total > 1)
    {
        YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR routepoint = vector_get(vec, total - 1);
        return routepoint->Distance;
    }
    else
    {
        return 0;
    }
}

inline int yinyue200_GetTrainPlanRecordStationCount(YINYUE200_TRAINPLANRECORD_PTR obj)
{
    vector* vec = &obj->RoutePoints;
    return vector_total(vec);
}

inline FILETIME yinyue200_GetTrainPlanRecordLocalArrTime(YINYUE200_TRAINPLANRECORD_PTR obj)
{
    FILETIME utcarrtime = Yinyue200_ConvertToFileTimeFromUINT64(obj->StartTimePoint);
    int totalstations = vector_total(&obj->RoutePoints);
    if (totalstations > 1)
    {
        YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR endstation = vector_get(&obj->RoutePoints, totalstations - 1);
        utcarrtime = Yinyue200_ConvertToFileTimeFromUINT64(Yinyue200_ConvertToUINT64FromFileTime(utcarrtime) + endstation->RouteRunTimeSpan);
    }
    FILETIME localarrtime;
    Yinyue200_FileTimeToLocalFileTime(&utcarrtime, &localarrtime);
    return localarrtime;
}

inline uint64_t yinyue200_GetTrainPlanRecordLocalArrOnlyTimeUINT64(YINYUE200_TRAINPLANRECORD_PTR obj)
{
    return GetTimePartUINT64OFUINT64(Yinyue200_ConvertToUINT64FromFileTime(yinyue200_GetTrainPlanRecordLocalArrTime(obj)));
}

//========

