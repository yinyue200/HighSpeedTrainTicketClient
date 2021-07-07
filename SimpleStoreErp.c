// SimpleStoreErp.c : This file contains the 'main' function. Program execution begins and ends there.
//
//  SimpleStoreErp
//	Copyright(C) 2021 ÒóÔ½
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
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")//to Enable Visual Styles
#include "common.h"
#include "stdatomic.h"
#include "vector.h"
#include "ProductRecord.h"
#include "UserSetting.h"
#include "LoginWindow.h"

vector yinyue200_ProductList;
HINSTANCE yinyue200_hInstance;
int yinyue200_nCmdShow;
atomic_int yinyue200_windowCount = 0;
USERDATAINFO_PTR yinyue200_LoganUserInfo = NULL;
void AddWindowCount()
{
    atomic_fetch_add(&yinyue200_windowCount,1);
}
void DecreaseWindowCount()
{
    atomic_fetch_sub(&yinyue200_windowCount, 1);
}
bool CheckIfNoWindow()
{
    if (atomic_load(&yinyue200_windowCount) == 0)
    {
        return true;
    }
    return false;
}
void createmain(void* context)
{
    CreateMainWindow();
}

//Ö÷º¯Êý
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    {
        vector* p = ProductRecordLoadToVector(yinyue200_GetConfigFilePath());
        VECTOR_MOVE(yinyue200_ProductList, *p);
    }

    yinyue200_hInstance = hInstance;
    yinyue200_nCmdShow = nCmdShow;

    CreateLoginWindow(NULL, createmain, NULL);
    return MessageLoop();
}

int MessageLoop()
{
    // Run the message loop.

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}



