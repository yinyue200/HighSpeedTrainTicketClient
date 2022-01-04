#pragma once
//  SimpleStoreErp
//	Copyright(C) 2021 殷越
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
//构造获取 ProductRecord 成员的函数声明的宏
#define YINYUE200_DEFINESIG_GETMEMBERMETHOD(name) void* yinyue200_GetTrainPlanRecord##name(void* obj);
//车次状态（暂时未使用）
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
	uint64_t RouteRunTimeSpan;//预计从起点站到本站的时间
	double Distance;//起点站至本站里程，以千米为单位，用于票价计算
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
	int64_t Repeat;//表示每多少天开行一次本车次列车//6
	vector RoutePoints;//表示行车路线，YINYUE200_TRAINPLANRECORD_ROUTEPOINT 类型
	vector TicketCount;//类型 YINYUE200_PAIR_OF_wchar_t_int32_t
	uint64_t StartTimePoint;//车次首次开行日期时间
	wchar_t* PricePolicy;//表示本车次是否应用递远递减等特殊票价计算规则
						 //实际票价计算相当复杂，见参考资料
						 //目前的设计是一种简化设计
						 //实际票价计算规则以中国国家铁路集团有限公司及有关单位相关规定为准
						 //参考资料： 中国铁道出版社 （2021新版）铁路客运运价里程表
						 //中国铁道出版社 全国铁路客运运价里程接算站示意图 
} YINYUE200_TRAINPLANRECORD;
typedef YINYUE200_TRAINPLANRECORD* YINYUE200_TRAINPLANRECORD_PTR;
//创建记录，并传递所有权（调用者负责free）
YINYUE200_TRAINPLANRECORD_PTR CreateTrainPlanRecord();
//读取记录信息
vector* ProductRecordLoadToVector(LPWSTR path);
//写入记录到文件
bool yinyue200_ProductRecordSaveToFile(LPWSTR path, vector* vec);
//写入宽字符串到文件
bool WritePWSTR(PCWSTR str, HANDLE hFile);

//=======构造获取 TrainPlanRecord 成员的函数声明=======
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

