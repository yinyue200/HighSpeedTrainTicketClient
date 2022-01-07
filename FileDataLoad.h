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
//caseid是信息列数（0开始） member是成员名称
#define LOADINT32DATATOVECTOR(member,caseid) case caseid:\
{\
    int32_t id;\
    if (swscanf(info, L"%d", &id) == 1)\
    {\
        p->##member = id;\
    }\
    free(info);\
    break;\
}
#define LOADINTDATATOVECTOR(member,caseid) case caseid:\
{\
    int64_t id;\
    if (swscanf(info, L"%lld", &id) == 1)\
    {\
        p->##member = id;\
    }\
    free(info);\
    break;\
}
//caseid是信息列数（0开始） member是成员名称
#define LOADUINTDATATOVECTOR(member,caseid) case caseid:\
{\
    uint64_t id;\
    if (swscanf(info, L"%llu", &id) == 1)\
    {\
        p->##member = id;\
    }\
    free(info);\
    break;\
}
//caseid是信息列数（0开始） member是成员名称
#define LOADPRICEDATATOVECTOR(member,caseid) case caseid:\
{\
    double id;\
    if (swscanf(info, L"%lf", &id) == 1)\
    {\
        p->##member = id;\
    }\
    free(info);\
    break;\
}
//caseid是信息列数（0开始） member是成员名称
#define LOADWSTRDATATOVECTOR(member,caseid) case caseid:\
{\
    p->##member = info;\
    break;\
}
//caseid是信息列数（0开始） member是成员名称
#define LOADVECTORDATATOVECTOR(member,caseid,func) case caseid:\
{\
    PWSTR decodeinfo = Yinyue200_TsvDecode(info);\
    p->##member = Yinyue200_ConvertStringToVector(decodeinfo, (func));\
    free(decodeinfo);\
    break;\
}
//caseid是信息列数（0开始） member是成员名称
#define LOADWPAIRINTDATATOVECTOR(member,caseid) \
case caseid:\
{\
    uint64_t id_high;\
    uint64_t id_low;\
    if (swscanf(info, L"%llu;%llu", &id_high, &id_low) == 2)\
    {\
        YINYUE200_PAIR_OF_uint64_t_uint64_t pair = { id_high,id_low };\
        p->##member = pair;\
    }\
    free(info);\
    break;\
}
//member是成员名称
#define SAVEWSTRDATATOVECTOR(member) \
do{\
    FailedIfFalse(WritePWSTR(record->##member, hFile));\
    FailedIfFalse(WritePWSTR(L"\t", hFile));\
}while(0)
//member是成员名称
#define SAVEINTDATATOVECTOR(member) \
do{\
    wchar_t idbuffer[30];\
    swprintf_s(idbuffer, 30, L"%lld", record->##member);\
    FailedIfFalse(WritePWSTR(idbuffer, hFile));\
    FailedIfFalse(WritePWSTR(L"\t", hFile));\
}while(0)
#define SAVEINT32DATATOVECTOR(member) \
do{\
    wchar_t idbuffer[30];\
    swprintf_s(idbuffer, 30, L"%d", record->##member);\
    FailedIfFalse(WritePWSTR(idbuffer, hFile));\
    FailedIfFalse(WritePWSTR(L"\t", hFile));\
}while(0)
#define SAVEUINTDATATOVECTOR(member) \
do{\
    wchar_t idbuffer[30];\
    swprintf_s(idbuffer, 30, L"%llu", record->##member);\
    FailedIfFalse(WritePWSTR(idbuffer, hFile));\
    FailedIfFalse(WritePWSTR(L"\t", hFile));\
}while(0)
#define SAVEPAIRINTDATATOVECTOR(member) \
do{\
    wchar_t idbuffer[100];\
    swprintf_s(idbuffer, 100, L"%llu;%llu", record->##member.Item1,record->##member.Item2);\
    FailedIfFalse(WritePWSTR(idbuffer, hFile));\
    FailedIfFalse(WritePWSTR(L"\t", hFile));\
}while(0)
//member是成员名称
#define SAVEDOUBLEDATATOVECTOR(member) \
do{\
    wchar_t idbuffer[30];\
    swprintf_s(idbuffer, 30, L"%lf", record->##member);\
    FailedIfFalse(WritePWSTR(idbuffer, hFile));\
    FailedIfFalse(WritePWSTR(L"\t", hFile));\
}while(0)
#define SAVEVECTORDATATOVECTOR(member,func) \
do{\
PWSTR str = Yinyue200_ConvertVectorToString(&record->##member, (func));\
PWSTR str1 = Yinyue200_TsvEncode(str);\
FailedIfFalse(WritePWSTR(str1, hFile));\
FailedIfFalse(WritePWSTR(L"\t", hFile));\
free(str1);\
free(str);\
}while(0)