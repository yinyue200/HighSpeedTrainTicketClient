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
typedef struct Yinyue200_MainWindowData
{
    HWND WindowHwnd;
    vector UnsortedNowList;
    vector NowList;
    vector PagedNowList;
    int pagestart;//当前页起始下标
    int sortcomindex;//排序的列号
    int sortstate;//是否已经应用排序
    int sortmethod;//升降序
    HWND Font;
} YINYUE200_MAINWINDOWDATA;
void CreateMainWindow();
void Yinyue200_Main_UpdateListViewData(HWND hwnd);
typedef struct Yinyue200_Main_ListViewSortContext
{
    //获取排序成员
    void* (*GetCompareObject)(void* obj);
    //是否反转比较结果
    BOOL IS_REV_RESULT;
} YINYUE200_MAINLISTVIEWSORTCONTEXT;
int Yinyue200_Main_UpdateListViewData_PWSTRCompare(void* pcontext, void const* left, void const* right);
int Yinyue200_Main_UpdateListViewData_int64Compare(void* pcontext, void const* left, void const* right);
int Yinyue200_Main_UpdateListViewData_doubleCompare(void* pcontext, void const* left, void const* right);
