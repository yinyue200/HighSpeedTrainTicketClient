//  HighSpeedTrainTicketClient
//  Copyright(C) 2022 ��Խ
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

#define ID_LISTVIEW_DATA 1
#define ID_BUTTON_DEL 2
#define ID_BUTTON_ADD 3
#define ID_EDIT_SEARCH 4
#define ID_BUTTON_SEARCH 5
#define ID_COMBOBOX_SEARCH 6
typedef struct Yinyue200_TicketManageWindowData
{
    vector UnsortedNowList;
    vector NowList;
    int sortcomindex;//������к�
    int sortstate;//�Ƿ��Ѿ�Ӧ������
    int sortmethod;//������
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
        L"�˿͹���",    // Window text
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
//ȷ�����ؼ�λ��
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

    YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_ADD, 10, buttony, 50, 25);
    YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_DEL, 70, buttony, 50, 25);

}
YINYUE200_TICKETMANAGEWINDOW_PASSENGERADDOREDIT_CALLBACK_CONTEXT* CreateYinyue200_TicketManageWindow_PassengerAddOrEdit_Callback_Context()
{
    return yinyue200_safemallocandclear(sizeof(YINYUE200_TICKETMANAGEWINDOW_PASSENGERADDOREDIT_CALLBACK_CONTEXT));
}
void TicketManageWindow_addoreditcallback(YINYUE200_TICKET_PTR data, void* context)
{
    YINYUE200_TICKETMANAGEWINDOW_PASSENGERADDOREDIT_CALLBACK_CONTEXT* callbackcontext = context;
    HWND hwnd = callbackcontext->hwnd;
    if (data != NULL)
    {
        YINYUE200_TICKETMANAGEWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
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
#define YINYUE200_SEARCH_IMPL(name, id) case id:rev = wcscmp(record->name, searchtext)==0;break
#define YINYUE200_SEARCH_IMPL_DATE(name, id) case id:rev = wcscmp(record->name, searchtext)==0;break
#define YINYUE200_COMBOBOXITEMSCOUNT 7
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
        HWND HWND_SEARCH_BUTTON = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_SEARCH, L"��ѯ");
        HWND HWND_SEARCH_COMBOBOX = Yinyue200_FastCreateComboBoxDropListControl(hwnd, ID_COMBOBOX_SEARCH);
        HWND HWND_ADD_BUTTON = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_ADD, L"���");
        HWND HWND_DEL_BUTTON = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_DEL, L"ɾ��");


        {
            TCHAR ComboBoxItems[YINYUE200_COMBOBOXITEMSCOUNT][10] =
            {
                TEXT("֤������"), TEXT("֤����"), TEXT("����"), TEXT("������"),
                TEXT("�ֻ���"), TEXT("������ϵ������"), TEXT("������ϵ���ֻ���")
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
                    //�������г˿�����
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
                            YINYUE200_SEARCH_IMPL(PassengerIDType, 1);
                            YINYUE200_SEARCH_IMPL(PassengerID, 2);
                            YINYUE200_SEARCH_IMPL(CreatedDate, 3);
                            YINYUE200_SEARCH_IMPL(TrainName, 4);
                            YINYUE200_SEARCH_IMPL(TrainTime, 5);
                            YINYUE200_SEARCH_IMPL(StartStation, 6);
                            YINYUE200_SEARCH_IMPL(EndStation, 7);
                            YINYUE200_SEARCH_IMPL(Price, 8);
                            YINYUE200_SEARCH_IMPL(SeatNumber, 9);
                            YINYUE200_SEARCH_IMPL(SeatLevel, 10);
                            YINYUE200_SEARCH_IMPL(TrainStartTime, 13);
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
                    //�˴�����Ҫ�ͷ�vector�����ݣ���Ϊ����Ȩû�б�ת��
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
                    MessageBox(hwnd, L"��ǰû��ѡ���κ���", NULL, 0);
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
                    swprintf(warningmsg, 50, L"��ȷ��Ҫɾ�� %d ����¼��", vector_total_int(vec));
                    if (MessageBox(hwnd, warningmsg, L"��ʾ", MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
                    {
                        for (int i = vector_total(vec) - 1; i >= 0; i--)
                        {
                            int tobedelindex = vector_get_int(vec, i);
                            YINYUE200_TICKETMANAGEWINDOWDATA* tobedel = vector_get(&windata->NowList, tobedelindex);
                            DeletePassenger(tobedel);
                        }
                    }
                    vector_free_int(vec);
                    free(vec);
                }

                Yinyue200_TicketManageWindow_InsertListViewItems(hListView, vector_total(&windata->NowList));
            }
            break;
            case ID_BUTTON_ADD:
            {
                YINYUE200_TICKETMANAGEWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);

                YINYUE200_TICKETMANAGEWINDOW_PASSENGERADDOREDIT_CALLBACK_CONTEXT* callbackcontext = CreateYinyue200_TicketManageWindow_PassengerAddOrEdit_Callback_Context();
                callbackcontext->hwnd = hwnd;
                callbackcontext->add = true;

                CreatePassengerRecordEditWindow(NULL, TicketManageWindow_addoreditcallback, callbackcontext);
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
                    //LISTVIEWNOTIFTLOADCOLWSTR(0, IDType)
                    //    LISTVIEWNOTIFTLOADCOLWSTR(1, IDNumber)
                    //    LISTVIEWNOTIFTLOADCOLWSTR(2, FullName)
                    //    LISTVIEWNOTIFTLOADCOLWSTR(3, Owner)
                    //    LISTVIEWNOTIFTLOADCOLWSTR(4, PhoneNum)
                    //    LISTVIEWNOTIFTLOADCOLWSTR(5, EmergencyContactPersonFullName)
                    //    LISTVIEWNOTIFTLOADCOLWSTR(6, EmergencyContactPhoneNumber)
                default:
                    {
                        swprintf_s(szString, _countof(szString),
                            TEXT("��Ŀ %d - Column %d"),
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

                YINYUE200_TICKETMANAGEWINDOW_PASSENGERADDOREDIT_CALLBACK_CONTEXT* callbackcontext = CreateYinyue200_TicketManageWindow_PassengerAddOrEdit_Callback_Context();
                callbackcontext->hwnd = hWnd;

                CreatePassengerRecordEditWindow(ptr, TicketManageWindow_addoreditcallback, callbackcontext);
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
BOOL Yinyue200_TicketManageWindow_InsertListViewItems(HWND hwndListView, size_t count)
{
    //empty the list
    ListView_DeleteAllItems(hwndListView);

    //set the number of items in the list
    ListView_SetItemCount(hwndListView, count);

    return TRUE;
}
#define DEFINE_NAMEANDTHEIRDISPLAYSORTORDER(name){ TEXT(name) ,TEXT(name) L" ��",TEXT(name) L" ��" }
#define MAINWINDOW_COLUMNCOUNT 7
void Yinyue200_TicketManageWindow_SetListViewColumn(HWND hwnd, BOOL first)
{
    UINT dpi = yinyue200_GetDpiForWindow(hwnd);

    HWND hwndListView = GetDlgItem(hwnd, ID_LISTVIEW_DATA);
    LPWSTR       szString_o[MAINWINDOW_COLUMNCOUNT][3] = {
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("֤������"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("֤������"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("����"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("������"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("�ֻ���"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("������ϵ������"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("������ϵ�˵绰"),
    };//��ά���飬�洢��ͬ���ڲ�ͬ����״̬�µ���ͷ�ַ���
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
            //�����ǲ�ͬ�ֶ��������ʵ��
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

        Yinyue200_TicketManageWindow_SetListViewColumn(hwnd, false);
        Yinyue200_TicketManageWindow_InsertListViewItems(listview, VECTOR_TOTAL(windata->NowList));


    }
}
