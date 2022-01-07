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
#include <Windows.h>
//初始化DpiHelper
void yinyue200_InitDpiHelper();
//获取窗口的DPI
UINT yinyue200_GetDpiForWindow(HWND hWnd);
//Retrieves the value of one of the system-wide parameters, taking into account the provided DPI value.
//uiAction： The system-wide parameter to be retrieved. This function is only intended for use with SPI_GETICONTITLELOGFONT, SPI_GETICONMETRICS,
//or SPI_GETNONCLIENTMETRICS. See SystemParametersInfo for more information on these values.
UINT yinyue200_SystemParametersInfoForDpi(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni, UINT dpi);
HFONT Yinyue200_CreateFont(UINT size, UINT dpi);
//创建默认的字体
HFONT yinyue200_CreateDefaultFont(HWND winhwnd);
//删除字体
BOOL yinyue200_DeleteFont(HFONT font);
#define YINYUE200_CONST_DPI 96
//将逻辑坐标转换为物理坐标
#define YINYUE200_LOGICTOPHYBYDPI(x,dpi) MulDiv((x), (dpi), YINYUE200_CONST_DPI)
//将物理坐标转换为逻辑坐标
#define YINYUE200_PHYTOLOGICBYDPI(x,dpi) ((x) * (double)YINYUE200_CONST_DPI / dpi)
//将逻辑坐标转换为物理坐标
#define YINYUE200_LOGICTOPHYBYDPI_DOUBLE(x,dpi) ((x) * (dpi) / (double)YINYUE200_CONST_DPI)