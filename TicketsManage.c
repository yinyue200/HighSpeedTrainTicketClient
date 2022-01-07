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
#include "TicketsManage.h"
#include "common.h"
#include "HashMap.h"
#include "xxhash.h"
#include "BitVector.h"


void free_Yinyue200_SeatInfoCache(YINYUE200_SEATINFOCACHE_PTR cache)
{
	for (size_t i = 0; i < cache->seatinfocount; i++)
	{
		BitVector_Free(&cache->seatinfo[i]);
	}
	free(cache);
}
static vector Yinyue200_AllTickets = { 0 };
static HASHMAP Yinyue200_TrainIdAndLocalDateHashMap;
/// <summary>
/// ��Ʊ��Ϣ����
/// Ԫ������ YINYUE200_SEATINFOCACHE_PTR
/// </summary>
static HASHMAP Yinyue200_TicketCountHashMap;
void Yinyue200_SetCacheInfoForNewTicket(YINYUE200_SEATINFOCACHE_PTR cache, YINYUE200_TICKET_PTR ticket, YINYUE200_TRAINPLANRECORD_PTR train);
bool Yinyue200_TrainIdAndDateHash_IsKeyEqualFunc(void* objkey, void* searchkey)
{
	YINYUE200_TICKET_PTR ticket = objkey;
	YINYUE200_PAIR_OF_YINYUE200_PAIR_OF_uint64_t_uint64_t_uint64_t* pair = searchkey;
	return ticket->TrainID.Item1 == pair->Item1.Item1
		&& ticket->TrainID.Item2 == pair->Item1.Item2
		&& GetLocalDatePartUINT64OFUINT64(ticket->TrainStartTime) == pair->Item2;
}
bool Yinyue200_TrainIdAndDateSeatInfoCache_IsKeyEqualFunc(void* objkey, void* searchkey)
{
	YINYUE200_SEATINFOCACHE_PTR seatinfo = objkey;
	YINYUE200_PAIR_OF_YINYUE200_PAIR_OF_uint64_t_uint64_t_uint64_t* pair = searchkey;
	return seatinfo->TrainID.Item1 == pair->Item1.Item1
		&& seatinfo->TrainID.Item2 == pair->Item1.Item2
		&& seatinfo->Date == pair->Item2;
}
uint64_t Yinyue200_SeatInfoCacheHash(YINYUE200_SEATINFOCACHE_PTR seatinfo)
{
	XXHASH64 xxhash = CreateXXHash64(0);
	xxhash_add64(&xxhash, &seatinfo->TrainID, 16);
	xxhash_add64(&xxhash, &seatinfo->Date, 8);
	return xxhash_hash64(&xxhash);
}
uint64_t Yinyue200_TrainIdAndDateHash(YINYUE200_TICKET_PTR ticket)
{
	XXHASH64 xxhash = CreateXXHash64(0);
	xxhash_add64(&xxhash, &ticket->ID, 16);
	uint64_t localdatepart = GetLocalDatePartUINT64OFUINT64(ticket->TrainStartTime);
	xxhash_add64(&xxhash, &localdatepart, 8);
	return xxhash_hash64(&xxhash);
}
uint64_t Yinyue200_TrainIdAndDateHash_Pair(YINYUE200_PAIR_OF_YINYUE200_PAIR_OF_uint64_t_uint64_t_uint64_t* pair)
{
	XXHASH64 xxhash = CreateXXHash64(0);
	xxhash_add64(&xxhash, &pair->Item1, 16);
	xxhash_add64(&xxhash, &pair->Item2, 8);
	return xxhash_hash64(&xxhash);
}
void Yinyue200_InitTicketBookingSystemIfNeed()
{
	if (Yinyue200_AllTickets.capacity == 0)
	{
		//TODO : load tickets from file

		Yinyue200_TrainIdAndLocalDateHashMap = HashMap_Create(vector_total(&Yinyue200_AllTickets),
			Yinyue200_TrainIdAndDateHash, 
			Yinyue200_TrainIdAndDateHash_Pair, 
			Yinyue200_TrainIdAndDateHash_IsKeyEqualFunc,
			HashMap_GetKeyNone, 
			HashMap_NoFree);

		for (size_t i = 0; i < vector_total(&Yinyue200_AllTickets); i++)//�������ڳ��εĳ�Ʊ����
		{
			HashMap_Add(&Yinyue200_TrainIdAndLocalDateHashMap, vector_get(&Yinyue200_AllTickets, i));
		}

		Yinyue200_TicketCountHashMap = HashMap_Create(0, 
			Yinyue200_SeatInfoCacheHash, 
			Yinyue200_TrainIdAndDateHash_Pair, 
			Yinyue200_TrainIdAndDateSeatInfoCache_IsKeyEqualFunc, 
			HashMap_GetKeyNone, 
			free_Yinyue200_SeatInfoCache);
	}
}

