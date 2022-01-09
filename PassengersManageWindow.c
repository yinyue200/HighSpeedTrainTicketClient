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

#include "common.h"
#include "DpiHelper.h"
#include "ControlsCommonOperation.h"
#include <CommCtrl.h>
#include "PassengersManage.h"
#include "PassengersManageWindow.h"
#include "MainWindow.h"
#include "PassengerRecordEditWindow.h"
#include "LoginWindow.h"
#define ID_LISTVIEW_DATA 1
#define ID_BUTTON_DEL 2
#define ID_BUTTON_ADD 3
#define ID_EDIT_SEARCH 4
#define ID_BUTTON_SEARCH 5
#define ID_COMBOBOX_SEARCH 6
typedef struct Yinyue200_PassengersManageWindowData
{
    vector UnsortedNowList;
    vector NowList;
    int sortcomindex;//排序的列号
    int sortstate;//是否已经应用排序
    int sortmethod;//升降序
    HFONT lastfont;
} YINYUE200_PASSENGERSMANAGEWINDOWDATA;
LRESULT CALLBACK PassengersManageWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void Yinyue200_PassengersManageWindow_UpdateListViewData(HWND hwnd);
void Yinyue200_PassengersManageWindow_SetListViewColumn(HWND hwnd, BOOL first);
LRESULT Yinyue200_PassengersManageWindow_InsertListViewItems_ListViewNotify(HWND hWnd, LPARAM lParam);
void CreatePassengersManageWindow()
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"yinyue200.HighSpeedTrainTicketClient.PassengersManageWindow";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = PassengersManageWindowProc;
    wc.hInstance = yinyue200_hInstance;
    wc.lpszClassName = CLASS_NAME;

    WORD result = RegisterClass(&wc);

    YINYUE200_PASSENGERSMANAGEWINDOWDATA* windowdata = yinyue200_safemallocandclear(sizeof(YINYUE200_PASSENGERSMANAGEWINDOWDATA));
    windowdata->sortcomindex = -1;

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"乘客管理",    // Window text
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


    return 0;
}
BOOL Yinyue200_PassengersManageWindow_InitListView(HWND hwnd, HWND hwndListView)
{
    Yinyue200_PassengersManageWindow_SetListViewColumn(hwnd, true);

    Yinyue200_PassengersManageWindow_InsertListViewItems(hwndListView, 0);

    return TRUE;
}
//确定各控件位置
void LayoutControls_PassengersManageWindow(HWND hwnd, UINT dpi, YINYUE200_PASSENGERSMANAGEWINDOWDATA* windata,int x,int y)
{
    HFONT font = windata->lastfont;
    YINYUE200_SETCONTROLPOSANDFONT(ID_COMBOBOX_SEARCH, 10, 10, 100, 25);
    YINYUE200_SETCONTROLPOSANDFONT(ID_EDIT_SEARCH, 120, 10, 300, 25);
    YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_SEARCH, 430, 10, 70, 25);
    double ly = YINYUE200_PHYTOLOGICBYDPI(y, dpi);
    double lx = YINYUE200_PHYTOLOGICBYDPI(x, dpi);
    double listviewy = max(ly - 50 - 45, 0);
    YINYUE200_SETCONTROLPOSANDFONT(ID_LISTVIEW_DATA, 5, 40, lx - 10, listviewy);

    double buttony = ly - 30;

    YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_ADD, 10, buttony, 50, 25);
    YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_DEL, 70, buttony, 50, 25);

}
YINYUE200_PASSENGERSMANAGEWINDOW_PASSENGERADDOREDIT_CALLBACK_CONTEXT* CreateYinyue200_PassengersManageWindow_PassengerAddOrEdit_Callback_Context()
{
    return yinyue200_safemallocandclear(sizeof(YINYUE200_PASSENGERSMANAGEWINDOW_PASSENGERADDOREDIT_CALLBACK_CONTEXT));
}
void PassengersManageWindow_addoreditcallback(YINYUE200_PASSENGERINFO_PTR data, void* context)
{
    YINYUE200_PASSENGERSMANAGEWINDOW_PASSENGERADDOREDIT_CALLBACK_CONTEXT* callbackcontext = context;
    HWND hwnd = callbackcontext->hwnd;
    if (data != NULL)
    {
        YINYUE200_PASSENGERSMANAGEWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if (windowdata)
        {
            if (callbackcontext->add)
            {
                AddPassenger(data);
            }
        }
        else
        {
            FreePassengerInfo(data);
        }
    }
    free(context);
}
#define YINYUE200_SEARCH_IMPL(id,name) case id:rev = wcscmp(record->name, searchtext)==0;break
#define YINYUE200_COMBOBOXITEMSCOUNT 7
LRESULT CALLBACK PassengersManageWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

        YINYUE200_PASSENGERSMANAGEWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        windata->lastfont = yinyue200_CreateDefaultFont(hwnd);

        HWND HWND_Listview = Yinyue200_FastCreateListViewControl(hwnd, ID_LISTVIEW_DATA);
        HWND HWND_SEARCH_EDIT = Yinyue200_FastCreateEditControl(hwnd, ID_EDIT_SEARCH);
        HWND HWND_SEARCH_BUTTON = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_SEARCH, L"查询");
        HWND HWND_SEARCH_COMBOBOX = Yinyue200_FastCreateComboBoxDropListControl(hwnd, ID_COMBOBOX_SEARCH);
        HWND HWND_ADD_BUTTON = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_ADD, L"添加");
        HWND HWND_DEL_BUTTON = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_DEL, L"删除");


        {
            TCHAR ComboBoxItems[YINYUE200_COMBOBOXITEMSCOUNT][10] =
            {
                TEXT("姓名"), TEXT("证件类型"), TEXT("证件号"), TEXT("创建人"),
                TEXT("手机号"), TEXT("紧急联系人姓名"), TEXT("紧急联系人手机号")
            };

            TCHAR A[16];
            int  k = 0;

            memset(&A, 0, sizeof(A));
            for (k = 0; k < YINYUE200_COMBOBOXITEMSCOUNT; k += 1)
            {
                wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)ComboBoxItems[k]);

                // Add string to combobox.
                SendMessage(HWND_SEARCH_COMBOBOX, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);
            }
        }

        Yinyue200_PassengersManageWindow_InitListView(hwnd, HWND_Listview);

        SIZE winsize = Yinyue200_GetWindowClientAreaSize(hwnd);
        LayoutControls_PassengersManageWindow(hwnd, yinyue200_GetDpiForWindow(hwnd), windata, winsize.cx, winsize.cy);

    }
    break;
    case WM_SIZE:
    {
        LayoutControls_PassengersManageWindow(hwnd, yinyue200_GetDpiForWindow(hwnd),
            GetProp(hwnd, YINYUE200_WINDOW_DATA), LOWORD(lParam), HIWORD(lParam));
        break;
    }
    case WM_DESTROY:
    {
        YINYUE200_PASSENGERSMANAGEWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        yinyue200_DeleteFont(windowdata->lastfont);
        if (windowdata != NULL)
            free(windowdata);
        RemoveProp(hwnd, YINYUE200_WINDOW_DATA);
        DecreaseWindowCount();
        CheckIfNoWindowAndQuit();
    }
    break;
    case WM_DPICHANGED:
    {
        YINYUE200_PASSENGERSMANAGEWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if (windowdata && windowdata->lastfont)
        {
            yinyue200_DeleteFont(windowdata->lastfont);
            windowdata->lastfont = yinyue200_CreateDefaultFont(hwnd);
            SIZE winsize = Yinyue200_GetWindowClientAreaSize(hwnd);
            LayoutControls_PassengersManageWindow(hwnd, yinyue200_GetDpiForWindow(hwnd), windowdata, winsize.cx, winsize.cy);
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);



        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
    }
    break;
    case WM_NOTIFY:
    {
        if (LOWORD(wParam) == ID_LISTVIEW_DATA)
        {
            Yinyue200_PassengersManageWindow_InsertListViewItems_ListViewNotify(hwnd, lParam);
        }
    }
    break;
    case WM_COMMAND:
    {
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
        {
            switch (LOWORD(wParam))
            {
            case ID_BUTTON_SEARCH:
            {
                PWSTR searchtext = CreateWstrForWindowText(Yinyue200_GetChildControlById(hwnd, ID_EDIT_SEARCH));
                size_t searchtextlen = wcslen(searchtext);
                YINYUE200_PASSENGERSMANAGEWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
                vector tobefree = { 0 };
                vector* tobesearch = NULL;
                if (yinyue200_LoganUserInfo == NULL || wcscmp(yinyue200_LoganUserInfo->Type, L"ADMIN") == 0)
                {
                    //加载所有乘客数据
                    tobesearch = GetFullListOfPassengerInfo();
                }
                else
                {
                    tobefree = CreateFullListOfPassengerInfoRefWithOwner(GetNowLoginedUserName());
                    tobesearch = &tobefree;
                }
                if (windata->UnsortedNowList.capacity == 0)
                {
                    vector_init(&windata->UnsortedNowList);
                }
                vector_clear(&windata->UnsortedNowList);

                int ItemIndex = SendMessage(Yinyue200_GetChildControlById(hwnd, ID_COMBOBOX_SEARCH), (UINT)CB_GETCURSEL,
                    (WPARAM)0, (LPARAM)0);

                for (int i = 0; i < vector_total(tobesearch); i++)
                {
                    YINYUE200_PASSENGERINFO_PTR record = vector_get(tobesearch, i);
                    if (record->deled == false)
                    {
                        bool rev = false;
                        if (searchtextlen == 0)
                        {
                            rev = true;
                        }
                        else
                        {
                            switch (ItemIndex)
                            {
                            case -1:
                                rev = true;
                                break;
                                YINYUE200_SEARCH_IMPL(1, IDType);
                                YINYUE200_SEARCH_IMPL(2, IDNumber);
                                YINYUE200_SEARCH_IMPL(0, FullName);
                                YINYUE200_SEARCH_IMPL(3, Owner);
                                YINYUE200_SEARCH_IMPL(4, PhoneNum);
                                YINYUE200_SEARCH_IMPL(5, EmergencyContactPersonFullName);
                                YINYUE200_SEARCH_IMPL(6, EmergencyContactPhoneNumber);
                            default:
                                break;
                            }
                        }
                        if (rev)
                        {
                            vector_add(&windata->UnsortedNowList, record);
                        }
                    }
                }

                if (tobefree.capacity > 0)
                {
                    //此处不需要释放vector的内容，因为所有权没有被转移
                    vector_free(&tobefree);
                }

                windata->sortstate = 0;
                Yinyue200_PassengersManageWindow_UpdateListViewData(hwnd);

                free(searchtext);
            }
                break;
            case ID_BUTTON_DEL:
            {
                YINYUE200_PASSENGERSMANAGEWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
                HWND hListView = GetDlgItem(hwnd, ID_LISTVIEW_DATA);
                int iPos = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
                if (iPos == -1)
                {
                    MessageBox(hwnd, L"当前没有选择任何项", NULL, 0);
                }
                else
                {
                    vector* vec = yinyue200_safemalloc(sizeof(vector));
                    vector_init_int(vec);

                    while (iPos != -1) {
                        // iPos is the index of a selected item
                        // do whatever you want with it
                        vector_add_int(vec, iPos);

                        // Get the next selected item
                        iPos = ListView_GetNextItem(hListView, iPos, LVNI_SELECTED);
                    }
                    wchar_t warningmsg[50];
                    swprintf(warningmsg, 50, L"你确定要删除 %d 条记录吗？", vector_total_int(vec));
                    if (MessageBox(hwnd, warningmsg, L"提示", MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
                    {
                        for (int i = vector_total(vec) - 1; i >= 0; i--)
                        {
                            int tobedelindex = vector_get_int(vec, i);
                            YINYUE200_PASSENGERSMANAGEWINDOWDATA *tobedel = vector_get(&windata->NowList, tobedelindex);
                            DeletePassenger(tobedel);
                        }
                    }
                    vector_free_int(vec);
                    free(vec);
                }

                Yinyue200_PassengersManageWindow_InsertListViewItems(hListView, vector_total(&windata->NowList));
            }
            break;
            case ID_BUTTON_ADD:
            {
                YINYUE200_PASSENGERSMANAGEWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);

                YINYUE200_PASSENGERSMANAGEWINDOW_PASSENGERADDOREDIT_CALLBACK_CONTEXT* callbackcontext = CreateYinyue200_PassengersManageWindow_PassengerAddOrEdit_Callback_Context();
                callbackcontext->hwnd = hwnd;
                callbackcontext->add = true;

                CreatePassengerRecordEditWindow(NULL, PassengersManageWindow_addoreditcallback, callbackcontext);
                break;
            }
            }
        }
        break;
        }
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
#define LISTVIEWNOTIFTLOADCOLWSTR(caseid,membername) case caseid:\
{\
    if(record->##membername!=NULL)\
        wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,record->##membername, _TRUNCATE);\
    else\
        wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,L"", _TRUNCATE);\
    break;\
}
LRESULT Yinyue200_PassengersManageWindow_InsertListViewItems_ListViewNotify(HWND hWnd, LPARAM lParam)
{
    LPNMHDR  lpnmh = (LPNMHDR)lParam;
    HWND     hwndListView = GetDlgItem(hWnd, ID_LISTVIEW_DATA);
    YINYUE200_PASSENGERSMANAGEWINDOWDATA* windata = GetProp(hWnd, YINYUE200_WINDOW_DATA);
    if (windata == NULL)
    {
        return 0;
    }
    switch (lpnmh->code)
    {
    case LVN_GETDISPINFO:
    {
        LV_DISPINFO* lpdi = (LV_DISPINFO*)lParam;
        TCHAR szString[MAX_PATH];
        YINYUE200_PASSENGERINFO_PTR record = VECTOR_GET(windata->NowList, YINYUE200_PASSENGERINFO_PTR, lpdi->item.iItem);
        if (record)
        {
            if (lpdi->item.mask & LVIF_TEXT)
            {
                switch (lpdi->item.iSubItem)
                {
                    LISTVIEWNOTIFTLOADCOLWSTR(1,IDType)
                    LISTVIEWNOTIFTLOADCOLWSTR(2,IDNumber)
                    LISTVIEWNOTIFTLOADCOLWSTR(0,FullName)
                    LISTVIEWNOTIFTLOADCOLWSTR(3,Owner)
                    LISTVIEWNOTIFTLOADCOLWSTR(4,PhoneNum)
                    LISTVIEWNOTIFTLOADCOLWSTR(5,EmergencyContactPersonFullName)
                    LISTVIEWNOTIFTLOADCOLWSTR(6,EmergencyContactPhoneNumber)
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
        Yinyue200_PassengersManageWindow_UpdateListViewData(hWnd);
    }
    case NM_DBLCLK:
    {
        LPNMITEMACTIVATE lpnmitem = lParam;
        if (lpnmitem->iItem >= 0)
        {
            HWND hListView = GetDlgItem(hWnd, ID_LISTVIEW_DATA);
            YINYUE200_PASSENGERINFO_PTR ptr;
            int iPos = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
            if (iPos >= 0)
            {
                ptr = vector_get(&windata->NowList, iPos);

                YINYUE200_PASSENGERSMANAGEWINDOW_PASSENGERADDOREDIT_CALLBACK_CONTEXT* callbackcontext = CreateYinyue200_PassengersManageWindow_PassengerAddOrEdit_Callback_Context();
                callbackcontext->hwnd = hWnd;

                CreatePassengerRecordEditWindow(ptr, PassengersManageWindow_addoreditcallback, callbackcontext);
            }

        }
        break;
    }
    }

    return 0;
}
#define ITEM_COMPAREIMPL(casenum,name)  case casenum: \
{\
    qsortcontext.GetCompareObject = yinyue200_GetPassengerInfo##name;\
    vector_qsort(&windata->NowList, Yinyue200_Main_UpdateListViewData_PWSTRCompare, &qsortcontext);\
    break;\
}
BOOL Yinyue200_PassengersManageWindow_InsertListViewItems(HWND hwndListView, size_t count)
{
    //empty the list
    ListView_DeleteAllItems(hwndListView);

    //set the number of items in the list
    ListView_SetItemCount(hwndListView, count);

    return TRUE;
}
#define DEFINE_NAMEANDTHEIRDISPLAYSORTORDER(name){ TEXT(name) ,TEXT(name) L" ↑",TEXT(name) L" ↓" }
#define MAINWINDOW_COLUMNCOUNT 7
void Yinyue200_PassengersManageWindow_SetListViewColumn(HWND hwnd, BOOL first)
{
    UINT dpi = yinyue200_GetDpiForWindow(hwnd);

    HWND hwndListView = GetDlgItem(hwnd, ID_LISTVIEW_DATA);
    LPWSTR       szString_o[MAINWINDOW_COLUMNCOUNT][3] = {
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("姓名"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("证件类型"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("证件号码"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("创建者"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("手机号"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("紧急联系人姓名"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("紧急联系人电话"),
    };//二维数组，存储不同列在不同排序状态下的列头字符串
    LV_COLUMN   lvColumn;
    int         i;
    LPWSTR       szString[MAINWINDOW_COLUMNCOUNT];

    YINYUE200_PASSENGERSMANAGEWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
    int selecol = -1;
    int mode = 0;
    if (windata)
    {
        selecol = windata->sortcomindex;
        if (selecol >= 0)
        {
            mode = windata->sortmethod;
        }
    }

    for (int i = 0; i < MAINWINDOW_COLUMNCOUNT; i++)
    {
        szString[i] = szString_o[i][i == selecol ? mode : 0];
    }

    //empty the list
    if (first)
        ListView_DeleteAllItems(hwndListView);

    //initialize the columns
    lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvColumn.fmt = LVCFMT_LEFT;
    lvColumn.cx = YINYUE200_LOGICTOPHYBYDPI(120, dpi);
    for (i = 0; i < MAINWINDOW_COLUMNCOUNT; i++)
    {
        lvColumn.pszText = szString[i];
        if (first)
            ListView_InsertColumn(hwndListView, i, &lvColumn);
        else
            ListView_SetColumn(hwndListView, i, &lvColumn);
    }
}
void Yinyue200_PassengersManageWindow_UpdateListViewData(HWND hwnd)
{
    HWND listview = GetDlgItem(hwnd, ID_LISTVIEW_DATA);
    YINYUE200_PASSENGERSMANAGEWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
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
            //以下是不同字段排序代码实现
                ITEM_COMPAREIMPL(0, IDType)
                ITEM_COMPAREIMPL(1, IDNumber)
                ITEM_COMPAREIMPL(2, FullName)
                ITEM_COMPAREIMPL(3, Owner)
                ITEM_COMPAREIMPL(4, PhoneNum)
                ITEM_COMPAREIMPL(5, EmergencyContactPersonFullName)
                ITEM_COMPAREIMPL(6, EmergencyContactPhoneNumber)

            default:
                break;
            }
        }

        Yinyue200_PassengersManageWindow_SetListViewColumn(hwnd, false);
        Yinyue200_PassengersManageWindow_InsertListViewItems(listview, VECTOR_TOTAL(windata->NowList));


    }
}
