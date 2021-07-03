#include "common.h"
#include "EditItemWindow.h"
#include <CommCtrl.h>
#define ID_MENU_ABOUT 1
#define ID_MENU_VWS 2
#define ID_MENU_ADDRECORD 3
#define ID_LISTVIEW_MAIN 4
#define ID_BUTTON_PREVPAGE 5
#define ID_BUTTON_NEXTPAGE 6
#define ID_EDIT_PAGE 7
#define ID_CHECKBOX_PAGE 8
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateMainWindow()
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"yinyue200.SimpleStoreErp.MainWindow";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = MainWindowProc;
    wc.hInstance = yinyue200_hInstance;
    wc.lpszClassName = CLASS_NAME;

    WORD result = RegisterClass(&wc);
    //printf("%d", result);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"�ҵ�С���������ϵͳ",    // Window text
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
LRESULT ListViewNotify(HWND hWnd, LPARAM lParam)
{
    LPNMHDR  lpnmh = (LPNMHDR)lParam;
    HWND     hwndListView = GetDlgItem(hWnd, ID_LISTVIEW_MAIN);

    switch (lpnmh->code)
    {
    case LVN_GETDISPINFO:
    {
        LV_DISPINFO* lpdi = (LV_DISPINFO*)lParam;
        TCHAR szString[MAX_PATH];

        if (lpdi->item.iSubItem)
        {
            if (lpdi->item.mask & LVIF_TEXT)
            {
                swprintf_s(szString, _countof(szString),
                    TEXT("Item %d - Column %d"),
                    lpdi->item.iItem + 1, lpdi->item.iSubItem);
                wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,
                    szString, _TRUNCATE);
            }
        }
        else
        {
            if (lpdi->item.mask & LVIF_TEXT)
            {
                swprintf_s(szString, _countof(szString),
                    TEXT("Item %d"), lpdi->item.iItem + 1);
                wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,
                    szString, _TRUNCATE);
            }

            if (lpdi->item.mask & LVIF_IMAGE)
            {
                lpdi->item.iImage = 0;
            }
        }
    }
    return 0;

    case LVN_ODCACHEHINT:
    {
        LPNMLVCACHEHINT   lpCacheHint = (LPNMLVCACHEHINT)lParam;
        /*
        This sample doesn't use this notification, but this is sent when the
        ListView is about to ask for a range of items. On this notification,
        you should load the specified items into your local cache. It is still
        possible to get an LVN_GETDISPINFO for an item that has not been cached,
        therefore, your application must take into account the chance of this
        occurring.
        */
    }
    return 0;

    case LVN_ODFINDITEM:
    {
        LPNMLVFINDITEM lpFindItem = (LPNMLVFINDITEM)lParam;
        /*
        This sample doesn't use this notification, but this is sent when the
        ListView needs a particular item. Return -1 if the item is not found.
        */
    }
    return 0;
    }

    return 0;
}
HWND Yinyue200_Main_CreateListView(HWND hwndParent)
{
    DWORD       dwStyle;
    HWND        hwndListView;
    BOOL        bSuccess = TRUE;

    dwStyle = WS_TABSTOP |
        WS_CHILD |
        WS_BORDER |
        WS_VISIBLE |
        LVS_AUTOARRANGE |
        LVS_REPORT |
        LVS_OWNERDATA;

    hwndListView = CreateWindowEx(WS_EX_CLIENTEDGE,          // ex style
        WC_LISTVIEW,               // class name - defined in commctrl.h
        TEXT(""),                        // dummy text
        dwStyle,                   // style
        0,                         // x position
        0,                         // y position
        1000,                         // width
        500,                         // height
        hwndParent,                // parent
        (HMENU)ID_LISTVIEW_MAIN,        // ID
        (HINSTANCE)GetWindowLongPtr(hwndParent, GWLP_HINSTANCE),  // instance
        NULL);                     // no extra data

    if (!hwndListView)
        return NULL;

    return hwndListView;
}
BOOL InsertListViewItems(HWND hwndListView)
{
    //empty the list
    ListView_DeleteAllItems(hwndListView);

    //set the number of items in the list
    ListView_SetItemCount(hwndListView, 100);

    return TRUE;
}
BOOL Yinyue200_Main_InitListView(HWND hwndListView)
{
    LV_COLUMN   lvColumn;
    int         i;
    TCHAR       szString[5][20] = { TEXT("����"), TEXT("ID"), TEXT("Column 2"), TEXT("Column 3"), TEXT("Column 4") };

    //empty the list
    ListView_DeleteAllItems(hwndListView);

    //initialize the columns
    lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvColumn.fmt = LVCFMT_LEFT;
    lvColumn.cx = 120;
    for (i = 0; i < 5; i++)
    {
        lvColumn.pszText = szString[i];
        ListView_InsertColumn(hwndListView, i, &lvColumn);
    }

    InsertListViewItems(hwndListView);

    return TRUE;
}
void UpdateCheckBoxInfo(HWND hwnd)
{
    HWND lastpagebtn = GetDlgItem(hwnd, ID_BUTTON_PREVPAGE);
    HWND nextpagebtn = GetDlgItem(hwnd, ID_BUTTON_NEXTPAGE);
    HWND editpagebtn = GetDlgItem(hwnd, ID_EDIT_PAGE);

    if (IsDlgButtonChecked(hwnd, ID_CHECKBOX_PAGE))
    {
        ShowWindow(lastpagebtn, SW_SHOW);
        ShowWindow(nextpagebtn, SW_SHOW);
        ShowWindow(editpagebtn, SW_SHOW);
    }
    else
    {
        ShowWindow(lastpagebtn, SW_HIDE);
        ShowWindow(nextpagebtn, SW_HIDE);
        ShowWindow(editpagebtn, SW_HIDE);
    }
}
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {

        AddWindowCount();

        HMENU hMenubar = CreateMenu();
        HMENU hFile = CreateMenu();
        HMENU hFind = CreateMenu();
        HMENU hAdd = CreateMenu();
        HMENU hAddItem = CreateMenu();
        HMENU hAns = CreateMenu();
        HMENU hUsr = CreateMenu();
        HMENU hHelp = CreateMenu();
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hFile, L"�ļ�");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hFind, L"��ѯ");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hAdd, L"����");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hAns, L"ͳ��");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hUsr, L"�û�����");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hHelp, L"����");

        AppendMenu(hHelp, MF_STRING, ID_MENU_VWS, L"GitHub");
        AppendMenu(hHelp, MF_STRING, ID_MENU_ABOUT, L"����");
        AppendMenu(hAdd, MF_STRING, ID_MENU_ADDRECORD, L"��ӵ�����Ŀ");

        SetMenu(hwnd, hMenubar);

        HWND listview = Yinyue200_Main_CreateListView(hwnd);
        Yinyue200_Main_InitListView(listview);

        int buttony = 500;
        int buttonx = 10;
        HWND hwndPageButton = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"��ҳ��ʾ",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,  // Styles 
            buttonx,         // x position 
            buttony,         // y position 
            100,        // Button width
            50,        // Button height
            hwnd,     // Parent window
            ID_CHECKBOX_PAGE,       // No menu.
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.
        buttonx+=100;
        HWND hwndPrevPageButton = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"��һҳ",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            buttonx,         // x position 
            buttony,         // y position 
            100,        // Button width
            50,        // Button height
            hwnd,     // Parent window
            ID_BUTTON_PREVPAGE,       // No menu.
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.
        buttonx += 100;
        HWND hwndPageInputEdit = CreateWindow(
            L"EDIT",  // Predefined class; Unicode assumed 
            L"1",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD| WS_BORDER | ES_LEFT,  // Styles 
            buttonx,         // x position 
            buttony,         // y position 
            100,        // Button width
            50,        // Button height
            hwnd,     // Parent window
            ID_EDIT_PAGE,       // No menu.
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.
        buttonx += 100;
        HWND hwndNextPageButton = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"��һҳ",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            buttonx,         // x position 
            buttony,         // y position 
            100,        // Button width
            50,        // Button height
            hwnd,     // Parent window
            ID_BUTTON_NEXTPAGE,       // No menu.
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.
        UpdateCheckBoxInfo(hwnd);
    }
    return 0;
    case WM_NOTIFY:
    {
        if (LOWORD(wParam) == ID_LISTVIEW_MAIN)
        {
            ListViewNotify(hwnd, lParam);
        }
        return 0;
    }
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == ID_MENU_VWS)
        {
            ShellExecute(NULL, L"Open", L"https://github.com/yinyue200", NULL, NULL, SW_SHOWNORMAL);
        }
        else if (LOWORD(wParam) == ID_MENU_ABOUT)
        {
            MessageBox(NULL, L"�汾 0.0.1\r\nA GUI program\r\nold fashion design", L"����", 0);
        }
        else if (LOWORD(wParam) == ID_MENU_ADDRECORD)
        {
            CreateEditItemWindow();
        }
        else
        {
            switch (HIWORD(wParam))
            {
            case BN_CLICKED:
            {

                if (LOWORD(wParam) == ID_CHECKBOX_PAGE)
                {
                    UpdateCheckBoxInfo(hwnd);
                }
            }
            break;
            }
        }
    }
    return 0;
    case WM_DESTROY:
        DecreaseWindowCount();
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
