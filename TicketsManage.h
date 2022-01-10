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
#pragma once
#include "common.h"
#include "PassengersManage.h"
#include "TrainPlanRecord.h"
#include "BitVector.h"
typedef struct Yinyue200_Ticket
{
	/// <summary>
	/// 车票唯一 ID
	/// </summary>
	YINYUE200_PAIR_OF_uint64_t_uint64_t ID;
	/// <summary>
	/// 乘客姓名
	/// </summary>
	PWSTR PassengerName;
	/// <summary>
	/// 乘客证件类型
	/// </summary>
	PWSTR PassengerIDType;
	/// <summary>
	/// 乘客证件号
	/// </summary>
	PWSTR PassengerID;
	/// <summary>
	/// 订票时间 UTC 时间
	/// </summary>
	uint64_t CreatedTime;
	/// <summary>
	/// 车次
	/// </summary>
	PWSTR TrainName;
	/// <summary>
	/// 车次 ID
	/// </summary>
	YINYUE200_PAIR_OF_uint64_t_uint64_t TrainID;
	/// <summary>
	/// UTC 发车时间和日期
	/// </summary>
	uint64_t TrainTime;
	/// <summary>
	/// 本车票起点
	/// </summary>
	PWSTR StartStation;
	/// <summary>
	/// 本车票终点
	/// </summary>
	PWSTR EndStation;
	/// <summary>
	/// 票价，以分为单位
	/// </summary>
	int32_t Price;
	/// <summary>
	/// 座号
	/// </summary>
	int32_t SeatNumber;
	/// <summary>
	/// 座位等级
	/// </summary>
	int32_t SeatLevel;
	/// <summary>
	/// UTC 始发站站开车时间和日期
	/// </summary>
	uint64_t TrainStartTime;
	/// <summary>
	/// 订票人
	/// </summary>
	PWSTR Owner;
} YINYUE200_TICKET;
typedef YINYUE200_TICKET* YINYUE200_TICKET_PTR;
typedef struct Yinyue200_SeatInfoCache
{
	/// <summary>
	/// 车次 ID
	/// </summary>
	YINYUE200_PAIR_OF_uint64_t_uint64_t TrainID;
	/// <summary>
	/// 列车始发站发车本地日期
	/// </summary>
	uint64_t Date;
	/// <summary>
	/// 区间座位占用信息
	/// </summary>
	BITVECTOR* seatinfo;
	/// <summary>
	/// 区间数量
	/// </summary>
	int seatinfocount;
	/// <summary>
	/// 列车所有座位数量
	/// </summary>
	int32_t seatcount;

} YINYUE200_SEATINFOCACHE;
enum Yinyue200_TicketRefuseReason
{
	YINYUE200_TICKETREFUSERESON_NOREFUSE = 0,
	YINYUE200_TICKETREFUSERESON_NOPASTTICKET = 1,
	YINYUE200_TICKETREFUSERESON_TOOLATE = 2,
	YINYUE200_TICKETREFUSERESON_TOOEARLY = 3,
	YINYUE200_TICKETREFUSERESON_NOTRAIN = 4
};
typedef YINYUE200_SEATINFOCACHE* YINYUE200_SEATINFOCACHE_PTR;
typedef struct Yinyue200_TrainIdAndDate
{
	YINYUE200_PAIR_OF_uint64_t_uint64_t TrainId;
	uint64_t LocalDate;
} YINYUE200_TRAINIDAANDDATE;
typedef struct Yinyue200_TrainIdAndDateAndPassenger
{
	YINYUE200_PAIR_OF_uint64_t_uint64_t TrainId;
	uint64_t LocalDate;
	PWSTR PassengerID;
	PWSTR PassengerIDType;
} YINYUE200_TRAINIDAANDDATEANDPASSENGER;
YINYUE200_TICKET_PTR Create_Yinyue200_Ticket();
bool yinyue200_TicketsInfoSave();
inline int Yinyue200_IsLeapYear(int year)
{
	return ((0 == year % 4) && (0 != year % 100)) || (0 == year % 400);
}
/// <summary>
/// 获取月份最大天数
/// </summary>
/// <param name="year">年份</param>
/// <param name="month">月份</param>
/// <returns></returns>
int Yinyue200_GetMonthMaxDay(int year, int month);/// <summary>
/// 计算两站之间的座位使用情况
/// </summary>
/// <param name="train">车次信息</param>
/// <param name="date">发车本地日期</param>
/// <param name="startstation">区间起点</param>
/// <param name="endstation">区间终点</param>
/// <param name="seatinfo">座位缓存信息</param>
/// <returns></returns>
BITVECTOR Yinyue200_GetSeatUsability(YINYUE200_TRAINPLANRECORD_PTR train, uint64_t date, PWSTR startstation, PWSTR endstation, YINYUE200_SEATINFOCACHE_PTR seatinfo);
/// <summary>
/// 获取余票数量
/// </summary>
/// <param name="train">车次</param>
/// <param name="seatusability">区间座位情况</param>
/// <param name="seatLevel">座位等级</param>
/// <param name="seatinfo">座位情况</param>
/// <returns></returns>
int32_t Yinyue200_GetUseableSeatsNumber(YINYUE200_TRAINPLANRECORD_PTR train, BITVECTOR* seatusability, enum TrainSeatType seatLevel, YINYUE200_SEATINFOCACHE_PTR seatinfo);
/// <summary>
/// 分配座位号
/// </summary>
int32_t Yinyue200_AllocSeatNumber(YINYUE200_TRAINPLANRECORD_PTR train, BITVECTOR* seatusability, enum TrainSeatType seatLevel, YINYUE200_SEATINFOCACHE_PTR seatinfo);
/// <summary>
/// 计算区间票价
/// </summary>
/// <param name="train"></param>
/// <param name="startstation"></param>
/// <param name="endstation"></param>
/// <param name=""></param>
/// <returns>以分为单位</returns>
int32_t Yinyue200_TicketManage_GetPrice(YINYUE200_TRAINPLANRECORD_PTR train, PWSTR startstation, PWSTR endstation, enum TrainSeatType seatLevel);
/// <summary>
/// 检查乘客是否已预定
/// </summary>
/// <param name="train">车次信息</param>
/// <param name="passenger">乘客</param>
/// <param name="localdate">列车从起点站发车的本地日期</param>
/// <returns></returns>
bool Yinyue200_IsTicketBookedForPassenger(YINYUE200_TRAINPLANRECORD_PTR train, YINYUE200_PASSENGERINFO_PTR passenger, uint64_t localdate);;
/// <summary>
/// 传入的是本地时间
/// </summary>
enum Yinyue200_TicketRefuseReason Yinyue200_CheckTrainPlanRecordDateWithBookLimit(YINYUE200_TRAINPLANRECORD_PTR Train, uint64_t localdate, PWSTR startstation, PWSTR endstation, uint64_t* thistrainstartdatetime_out);
YINYUE200_TICKET_PTR Yinyue200_BookTickets(YINYUE200_TRAINPLANRECORD_PTR train,
	YINYUE200_PASSENGERINFO_PTR PassengerInfo,
	int year, int month, int day,
	PWSTR startstation,
	PWSTR endstation,
	enum TrainSeatType seatLevel,
	PWSTR owner
);
/// <summary>
/// 为删除车票调整座位使用情况缓存信息
/// </summary>
/// <param name="cache">要调整的缓存信息</param>
/// <param name="ticket">车票</param>
/// <param name="train">车次信息</param>
void Yinyue200_SetCacheInfoForRemovingTicket(YINYUE200_SEATINFOCACHE_PTR cache, YINYUE200_TICKET_PTR ticket, YINYUE200_TRAINPLANRECORD_PTR train);/// <summary>
/// 退票
/// </summary>
/// <param name="ticket">要退票的车票</param>
/// <param name="refundprice">设置为退票费，若退票失败则设置为0</param>
/// <returns>退票是否成功</returns>
bool Yinyue200_RefundTicket(YINYUE200_TICKET_PTR ticket, int32_t* refundprice);
/// <summary>
/// 获取当前的车票情况，有可能直接写入缓存或从缓存中直接读取
/// </summary>
/// <param name="train"></param>
/// <param name="date">本地时间,仅包含日期部分</param>
/// <returns></returns>
YINYUE200_SEATINFOCACHE_PTR Yinyue200_GetUsedTicketCount(YINYUE200_TRAINPLANRECORD_PTR train, uint64_t date);
/// <summary>
/// 获取某人下单的所有车票
/// </summary>
/// <param name="owner">用户名称</param>
/// <returns>一个新创建的列表</returns>
vector Yinyue200_CreateFullListOfTicketInfoRefWithOwner(PWCHAR owner);
/// <summary>
/// 获取座位等级名称
/// </summary>
/// <param name="level">座位等级</param>
/// <returns>座位名称</returns>
PWSTR Yinyue200_GetSeatLevelName(int32_t level);
vector* Yinyue200_GetFullListOfTicketInfo();

