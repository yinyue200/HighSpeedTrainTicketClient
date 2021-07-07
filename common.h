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
#pragma once
#include <windows.h>
#include <windowsx.h>
#include <stdbool.h>
#include "vector.h"
#include <string.h>
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
#include <stdio.h>
#include "UserManage.h"

#define YINYUE200_WINDOW_DATA L"YINYUE200_WINDOW_DATA"

//进程标识
extern HINSTANCE yinyue200_hInstance;
//specifies how the application windows should be display
extern int yinyue200_nCmdShow;
//全局记录信息
extern vector yinyue200_ProductList;
//当前登录用户信息
extern USERDATAINFO_PTR yinyue200_LoganUserInfo;

LPWORD lpwAlign(LPWORD lpIn);
//增加当前的窗体计数
void AddWindowCount();
//减少当前的窗体计数
void DecreaseWindowCount();
//返回当前是否仍有打开的窗体
bool CheckIfNoWindow();
wchar_t* CreateWSTR(size_t length);
wchar_t* CreateWstrForWindowText(HWND hwnd);
//无法恢复的错误，直接退出程序
void UnrecoveryableFailed();
//检查当前是否没有打卡的窗口，如果没有则退出程序
void CheckIfNoWindowAndQuit();
//当 state 为 false 时调用 UnrecoveryableFailed()
void FailedIfFalse(bool state);
//当 malloc 失败时调用 UnrecoveryableFailed()
void* yinyue200_safemalloc(size_t size);