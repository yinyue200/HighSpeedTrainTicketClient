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
 

#include "ProductRecord.h"
#include "HashMap.h"
#include "xxhash.h"

static HASHMAP Yinyue200_TrainIdIndexHashMap = { 0 };
uint64_t Yinyue200_xxhashfortrainid(YINYUE200_PAIR_OF_uint64_t_uint64_t train)
{
    return xxhash_hash64_once(&train, 16, 0);
}
bool Yinyue200_CompareTrainId(YINYUE200_PAIR_OF_uint64_t_uint64_t* left, YINYUE200_PAIR_OF_uint64_t_uint64_t* right)
{
    return left->Item1 == right->Item1 && left->Item2 == right->Item2;
}

/// <summary>
/// 初始化索引
/// </summary>
void Yinyue200_InitTrainPlanRecordIndexs()
{
    Yinyue200_TrainIdIndexHashMap = HashMap_Create(vector_total(&yinyue200_ProductList), 
        Yinyue200_xxhashfortrainid, 
        Yinyue200_xxhashfortrainid, 
        Yinyue200_CompareTrainId,
        yinyue200_GetTrainPlanRecordID,
        HashMap_NoFree);

    for (int i = 0; i < vector_total(&yinyue200_ProductList); i++)
    {
        HashMap_Add(&Yinyue200_TrainIdIndexHashMap, vector_get(&yinyue200_ProductList, i));
    }
}

YINYUE200_TRAINPLANRECORD_PTR Yinyue200_GetTrainPlanRecordByTrainID(YINYUE200_PAIR_OF_uint64_t_uint64_t ID)
{
    return HashMap_GetByKey(&Yinyue200_TrainIdIndexHashMap, &ID);
}
YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR Yinyue200_GetTrainPlanRecordRoutePointFromStationDisplayName(YINYUE200_TRAINPLANRECORD_PTR plan, PWSTR station)
{
    for (int i = 0; i < vector_total(&plan->RoutePoints); i++)
    {
        YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR one = vector_get(&plan->RoutePoints, i);
        if (wcscmp(one->Station.DisplayName, station)==0)
        {
            return one;
        }
    }
    return NULL;
}
YINYUE200_TRAINPLANRECORD_PTR CreateTrainPlanRecord()
{
    YINYUE200_TRAINPLANRECORD_PTR PT = malloc(sizeof(YINYUE200_TRAINPLANRECORD));
    if (PT == NULL)
        return PT;
    memset(PT, 0, sizeof(YINYUE200_TRAINPLANRECORD));
    vector_init(&PT->TicketCount);
    return PT;
}
void AddTrainPlanRecord(YINYUE200_TRAINPLANRECORD_PTR record)
{
    VECTOR_ADD(yinyue200_ProductList, record);
    HashMap_Add(&Yinyue200_TrainIdIndexHashMap, record);
}
void RemoveTrainPlanRecord(YINYUE200_TRAINPLANRECORD_PTR record)
{
    for (size_t i = 0; i < VECTOR_TOTAL(yinyue200_ProductList); i++)
    {
        YINYUE200_TRAINPLANRECORD_PTR allproduct = VECTOR_GET(yinyue200_ProductList, YINYUE200_TRAINPLANRECORD_PTR, i);
        if (allproduct == record)
        {
            VECTOR_DELETE(yinyue200_ProductList, i);
            break;
        }
    }

    HashMap_RemoveItem(&Yinyue200_TrainIdIndexHashMap, record);
}
void freeTrainPlanRecord(YINYUE200_TRAINPLANRECORD_PTR record)
{
    free(record->Name);
    free(record->Type);
    free(record->State);

    for (int i = 0; i < vector_total(&record->TicketCount); i++) 
    {
        free(vector_get(&record->TicketCount, i));
    }
    vector_free(&record->TicketCount);

    if (record->RoutePoints.capacity > 0)
    {
        freeTrainPlanRecord_RoutePoints(&record->RoutePoints);
    }

    free(record);
}
void freeTrainPlanRecord_RoutePoints(vector *vec)
{
    for (int i = 0; i < vector_total(vec); i++)
    {
        YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR one = vector_get(vec, i);
        free(one->Station.DisplayName);
        free(one);
    }
    vector_free(vec);
}
// 假定 srcvec 是未初始化的 vector
void deepcopy_TrainPlanRecord_RoutePoints(vector *srcvec,vector *orivec)
{
    vector_initwithcap(srcvec, orivec->capacity);
    for (int i = 0; i < vector_total(orivec); i++)
    {
        YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR ptr = yinyue200_safemalloc(sizeof(YINYUE200_TRAINPLANRECORD_ROUTEPOINT));
        YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR oriitem = vector_get(orivec, i);
        ptr->Distance = oriitem->Distance;
        ptr->RouteRunTimeSpan = oriitem->RouteRunTimeSpan;
        ptr->Station.DisplayName = CreateWstrFromWstr(oriitem->Station.DisplayName);
        ptr->ResidenceTime = oriitem->ResidenceTime;
        vector_add(srcvec, ptr);
    }
}
#include "FileDataLoad.h"