//构造获取 YINYUE200_TICKET 成员的函数定义的宏
#define YINYUE200_TICKETINFO_DEFINE_GETMEMBERMETHOD(name) inline void* yinyue200_GetTicketInfo##name(void* obj)\
{\
YINYUE200_TICKET_PTR p = obj;\
return p->name;\
}
//构造获取 YINYUE200_TICKET 成员的地址的函数定义的宏
#define YINYUE200_TICKETINFO_DEFINE_GETMEMBERADDRMETHOD(name) inline void* yinyue200_GetTicketInfo##name(void* obj)\
{\
YINYUE200_TICKET_PTR p = obj;\
return &p->name;\
}


YINYUE200_TICKETINFO_DEFINE_GETMEMBERADDRMETHOD(ID)
YINYUE200_TICKETINFO_DEFINE_GETMEMBERMETHOD(PassengerName)
YINYUE200_TICKETINFO_DEFINE_GETMEMBERMETHOD(PassengerIDType)
YINYUE200_TICKETINFO_DEFINE_GETMEMBERMETHOD(PassengerID)
YINYUE200_TICKETINFO_DEFINE_GETMEMBERADDRMETHOD(CreatedTime)
YINYUE200_TICKETINFO_DEFINE_GETMEMBERMETHOD(TrainName)
YINYUE200_TICKETINFO_DEFINE_GETMEMBERADDRMETHOD(TrainID)
YINYUE200_TICKETINFO_DEFINE_GETMEMBERADDRMETHOD(TrainTime)
YINYUE200_TICKETINFO_DEFINE_GETMEMBERMETHOD(StartStation)
YINYUE200_TICKETINFO_DEFINE_GETMEMBERMETHOD(EndStation)
YINYUE200_TICKETINFO_DEFINE_GETMEMBERADDRMETHOD(Price)
YINYUE200_TICKETINFO_DEFINE_GETMEMBERADDRMETHOD(SeatNumber)
YINYUE200_TICKETINFO_DEFINE_GETMEMBERADDRMETHOD(SeatLevel)
YINYUE200_TICKETINFO_DEFINE_GETMEMBERADDRMETHOD(TrainStartTime)
YINYUE200_TICKETINFO_DEFINE_GETMEMBERMETHOD(Owner)