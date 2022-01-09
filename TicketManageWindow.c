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
#include "TicketManageWindow.h"
#include "TicketsManage.h"
#include "MainWindow.h"
#include "LoginWindow.h"
#include "TicketPreviewWindow.h"

#define ID_LISTVIEW_DATA 1
#define ID_BUTTON_DEL 2
#define ID_BUTTON_PRINT 3
#define ID_EDIT_SEARCH 4
#define ID_BUTTON_SEARCH 5
#define ID_COMBOBOX_SEARCH 6
typedef struct Yinyue200_TicketManageWindowData
{
    vector UnsortedNowList;
    vector NowList;
    int sortcomindex;//排序的列号
    int sortstate;//是否已经应用排序
    int sortmethod;//升降序
    HFONT lastfont;
} YINYUE200_TICKETMANAGEWINDOWDATA;
LRESULT CALLBACK TicketManageWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void Yinyue200_TicketManageWindow_UpdateListViewData(HWND hwnd);
void Yinyue200_TicketManageWindow_SetListViewColumn(HWND hwnd, BOOL first);
LRESULT Yinyue200_TicketManageWindow_InsertListViewItems_ListViewNotify(HWND hWnd, LPARAM lParam);
void CreateTicketManageWindow()
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"yinyue200.HighSpeedTrainTicketClient.TicketManageWindow";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = TicketManageWindowProc;
    wc.hInstance = yinyue200_hInstance;
    wc.lpszClassName = CLASS_NAME;

    WORD result = RegisterClass(&wc);

    YINYUE200_TICKETMANAGEWINDOWDATA* windowdata = yinyue200_safemallocandclear(sizeof(YINYUE200_TICKETMANAGEWINDOWDATA));
    windowdata->sortcomindex = -1;

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"车票管理",    // Window text
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
BOOL Yinyue200_TicketManageWindow_InitListView(HWND hwnd, HWND hwndListView)
{
    Yinyue200_TicketManageWindow_SetListViewColumn(hwnd, true);

    Yinyue200_TicketManageWindow_InsertListViewItems(hwndListView, 0);

    return TRUE;
}
//确定各控件位置
void LayoutControls_TicketManageWindow(HWND hwnd, UINT dpi, YINYUE200_TICKETMANAGEWINDOWDATA* windata, int x, int y)
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

    YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_PRINT, 10, buttony, 50, 25);
    YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_DEL, 70, buttony, 50, 25);

}

