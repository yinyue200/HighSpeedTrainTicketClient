//  SimpleStoreErp
//	Copyright(C) 2021 殷越
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
#include "LoginWindow.h"
#include "UserManage.h"
#include "UserSetting.h"
#define ID_EDIT_NAME 1
#define ID_EDIT_PWD 2
#define ID_BUTTON_SAVE 3
#define ID_BUTTON_CANCEL 4
LRESULT CALLBACK LoginWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LPWSTR GetNowLoginedUserName()
{
    if (yinyue200_LoganUserInfo == NULL)
        return NULL;
    return yinyue200_LoganUserInfo->Name;
}
typedef struct LoginWindowData
{
    LPWSTR UserName;
    void (*callback)(void*);
    void* callbackcontext;
} LOGINWINDOWDATA;
void CreateLoginWindow(LPWSTR username,void (*callback)(void*),void* callbackcontext)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"yinyue200.SimpleStoreErp.LoginWindow";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = LoginWindowProc;
    wc.hInstance = yinyue200_hInstance;
    wc.lpszClassName = CLASS_NAME;

    WORD result = RegisterClass(&wc);
    //printf("%d", result);

    LOGINWINDOWDATA* windowdata = yinyue200_safemalloc(sizeof(LOGINWINDOWDATA));
    windowdata->UserName = username;
    windowdata->callback = callback;
    windowdata->callbackcontext = callbackcontext;

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"登录-我的小店出入库管理系统",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 300,

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
LRESULT CALLBACK LoginWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        AddWindowCount();
        int lasty = 10;

        {
            CREATESTRUCT* cs = lParam;
            SetProp(hwnd, YINYUE200_WINDOW_DATA, cs->lpCreateParams);
        }

        LOGINWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);

        HWND NameLabelHwnd = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE, 10, lasty, 500, 25
            , hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 30;
        HWND hwndEdit_Name = CreateWindow(
            L"EDIT",   // predefined class 
            NULL,         // no window title 
            WS_BORDER | WS_CHILD | WS_VISIBLE |
            ES_LEFT,
            10, lasty, 500, 25,   //MAYBE set size in WM_SIZE message 
            hwnd,         // parent window 
            (HMENU)ID_EDIT_NAME,   // edit control ID 
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);        // pointer not needed 
        lasty += 30;
        HWND NameLabelId = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE, 10, lasty, 500, 25
            , hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 30;
        HWND hwndEdit_Pwd = CreateWindow(
            L"EDIT",   // predefined class 
            NULL,         // no window title 
            WS_BORDER | WS_CHILD | WS_VISIBLE |
            ES_LEFT,
            10, lasty, 500, 25,
            hwnd,         // parent window 
            (HMENU)ID_EDIT_PWD,   // edit control ID 
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);        // pointer not needed
        lasty += 35;
        HWND hwndButton_Save = CreateWindow(//see https://docs.microsoft.com/en-us/windows/win32/controls/buttons
            L"BUTTON",
            L"登录",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            10, lasty, 100, 50,
            hwnd, (HMENU)ID_BUTTON_SAVE, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        HWND hwndButton_Cancel = CreateWindow(
            L"BUTTON",
            L"取消",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            10 + 100 + 10, lasty, 100, 50,
            hwnd, (HMENU)ID_BUTTON_CANCEL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 50;
        Edit_SetPasswordChar(hwndEdit_Pwd, L'*');
        SendMessage(NameLabelHwnd, WM_SETTEXT, 0, L"用户名");
        if (windowdata->UserName != NULL)
            SendMessage(hwndEdit_Name, WM_SETTEXT, 0, windowdata->UserName);
        SendMessage(NameLabelId, WM_SETTEXT, 0, L"密码");
        PostMessage(hwndEdit_Pwd, WM_SETTEXT, 0, L"");

    }
    return 0;
    case WM_COMMAND:
    {
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
        {
            LOGINWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
            if (LOWORD(wParam) == ID_BUTTON_SAVE)
            {
                vector* vec = UserRecordLoadToVector(yinyue200_GetUserConfigFilePath());
                LPWSTR username = CreateWstrForWindowText(GetDlgItem(hwnd,ID_EDIT_NAME));
                LPWSTR password = CreateWstrForWindowText(GetDlgItem(hwnd, ID_EDIT_PWD));
                wchar_t pwdhash[65];
                Hash256LPWSTR(password, pwdhash);
                free(password);
                bool SUCC = false;
                for (size_t i = 0; i < vector_total(vec); i++)
                {
                    USERDATAINFO_PTR item = vector_get(vec, i);
                    if (wcscmp(username, item->Name) == 0 && wcscmp(pwdhash, item->PasswordHash) == 0)
                    {
                        if (yinyue200_LoganUserInfo != NULL)
                        {
                            free(yinyue200_LoganUserInfo->Name);
                            free(yinyue200_LoganUserInfo->PasswordHash);
                            free(yinyue200_LoganUserInfo->Type);
                            free(yinyue200_LoganUserInfo);
                        }
                        yinyue200_LoganUserInfo = item;
                        SUCC = true;
                        break;
                    }
                    else
                    {
                        free(item->Name);
                        free(item->PasswordHash);
                        free(item->Type);
                        free(item);
                    }
                }
                if (!SUCC)
                {
                    if (yinyue200_LoganUserInfo != NULL)
                    {
                        free(yinyue200_LoganUserInfo->Name);
                        free(yinyue200_LoganUserInfo->PasswordHash);
                        free(yinyue200_LoganUserInfo->Type);
                        free(yinyue200_LoganUserInfo);
                    }
                    yinyue200_LoganUserInfo = NULL;
                }
                vector_free(vec);
                free(vec);
                free(username);
                windowdata->callback(windowdata->callbackcontext);
                SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            }
            else if (LOWORD(wParam) == ID_BUTTON_CANCEL)
            {
                yinyue200_LoganUserInfo = NULL;
                windowdata->callback(windowdata->callbackcontext);
                SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            }
        }
        break;
        }
    }
    return 0;
    case WM_DESTROY:
    {
        HANDLE windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if (windowdata != NULL)
            free(windowdata);
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