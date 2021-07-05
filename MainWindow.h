#pragma once
//  SimpleStoreErp
//	Copyright(C) 2021 殷越
//
//	This program is free software : you can redistribute it and /or modify
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
typedef struct Yinyue200_MainWindowData
{
    HWND WindowHwnd;
    vector UnsortedNowList;
    vector NowList;
    vector PagedNowList;
    int pagestart;
    int sortcomindex;//排序的列号
    int sortstate;//是否已经应用排序
    int sortmethod;//升降序
} YINYUE200_MAINWINDOWDATA;
void CreateMainWindow();
void Yinyue200_Main_UpdateListViewData(HWND hwnd);
