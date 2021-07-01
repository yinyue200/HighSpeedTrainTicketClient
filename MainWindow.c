#include "common.h"
#define ID_MENU_ABOUT 1
#define ID_MENU_VWS 2
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateMainWindow()
{
    // Register the window class.
        const wchar_t CLASS_NAME[] = L"yinyue200.SimpleStoreErp";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = yinyue200_hInstance;
    wc.lpszClassName = CLASS_NAME;

    WORD result = RegisterClass(&wc);
    //printf("%d", result);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"我的小店出入库管理系统",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        yinyue200_hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        int a = GetLastError();
        return 0;
    }

    ShowWindow(hwnd, yinyue200_nCmdShow);


    return 0;
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        HMENU hMenubar = CreateMenu();
        HMENU hFile = CreateMenu();
        HMENU hEdit = CreateMenu();
        HMENU hHelp = CreateMenu();
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hFile, L"文件");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hEdit, L"编辑");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hHelp, L"帮助");



        AppendMenu(hHelp, MF_STRING, ID_MENU_VWS, L"GitHub");
        AppendMenu(hHelp, MF_STRING, ID_MENU_ABOUT, L"关于");
        SetMenu(hwnd, hMenubar);
    }
    return 0;
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == ID_MENU_VWS)
        {
            ShellExecute(NULL, L"Open", L"https://github.com/yinyue200", NULL, NULL, SW_SHOWNORMAL);
        }
        else if (LOWORD(wParam) == ID_MENU_ABOUT)
        {
            MessageBox(NULL, L"版本 0.0.1\r\nA GUI program", L"关于", 0);
        }

    }
    return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

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
