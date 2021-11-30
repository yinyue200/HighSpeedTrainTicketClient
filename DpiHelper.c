//  HighSpeedTrainTicketClient
//	Copyright(C) 2021 ��Խ
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
#include "common.h"
HMODULE huser32 = NULL;
typedef UINT (* WINAPI GetDpiForWindowFuncType)(HWND hwnd);
GetDpiForWindowFuncType hGetDpiForWindow = NULL;
typedef BOOL (* WINAPI SystemParametersInfoForDpiFuncType)
    (UINT uiAction,UINT uiParam,PVOID pvParam,UINT fWinIni,UINT dpi);
SystemParametersInfoForDpiFuncType hSystemParametersInfoForDpi = NULL;
void yinyue200_InitDpiHelper()
{
    huser32 = GetModuleHandleW(L"user32.dll");
    if (huser32)
    {
        hGetDpiForWindow = GetProcAddress(huser32, "GetDpiForWindow");
        hSystemParametersInfoForDpi = GetProcAddress(huser32, "SystemParametersInfoForDpi");
    }
    else
    {
        UnrecoveryableFailed();
    }
}
UINT yinyue200_GetDpiForWindow(HWND hWnd)
{
    if (hGetDpiForWindow)
        return hGetDpiForWindow(hWnd);
    else
    {
        HDC hdc = GetDC(hWnd);
        UINT dpi = GetDeviceCaps(hdc, LOGPIXELSY);
        ReleaseDC(hWnd, hdc);
        return dpi;
    }
}

UINT yinyue200_SystemParametersInfoForDpi(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni, UINT dpi)
{
    if (hSystemParametersInfoForDpi)
        return hSystemParametersInfoForDpi(uiAction, uiParam, pvParam, fWinIni, dpi);
    else
        return SystemParametersInfoW(uiAction, uiParam, pvParam, fWinIni);
}

HFONT yinyue200_CreateDefaultFont(HWND winhwnd)
{
    UINT dpi = yinyue200_GetDpiForWindow(winhwnd);
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof ncm;
    yinyue200_SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0, dpi);
    HFONT font = CreateFontIndirect(&ncm.lfMessageFont);
    return font;
}
BOOL yinyue200_DeleteFont(HFONT font)
{
    return DeleteObject(font);
}