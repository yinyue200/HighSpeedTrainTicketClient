//  SimpleStoreErp
//	Copyright(C) 2021 “Û‘Ω
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
#include <windows.h>
#include <windowsx.h>
#include <stdbool.h>
#include "vector.h"
#include <string.h>
#include <stdio.h>

#define YINYUE200_WINDOW_DATA L"YINYUE200_WINDOW_DATA"

extern HINSTANCE yinyue200_hInstance;
extern int yinyue200_nCmdShow;
extern vector yinyue200_ProductList;

void AddWindowCount();
void DecreaseWindowCount();
bool CheckIfNoWindow();
wchar_t* CreateWSTR(size_t length);
wchar_t* CreateWstrForWindowText(HWND hwnd);
void UnrecoveryableFailed();
void CheckIfNoWindowAndQuit();
void FailedIfFalse(bool state);