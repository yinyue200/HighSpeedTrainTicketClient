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
#include "InputBoxWindow.h"
#include "common.h"
#include "ControlsCommonOperation.h"
#include "DpiHelper.h"
#define ID_LABEL_STR 2
#define ID_EDIT_STR 1
#define ID_BUTTON_SAVE 3
#define ID_BUTTON_CANCEL 4
LRESULT CALLBACK InputBoxWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
typedef struct InputBoxWindowData
{
    bool ispwd;
    LPWSTR MSG;
    InputBoxCallbackFunc callback;
    void* callbackcontext;
    HFONT lastfont;
} INPUTBOXWINDOWDATA;
void CreateInputBoxWindow(LPWSTR msg, InputBoxCallbackFunc callback, void* callbackcontext,bool ispwd)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"yinyue200.HighSpeedTrainTicketClient.InputBoxWindow";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = InputBoxWindowProc;
    wc.hInstance = yinyue200_hInstance;
    wc.lpszClassName = CLASS_NAME;

    WORD result = RegisterClass(&wc);
    //printf("%d", result);

    INPUTBOXWINDOWDATA* windowdata = yinyue200_safemalloc(sizeof(INPUTBOXWINDOWDATA));
    windowdata->callback = callback;
    windowdata->callbackcontext = callbackcontext;
    windowdata->MSG = msg;
    windowdata->ispwd = ispwd;

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"输入信息",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        yinyue200_hInstance,  // Instance handle
        windowdata        // Additional application data
    );

    if (hwnd == NULL)
    {
        int a = GetLastError();
        return 0;
    }



    ShowWindow(hwnd, yinyue200_nCmdShow);


    return;
}
void LayoutControls_InputBoxWindow(HWND hwnd, UINT dpi, INPUTBOXWINDOWDATA *windata)
{
    HFONT font = windata->lastfont;
    int lasty = 10;
    YINYUE200_SETCONTROLPOSANDFONT(ID_LABEL_STR, 10, lasty, 500, 25);
    lasty += 25;
    YINYUE200_SETCONTROLPOSANDFONT(ID_EDIT_STR, 10, lasty, 500, 25);
    lasty += 35;
    YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_SAVE, 10, lasty, 100, 50);
    YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_CANCEL, 10 + 100 + 10, lasty, 100, 50);
    lasty += 50;
}
LRESULT CALLBACK InputBoxWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        AddWindowCount();

        {
            CREATESTRUCT* cs = lParam;
            SetProp(hwnd, YINYUE200_WINDOW_DATA, cs->lpCreateParams);
        }
        UINT dpi = yinyue200_GetDpiForWindow(hwnd);

        INPUTBOXWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);

        windowdata->lastfont = yinyue200_CreateDefaultFont(hwnd);

        HWND NameLabelHwnd = Yinyue200_FastCreateLabelControl(hwnd, ID_LABEL_STR, windowdata->MSG);
        HWND hwndEdit_Name = Yinyue200_FastCreateEditControl(hwnd, ID_EDIT_STR);
        HWND hwndButton_Save = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_SAVE, L"确定");
        HWND hwndButton_Cancel = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_CANCEL, L"取消");

        Yinyue200_SetWindowSize(hwnd, 600, 300, dpi);

        if(windowdata->ispwd)
            Edit_SetPasswordChar(hwndEdit_Name, L'*');

        LayoutControls_InputBoxWindow(hwnd, dpi, windowdata);
    }
    return 0;
    case WM_COMMAND:
    {
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
        {
            INPUTBOXWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
            if (LOWORD(wParam) == ID_BUTTON_SAVE)
            {
                LPWSTR info = CreateWstrForWindowText(GetDlgItem(hwnd, ID_EDIT_STR));
                windowdata->callback(windowdata->callbackcontext,info);
                SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            }
            else if (LOWORD(wParam) == ID_BUTTON_CANCEL)
            {
                windowdata->callback(windowdata->callbackcontext,NULL);
                SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            }
        }
        break;
        }
    }
    return 0;
    case WM_DPICHANGED:
    {
        INPUTBOXWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if (windowdata && windowdata->lastfont)
        {
            yinyue200_DeleteFont(windowdata->lastfont);
            windowdata->lastfont = yinyue200_CreateDefaultFont(hwnd);
            LayoutControls_InputBoxWindow(hwnd, yinyue200_GetDpiForWindow(hwnd), windowdata);
        }
        break;
    }
    case WM_DESTROY:
    {
        INPUTBOXWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if (windowdata != NULL)
        {
            free(windowdata);
            yinyue200_DeleteFont(windowdata->lastfont);
        }
        RemoveProp(hwnd, YINYUE200_WINDOW_DATA);
        DecreaseWindowCount();
        return 0;
    }


    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);



        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}