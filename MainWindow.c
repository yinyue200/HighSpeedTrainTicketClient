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
#include "common.h"
#include "MainWindow.h"
#include "EditItemWindow.h"
#include "ProductRecord.h"
#include "UserSetting.h"
#include "LoadDataFilterWindow.h"
#include <CommCtrl.h>
#define MAIN_DISPLAYPAGESIZE 10
#define MAIN_STATUSBAR_COM 4
#define ID_MENU_ABOUT 1
#define ID_MENU_VWS 2
#define ID_MENU_ADDRECORD 3
#define ID_LISTVIEW_MAIN 4
#define ID_BUTTON_PREVPAGE 5
#define ID_BUTTON_NEXTPAGE 6
#define ID_EDIT_PAGE 7
#define ID_CHECKBOX_PAGE 8
#define ID_MENU_SAVE 9
#define ID_MENU_LOADALL 10
#define ID_MENU_FLITER 11
#define ID_MENU_FLITERLOADALL 12
#define ID_STATUSBAR_MAIN 13
#define ID_BUTTON_REMOVESELECTEDITEMS 14
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void configstatusbar(HWND hwndParent,HWND  hwndStatus)
{
    int cParts = MAIN_STATUSBAR_COM;
    RECT rcClient;

    // Get the coordinates of the parent window's client area.
    GetClientRect(hwndParent, &rcClient);

    // Allocate an array for holding the right edge coordinates.
    HLOCAL hloc = LocalAlloc(LHND, sizeof(int) * cParts);
    if (hloc)
    {
        PINT paParts = (PINT)LocalLock(hloc);
        if (paParts)
        {
            // Calculate the right edge coordinate for each part, and
// copy the coordinates to the array.
            int nWidth = rcClient.right / cParts;
            int rightEdge = nWidth;
            for (int i = 0; i < cParts; i++) {
                paParts[i] = rightEdge;
                rightEdge += nWidth;
            }

            // Tell the status bar to create the window parts.
            SendMessage(hwndStatus, SB_SETPARTS, (WPARAM)cParts, (LPARAM)
                paParts);

            // Free the array, and return.
            LocalUnlock(hloc);
        }
        else
        {
            UnrecoveryableFailed();
        }

        LocalFree(hloc);
    }
    else 
    {
        UnrecoveryableFailed();
    }
}
//
//   FUNCTION: OnStatusbarSize(HWND, UINT, int, int)
//
//   PURPOSE: Process the WM_SIZE message
//
void OnStatusbarSize(HWND hWnd, UINT state, int cx, int cy)
{
    HWND hStatusbar = GetDlgItem(hWnd, ID_STATUSBAR_MAIN);

    configstatusbar(hWnd, hStatusbar);

    // Resize statusbar so it's always same width as parent's client area
    SendMessage(hStatusbar, WM_SIZE, 0, 0);
}
// Description: 
//   Creates a status bar and divides it into the specified number of parts.
// Parameters:
//   hwndParent - parent window for the status bar.
//   idStatus - child window identifier of the status bar.
//   hinst - handle to the application instance.
//   cParts - number of parts into which to divide the status bar.
// Returns:
//   The handle to the status bar.
//
HWND DoCreateStatusBar(HWND hwndParent, int idStatus, HINSTANCE
    hinst, int cParts)
{
    //see https://docs.microsoft.com/en-us/windows/win32/controls/create-status-bars

    HWND hwndStatus;

    // Ensure that the common control DLL is loaded.
    //InitCommonControls();

    // Create the status bar.
    hwndStatus = CreateWindowEx(
        0,                       // no extended styles
        STATUSCLASSNAME,         // name of status bar class
        (PCTSTR)NULL,           // no text when first created
        SBARS_SIZEGRIP |         // includes a sizing grip
        WS_CHILD | WS_VISIBLE,   // creates a visible child window
        0, 0, 0, 0,              // ignores size and position
        hwndParent,              // handle to parent window
        (HMENU)idStatus,       // child window identifier
        hinst,                   // handle to application instance
        NULL);                   // no window creation data

    
    configstatusbar(hwndParent, hwndStatus);

    return hwndStatus;
}
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
    DoCreateStatusBar(hwnd, ID_STATUSBAR_MAIN, yinyue200_hInstance, 4);
    ShowWindow(hwnd, yinyue200_nCmdShow);


    return 0;
}

