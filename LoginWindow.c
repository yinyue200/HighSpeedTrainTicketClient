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
#include "DpiHelper.h"
#include "ControlsCommonOperation.h"
#define ID_EDIT_NAME 1
#define ID_EDIT_PWD 2
#define ID_BUTTON_SAVE 3
#define ID_BUTTON_CANCEL 4
#define ID_LABEL_USERNAME 5
#define ID_LABEL_PWD 6
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
    vector usedfont;
} LOGINWINDOWDATA;
void CreateLoginWindow(LPWSTR username,void (*callback)(void*),void* callbackcontext)
{
#if YINYUE200_SKIP_LOGINCHECK
    yinyue200_LoganUserInfo = yinyue200_safemalloc(sizeof(USERDATAINFO));
    yinyue200_LoganUserInfo->Name= yinyue200_safemalloc(2);
    yinyue200_LoganUserInfo->Name[0] = 0;
    yinyue200_LoganUserInfo->Type = L"ADMIN";
    yinyue200_LoganUserInfo->PasswordHash = NULL;
    callback(callbackcontext);
#else

    // Register the window class.
    const wchar_t CLASS_NAME[] = L"yinyue200.HighSpeedTrainTicketClient.LoginWindow";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = LoginWindowProc;
    wc.hInstance = yinyue200_hInstance;
    wc.lpszClassName = CLASS_NAME;

    WORD result = RegisterClass(&wc);
    //printf("%d", result);

    LOGINWINDOWDATA* windowdata = yinyue200_safemallocandclear(sizeof(LOGINWINDOWDATA));
    windowdata->UserName = username;
    windowdata->callback = callback;
    windowdata->callbackcontext = callbackcontext;
    vector_init(&windowdata->usedfont);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"登录-我的小店出入库管理系统",    // Window text
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
#endif
}
void LayoutControls_LoginWindow(HWND hwnd, UINT dpi, LOGINWINDOWDATA *windata)
{
    Yinyue200_SetWindowSize(hwnd, 600, 300, dpi);
    int lasty = 10;
    HFONT font = yinyue200_CreateDefaultFont(hwnd);
    vector_add(&windata->usedfont, font);
    YINYUE200_SETCONTROLPOSANDFONT(ID_LABEL_USERNAME, 10, lasty, 500, 25);
    lasty += 30;
    YINYUE200_SETCONTROLPOSANDFONT(ID_EDIT_NAME, 10, lasty, 500, 25);
    lasty += 30;
    YINYUE200_SETCONTROLPOSANDFONT(ID_LABEL_PWD, 10, lasty, 500, 25);
    lasty += 30;
    YINYUE200_SETCONTROLPOSANDFONT(ID_EDIT_PWD, 10, lasty, 500, 25);
    lasty += 30;
    YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_SAVE,10, lasty, 100, 50);
    YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_CANCEL, 10 + 100 + 50, lasty, 100, 50);

}
LRESULT CALLBACK LoginWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

        LOGINWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);

        HWND hwndLabel_Name = Yinyue200_FastCreateLabelControl(hwnd, ID_LABEL_USERNAME, L"用户名");
        HWND hwndEdit_Name = Yinyue200_FastCreateEditControl(hwnd, ID_EDIT_NAME);
        HWND hwndLabel_Pwd = Yinyue200_FastCreateLabelControl(hwnd, ID_LABEL_PWD, L"密码");
        HWND hwndEdit_Pwd = Yinyue200_FastCreateEditControl(hwnd, ID_EDIT_PWD);
        HWND hwndButton_Save = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_SAVE, L"登录");
        HWND hwndButton_Cancel = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_CANCEL, L"取消");
        Edit_SetPasswordChar(hwndEdit_Pwd, L'*');
        if (windowdata->UserName != NULL)
            SendMessage(hwndEdit_Name, WM_SETTEXT, 0, windowdata->UserName);
        PostMessage(hwndEdit_Pwd, WM_SETTEXT, 0, L"");
        LayoutControls_LoginWindow(hwnd, yinyue200_GetDpiForWindow(hwnd), windowdata);
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
        LOGINWINDOWDATA *windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if (windowdata != NULL)
        {
            for (int i = 0; i < windowdata->usedfont.total; i++)
            {
                yinyue200_DeleteFont(vector_get(&windowdata->usedfont, i));
            }
            vector_free(&windowdata->usedfont);
            free(windowdata);
        }
        RemoveProp(hwnd, YINYUE200_WINDOW_DATA);
        DecreaseWindowCount();
        CheckIfNoWindowAndQuit();
        return 0;
    }
    case WM_DPICHANGED:
    {
        LOGINWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        UINT dpi = HIWORD(wParam);
        RECT* prcNewWin = (RECT*)lParam;
        // 重新摆放子窗口控件
        LayoutControls_LoginWindow(hwnd, dpi, windowdata);
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