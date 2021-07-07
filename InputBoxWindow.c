#include "InputBoxWindow.h"
#include "common.h"
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
} INPUTBOXWINDOWDATA;
void CreateInputBoxWindow(LPWSTR msg, InputBoxCallbackFunc callback, void* callbackcontext,bool ispwd)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"yinyue200.SimpleStoreErp.InputBoxWindow";

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
        L"������Ϣ-�ҵ�С���������ϵͳ",    // Window text
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
LRESULT CALLBACK InputBoxWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

        INPUTBOXWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);

        HWND NameLabelHwnd = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE, 10, lasty, 500, 25
            , hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 25;
        HWND hwndEdit_Name = CreateWindow(
            L"EDIT",   // predefined class 
            NULL,         // no window title 
            WS_BORDER | WS_CHILD | WS_VISIBLE |
            ES_LEFT,
            10, lasty, 500, 25,   //MAYBE set size in WM_SIZE message 
            hwnd,         // parent window 
            (HMENU)ID_EDIT_STR,   // edit control ID 
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);        // pointer not needed 
        lasty += 25;
        HWND hwndButton_Save = CreateWindow(//see https://docs.microsoft.com/en-us/windows/win32/controls/buttons
            L"BUTTON",
            L"����",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            10, lasty, 100, 50,
            hwnd, (HMENU)ID_BUTTON_SAVE, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        HWND hwndButton_Cancel = CreateWindow(
            L"BUTTON",
            L"ȡ��",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            10 + 100 + 10, lasty, 100, 50,
            hwnd, (HMENU)ID_BUTTON_CANCEL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 50;
        if(windowdata->ispwd)
            Edit_SetPasswordChar(hwndEdit_Name, L'*');
        SendMessage(NameLabelHwnd, WM_SETTEXT, 0, windowdata->MSG);

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