LRESULT ListViewNotify(HWND hWnd, LPARAM lParam)
{
    LPNMHDR  lpnmh = (LPNMHDR)lParam;
    HWND     hwndListView = GetDlgItem(hWnd, ID_LISTVIEW_MAIN);
    YINYUE200_MAINWINDOWDATA* windata = GetProp(hWnd, YINYUE200_WINDOW_DATA);
    if (windata==NULL)
    {
        return 0;
    }
    switch (lpnmh->code)
    {
    case LVN_GETDISPINFO:
    {
        LV_DISPINFO* lpdi = (LV_DISPINFO*)lParam;
        TCHAR szString[MAX_PATH];
        PRODUCTRECORD* record = VECTOR_GET(windata->PagedNowList, PRODUCTRECORD*, lpdi->item.iItem);
        if (record)
        {
            if (lpdi->item.iSubItem)
            {
                if (lpdi->item.mask & LVIF_TEXT)
                {
                    switch (lpdi->item.iSubItem)
                    {
                    case 1:
                    {
                        swprintf_s(szString, _countof(szString),
                            TEXT("%lld"),
                            record->ID);
                        wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,
                            szString, _TRUNCATE);
                        break;
                    }
                    default:
                    {
                        swprintf_s(szString, _countof(szString),
                            TEXT("项目 %d - Column %d"),
                            lpdi->item.iItem + 1, lpdi->item.iSubItem);
                        wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,
                            szString, _TRUNCATE);
                    }
                    }

                }
            }
            else
            {
                //load name
                if (lpdi->item.mask & LVIF_TEXT)
                {
                    wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,
                        record->Name, _TRUNCATE);
                }

                if (lpdi->item.mask & LVIF_IMAGE)
                {
                    lpdi->item.iImage = 0;
                }
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
    case LVN_COLUMNCLICK:
    {
        LPNMLISTVIEW info = lParam;
        YINYUE200_MAINWINDOWDATA* windata = GetProp(hWnd, YINYUE200_WINDOW_DATA);
        if (windata->sortcomindex == info->iSubItem)
        {
            windata->sortmethod++;
            if (windata->sortmethod > 2)
            {
                windata->sortmethod = 0;
                windata->sortcomindex = -1;
            }
        }
        else
        {
            windata->sortcomindex = info->iSubItem;
            windata->sortmethod = 1;
        }
        windata->sortstate = 0;
        Yinyue200_Main_UpdateListViewData(hWnd);
    }
    case NM_DBLCLK:
    {
        LPNMITEMACTIVATE lpnmitem = lParam;
        if (lpnmitem->iItem >= 0)
        {
            CreateEditItemWindow(VECTOR_GET(windata->PagedNowList, PRODUCTRECORD_PTR, lpnmitem->iItem));
        }
        break;
    }
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
BOOL InsertListViewItems(HWND hwndListView,size_t count)
{
    //empty the list
    ListView_DeleteAllItems(hwndListView);

    //set the number of items in the list
    ListView_SetItemCount(hwndListView, count);

    return TRUE;
}
#define DEFINE_NAMEANDTHEIRDISPLAYSORTORDER(name){ TEXT(name) ,TEXT(name) L" ↓",TEXT(name) L" ↑" }
#define MAINWINDOW_COLUMNCOUNT 5
void Yinyue200_Main_SetListViewColumn(HWND hwnd,BOOL first)
{
    HWND hwndListView = GetDlgItem(hwnd, ID_LISTVIEW_MAIN);
    LPWSTR       szString_o[MAINWINDOW_COLUMNCOUNT][3] = {
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("名称"), 
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("ID"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("Column 2"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("Column 3"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("Column 4")
    };
    LV_COLUMN   lvColumn;
    int         i;
    LPWSTR       szString[MAINWINDOW_COLUMNCOUNT];

    YINYUE200_MAINWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
    int selecol = -1;
    int mode = 0;
    if (windata)
    {
        selecol = windata->sortcomindex;
        mode = windata->sortmethod;
    }

    for (int i = 0; i < MAINWINDOW_COLUMNCOUNT; i++)
    {
        szString[i] = szString_o[i][i == selecol ? mode : 0];
    }

    //empty the list
    if(first)
        ListView_DeleteAllItems(hwndListView);

    //initialize the columns
    lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvColumn.fmt = LVCFMT_LEFT;
    lvColumn.cx = 120;
    for (i = 0; i < MAINWINDOW_COLUMNCOUNT; i++)
    {
        lvColumn.pszText = szString[i];
        if(first)
            ListView_InsertColumn(hwndListView, i, &lvColumn);
        else
            ListView_SetColumn(hwndListView, i, &lvColumn);
    }
}
BOOL Yinyue200_Main_InitListView(HWND hwnd,HWND hwndListView)
{
    Yinyue200_Main_SetListViewColumn(hwnd, true);

    InsertListViewItems(hwndListView, 0);

    return TRUE;
}
typedef struct Yinyue200_Main_ListViewSortContext
{
    void* (*GetCompareObject)(void* obj);
    BOOL IS_REV_RESULT;
} YINYUE200_MAINLISTVIEWSORTCONTEXT;
int Yinyue200_Main_UpdateListViewData_PWSTRCompare(void* pcontext, void const* left, void const* right)
{
    YINYUE200_MAINLISTVIEWSORTCONTEXT* context = pcontext;
    int result = wcscmp(context->GetCompareObject(left), context->GetCompareObject(right));
    if (context->IS_REV_RESULT)
        return -result;
    else
        return result;
}
int Yinyue200_Main_UpdateListViewData_int64Compare(void* pcontext, void const* left, void const* right)
{
    YINYUE200_MAINLISTVIEWSORTCONTEXT* context = pcontext;
    int64_t const* l = context->GetCompareObject(left);
    int64_t const* r = context->GetCompareObject(right);
    int result = *l - *r;
    if (context->IS_REV_RESULT)
        return -result;
    else
        return result;
}
void Yinyue200_Main_UpdateListViewData(HWND hwnd)
{
    HWND lastpagebtn = GetDlgItem(hwnd, ID_BUTTON_PREVPAGE);
    HWND nextpagebtn = GetDlgItem(hwnd, ID_BUTTON_NEXTPAGE);
    HWND editpagebtn = GetDlgItem(hwnd, ID_EDIT_PAGE);
    HWND listview = GetDlgItem(hwnd, ID_LISTVIEW_MAIN);
    YINYUE200_MAINWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
    if (windata)
    {
        YINYUE200_MAINLISTVIEWSORTCONTEXT qsortcontext = { 0 };
        qsortcontext.IS_REV_RESULT = windata->sortmethod == 2;
        if (windata->sortstate == 0)
        {
            windata->sortstate = 1;
            vector nn = vector_clone(&windata->UnsortedNowList);
            VECTOR_FREE(windata->NowList);
            VECTOR_MOVE(windata->NowList, nn);
            switch (windata->sortcomindex)
            {
            case -1:
            {
                //do nothing
                break;
            }
            case 0:
            {
                qsortcontext.GetCompareObject = yinyue200_GetProductRecordName;
                vector_qsort(&windata->NowList, Yinyue200_Main_UpdateListViewData_PWSTRCompare, &qsortcontext);
                break;
            }
            case 1:
                qsortcontext.GetCompareObject = yinyue200_GetProductRecordID;
                vector_qsort(&windata->NowList, Yinyue200_Main_UpdateListViewData_int64Compare, &qsortcontext);
                break;
            default:
                break;
            }
        }
        if (IsDlgButtonChecked(hwnd, ID_CHECKBOX_PAGE))
        {
        RESETPAGE:;
            VECTOR_CLEAR(windata->PagedNowList);
            for (size_t i = windata->pagestart; i < VECTOR_TOTAL(windata->NowList); i++)
            {
                VECTOR_ADD(windata->PagedNowList, vector_get(&windata->NowList, i));
            }
            if (VECTOR_TOTAL(windata->PagedNowList) == 0)
            {
                windata->pagestart = 0;
                if (VECTOR_TOTAL(windata->NowList) > 0)
                {
                    goto RESETPAGE;
                }
            }
        }
        else
        {
            vector nn = vector_clone(&windata->NowList);
            VECTOR_FREE(windata->PagedNowList);
            VECTOR_MOVE(windata->PagedNowList, nn);
        }
        Yinyue200_Main_SetListViewColumn(hwnd, false);
        InsertListViewItems(listview, VECTOR_TOTAL(windata->PagedNowList));

        wchar_t message[30];
        swprintf(message, 30, L"已加载 %d 项", (int)(VECTOR_TOTAL(windata->NowList)));
        SendMessage(GetDlgItem(hwnd, ID_STATUSBAR_MAIN), SB_SETTEXT, MAKEWPARAM(0, 0), message);
    }
}

void UpdateCheckBoxInfo(HWND hwnd,YINYUE200_MAINWINDOWDATA* windowdata)
{
    HWND lastpagebtn = GetDlgItem(hwnd, ID_BUTTON_PREVPAGE);
    HWND nextpagebtn = GetDlgItem(hwnd, ID_BUTTON_NEXTPAGE);
    HWND editpagebtn = GetDlgItem(hwnd, ID_EDIT_PAGE);

    if (IsDlgButtonChecked(hwnd, ID_CHECKBOX_PAGE))
    {
        ShowWindow(lastpagebtn, SW_SHOW);
        ShowWindow(nextpagebtn, SW_SHOW);
        ShowWindow(editpagebtn, SW_SHOW);
        if (windowdata->pagestart + VECTOR_TOTAL(windowdata->PagedNowList) >= VECTOR_TOTAL(windowdata->NowList))
        {
            EnableWindow(nextpagebtn, FALSE);
        }
        else
        {
            EnableWindow(nextpagebtn, TRUE);
        }
        EnableWindow(lastpagebtn, windowdata->pagestart > 0);
    }
    else
    {
        ShowWindow(lastpagebtn, SW_HIDE);
        ShowWindow(nextpagebtn, SW_HIDE);
        ShowWindow(editpagebtn, SW_HIDE);
    }
    Yinyue200_Main_UpdateListViewData(hwnd);
}
void yinyue200_main_loadnowlist(HWND hwnd,YINYUE200_MAINWINDOWDATA *windata)
{
    vector t = vector_clone(&yinyue200_ProductList);;
    VECTOR_MOVE(windata->UnsortedNowList, t);
    windata->sortstate = 0;
    //OutputDebugString((VECTOR_GET(windata->UnsortedNowList,PRODUCTRECORD_PTR,0))->Name);

    Yinyue200_Main_UpdateListViewData(hwnd);
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
        //HMENU hSave = CreateMenu();
        //HMENU hLoadAll = CreateMenu();
        //HMENU hfliter = CreateMenu();
        //HMENU hfliterLoadAll = CreateMenu();
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hFile, L"文件");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hFind, L"查询");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hAdd, L"新增");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hAns, L"统计");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hUsr, L"用户管理");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hHelp, L"帮助");

        AppendMenu(hHelp, MF_STRING, ID_MENU_VWS, L"GitHub");
        AppendMenu(hHelp, MF_STRING, ID_MENU_ABOUT, L"关于");
        AppendMenu(hAdd, MF_STRING, ID_MENU_ADDRECORD, L"添加单个条目");
        AppendMenu(hFile, MF_STRING, ID_MENU_SAVE, L"保存");
        AppendMenu(hFind, MF_STRING, ID_MENU_LOADALL, L"查询所有");
        AppendMenu(hFind, MF_STRING, ID_MENU_FLITER, L"筛选现有数据");
        AppendMenu(hFind, MF_STRING, ID_MENU_FLITERLOADALL, L"条件查询");

        SetMenu(hwnd, hMenubar);

        HWND listview = Yinyue200_Main_CreateListView(hwnd);
        Yinyue200_Main_InitListView(hwnd,listview);

        int buttony = 500;
        int buttonx = 10;
        HWND hwndPageButton = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"按页显示",      // Button text 
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
            L"上一页",      // Button text 
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
            L"下一页",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            buttonx,         // x position 
            buttony,         // y position 
            100,        // Button width
            50,        // Button height
            hwnd,     // Parent window
            ID_BUTTON_NEXTPAGE,       // No menu.
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.
        HWND hwndRemoveSelectedItemsButton = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"删除所选",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            buttonx,         // x position 
            buttony,         // y position 
            100,        // Button width
            50,        // Button height
            hwnd,     // Parent window
            ID_BUTTON_REMOVESELECTEDITEMS,       // No menu.
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.

        YINYUE200_MAINWINDOWDATA* windata = malloc(sizeof(YINYUE200_MAINWINDOWDATA));
        if (windata)
        {
            memset(windata, 0, sizeof(YINYUE200_MAINWINDOWDATA));
            windata->sortcomindex = -1;
            windata->WindowHwnd = hwnd;
            SetProp(hwnd, YINYUE200_WINDOW_DATA, windata);
            UpdateCheckBoxInfo(hwnd, windata);
        }
        else
        {
            UnrecoveryableFailed();
        }
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
            MessageBox(NULL, L"版本 0.0.1\r\nA GUI program\r\nold fashion design", L"关于", 0);
        }
        else if (LOWORD(wParam) == ID_MENU_ADDRECORD)
        {
            CreateEditItemWindow(NULL);
        }
        else if (LOWORD(wParam) == ID_MENU_SAVE)
        {
            yinyue200_ProductRecordSaveToFile(yinyue200_GetConfigFilePath(),&yinyue200_ProductList);
            MessageBox(hwnd, L"保存成功", L"消息", 0);
        }
        else if (LOWORD(wParam) == ID_MENU_LOADALL)
        {
            {
                YINYUE200_MAINWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
                if (windata)
                {
                    yinyue200_main_loadnowlist(hwnd, windata);
                }
            }
        }
        else if (LOWORD(wParam) == ID_MENU_FLITERLOADALL)
        {
            YINYUE200_MAINWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
            if (windata)
            {
                yinyue200_main_loadnowlist(hwnd, windata);
                CreateLoadDataFilterWindow(windata);
            }
        }
        else if (LOWORD(wParam) == ID_MENU_FLITER)
        {
            YINYUE200_MAINWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
            if (windata)
            {
                CreateLoadDataFilterWindow(windata);
            }
        }
        else
        {
            YINYUE200_MAINWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
            if (windata)
            {
                switch (HIWORD(wParam))
                {
                case BN_CLICKED:
                {
                    switch (LOWORD(wParam))
                    {
                    case ID_CHECKBOX_PAGE:
                        UpdateCheckBoxInfo(hwnd, windata);
                        break;
                    case ID_BUTTON_NEXTPAGE:
                        windata->pagestart += MAIN_DISPLAYPAGESIZE;
                        Yinyue200_Main_UpdateListViewData(hwnd);
                        break;
                    case ID_BUTTON_PREVPAGE:
                        windata->pagestart -= MAIN_DISPLAYPAGESIZE;
                        if (windata->pagestart < 0)
                            windata->pagestart = 0;
                        Yinyue200_Main_UpdateListViewData(hwnd);
                        break;
                    case ID_BUTTON_REMOVESELECTEDITEMS: 
                    {
                        HWND hListView = GetDlgItem(hwnd, ID_LISTVIEW_MAIN);
                        int iPos = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
                        if (iPos == -1)
                        {
                            MessageBox(hwnd, L"当前没有选择任何项", NULL, 0);
                        }
                        windata->sortstate = 0;
                        while (iPos != -1) {
                            // iPos is the index of a selected item
                            // do whatever you want with it
                            PRODUCTRECORD_PTR productrecord = VECTOR_GET(windata->PagedNowList, PRODUCTRECORD_PTR, iPos);
                            for (size_t i = 0; i < VECTOR_TOTAL(yinyue200_ProductList); i++)
                            {
                                PRODUCTRECORD_PTR allproduct = VECTOR_GET(yinyue200_ProductList, PRODUCTRECORD_PTR, i);
                                if (allproduct == productrecord)
                                {
                                    VECTOR_DELETE(yinyue200_ProductList, i);
                                    break;
                                }
                            }
                            for (size_t i = 0; i < VECTOR_TOTAL(windata->UnsortedNowList); i++)
                            {
                                PRODUCTRECORD_PTR allproduct = VECTOR_GET(windata->UnsortedNowList, PRODUCTRECORD_PTR, i);
                                if (allproduct == productrecord)
                                {
                                    VECTOR_DELETE(windata->UnsortedNowList, i);
                                    break;
                                }
                            }


                            // Get the next selected item
                            iPos = ListView_GetNextItem(hListView, iPos, LVNI_SELECTED);
                        }
                        Yinyue200_Main_UpdateListViewData(hwnd);
                    }

                    default:
                        break;
                    }
                }
                break;
                case EN_CHANGE:
                {
                    switch (LOWORD(wParam))
                    {
                    case ID_EDIT_PAGE:
                    {
                        HWND editpagectrl = GetDlgItem(hwnd,ID_EDIT_PAGE);
                        LPWSTR pagestr = CreateWstrForWindowText(editpagectrl);
                        int pagenum;
                        int startpos;
                        if (swscanf(pagestr, L"%d", &pagenum) == 1&& 
                            ((startpos=(pagenum-1)*MAIN_DISPLAYPAGESIZE)<VECTOR_TOTAL(windata->NowList)||startpos==0)
                            &&startpos>=0
                            )
                        {
                            windata->pagestart = startpos;
                            Yinyue200_Main_UpdateListViewData(hwnd);
                            PostMessage(GetDlgItem(hwnd, ID_STATUSBAR_MAIN), SB_SETTEXT, MAKEWPARAM(1, 0), L"");
                        }
                        else
                        {
                            PostMessage(GetDlgItem(hwnd, ID_STATUSBAR_MAIN), SB_SETTEXT, MAKEWPARAM(1, 0), L"无效的页码");
                        }
                    }
                        break;
                    default:
                        break;
                    }
                }
                break;
                }
            }

        }
    }
    return 0;
    case WM_DESTROY:
    {
        YINYUE200_MAINWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if (windata)
        {
            free(windata);
        }
        RemoveProp(hwnd, YINYUE200_WINDOW_DATA);
        DecreaseWindowCount();
        PostQuitMessage(0);
        return 0;
    }
    case WM_SIZE:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        OnStatusbarSize(hwnd, wParam, x, y);
        break;
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
