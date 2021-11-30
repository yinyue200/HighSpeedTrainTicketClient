#pragma once
#include <Windows.h>
void yinyue200_InitDpiHelper();
UINT yinyue200_GetDpiForWindow(HWND hWnd);
UINT yinyue200_SystemParametersInfoForDpi(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni, UINT dpi);
HFONT yinyue200_CreateDefaultFont(HWND winhwnd);
BOOL yinyue200_DeleteFont(HFONT font);