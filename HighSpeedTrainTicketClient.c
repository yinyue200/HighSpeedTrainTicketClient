// HighSpeedTrainTicketClient.c : This file contains the 'main' function. Program execution begins and ends there.
//
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

#include "common.h"
#include "stdatomic.h"
#include "vector.h"
#include "TrainPlanRecord.h"
#include "UserSetting.h"
#include "LoginWindow.h"
#include "DpiHelper.h"

vector yinyue200_ProductList;
HINSTANCE yinyue200_hInstance;//global handle to the program instance.
int yinyue200_nCmdShow;
atomic_int yinyue200_windowCount = 0;
USERDATAINFO_PTR yinyue200_LoganUserInfo = NULL;
uint64_t yinyue200_ADay = 10000000llu * 3600llu * 24llu;
//增加当前的窗体计数
void AddWindowCount()
{
    atomic_fetch_add(&yinyue200_windowCount,1);
}
//减少当前的窗体计数
void DecreaseWindowCount()
{
    atomic_fetch_sub(&yinyue200_windowCount, 1);
}
//返回当前是否仍有打开的窗体
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

//主函数
#ifdef _MSC_VER
#pragma warning(disable:28251)
#endif
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#ifdef _MSC_VER
#pragma warning(default:28251)
#endif
{
    {
        vector* p = ProductRecordLoadToVector(yinyue200_GetConfigFilePath());
        VECTOR_MOVE(yinyue200_ProductList, *p);

        Yinyue200_InitTrainPlanRecordIndexs();
    }

    yinyue200_hInstance = hInstance;
    yinyue200_nCmdShow = nCmdShow;
    yinyue200_InitDpiHelper();

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



