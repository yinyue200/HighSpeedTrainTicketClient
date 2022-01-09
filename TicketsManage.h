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
#include "ProductRecord.h"
#include "BitVector.h"
typedef struct Yinyue200_Ticket
{
	YINYUE200_PAIR_OF_uint64_t_uint64_t ID;
	PWSTR PassengerName;
	PWSTR PassengerIDType;
	PWSTR PassengerID;
	/// <summary>
	/// 订票时间 UTC 时间
	/// </summary>
	uint64_t CreatedTime;
	PWSTR TrainName;//车次
	YINYUE200_PAIR_OF_uint64_t_uint64_t TrainID;//车次 ID
	uint64_t TrainTime;//UTC 发车时间和日期
	PWSTR StartStation;
	PWSTR EndStation;
	int32_t Price;//票价，以分为单位
	int32_t SeatNumber;//座号
	int32_t SeatLevel;//座位等级
	uint64_t TrainStartTime;//UTC 起点站开车时间和日期
	PWSTR Owner;
} YINYUE200_TICKET;
typedef YINYUE200_TICKET* YINYUE200_TICKET_PTR;
typedef struct Yinyue200_SeatInfoCache
{
	YINYUE200_PAIR_OF_uint64_t_uint64_t TrainID;
	uint64_t Date;//local date
	BITVECTOR* seatinfo;
	int seatinfocount;
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
int Yinyue200_GetMonthMaxDay(int year, int month);
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
bool Yinyue200_IsTicketBookedForPassenger(YINYUE200_TRAINPLANRECORD_PTR train, YINYUE200_PASSENGERINFO_PTR passenger, uint64_t localdate);;
enum Yinyue200_TicketRefuseReason Yinyue200_CheckTrainPlanRecordDateWithBookLimit(YINYUE200_TRAINPLANRECORD_PTR Train, uint64_t localdate, PWSTR startstation, PWSTR endstation, uint64_t* thistrainstartdatetime_out);
YINYUE200_TICKET_PTR Yinyue200_BookTickets(YINYUE200_TRAINPLANRECORD_PTR train,
	YINYUE200_PASSENGERINFO_PTR PassengerInfo,
	int year, int month, int day,
	PWSTR startstation,
	PWSTR endstation,
	enum TrainSeatType seatLevel,
	PWSTR owner
);
bool Yinyue200_RefundTicket(YINYUE200_TICKET_PTR ticket, uint64_t* refundprice);
YINYUE200_SEATINFOCACHE_PTR Yinyue200_GetUsedTicketCount(YINYUE200_TRAINPLANRECORD_PTR train, uint64_t date);
vector Yinyue200_CreateFullListOfTicketInfoRefWithOwner(PWCHAR owner);
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