//写入记录到文件
bool yinyue200_ProductRecordSaveToFile(LPWSTR path, vector* vec)
{
    HANDLE hFile = CreateFile(path,               // file to open
        GENERIC_READ|GENERIC_WRITE,          // open for reading
        0,       // share for reading
        NULL,                  // default security
        CREATE_ALWAYS, //create file always
        FILE_ATTRIBUTE_NORMAL, // normal file
        NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        int errcod = GetLastError();
        MessageBox(NULL, L"Save Failed!", NULL, 0);
        return false;
    }
    size_t length = VECTOR_TOTAL(*vec);
    for (size_t i = 0; i < length; i++)
    {
        YINYUE200_TRAINPLANRECORD_PTR record = VECTOR_GET(*vec, YINYUE200_TRAINPLANRECORD_PTR, i);
        SAVEWSTRDATATOVECTOR(Name);
        SAVEPAIRINTDATATOVECTOR(ID);
        SAVEWSTRDATATOVECTOR(Type);
        SAVEWSTRDATATOVECTOR(State);
        SAVEUINTDATATOVECTOR(Repeat);
        SAVEVECTORDATATOVECTOR(RoutePoints, ConvertToStringFrom_Yinyue200_TrainPlanRecord_RoutePoint);//写入路线信息
        SAVEVECTORDATATOVECTOR(TicketCount, ConvertToStringFrom_YINYUE200_PAIR_OF_int32_t_int32_t);//写入座位信息
        SAVEUINTDATATOVECTOR(StartTimePoint);
        FailedIfFalse(WritePWSTR(L"\n", hFile));
    }
    CloseHandle(hFile);
    return true;
}
//读取记录信息
vector* ProductRecordLoadToVector(LPWSTR path)
{
    //FILE SHOULD BE UTF-8 ENCODED
    vector* vec = yinyue200_safemalloc(sizeof(vector));
    vector_init(vec);

    HANDLE hFile = CreateFile(path,               // file to open
        GENERIC_READ,          // open for reading
        FILE_SHARE_READ,       // share for reading
        NULL,                  // default security
        OPEN_EXISTING,         // existing file only
        FILE_ATTRIBUTE_NORMAL, // normal file
        NULL);                 // no attr. template
    if (hFile == INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
        return vec;//找不到文件，返回空 vector
    }

    LARGE_INTEGER FILESIZEINFO;
    GetFileSizeEx(hFile, &FILESIZEINFO);
    if (FILESIZEINFO.QuadPart == 0)
    {
        CloseHandle(hFile);
        return vec;
    }
    char* data = yinyue200_safemalloc(FILESIZEINFO.QuadPart);
    DWORD readsize;

    BOOL readfilesucc = ReadFile(hFile, data, FILESIZEINFO.QuadPart, &readsize, NULL);

    if (readfilesucc)
    {
        int laststart = 0;
        int tindex = 0;
        BOOL lastisbl = FALSE;
        YINYUE200_TRAINPLANRECORD_PTR p = CreateTrainPlanRecord();
        if (p)
        {
            for (size_t i = 0; i < FILESIZEINFO.QuadPart; i++)
            {
                char one = data[i];
                size_t size = i - laststart;
                if (one == '\t' || one == '\r' || one == '\n')
                {
                    if (size > 0)
                    {
                        PWCHAR info = malloc(size * 2 + 2);
                        if (info == NULL)
                        {
                            UnrecoveryableFailed();
                            return NULL;
                        }
                        int sizechars = MultiByteToWideChar(CP_UTF8, 0, &data[laststart], size, info, size);
                        info[sizechars] = 0;
                        switch (tindex)
                        {
                            LOADWSTRDATATOVECTOR(Name, 0);
                            LOADWPAIRINTDATATOVECTOR(ID, 1);
                            LOADWSTRDATATOVECTOR(Type, 2);
                            LOADWSTRDATATOVECTOR(State, 3);
                            LOADINTDATATOVECTOR(Repeat, 4);
                            LOADVECTORDATATOVECTOR(RoutePoints, 5, ConvertStringToYinyue200_TrainPlanRecord_RoutePoint);
                            LOADVECTORDATATOVECTOR(TicketCount, 6, ConvertStringToYINYUE200_PAIR_OF_int32_t_int32_t);
                            LOADUINTDATATOVECTOR(StartTimePoint, 7);
                        default:
                            break;
                        }
                    }
                    tindex++;
                    laststart = i + 1;
                }
                if (one == '\r' || one == '\n')
                {
                    laststart = i + 1;
                    tindex = 0;
                    if (!lastisbl)
                    {
                        VECTOR_ADD(*vec, p);
                        p = CreateTrainPlanRecord();
                        if (p == NULL)
                        {
                            UnrecoveryableFailed();
                        }
                    }
                    lastisbl = true;
                }
                else
                {
                    lastisbl = false;
                }
            }
        }
        else
        {
            UnrecoveryableFailed();
        }
        free(p);
    }
    else
    {
        UnrecoveryableFailed();
    }
    free(data);
    CloseHandle(hFile);
    return vec;
}

