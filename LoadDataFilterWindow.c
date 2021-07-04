#include "common.h"
#include "MainWindow.h"
#include "EditItemWindow.h"
#include "ProductRecord.h"
#include "UserSetting.h"
#define ID_EDIT_NAME 1
#define ID_BUTTON_SAVE 2
#define ID_BUTTON_CANCEL 3
#define ID_EDIT_ID 4
#define ID_EDIT_TYPE 5
#define ID_CHK_ID 6
#define ID_CHK_NAME 7
#define ID_CHK_TYPE 8
typedef struct Yinyue200_LoadDataFilterWindowData
{
    YINYUE200_MAINWINDOWDATA* mainwindowdata;
} YINYUE200_LOADDATAFILTERWINDOWDATA;
LRESULT CALLBACK LoadDataFilterWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateLoadDataFilterWindow(YINYUE200_MAINWINDOWDATA *mainwindowdata)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"yinyue200.SimpleStoreErp.LoadDataFilterWindow";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = LoadDataFilterWindowProc;
    wc.hInstance = yinyue200_hInstance;
    wc.lpszClassName = CLASS_NAME;

    WORD result = RegisterClass(&wc);
    //printf("%d", result);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"选择筛选条件-我的小店出入库管理系统",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        yinyue200_hInstance,  // Instance handle
        NULL        // Additional application data
    );

    YINYUE200_LOADDATAFILTERWINDOWDATA* windata = yinyue200_safemalloc(sizeof(YINYUE200_LOADDATAFILTERWINDOWDATA));
    memset(windata, 0, sizeof(YINYUE200_LOADDATAFILTERWINDOWDATA));
    windata->mainwindowdata = mainwindowdata;
    SetProp(hwnd, YINYUE200_WINDOW_DATA, windata);

    if (hwnd == NULL)
    {
        int a = GetLastError();
        return 0;
    }

    ShowWindow(hwnd, yinyue200_nCmdShow);


    return 0;
}
LRESULT CALLBACK LoadDataFilterWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {

        AddWindowCount();
       
        int lasty = 10;
        HWND WindowLabel = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE, 10, lasty, 500, 25
            , hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 25;
        HWND NameLabelHwnd = CreateWindow(L"BUTTON", L"名称", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, lasty, 500, 25
            , hwnd, ID_CHK_NAME, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 25;
        HWND hwndEdit_Name = CreateWindow(
            L"EDIT",   // predefined class 
            NULL,         // no window title 
            WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT,
            10, lasty, 500, 25,   //MAYBE set size in WM_SIZE message 
            hwnd,         // parent window 
            ID_EDIT_NAME,   // edit control ID 
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);        // pointer not needed 
        lasty += 25;
        HWND NameLabelId = CreateWindow(L"BUTTON", L"ID", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, lasty, 500, 25
            , hwnd, ID_CHK_ID, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 25;
        HWND hwndEdit_ID = CreateWindow(
            L"EDIT",   // predefined class 
            NULL,         // no window title 
            WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT,
            10, lasty, 500, 25,
            hwnd,         // parent window 
            ID_EDIT_ID,   // edit control ID 
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);        // pointer not needed
        lasty += 25;
        HWND NameLabelType = CreateWindow(L"BUTTON", L"类型", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, lasty, 500, 25
            , hwnd, ID_CHK_TYPE, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 25;
        HWND hwndEdit_Type = CreateWindow(
            L"EDIT",
            NULL,
            WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT,
            10, lasty, 500, 25,
            hwnd,
            ID_EDIT_NAME,
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);
        lasty += 25;
        HWND hwndButton_Save = CreateWindow(
            L"BUTTON",
            L"确定",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            10, lasty, 100, 50,
            hwnd, ID_BUTTON_SAVE, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        HWND hwndButton_Cancel = CreateWindow(
            L"BUTTON",
            L"取消",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            10 + 100 + 10, lasty, 100, 50,
            hwnd, ID_BUTTON_CANCEL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 50;
        SendMessage(WindowLabel, WM_SETTEXT, 0, L"填写筛选条件，留空为不填");
        SendMessage(hwndEdit_Name, WM_SETTEXT, 0, L"");
        SendMessage(hwndEdit_ID, WM_SETTEXT, 0, L"");


    }
    return 0;
    case WM_NOTIFY:
    {

        return 0;
    }
    case WM_COMMAND:
    {
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
        {
            if (LOWORD(wParam) == ID_BUTTON_SAVE)
            {
                YINYUE200_LOADDATAFILTERWINDOWDATA *ldfwindow = GetProp(hwnd, YINYUE200_WINDOW_DATA);
                YINYUE200_MAINWINDOWDATA* mainwindow = ldfwindow->mainwindowdata;

                HWND NameFieldCtrl = GetDlgItem(hwnd, ID_EDIT_NAME);
                HWND IdFieldCtrl = GetDlgItem(hwnd, ID_EDIT_ID);
                BOOL IsNameFieldChk = IsDlgButtonChecked(hwnd, ID_CHK_NAME);
                BOOL IsIdFieldChk = IsDlgButtonChecked(hwnd, ID_CHK_ID);
                PWCHAR NameFieldText = CreateWstrForWindowText(NameFieldCtrl);
                PWCHAR IdFieldText = CreateWstrForWindowText(IdFieldCtrl);
                int64_t id;
                if (swscanf(IdFieldText, L"%lld", &id) == 1)
                {
                    
                }
                else
                {
                    MessageBox(hwnd, L"ID 格式错误", NULL, 0);
                    IsIdFieldChk = false;
                }


                for (size_t i = 0; i < VECTOR_TOTAL(mainwindow->NowList); i++)
                {
                    PRODUCTRECORD* record = VECTOR_GET(mainwindow->NowList, PRODUCTRECORD*, i);
                    BOOL SHOULDREV = TRUE;
                    if (IsNameFieldChk)
                    {
                        if (wcscmp(record->Name, NameFieldText))
                        {
                            SHOULDREV &= TRUE;
                        }
                        else
                        {
                            SHOULDREV &= FALSE;
                        }
                    }
                    if (IsIdFieldChk)
                    {
                        if (record->ID == id)
                        {
                            SHOULDREV &= TRUE;
                        }
                        else
                        {
                            SHOULDREV &= FALSE;
                        }
                    }


                    if (!SHOULDREV)
                    {
                        VECTOR_DELETE(mainwindow->NowList, i);
                        i--;
                    }
                }

                free(NameFieldText);
                free(IdFieldText);
                Yinyue200_Main_UpdateListViewData(mainwindow->WindowHwnd);
                SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            }
            else if (LOWORD(wParam) == ID_BUTTON_CANCEL)
            {
                SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            }
        }
        break;
        }
    }
    return 0;
    case WM_DESTROY:
    {
        YINYUE200_LOADDATAFILTERWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if (windata)
        {
            free(windata);
        }
        RemoveProp(hwnd, YINYUE200_WINDOW_DATA);
        DecreaseWindowCount();
        CheckIfNoWindowAndQuit();
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