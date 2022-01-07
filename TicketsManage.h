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
typedef struct Yinyue200_Ticket
{
	YINYUE200_PAIR_OF_uint64_t_uint64_t ID;
	PWSTR PassengerName;
	PWSTR PassengerIDType;
	PWSTR PassengerID;
	/// <summary>
	/// 订票时间 UTC 时间
	/// </summary>
	uint64_t CreatedDate;
	PWSTR TrainName;//车次
	YINYUE200_PAIR_OF_uint64_t_uint64_t TrainID;//车次 ID
	uint64_t TrainTime;//UTC 发车时间和日期
	PWSTR StartStation;
	PWSTR EndStation;
	uint64_t Price;//票价，以分为单位
	int32_t SeatNumber;//座号
	int32_t SeatLevel;//座位等级
	uint64_t TrainStartTime;//UTC 起点站开车时间和日期
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
typedef YINYUE200_SEATINFOCACHE* YINYUE200_SEATINFOCACHE_PTR;
YINYUE200_DEFINE_PAIR(YINYUE200_PAIR_OF_uint64_t_uint64_t,uint64_t)
inline int Yinyue200_IsLeapYear(int year)
{
	return ((0 == year % 4) && (0 != year % 100)) || (0 == year % 400);
}
int Yinyue200_GetMonthMaxDay(int year, int month);
bool Yinyue200_CheckTrainPlanRecordDateWithBookLimit(YINYUE200_TRAINPLANRECORD_PTR Train, int year, int month, int day, PWSTR startstation, PWSTR endstation);
YINYUE200_TICKET_PTR Yinyue200_BookTickets(YINYUE200_TRAINPLANRECORD_PTR Train,
	YINYUE200_PASSENGERINFO_PTR PassengerInfo,
	size_t count,
	int year, int month, int day,
	PWSTR startstation,
	PWSTR endstation,
	enum TrainSeatType seatLevel
);
bool Yinyue200_RefundTicket(YINYUE200_TICKET_PTR ticket, uint64_t* refundprice);
YINYUE200_SEATINFOCACHE_PTR Yinyue200_GetUsedTicketCount(YINYUE200_TRAINPLANRECORD_PTR train, uint64_t date);