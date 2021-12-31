#pragma once
#include <Windows.h>
void yinyue200_InitDpiHelper();
UINT yinyue200_GetDpiForWindow(HWND hWnd);
UINT yinyue200_SystemParametersInfoForDpi(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni, UINT dpi);
HFONT yinyue200_CreateDefaultFont(HWND winhwnd);
BOOL yinyue200_DeleteFont(HFONT font);
#define YINYUE200_CONST_DPI 96
#define YINYUE200_LOGICTOPHYBYDPI(x,dpi) MulDiv((x), (dpi), YINYUE200_CONST_DPI)
#define YINYUE200_PHYTOLOGICBYDPI(x,dpi) ((x) * (double)YINYUE200_CONST_DPI / dpi)
#define YINYUE200_LOGICTOPHYBYDPI_DOUBLE(x,dpi) ((x) * (dpi) / (double)YINYUE200_CONST_DPI)