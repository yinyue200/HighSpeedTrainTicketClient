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