#define YINYUE200_SEARCH_IMPL(name, id) case id:rev = wcscmp(record->name, searchtext)==0;break
#define YINYUE200_SEARCH_IMPL_DATE(name, id) case id:\
{\
int _temp_year = 0;\
int _temp_month = 0;\
int _temp_day = 0;\
int _temp_ret=swscanf(searchtext, L"%d/%d/%d",&_temp_year,&_temp_month,&_temp_day);\
uint64_t _temp_localdatepart=GetLocalDatePartUINT64OFUINT64(record->name);\
FILETIME _temp_filetime= Yinyue200_ConvertToFileTimeFromUINT64(_temp_localdatepart);\
SYSTEMTIME _temp_systime; \
FileTimeToSystemTime(&_temp_filetime,&_temp_systime);\
rev=_temp_systime.wYear==_temp_year;\
if(_temp_month!=0) rev&= _temp_systime.wMonth==_temp_month;\
if(_temp_day!=0) rev&= _temp_systime.wDay==_temp_day;\
}\
break
#define YINYUE200_SEARCH_IMPL_DATETIME(name, id) case id:rev = wcscmp(record->name, searchtext)==0;break
#define YINYUE200_COMBOBOXITEMSCOUNT 8
LRESULT CALLBACK TicketManageWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

        YINYUE200_TICKETMANAGEWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        windata->lastfont = yinyue200_CreateDefaultFont(hwnd);

        HWND HWND_Listview = Yinyue200_FastCreateListViewControl(hwnd, ID_LISTVIEW_DATA);
        HWND HWND_SEARCH_EDIT = Yinyue200_FastCreateEditControl(hwnd, ID_EDIT_SEARCH);
        HWND HWND_SEARCH_BUTTON = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_SEARCH, L"查询");
        HWND HWND_SEARCH_COMBOBOX = Yinyue200_FastCreateComboBoxDropListControl(hwnd, ID_COMBOBOX_SEARCH);
        HWND HWND_ADD_BUTTON = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_PRINT, L"打印");
        HWND HWND_DEL_BUTTON = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_DEL, L"退票");


        {
            TCHAR ComboBoxItems[YINYUE200_COMBOBOXITEMSCOUNT][10] =
            {
                TEXT("姓名"), TEXT("证件号"), TEXT("乘车日期"), TEXT("购买日期"),
                TEXT("车次"), TEXT("起点"), TEXT("终点"), TEXT("证件类型")
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

        Yinyue200_TicketManageWindow_InitListView(hwnd, HWND_Listview);

        SIZE winsize = Yinyue200_GetWindowClientAreaSize(hwnd);
        LayoutControls_TicketManageWindow(hwnd, yinyue200_GetDpiForWindow(hwnd), windata, winsize.cx, winsize.cy);

    }
    break;
    case WM_SIZE:
    {
        LayoutControls_TicketManageWindow(hwnd, yinyue200_GetDpiForWindow(hwnd),
            GetProp(hwnd, YINYUE200_WINDOW_DATA), LOWORD(lParam), HIWORD(lParam));
        break;
    }
    case WM_DESTROY:
    {
        YINYUE200_TICKETMANAGEWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
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
        YINYUE200_TICKETMANAGEWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if (windowdata && windowdata->lastfont)
        {
            yinyue200_DeleteFont(windowdata->lastfont);
            windowdata->lastfont = yinyue200_CreateDefaultFont(hwnd);
            SIZE winsize = Yinyue200_GetWindowClientAreaSize(hwnd);
            LayoutControls_TicketManageWindow(hwnd, yinyue200_GetDpiForWindow(hwnd), windowdata, winsize.cx, winsize.cy);
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
            Yinyue200_TicketManageWindow_InsertListViewItems_ListViewNotify(hwnd, lParam);
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
                YINYUE200_TICKETMANAGEWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
                vector tobefree = { 0 };
                vector* tobesearch = NULL;
                if (yinyue200_LoganUserInfo == NULL || wcscmp(yinyue200_LoganUserInfo->Type, L"ADMIN") == 0)
                {
                    //加载所有乘客数据
                    tobesearch = Yinyue200_GetFullListOfTicketInfo();
                }
                else
                {
                    tobefree = Yinyue200_CreateFullListOfTicketInfoRefWithOwner(GetNowLoginedUserName());
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
                    YINYUE200_TICKET_PTR record = vector_get(tobesearch, i);

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
                            YINYUE200_SEARCH_IMPL(PassengerName, 0);//todo
                            YINYUE200_SEARCH_IMPL(PassengerID, 1);
                        //case 2:
                        //{
                        //    int _temp_year = 0; 
                        //    int _temp_month = 0; 
                        //    int _temp_day = 0; 
                        //    int _temp_ret = swscanf(searchtext, L"%d/%d/%d", &_temp_year, &_temp_month, &_temp_day); 
                        //    uint64_t _temp_localdatepart = GetLocalDatePartUINT64OFUINT64(record->TrainTime); 
                        //    FILETIME _temp_filetime = Yinyue200_ConvertToFileTimeFromUINT64(_temp_localdatepart); 
                        //    SYSTEMTIME _temp_systime; 
                        //    FileTimeToSystemTime(&_temp_filetime, &_temp_systime); 
                        //    rev = _temp_systime.wYear == _temp_year; 
                        //    if (_temp_month != 0) rev &= _temp_systime.wMonth == _temp_month; 
                        //        if (_temp_day != 0) rev &= _temp_systime.wDay == _temp_day; 
                        //}
                        //    break;
                            YINYUE200_SEARCH_IMPL_DATE(TrainTime, 2);
                            YINYUE200_SEARCH_IMPL_DATE(CreatedTime, 3);
                            YINYUE200_SEARCH_IMPL(TrainName, 4);
                            YINYUE200_SEARCH_IMPL(StartStation, 5);
                            YINYUE200_SEARCH_IMPL(EndStation, 6);
                            YINYUE200_SEARCH_IMPL(PassengerIDType, 7);
                        default:
                            break;
                        }
                    }
                    if (rev)
                    {
                        vector_add(&windata->UnsortedNowList, record);
                    }
                }

                if (tobefree.capacity > 0)
                {
                    //此处不需要释放vector的内容，因为所有权没有被转移
                    vector_free(&tobefree);
                }

                windata->sortstate = 0;
                Yinyue200_TicketManageWindow_UpdateListViewData(hwnd);

                free(searchtext);
            }
            break;
            case ID_BUTTON_DEL:
            {
                YINYUE200_TICKETMANAGEWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
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
                    swprintf(warningmsg, 50, L"你确定要退票这 %d 张车票吗？", vector_total_int(vec));
                    if (MessageBox(hwnd, warningmsg, L"提示", MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
                    {
                        int32_t price = 0;
                        int succcount = 0;
                        int failedcount = 0;

                        for (int i = vector_total(vec) - 1; i >= 0; i--)
                        {
                            int tobedelindex = vector_get_int(vec, i);
                            YINYUE200_TICKETMANAGEWINDOWDATA* tobedel = vector_get(&windata->NowList, tobedelindex);
                            int32_t thisprice = 0;
                            if (Yinyue200_RefundTicket(tobedel,&thisprice))
                            {
                                price += thisprice;
                                succcount++;
                            }
                            else
                            {
                                failedcount++;
                            }
                        }

                        double doubleprice = price / 100.0;
                        PWSTR buffer = CreateWSTR(100);
                        swprintf(buffer, 100, L"操作完成，退票成功 %d 张，退票失败 %d 张，共 %.2lf 元", succcount, failedcount, doubleprice);
                        MessageBox(hwnd, buffer, L"提示", 0);
                        free(buffer);
                    }
                    vector_free_int(vec);
                    free(vec);
                }

                Yinyue200_TicketManageWindow_InsertListViewItems(hListView, vector_total(&windata->NowList));
            }
            break;
            case ID_BUTTON_PRINT:
            {
                YINYUE200_TICKETMANAGEWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
                HWND hListView = GetDlgItem(hwnd, ID_LISTVIEW_DATA);
                int iPos = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
                if (iPos == -1)
                {
                    MessageBox(hwnd, L"当前没有选择任何项", NULL, 0);
                }
                else
                {
                    vector* vec = yinyue200_safemalloc(sizeof(vector));
                    vector_init(vec);

                    while (iPos != -1) {
                        // iPos is the index of a selected item
                        // do whatever you want with it
                        vector_add(vec, vector_get(&windata->NowList, iPos));

                        // Get the next selected item
                        iPos = ListView_GetNextItem(hListView, iPos, LVNI_SELECTED);
                    }
                    wchar_t warningmsg[50];
                    swprintf(warningmsg, 50, L"这将通过打印机打印 %d 张车票，你确定要继续吗？", vector_total_int(vec));
                    if (MessageBox(hwnd, warningmsg, L"提示", MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
                    {
                        Yinyue200_PrintTickets(hwnd, vec->items, vector_total(vec));
                    }
                    vector_free(vec);
                    free(vec);
                }
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
#define LISTVIEWNOTIFTLOADCOLPAIROFINT64(caseid,membername) case caseid:\
{\
    YINYUE200_PAIR_OF_uint64_t_uint64_t _temp_pair = record->##membername;\
    swprintf_s(szString, _countof(szString),\
        TEXT("%llu;%llu"),\
        _temp_pair.Item1,_temp_pair.Item2);\
    wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,\
        szString, _TRUNCATE);\
    break;\
}
#define LISTVIEWNOTIFTLOADCOLDATE(caseid,membername) case caseid:\
{\
    FILETIME _temp_filetime = Yinyue200_ConvertToFileTimeFromUINT64(record->##membername);\
    FILETIME _temp_local_filetime;\
    Yinyue200_FileTimeToLocalFileTime(&_temp_filetime, &_temp_local_filetime);\
    SYSTEMTIME _temp_systemtime;\
    FileTimeToSystemTime(&_temp_local_filetime,&_temp_systemtime);\
    swprintf_s(szString, _countof(szString),\
        TEXT("%d/%d/%d %d:%d"),\
        _temp_systemtime.wYear,_temp_systemtime.wMonth,_temp_systemtime.wDay,_temp_systemtime.wHour,_temp_systemtime.wMinute);\
    wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,\
        szString, _TRUNCATE);\
    break;\
}
#define LISTVIEWNOTIFTLOADCOLPREICE(caseid,membername) case caseid:\
{\
    double tempprice = record->##membername;\
    swprintf_s(szString, _countof(szString),\
        TEXT("%.2lf"),\
        tempprice / 100.0);\
    wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,\
        szString, _TRUNCATE);\
    break;\
}
#define LISTVIEWNOTIFTLOADCOLINT32(caseid,membername) case caseid:\
{\
    swprintf_s(szString, _countof(szString),\
        TEXT("%d"),\
        record->##membername);\
    wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,\
        szString, _TRUNCATE);\
    break;\
}
#define LISTVIEWNOTIFTLOADCOLINT(caseid,membername) case caseid:\
{\
    swprintf_s(szString, _countof(szString),\
        TEXT("%lld"),\
        record->##membername);\
    wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,\
        szString, _TRUNCATE);\
    break;\
}
#define LISTVIEWNOTIFTLOADCOLDOUBLE(caseid,membername) case caseid:\
{\
    swprintf_s(szString, _countof(szString),\
        TEXT("%lf"),\
        record->##membername);\
    wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,\
        szString, _TRUNCATE);\
    break;\
}
#define LISTVIEWNOTIFTLOADCOLWSTR(caseid,membername) case caseid:\
{\
    if(record->##membername!=NULL)\
        wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,record->##membername, _TRUNCATE);\
    else\
        wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,L"", _TRUNCATE);\
    break;\
}
LRESULT Yinyue200_TicketManageWindow_InsertListViewItems_ListViewNotify(HWND hWnd, LPARAM lParam)
{
    LPNMHDR  lpnmh = (LPNMHDR)lParam;
    HWND     hwndListView = GetDlgItem(hWnd, ID_LISTVIEW_DATA);
    YINYUE200_TICKETMANAGEWINDOWDATA* windata = GetProp(hWnd, YINYUE200_WINDOW_DATA);
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
        YINYUE200_TICKET_PTR record = VECTOR_GET(windata->NowList, YINYUE200_TICKET_PTR, lpdi->item.iItem);
        if (record)
        {
            if (lpdi->item.mask & LVIF_TEXT)
            {
                switch (lpdi->item.iSubItem)
                {
                    LISTVIEWNOTIFTLOADCOLWSTR(0, PassengerIDType);
                    LISTVIEWNOTIFTLOADCOLWSTR(1, PassengerID);
                    LISTVIEWNOTIFTLOADCOLWSTR(2, PassengerName);
                    LISTVIEWNOTIFTLOADCOLWSTR(3, Owner);
                    LISTVIEWNOTIFTLOADCOLDATE(4, CreatedTime);
                    LISTVIEWNOTIFTLOADCOLWSTR(5, TrainName);
                    LISTVIEWNOTIFTLOADCOLDATE(6, TrainTime);
                    LISTVIEWNOTIFTLOADCOLWSTR(7, StartStation);
                    LISTVIEWNOTIFTLOADCOLWSTR(8, EndStation);
                    LISTVIEWNOTIFTLOADCOLPREICE(9, Price);
                case 10:
                {
                    wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax, Yinyue200_GetSeatLevelName(record->SeatLevel), _TRUNCATE);
                    break;
                }
                    LISTVIEWNOTIFTLOADCOLINT32(11, SeatNumber);
                    LISTVIEWNOTIFTLOADCOLPAIROFINT64(12, ID);
                    LISTVIEWNOTIFTLOADCOLPAIROFINT64(13, TrainID);
                    LISTVIEWNOTIFTLOADCOLDATE(14, TrainStartTime);
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
        Yinyue200_TicketManageWindow_UpdateListViewData(hWnd);
    }
    case NM_DBLCLK:
    {
        LPNMITEMACTIVATE lpnmitem = lParam;
        if (lpnmitem->iItem >= 0)
        {
            HWND hListView = GetDlgItem(hWnd, ID_LISTVIEW_DATA);
            YINYUE200_TICKET_PTR ptr;
            int iPos = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
            if (iPos >= 0)
            {
                ptr = vector_get(&windata->NowList, iPos);

                //CreatePassengerRecordEditWindow(ptr, TicketManageWindow_addoreditcallback, callbackcontext);
                CreateTicketPreviewWindow(ptr);
            }

        }
        break;
    }
    }

    return 0;
}
#define ITEM_COMPAREIMPL(casenum,tail,name)  case casenum: \
{\
    qsortcontext.GetCompareObject = yinyue200_GetTicketInfo##name;\
    vector_qsort(&windata->NowList, Yinyue200_Main_UpdateListViewData_##tail##Compare, &qsortcontext);\
    break;\
}
BOOL Yinyue200_TicketManageWindow_InsertListViewItems(HWND hwndListView, size_t count)
{
    //empty the list
    ListView_DeleteAllItems(hwndListView);

    //set the number of items in the list
    ListView_SetItemCount(hwndListView, count);

    return TRUE;
}
#define DEFINE_NAMEANDTHEIRDISPLAYSORTORDER(name){ TEXT(name) ,TEXT(name) L" ↑",TEXT(name) L" ↓" }
#define MAINWINDOW_COLUMNCOUNT 15
void Yinyue200_TicketManageWindow_SetListViewColumn(HWND hwnd, BOOL first)
{
    UINT dpi = yinyue200_GetDpiForWindow(hwnd);

    HWND hwndListView = GetDlgItem(hwnd, ID_LISTVIEW_DATA);
    LPWSTR       szString_o[MAINWINDOW_COLUMNCOUNT][3] = {
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("证件类型"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("证件号码"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("姓名"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("订票人"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("订票时间"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("车次"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("开车时间"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("起点"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("终点"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("票价"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("座位等级"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("座号"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("ID"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("车次ID"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("列车始发时间")
    };//二维数组，存储不同列在不同排序状态下的列头字符串
    LV_COLUMN   lvColumn;
    int         i;
    LPWSTR       szString[MAINWINDOW_COLUMNCOUNT];

    YINYUE200_TICKETMANAGEWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
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
void Yinyue200_TicketManageWindow_UpdateListViewData(HWND hwnd)
{
    HWND listview = GetDlgItem(hwnd, ID_LISTVIEW_DATA);
    YINYUE200_TICKETMANAGEWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
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
            ITEM_COMPAREIMPL(0, PWSTR,PassengerIDType);
            ITEM_COMPAREIMPL(1, PWSTR,PassengerID);
            ITEM_COMPAREIMPL(2, PWSTR, PassengerName);
            ITEM_COMPAREIMPL(3, PWSTR, Owner);
            ITEM_COMPAREIMPL(4, uint64, CreatedTime);
            ITEM_COMPAREIMPL(5, PWSTR, TrainName);
            ITEM_COMPAREIMPL(6, uint64, TrainTime);
            ITEM_COMPAREIMPL(7, PWSTR, StartStation);
            ITEM_COMPAREIMPL(8, PWSTR, EndStation);
            ITEM_COMPAREIMPL(9, uint64, Price);
            ITEM_COMPAREIMPL(10,int32, SeatLevel);
            ITEM_COMPAREIMPL(11,int32, SeatNumber);
            ITEM_COMPAREIMPL(12, pairofuint64, ID);
            ITEM_COMPAREIMPL(13, pairofuint64, TrainID);
            ITEM_COMPAREIMPL(14, uint64, TrainStartTime);
            default:
                break;
            }
        }

        Yinyue200_TicketManageWindow_SetListViewColumn(hwnd, false);
        Yinyue200_TicketManageWindow_InsertListViewItems(listview, VECTOR_TOTAL(windata->NowList));


    }
}