PWSTR ConvertToStringFrom_Yinyue200_TrainPlanRecord_RoutePoint(YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR routepoint)
{
    size_t stationnamelen = wcslen(routepoint->Station.DisplayName);
    size_t len = stationnamelen + 100;
    PWSTR newstr = yinyue200_safemalloc(len * sizeof(WCHAR));
    swprintf(newstr, len, L"%s\t%llu\t%llu\t%llu", routepoint->Station.DisplayName, routepoint->Distance, routepoint->RouteRunTimeSpan, routepoint->ResidenceTime);
    return newstr;
}
PWSTR ConvertToStringFrom_YINYUE200_PAIR_OF_int32_t_int32_t(YINYUE200_PAIR_OF_int32_t_int32_t *routepoint)
{
    size_t len = 100;
    PWSTR newstr = yinyue200_safemalloc(len * sizeof(WCHAR));
    swprintf(newstr, len, L"%d\t%d", routepoint->Item1, routepoint->Item2);
    return newstr;
}
YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR ConvertStringToYinyue200_TrainPlanRecord_RoutePoint(PWSTR str)
{
    size_t len = wcslen(str);
    PWSTR buf = yinyue200_safemalloc(len * sizeof(size_t));
    YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR ptr = yinyue200_safemallocandclear(sizeof(YINYUE200_TRAINPLANRECORD_ROUTEPOINT));
    int ret = swscanf(str, L"%s\t%llu\t%llu\t%llu", buf, &ptr->Distance, &ptr->RouteRunTimeSpan, &ptr->ResidenceTime);
    if (ret < 4)
    {
        UnrecoveryableFailed();
    }
    ptr->Station.DisplayName = buf;
    return ptr;
}
YINYUE200_PAIR_OF_int32_t_int32_t* ConvertStringToYINYUE200_PAIR_OF_int32_t_int32_t(PWSTR str)
{
    size_t len = wcslen(str);
    PWSTR buf = yinyue200_safemalloc(len * sizeof(size_t));
    YINYUE200_PAIR_OF_int32_t_int32_t *ptr = yinyue200_safemallocandclear(sizeof(YINYUE200_PAIR_OF_int32_t_int32_t));
    int ret = swscanf(str, L"%d\t%d", &ptr->Item1, &ptr->Item2);
    if (ret < 2)
    {
        UnrecoveryableFailed();
    }
    return ptr;
}


