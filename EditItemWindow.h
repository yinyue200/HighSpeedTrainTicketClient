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
#include "TrainPlanRecord.h"

//创建并打开编辑单个记录信息窗体
void CreateEditItemWindow(YINYUE200_TRAINPLANRECORD_PTR train, bool enablesave, bool bookmode);
typedef struct Yinyue200_EditItemWindow_RoutePointAddOrEdit_Callback_Context
{
    HWND hwnd;//窗口句柄
    bool add;

} YINYUE200_EDITITEMWINDOW_ROUTEPOINTADDOREDIT_CALLBACK_CONTEXT;