YINYUE200_TICKET_PTR Create_Yinyue200_Ticket()
{
	return yinyue200_safemallocandclear(sizeof(YINYUE200_TICKET));
}

int Yinyue200_GetMonthMaxDay(int year, int month)
{
	switch (month)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		return 31;
	case 2:
		if (Yinyue200_IsLeapYear(year))
		{
			return 29;
		}
		else
		{
			return 28;
		}
	default:
		return 30;
	}
}
void Yinyue200_adjustDateTime(int* year, int* month, int* day)
{
	if (*month > 12)
	{
		*month -= 12;
		(*year)++;
	}
	int maxday;
	if (*day > (maxday = Yinyue200_GetMonthMaxDay(*year, *month)))
	{
		(*month)++;
		*day -= maxday;
	}
	if (*month > 12)
	{
		*month -= 12;
		(*year)++;
	}
}
bool Yinyue200_CheckTrainPlanRecordDateWithBookLimit(YINYUE200_TRAINPLANRECORD_PTR Train, int year, int month, int day, PWSTR startstation, PWSTR endstation)
{
	SYSTEMTIME systime;
	GetSystemTime(&systime);//��ȡ����ʱ��
	FILETIME localfiletime;
	GetSystemTimeAsFileTime(&localfiletime);
	uint64_t localtime = Yinyue200_ConvertToUINT64FromFileTime(localfiletime);

	uint64_t localtrainstarttime = Yinyue200_GetLocalTrainStartTimePoint(Train);

	YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR startstaionroutepoint = Yinyue200_GetTrainPlanRecordRoutePointFromStationDisplayName(Train, startstation);
	uint64_t localstationstarttime = localtrainstarttime + startstaionroutepoint->RouteRunTimeSpan;

	uint64_t thistrainstationstarttime = Yinyue200_ConvertToUINT64FromFileTime(ConvertDateToLocalFILETIME(year, month, day)) + GetTimePartUINT64OFUINT64(localstationstarttime);

	uint64_t thistrainstarttime = thistrainstationstarttime - startstaionroutepoint->RouteRunTimeSpan;

	if (thistrainstationstarttime > localtime)
	{
		uint64_t span = thistrainstationstarttime - localtime;

		if (span < Yinyue200_ConvertToUINT64FromTotalSecond(60 * 10))
		{
			return false;//����ǰ10�������ڲ�������Ʊ
		}

		{
			int nnmonth = systime.wMonth + 2;
			int nnyear = systime.wYear;
			int nnday = systime.wDay;
			Yinyue200_adjustDateTime(&nnyear, &nnmonth, &nnday);//��ǰʱ�����������

			uint64_t twomonthago = Yinyue200_ConvertToUINT64FromFileTime(ConvertDateToLocalFILETIME(nnyear, nnmonth, nnday));

			if (thistrainstationstarttime > twomonthago)
			{
				return false;//����Ԥ�������º�ĳ�Ʊ
			}
		}

		if (Yinyue200_CheckTrainPlanRecordDate(Train, thistrainstarttime))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		//����Ԥ����ȥ��Ʊ
		return false;
	}

}
/// <summary>
/// ���һ���µĳ�Ʊ��¼
/// </summary>
/// <param name="ticket"></param>
void Yinyue200_AddTicketInfo(YINYUE200_TICKET_PTR ticket)
{
	Yinyue200_InitTicketBookingSystemIfNeed();
	vector_add(&Yinyue200_AllTickets, ticket);
	HashMap_Add(&Yinyue200_TrainIdAndLocalDateHashMap, ticket);

	YINYUE200_PAIR_OF_YINYUE200_PAIR_OF_uint64_t_uint64_t_uint64_t pair;
	pair.Item1 = ticket->TrainID;
	pair.Item2 = GetLocalDatePartUINT64OFUINT64(ticket->TrainStartTime);

	YINYUE200_SEATINFOCACHE_PTR seatinfocache = HashMap_GetByKey(&Yinyue200_TicketCountHashMap, &pair);

	if (seatinfocache)
	{
		YINYUE200_TRAINPLANRECORD_PTR train = Yinyue200_GetTrainPlanRecordByTrainID(ticket->TrainID);
		if (train)
		{
			//�ó��δ���
			Yinyue200_SetCacheInfoForNewTicket(seatinfocache, ticket, train);//������Ʊ����
		}
	}
}
BITVECTOR Yinyue200_GetSeatUsability(YINYUE200_TRAINPLANRECORD_PTR train, uint64_t date, PWSTR startstation, PWSTR endstation, YINYUE200_SEATINFOCACHE_PTR seatinfo)
{

	BITVECTOR seats = BitVector_CreateBitVector(seatinfo->seatcount);

	bool start = false;
	for (int i = 0; i < vector_total(&train->RoutePoints); i++)
	{
		YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR routepoint = vector_get(&train->RoutePoints, i);
		if (start == false)
		{
			if (wcscmp(routepoint->Station.DisplayName, startstation) == 0)
			{
				start = true;
			}
		}
		else
		{
			int seatindex = i - 1;
			BitVector_OrBitVector(&seats, &seatinfo->seatinfo[seatindex]);

			if (wcscmp(routepoint->Station.DisplayName, endstation) == 0)
			{
				break;
			}
		}
	}
	//����seats��Ϊ 0 ����λ�ǿ������λ
	return seats;
}
void Yinyue200_GetSeatBottomAndTopForSeatLevel(YINYUE200_TRAINPLANRECORD_PTR train, enum TrainSeatType seatLevel, int *seatcheckbottom, int *seatchecktop,int32_t seatcount)
{
	switch (seatLevel)
	{
	case TRAINTICKETTYPE_UNKNOWN:
		*seatcheckbottom = 0;
		*seatchecktop = seatcount;
		break;
	case TRAINTICKETTYPE_FIRSTCLASS:
		*seatcheckbottom = Yinyue200_GetTrainPlanRecordSeatCount(train, TRAINTICKETTYPE_BUSINESSCLASS);
		*seatchecktop = *seatcheckbottom + Yinyue200_GetTrainPlanRecordSeatCount(train, TRAINTICKETTYPE_FIRSTCLASS);
		break;
	case TRAINTICKETTYPE_SECONDCLASS:
		*seatcheckbottom = Yinyue200_GetTrainPlanRecordSeatCount(train, TRAINTICKETTYPE_BUSINESSCLASS) + Yinyue200_GetTrainPlanRecordSeatCount(train, TRAINTICKETTYPE_FIRSTCLASS);
		*seatchecktop = *seatcheckbottom + Yinyue200_GetTrainPlanRecordSeatCount(train, TRAINTICKETTYPE_SECONDCLASS);
		break;
	case TRAINTICKETTYPE_BUSINESSCLASS:
		*seatcheckbottom = 0;
		*seatchecktop = Yinyue200_GetTrainPlanRecordSeatCount(train, TRAINTICKETTYPE_BUSINESSCLASS);
		break;
	default:
		*seatcheckbottom = 0;
		*seatchecktop = 0;
		break;
	}
}
/// <summary>
/// ������λ��
/// </summary>
int32_t Yinyue200_AllocSeatNumber(YINYUE200_TRAINPLANRECORD_PTR train, BITVECTOR *seatusability, enum TrainSeatType seatLevel, YINYUE200_SEATINFOCACHE_PTR seatinfo)
{

	int seatcheckbottom;
	int seatchecktop;
	Yinyue200_GetSeatBottomAndTopForSeatLevel(train, seatLevel, &seatcheckbottom, &seatchecktop, seatinfo->seatcount);

	for (int32_t i = seatcheckbottom; i < seatchecktop; i++)
	{
		bool canuse = BitVector_GetBit(seatusability, i) == 0;
		if (canuse)
		{
			return i + 1; //��λ�Ŵ�1��ʼ
		}
	}
	
	return -1;
}
/// <summary>
/// ��ȡ��Ʊ����
/// </summary>
/// <param name="train">����</param>
/// <param name="seatusability">������λ���</param>
/// <param name="seatLevel">��λ�ȼ�</param>
/// <param name="seatinfo">��λ���</param>
/// <returns></returns>
int32_t Yinyue200_GetUseableSeatsNumber(YINYUE200_TRAINPLANRECORD_PTR train, BITVECTOR* seatusability, enum TrainSeatType seatLevel, YINYUE200_SEATINFOCACHE_PTR seatinfo)
{
	int32_t seatscount = 0;

	int seatcheckbottom;
	int seatchecktop;

	Yinyue200_GetSeatBottomAndTopForSeatLevel(train, seatLevel, &seatcheckbottom, &seatchecktop, seatinfo->seatcount);

	for (int32_t i = seatcheckbottom; i < seatchecktop; i++)
	{
		bool canuse = BitVector_GetBit(seatusability, i) == 0;
		if (canuse)
		{
			return seatscount++; //��λ�Ŵ�1��ʼ
		}
	}

	return seatscount;
}
/// <summary>
/// ע�⣬�ú�������У�鹺��Ʊ�Ƿ�Ϸ�������ڵ��øú���ǰ���м��
/// </summary>
/// <param name="Train"></param>
/// <param name="PassengerInfo"></param>
/// <param name="count">���ų�Ʊ</param>
/// <param name="year">����ʱ�䣬����Ʊ���</param>
/// <param name="month">����ʱ�䣬�·�</param>
/// <param name="day">����ʱ�䣬����</param>
/// <returns></returns>
YINYUE200_TICKET_PTR Yinyue200_BookTickets(YINYUE200_TRAINPLANRECORD_PTR Train, 
	YINYUE200_PASSENGERINFO_PTR PassengerInfo, 
	size_t count, 
	int year, int month, int day, 
	PWSTR startstation,
	PWSTR endstation,
	enum TrainSeatType seatLevel
)
{
	Yinyue200_InitTicketBookingSystemIfNeed();
	YINYUE200_TICKET_PTR ticket = Create_Yinyue200_Ticket();

	FILETIME localdatefiletime = ConvertDateToLocalFILETIME(year, month, day);
	uint64_t localdateuint64 = Yinyue200_ConvertToUINT64FromFileTime(localdatefiletime);

	uint64_t localtrainstarttime = Yinyue200_GetLocalTrainStartTimePoint(Train);

	YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR startstaionroutepoint = Yinyue200_GetTrainPlanRecordRoutePointFromStationDisplayName(Train, startstation);
	uint64_t localstationstarttime = localtrainstarttime + startstaionroutepoint->RouteRunTimeSpan;

	uint64_t thistrainstationstarttime = Yinyue200_ConvertToUINT64FromFileTime(ConvertDateToLocalFILETIME(year, month, day)) + GetTimePartUINT64OFUINT64(localstationstarttime);
	ticket->TrainTime = Yinyue200_ConvertLocalUint64ToUtcUint64(thistrainstationstarttime);

	uint64_t thistrainstarttime = thistrainstationstarttime - startstaionroutepoint->RouteRunTimeSpan;
	ticket->TrainStartTime = Yinyue200_ConvertLocalUint64ToUtcUint64(thistrainstarttime);

	uint64_t thistrainstarttimedate = GetDatePartUINT64OFUINT64(thistrainstarttime);

	ticket->TrainID = Train->ID;
	ticket->TrainName = CreateWstrFromWstr(Train->Name);
	FILETIME nowtime;
	GetSystemTimeAsFileTime(&nowtime);
	FILETIME nowutctime;
	Yinyue200_LocalFileTimeToFileTime(&nowtime, &nowutctime);
	ticket->CreatedDate = Yinyue200_ConvertToUINT64FromFileTime(nowutctime);

	ticket->StartStation = CreateWstrFromWstr(startstation);
	ticket->EndStation = CreateWstrFromWstr(endstation);
	
	GUID guid;
	CheckHResult(CoCreateGuid(&guid));
	ticket->ID = Yinyue200_ConvertToUint64PairFromGuid(guid);

	ticket->PassengerID = CreateWstrFromWstr(PassengerInfo->IDNumber);
	ticket->PassengerIDType = CreateWstrFromWstr(PassengerInfo->IDType);
	ticket->PassengerName = CreateWstrFromWstr(PassengerInfo->FullName);

	ticket->SeatLevel = seatLevel;

	YINYUE200_SEATINFOCACHE_PTR seatinfo = Yinyue200_GetUsedTicketCount(Train, thistrainstarttimedate);
	BITVECTOR seatuse = Yinyue200_GetSeatUsability(Train, thistrainstarttimedate, startstation, endstation, seatinfo);
	ticket->SeatNumber = Yinyue200_AllocSeatNumber(Train, &seatuse, seatLevel, seatinfo);

	free_Yinyue200_SeatInfoCache(seatinfo);
	BitVector_Free(&seatuse);

	Yinyue200_AddTicketInfo(ticket);

	return ticket;
}
void Yinyue200_SetCacheInfoForNewTicket(YINYUE200_SEATINFOCACHE_PTR cache, YINYUE200_TICKET_PTR ticket, YINYUE200_TRAINPLANRECORD_PTR train)
{
	if (ticket->SeatNumber > 0)
	{
		bool start = false;
		for (int i = 0; i < vector_total(&train->RoutePoints); i++)
		{
			YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR routepoint = vector_get(&train->RoutePoints, i);
			if (start)
			{
				int routespanindex = i - 1;

				BitVector_SetBit(&cache->seatinfo[routespanindex], ticket->SeatNumber - 1, true);//���Ŵ� 1 ��ʼ

				if (wcscmp(routepoint->Station.DisplayName, ticket->EndStation) == 0)
				{
					break;
				}
			}
			else
			{
				if (wcscmp(routepoint->Station.DisplayName, ticket->StartStation) == 0)
				{
					start = true;
				}
			}
		}
	}
}
/// <summary>
/// ���㵱ǰ��Ʊ���
/// </summary>
/// <param name="train"></param>
/// <param name="date">����ʱ��,���������ڲ���</param>
/// <returns></returns>
YINYUE200_SEATINFOCACHE Yinyue200_CalcUsedTicketCount(YINYUE200_TRAINPLANRECORD_PTR train, uint64_t date)
{
	//ֻ��ͨ����¼������λ��������ķ�ʽ��������Ʊ
	//��һ���򵥵����ӣ������г�һ����2����λ��·����A->B->C
	//4������Ʊ��2������A->B��2������B->C
	//��������������������������������������������������������
	//|   ����  |  ����   | ·��  |
	//|   ��    |  1      | A->B  |
	//|   ��    |  2      | A->B  |
	//|   ��    |  1      | B->C  |
	//|   ��    |  2      | B->C  |
	//��������������������������������������������������������
	//��ʱ�׺Ͷ���Ʊ
	//��������������������������������������������������������
	//|   ����  |  ����   | ·��  |
	//|   ��    |  2      | A->B  |
	//|   ��    |  1      | B->C  |
	//��������������������������������������������������������
	//��ʱ���г˿ͼƻ���A->C��Ʊ��������ÿһ�ζ��ܷ������/��һ����λ����������/������ĳ�Ʊ��˵����Ʊֻ���� 0 ��

	//��Ҫ������յ�һ����Ʊ����ᵼ�´���㿪ʼ������λȫ�̱���Ϊ��ռ��

	YINYUE200_SEATINFOCACHE cacheinfo;
	cacheinfo.seatcount = GetSeatCountOfAllTypeOfSeat(train);
	cacheinfo.seatinfocount = vector_total(&train->RoutePoints) - 1;
	cacheinfo.seatinfo = yinyue200_safemalloc(sizeof(BITVECTOR) * cacheinfo.seatinfocount);
	for (int i = 0; i < cacheinfo.seatinfocount; i++)
	{
		cacheinfo.seatinfo[i] = BitVector_CreateBitVector(cacheinfo.seatcount);
	}
	cacheinfo.Date = date;
	cacheinfo.TrainID = train->ID;

	YINYUE200_PAIR_OF_YINYUE200_PAIR_OF_uint64_t_uint64_t_uint64_t key;
	key.Item1 = train->ID;
	key.Item2 = date;
	size_t maxposscount;
	HASHMAPNODE* node = HashMap_GetPointersByKey(&Yinyue200_TrainIdAndLocalDateHashMap, &key, NULL, &maxposscount);
	while (node)
	{
		YINYUE200_TICKET_PTR record = node->value;

		Yinyue200_SetCacheInfoForNewTicket(&cacheinfo, record, train);

		node = HashMap_GetPointersByKey(&Yinyue200_TrainIdAndLocalDateHashMap, &key, node, NULL);
	}



	return cacheinfo;
}
/// <summary>
/// ��ȡ��ǰ�ĳ�Ʊ������п���ֱ��д�뻺���ӻ�����ֱ�Ӷ�ȡ
/// </summary>
/// <param name="train"></param>
/// <param name="date">����ʱ��,���������ڲ���</param>
/// <returns></returns>
YINYUE200_SEATINFOCACHE_PTR Yinyue200_GetUsedTicketCount(YINYUE200_TRAINPLANRECORD_PTR train, uint64_t date)
{
	Yinyue200_InitTicketBookingSystemIfNeed();
	YINYUE200_PAIR_OF_YINYUE200_PAIR_OF_uint64_t_uint64_t_uint64_t key;
	key.Item1 = train->ID;
	key.Item2 = date;
	YINYUE200_SEATINFOCACHE_PTR *result = HashMap_GetPointerByKey(&Yinyue200_TicketCountHashMap, &key, true);
	if (result == NULL)
	{
		YINYUE200_SEATINFOCACHE_PTR ptr = yinyue200_safemalloc(sizeof(YINYUE200_SEATINFOCACHE));
		*ptr = Yinyue200_CalcUsedTicketCount(train, date);
		*result = ptr;
		return ptr;
	}
	else
	{
		return *result;
	}
}