/// <summary>
/// 该函数获取车次开行至指定日期所经过的天数
/// </summary>
/// <param name="record"></param>
/// <param name="filetime">传入的时间是本地时间</param>
/// <returns></returns>
int Yinyue200_GetTrainPlanRecordCreatedTotalDateFromLocalUINT64(YINYUE200_TRAINPLANRECORD_PTR record, uint64_t filetime)
{
    SYSTEMTIME systime;
    FILETIME utcfiletime = Yinyue200_ConvertToFileTimeFromUINT64(record->StartTimePoint);
    FILETIME localfiletime;
    Yinyue200_FileTimeToLocalFileTime(&utcfiletime, &localfiletime);
    FileTimeToSystemTime(&localfiletime, &systime);
    systime.wHour = 0;
    systime.wMinute = 0;
    systime.wSecond = 0;
    systime.wMilliseconds = 0;
    FILETIME datetime;
    SystemTimeToFileTime(&systime, &datetime);
    uint64_t datetime_int64 = Yinyue200_ConvertToUINT64FromFileTime(datetime);//首次发车时间

    uint64_t inputdate = filetime;//待检查的时间

    if (inputdate > datetime_int64)
    {
        uint64_t timespan = inputdate - datetime_int64;

        uint64_t divresult = timespan / yinyue200_ADay;//自首次开通本次列车以来至今天数

        return divresult;
    }
    else
    {
        return -1;
    }
}
//该函数检查指定日期是否开行指定车次
//传入的时间是本地时间
bool Yinyue200_CheckTrainPlanRecordDate(YINYUE200_TRAINPLANRECORD_PTR record, uint64_t time)
{
    int days = Yinyue200_GetTrainPlanRecordCreatedTotalDateFromLocalUINT64(record, time);
    if (days % record->Repeat == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
int32_t* Yinyue200_GetTrainPlanRecordSeatCountPointer(YINYUE200_TRAINPLANRECORD_PTR record, enum TrainSeatType type)
{
    for (int i = 0; i < vector_total(&record->TicketCount); i++)
    {
        YINYUE200_PAIR_OF_int32_t_int32_t* pair = vector_get(&record->TicketCount, i);
        if (pair->Item1 == type)
        {
            return &pair->Item2;
        }
    }
    YINYUE200_PAIR_OF_int32_t_int32_t* nn = yinyue200_safemallocandclear(sizeof(YINYUE200_PAIR_OF_int32_t_int32_t));
    nn->Item1 = type;
    vector_add(&record->TicketCount, nn);
    return &nn->Item2;
}
int32_t Yinyue200_GetTrainPlanRecordSeatCount(YINYUE200_TRAINPLANRECORD_PTR record, enum TrainSeatType type)
{
    for (int i = 0; i < vector_total(&record->TicketCount); i++)
    {
        YINYUE200_PAIR_OF_int32_t_int32_t* pair = vector_get(&record->TicketCount, i);
        if (pair->Item1 == type)
        {
            return pair->Item2;
        }
    }
    return 0;
}
void Yinyue200_SetTrainPlanRecordSeatCount(YINYUE200_TRAINPLANRECORD_PTR record, enum TrainSeatType type, int32_t value)
{
    *Yinyue200_GetTrainPlanRecordSeatCountPointer(record, type) = value;
}

int32_t GetSeatCountOfAllTypeOfSeat(YINYUE200_TRAINPLANRECORD_PTR record)
{
    int32_t sum = 0;
    for (int i = 0; i < vector_total(&record->TicketCount); i++)
    {
        YINYUE200_PAIR_OF_int32_t_int32_t* pair = vector_get(&record->TicketCount, i);
        sum += pair->Item2;
    }
    return sum;
}

uint64_t Yinyue200_GetLocalTrainStartTimePoint(YINYUE200_TRAINPLANRECORD_PTR train)
{
    uint64_t utctrainstarttime = train->StartTimePoint;
    FILETIME utctrainstarttimefiletime = Yinyue200_ConvertToFileTimeFromUINT64(utctrainstarttime);
    FILETIME localtrainstarttimefiletime;
    Yinyue200_FileTimeToLocalFileTime(&utctrainstarttimefiletime, &localtrainstarttimefiletime);
    uint64_t localtrainstarttime = Yinyue200_ConvertToUINT64FromFileTime(localtrainstarttimefiletime);

    return localtrainstarttime;
}
