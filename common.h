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
#include <windows.h>
#include <windowsx.h>
#include <stdbool.h>
#include "vector.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#define YINYUE200_SKIP_LOGINCHECK 1 //调试时是否跳过登录
#define YINYUE200_ROUTEPOINTEDITWINDOW_ALWAYS_PERFORMCHANGES_ON_NEWOBJECT 0
#include "UserManage.h"
#define YINYUE200_WINDOW_DATA L"YINYUE200_WINDOW_DATA"
//C 语言下利用宏实现的类泛型元组结构体定义
#define YINYUE200_DEFINE_PAIR(type1,type2) typedef struct tag_yinyue200_pair_of_##type1##_##type2 \
{\
  type1 Item1;\
  type2 Item2;\
} YINYUE200_PAIR_OF_##type1##_##type2;
YINYUE200_DEFINE_PAIR(uint64_t, uint64_t)
YINYUE200_DEFINE_PAIR(wchar_t, double)
YINYUE200_DEFINE_PAIR(int32_t, int32_t)

extern HINSTANCE yinyue200_hInstance;//global handle to the program instance.
extern int yinyue200_nCmdShow;//specifies how the application windows should be display
extern vector yinyue200_ProductList;//全局记录信息
extern USERDATAINFO_PTR yinyue200_LoganUserInfo;//当前登录用户信息
extern uint64_t yinyue200_ADay;

LPWORD lpwAlign(LPWORD lpIn);
//增加当前的窗体计数
void AddWindowCount();
//减少当前的窗体计数
void DecreaseWindowCount();
//返回当前是否仍有打开的窗体
bool CheckIfNoWindow();
//创建一个字符串
//length: 要创建的字符串长度，包括末尾0
wchar_t* CreateWSTR(size_t length);
wchar_t* CreateWstrForWindowText(HWND hwnd);
//将传入的字符串复制一次
//str：要复制的字符串
wchar_t* CreateWstrFromWstr(wchar_t *hwnd);
wchar_t* Yinyue200_TsvEncode(wchar_t* str);
wchar_t* Yinyue200_TsvDecode(wchar_t* str);
//无法恢复的错误，直接退出程序
void UnrecoveryableFailed();
//检查当前是否没有打开的窗口，如果没有则退出程序
void CheckIfNoWindowAndQuit();
//检查 hresult,当 hresult 值代表失败时退出程序
void CheckHResult(HRESULT hresult);
//当 state 为 false 时调用 UnrecoveryableFailed()
void FailedIfFalse(bool state);
//当 malloc 失败时调用 UnrecoveryableFailed()
void* yinyue200_safemalloc(size_t size);
//调用yinyue200_safemalloc并将内存区域清0
void* yinyue200_safemallocandclear(size_t size);
GUID Yinyue200_ConvertToGuid(uint64_t high, uint64_t low);
YINYUE200_PAIR_OF_uint64_t_uint64_t Yinyue200_ConvertToUint64PairFromGuid(GUID guid);
FILETIME Yinyue200_ConvertToFileTimeFromUINT64(uint64_t time);
uint64_t Yinyue200_ConvertToUINT64FromFileTime(FILETIME time);
//将百纳秒转换成秒
double Yinyue200_ConvertToTotalSecondFromUINT64(uint64_t time);
//将秒转换成百纳秒
uint64_t Yinyue200_ConvertToUINT64FromTotalSecond(double time);
//将vector中的内容序列化到字符串
//func: 将 vector 的每一项序列化的函数
PWSTR Yinyue200_ConvertVectorToString(vector* vec, PWSTR(*func)(void* ptr));
vector Yinyue200_ConvertStringToVector(PWSTR str, void* (*func)(PWSTR str));
//输入的是本地时间
FILETIME ConvertDateToUTCFILETIME(int year, int month, int day);
//输入的是本地时间
FILETIME ConvertDateToLocalFILETIME(int year, int month, int day);
uint64_t ConvertTimeToUINT64(UINT hour, UINT minute, UINT second);
uint64_t GetLocalTimePartUINT64OFUINT64(uint64_t time);
uint64_t GetTimePartUINT64OFUINT64(uint64_t time);
vector SplitStringToVectorOfString(PWSTR str, PWSTR spl);
void FreeVectorOfString(vector* vec);
//写入宽字符串到文件
bool WritePWSTR(PCWSTR str, HANDLE hFile);
uint64_t xxHashPWSTR(PWSTR str);
bool ComparePWSTR(PCWSTR left, PCWSTR right);
inline void Yinyue200_FreeAndClear(void** str)
{
  free(*str);
  *str = NULL;
}