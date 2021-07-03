#include "common.h"
#include "ProductRecord.h"
#define ID_EDIT_NAME 1
#define ID_BUTTON_SAVE 2
#define ID_BUTTON_CANCEL 3
LRESULT CALLBACK EditItemWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateEditItemWindow()
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"yinyue200.SimpleStoreErp.EditItemWindow";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = EditItemWindowProc;
    wc.hInstance = yinyue200_hInstance;
    wc.lpszClassName = CLASS_NAME;

    WORD result = RegisterClass(&wc);
    //printf("%d", result);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"编辑单个记录-我的小店出入库管理系统",    // Window text
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
typedef struct EditItemWindowData
{
    HWND NameLabelHwnd;
    HWND hwndEdit_Name;
    HWND NameLabelId;
    HWND hwndEdit_ID;
    HWND NameLabelType;
    HWND hwndEdit_Type;
    HWND hwndButton_Save;
    HWND hwndButton_Cancel;
} EDITITEMWINDOWDATA;
LRESULT CALLBACK EditItemWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        AddWindowCount();
        int lasty = 10;
        HWND NameLabelHwnd = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE, 10, lasty, 500, 25
            , hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 25;
        HWND hwndEdit_Name = CreateWindow(
            L"EDIT",   // predefined class 
            NULL,         // no window title 
            WS_BORDER | WS_CHILD | WS_VISIBLE |
            ES_LEFT ,
            10, lasty, 500, 25,   //MAYBE set size in WM_SIZE message 
            hwnd,         // parent window 
            (HMENU)ID_EDIT_NAME,   // edit control ID 
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);        // pointer not needed 
        lasty += 25;
        HWND NameLabelId = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE, 10, lasty, 500, 25
            , hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 25;
        HWND hwndEdit_ID = CreateWindow(
            L"EDIT",   // predefined class 
            NULL,         // no window title 
            WS_BORDER | WS_CHILD | WS_VISIBLE |
            ES_LEFT,
            10, lasty, 500, 25,
            hwnd,         // parent window 
            (HMENU)ID_EDIT_NAME,   // edit control ID 
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);        // pointer not needed
        lasty += 25;
        HWND NameLabelType = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE, 10, lasty, 500, 25
            , hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 25;
        HWND hwndEdit_Type = CreateWindow(
            L"EDIT",
            NULL,
            WS_BORDER | WS_CHILD | WS_VISIBLE |
            ES_LEFT,
            10, lasty, 500, 25,
            hwnd,
            (HMENU)ID_EDIT_NAME,
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);
        lasty += 25;
        HWND hwndButton_Save = CreateWindow(//see https://docs.microsoft.com/en-us/windows/win32/controls/buttons
            L"BUTTON",
            L"保存",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
            10, lasty,100,50,
            hwnd, (HMENU)ID_BUTTON_SAVE, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),NULL);
        HWND hwndButton_Cancel = CreateWindow(
            L"BUTTON",
            L"取消",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
            10+100+10, lasty, 100, 50,
            hwnd, (HMENU)ID_BUTTON_CANCEL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 50;
        SendMessage(hwndEdit_Name, WM_SETTEXT, 0, L"");
        SendMessage(NameLabelHwnd, WM_SETTEXT, 0, L"名称");
        SendMessage(NameLabelId, WM_SETTEXT, 0, L"ID");
        SendMessage(hwndEdit_ID, WM_SETTEXT, 0, L"");
        SendMessage(NameLabelType, WM_SETTEXT, 0, L"类型");
        EDITITEMWINDOWDATA* windowdata = malloc(sizeof(EDITITEMWINDOWDATA));
        if (windowdata != NULL)
        {
            windowdata->hwndButton_Cancel = hwndButton_Cancel;
            windowdata->NameLabelHwnd = NameLabelHwnd;
            windowdata->hwndEdit_Name = hwndEdit_Name;
            windowdata->NameLabelId = NameLabelId;
            windowdata->hwndEdit_ID = hwndEdit_ID;
            windowdata->hwndButton_Save = hwndButton_Save;
            windowdata->NameLabelType = NameLabelType;
            windowdata->hwndEdit_Type = hwndEdit_Type;
            SetProp(hwnd, YINYUE200_WINDOW_DATA, windowdata);
        }
        else
        {
            UnrecoveryableFailed();
        }

    }
    return 0;
    case WM_COMMAND:
    {
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
        {
            if (LOWORD(wParam) == ID_BUTTON_SAVE)
            {
                EDITITEMWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
                PWCHAR name = CreateWstrForWindowText(windowdata->hwndEdit_Name);
                PWCHAR id = CreateWstrForWindowText(windowdata->hwndEdit_ID);
                
                PRODUCTRECORE_PTR productrecord = CreateProductRecord();

                productrecord->Name = name;
                productrecord->ID = id;

                VECTOR_ADD(yinyue200_ProductList, productrecord);
                
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
        HANDLE windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if(windowdata!=NULL)
            free(windowdata);
        RemoveProp(hwnd, YINYUE200_WINDOW_DATA);
        DecreaseWindowCount();
        if (CheckIfNoWindow())
            PostQuitMessage(0);
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