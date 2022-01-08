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
#include "EditItemWindow.h"
#include "ProductRecord.h"
#include "ControlsCommonOperation.h"
#include "DpiHelper.h"
#include "RoutePointEditWindow.h"
#include "TicketsManage.h"
#include "LoginWindow.h"
#include <commctrl.h>
#define EDITITEMWINDOW_COLUMNCOUNT 4
#define ID_EDIT_NAME 1
#define ID_BUTTON_SAVE 2
#define ID_BUTTON_CANCEL 3
#define ID_EDIT_ID 4
#define ID_EDIT_TYPE 5
#define ID_EDIT_STATE 6
#define ID_EDIT_REPEAT 13
#define ID_EDIT_STARTDATE 14 //首次发车日期编辑控件（日期选择控件）
#define ID_LABEL_STARTDATE 15
#define ID_BUTTON_SAVEANDNEXT 16
#define ID_LABEL_NAME 17
#define ID_LABEL_ID 18
#define ID_LABEL_STATE 19
#define ID_LABEL_REPEAT 20
#define ID_LABEL_TYPE 21
#define ID_EDIT_STARTTIME 22 //首次发车时间编辑控件（时间选择控件）
#define ID_LABEL_STARTTIME 23
#define ID_LABEL_ROUTEEDITNOTICE 24 //路段信息编辑控件上方提示
#define ID_LISTVIEW_ROUTE 25
#define ID_BUTTON_ROUTEADD 26
#define ID_BUTTON_ROUTEDELETE 27
#define ID_EDIT_FIRSTCLASSSEATCOUNT 28
#define ID_LABEL_FIRSTCLASSSEATCOUNT 29
#define ID_EDIT_SECONDCLASSSEATCOUNT 30
#define ID_LABEL_SECONDCLASSSEATCOUNT 31
#define ID_EDIT_BUSINESSCLASSSEATCOUNT 32
#define ID_LABEL_BUSINESSCLASSSEATCOUNT 33
#define ID_LABEL_TICKETINFO 34
#define ID_EDIT_BOOKTICKETDATESELECTION 35 //订票日期选择控件
#define ID_BUTTON_SEARCHTICKETS 36 //搜索余票
#define ID_LABEL_TICKETDATA 37
//#define ID_LABEL_BOOKTICKETTYPE 38
#define ID_EDIT_BOOKTICKETTYPE 39 //combobox
#define ID_LABEL_PASSENGERSELECTION 40
#define ID_EDIT_PASSENGERSELECTION_1 41
#define ID_EDIT_PASSENGERSELECTION_2 42
#define ID_EDIT_PASSENGERSELECTION_3 43
#define ID_BUTTON_BOOKTICKETS 44


typedef struct Yinyue200_EditItemWindowData
{
    YINYUE200_TRAINPLANRECORD_PTR TrainPlanRecord;
    vector Route;
    bool enablesave;
    bool bookmode;
    HFONT lastfont;
    HFONT largefont;

    PWSTR startstation;
    PWSTR endstation;

    vector passengers;
} YINYUE200_EDITITEMWINDOWDATA;
void editwindowaddoreditroutepointcallback(YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR data, void* context);
YINYUE200_EDITITEMWINDOW_ROUTEPOINTADDOREDIT_CALLBACK_CONTEXT* CreateYinyue200_EditItemWindow_RoutePointAddOrEdit_Callback_Context();
LRESULT CALLBACK EditItemWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateEditItemWindow(YINYUE200_TRAINPLANRECORD_PTR productrecord, bool enablesave, bool bookmode)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"yinyue200.SimpleStoreErp.EditItemWindow";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = EditItemWindowProc;
    wc.hInstance = yinyue200_hInstance;
    wc.lpszClassName = CLASS_NAME;

    WORD result = RegisterClass(&wc);
    //printf("%d", result);

    YINYUE200_EDITITEMWINDOWDATA* windowdata = yinyue200_safemallocandclear(sizeof(YINYUE200_EDITITEMWINDOWDATA));
    windowdata->TrainPlanRecord = productrecord;
    windowdata->enablesave = enablesave;
    windowdata->bookmode = bookmode;

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"车次信息",    // Window text
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
        return;
    }



    ShowWindow(hwnd, yinyue200_nCmdShow);


    return;
}
void Yinyue200_EditItemWindow_UpdateTicketInfo(HWND hwnd, YINYUE200_EDITITEMWINDOWDATA* windata)
{
    HWND ticketinfohwnd = Yinyue200_GetChildControlById(hwnd, ID_LABEL_TICKETINFO);
    if (windata->startstation && windata->endstation)
    {
        size_t count = 100 + wcslen(windata->startstation) + wcslen(windata->endstation);
        LPWSTR buffer = CreateWSTR(count);
        swprintf(buffer, count, L"你当前的订单信息\r\n%s → %s\r\n\r\n开车日期：", windata->startstation, windata->endstation);
        SendMessage(ticketinfohwnd, WM_SETTEXT, 0, buffer);
        free(buffer);
    }
    else if (windata->startstation)
    {
        size_t count = 100 + wcslen(windata->startstation);
        LPWSTR buffer = CreateWSTR(count);
        swprintf(buffer, count, L"你当前的订单信息\r\n%s →（未设置）\r\n\r\n开车日期：", windata->startstation);
        SendMessage(ticketinfohwnd, WM_SETTEXT, 0, buffer);
        free(buffer);
    }
    else if (windata->endstation)
    {
        size_t count = 100 + wcslen(windata->endstation);
        LPWSTR buffer = CreateWSTR(count);
        swprintf(buffer, count, L"你当前的订单信息\r\n（未设置）→ %s\r\n\r\n开车日期：", windata->endstation);
        SendMessage(ticketinfohwnd, WM_SETTEXT, 0, buffer);
        free(buffer);
    }
    else
    {
        SendMessage(ticketinfohwnd, WM_SETTEXT, 0, L"你当前的订单信息\r\n空（请先在左下方设置起点和终点）\r\n\r\n开车日期：");
    }
}

#define SETNULLORPRODUCTINFOMEMBERDATA(chwnd,member) SendMessage(GetDlgItem(hwnd,chwnd), WM_SETTEXT, 0, productrecord==NULL?L"":productrecord->##member);
#define SETNULLORPRODUCTINFOMEMBERINT32DATA_EXPR(chwnd,expr) if(productrecord==NULL)SendMessage(GetDlgItem(hwnd,chwnd), WM_SETTEXT, 0, L"");else{ WCHAR _temp_buffer[30];swprintf(_temp_buffer,30, L"%d", (expr)); SendMessage(GetDlgItem(hwnd,chwnd), WM_SETTEXT, 0, _temp_buffer);}
#define SETNULLORPRODUCTINFOMEMBERINTDATA_EXPR(chwnd,expr) if(productrecord==NULL)SendMessage(GetDlgItem(hwnd,chwnd), WM_SETTEXT, 0, L"");else{ WCHAR _temp_buffer[30];swprintf(_temp_buffer,30, L"%lld", (expr)); SendMessage(GetDlgItem(hwnd,chwnd), WM_SETTEXT, 0, _temp_buffer);}
#define SETNULLORPRODUCTINFOMEMBERINTDATA(chwnd,member) SETNULLORPRODUCTINFOMEMBERINTDATA_EXPR(chwnd,productrecord->##member)
#define SETNULLORPRODUCTINFOMEMBERPRICEDATA(chwnd,member) if(productrecord==NULL)SendMessage(GetDlgItem(hwnd,chwnd), WM_SETTEXT, 0, L"");else{ WCHAR _temp_buffer[30];swprintf(_temp_buffer,30, L"%lf", productrecord->##member); SendMessage(GetDlgItem(hwnd,chwnd), WM_SETTEXT, 0, _temp_buffer);}
#define SAVEPRODUCTINFOMEMBERDATA(memberid,member) do\
{\
    free(productrecord->##member);\
    productrecord->##member=CreateWstrForWindowText(GetDlgItem(hwnd,memberid));\
}while(0);
#define SAVEPRODUCTINFOMEMBERINT32DATA(memberid,member,expr,defaultdata) do{PWCHAR _temp_int32_str = CreateWstrForWindowText(GetDlgItem(hwnd,memberid));\
int32_t _temp_int32;\
int _temp_ret = swscanf(_temp_int32_str, L"%d", &_temp_int32);\
if (_temp_ret == 1)\
{\
    (expr) = _temp_int32;\
}\
else if(_temp_int32_str[0]!=0)\
{\
    MessageBox(hwnd, TEXT(#member) L"格式错误", NULL, 0);\
}\
if(_temp_ret < 1) (expr) = defaultdata;\
free(_temp_int32_str);\
}while(0)
#define SAVEPRODUCTINFOMEMBERINTDATA(memberid,member,defaultdata) do{PWCHAR _temp_int64_str = CreateWstrForWindowText(GetDlgItem(hwnd,memberid));\
int64_t _temp_int64;\
int _temp_ret = swscanf(_temp_int64_str, L"%lld", &_temp_int64);\
if (_temp_ret == 1)\
{\
    productrecord->##member = _temp_int64;\
}\
else if(_temp_int64_str[0]!=0)\
{\
    MessageBox(hwnd, TEXT(#member) L"格式错误", NULL, 0);\
}\
if(_temp_ret < 1) productrecord->##member = defaultdata;\
free(_temp_int64_str);\
}while(0)
#define SAVEPRODUCTINFOMEMBERPAIROFUINT64DATA(memberid,member) do{PWCHAR _temp_int64_str = CreateWstrForWindowText(GetDlgItem(hwnd,memberid));\
int64_t _temp_int64_1;int64_t _temp_int64_2;\
if (swscanf(_temp_int64_str, L"%llu;%llu", &_temp_int64_1, &_temp_int64_2) == 2)\
{\
    YINYUE200_PAIR_OF_uint64_t_uint64_t _temp_pair_uint64 = {_temp_int64_1,_temp_int64_2};\
    productrecord->##member = _temp_pair_uint64;\
}\
else if(_temp_int64_str[0]!=0)\
{\
    MessageBox(hwnd, TEXT(#member) L"格式错误", NULL, 0);\
}\
free(_temp_int64_str);\
}while(0)
#define SAVEPRODUCTINFOMEMBERDATEDATA(memberid,member) do{PWCHAR _temp_int64_str = CreateWstrForWindowText(GetDlgItem(hwnd,memberid));\
int64_t _temp_int64;\
int _temp_a,_temp_b,_temp_c;\
if (swscanf(_temp_int64_str, L"%d/%d/%d", &_temp_a,&_temp_b,&_temp_c) == 3)\
{\
    _temp_int64=_temp_a*10000+_temp_b*100+_temp_c;\
    productrecord->##member = _temp_int64;\
}\
else if (swscanf(_temp_int64_str, L"%lld", &_temp_int64) == 1)\
{\
    productrecord->##member = _temp_int64;\
}\
else  if(_temp_int64_str[0]!=0)\
{\
    MessageBox(hwnd, TEXT(#member) L"格式错误", NULL, 0);\
}\
free(_temp_int64_str);\
}while(0)
#define SAVEPRODUCTINFOMEMBERPRICEDATA(memberid,member) do{PWCHAR _temp_int64_str = CreateWstrForWindowText(GetDlgItem(hwnd,memberid));\
int _temp_ret=Yinyue200_EditWindowParseFromStringAndFree(_temp_int64_str,&(productrecord->##member));\
if(_temp_ret<0)\
{\
    MessageBox(hwnd, TEXT(#member) L"格式错误", NULL, 0);\
}\
}while(0)
#define ADDLABELANDEDIT(ctrl_id,displaylabel) HWND Hwnd_##ctrl_id##_Label =  Yinyue200_FastCreateLabelControl(hwnd,ID_LABEL_##ctrl_id,displaylabel);\
HWND hwnd_##ctrl_id##_Edit = Yinyue200_FastCreateEditControl(hwnd,ID_EDIT_##ctrl_id);
#define YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(tag) do{\
YINYUE200_SETCONTROLPOSANDFONT(ID_LABEL_##tag, 10, lasty, 500, 20);\
lasty+=25;\
YINYUE200_SETCONTROLPOSANDFONT(ID_EDIT_##tag, 10, lasty, 500, 20);\
lasty+=25;\
}while (0)
void LayoutControls_EditItemWindow(HWND hwnd, UINT dpi, YINYUE200_EDITITEMWINDOWDATA* windata)
{
    if (windata->lastfont)
    {
        HFONT font = windata->lastfont;
        int lasty = 10;
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(NAME);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(ID);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(TYPE);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(STATE);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(REPEAT);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(STARTDATE);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(STARTTIME);
        YINYUE200_SETCONTROLPOSANDFONT(ID_LABEL_ROUTEEDITNOTICE, 10, lasty, 100, 25);
        lasty += 25;
        //路线编辑列表在此
        YINYUE200_SETCONTROLPOSANDFONT(ID_LISTVIEW_ROUTE, 10, lasty, 500, 200);
        lasty += 200;
        YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_ROUTEADD, 10, lasty, 100, 25);
        YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_ROUTEDELETE, 10 + 100, lasty, 100, 25);
        lasty += 35;
        //座位label
        YINYUE200_SETCONTROLPOSANDFONT(ID_LABEL_BUSINESSCLASSSEATCOUNT, 10, lasty, 100, 25);
        YINYUE200_SETCONTROLPOSANDFONT(ID_LABEL_FIRSTCLASSSEATCOUNT, 20 + 100, lasty, 100, 25);
        YINYUE200_SETCONTROLPOSANDFONT(ID_LABEL_SECONDCLASSSEATCOUNT, 10 + 200 + 20, lasty, 100, 25);
        lasty += 25;
        //座位EDIT
        YINYUE200_SETCONTROLPOSANDFONT(ID_EDIT_BUSINESSCLASSSEATCOUNT, 10, lasty, 100, 25);
        YINYUE200_SETCONTROLPOSANDFONT(ID_EDIT_FIRSTCLASSSEATCOUNT, 20 + 100, lasty, 100, 25);
        YINYUE200_SETCONTROLPOSANDFONT(ID_EDIT_SECONDCLASSSEATCOUNT, 10 + 200 + 20, lasty, 100, 25);
        lasty += 25;
        lasty += 10;
        YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_SAVE, 10, lasty, 100, 50);
        YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_SAVEANDNEXT, 20 + 100, lasty, 100, 50);
        YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_CANCEL, 10 + 200 + 20, lasty, 100, 50);

        if (windata->bookmode)
        {
            font = windata->largefont;
            YINYUE200_SETCONTROLPOSANDFONT(ID_LABEL_TICKETINFO, 520, 10, 450, 100);
            YINYUE200_SETCONTROLPOSANDFONT(ID_LABEL_TICKETDATA, 520, 170, 450, 125);
            font = windata->lastfont;
            YINYUE200_SETCONTROLPOSANDFONT(ID_EDIT_BOOKTICKETDATESELECTION, 520, 110, 200, 25);
            YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_SEARCHTICKETS, 520, 140, 100, 25);

            YINYUE200_SETCONTROLPOSANDFONT(ID_EDIT_BOOKTICKETTYPE, 520, 295, 200, 25);

            YINYUE200_SETCONTROLPOSANDFONT(ID_LABEL_PASSENGERSELECTION, 520, 325, 450, 25);
            YINYUE200_SETCONTROLPOSANDFONT(ID_EDIT_PASSENGERSELECTION_1, 520, 355, 450, 25);
            YINYUE200_SETCONTROLPOSANDFONT(ID_EDIT_PASSENGERSELECTION_2, 520, 380, 450, 25);
            YINYUE200_SETCONTROLPOSANDFONT(ID_EDIT_PASSENGERSELECTION_3, 520, 405, 450, 25);

            YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_BOOKTICKETS, 700, 430, 50, 50);
        }

    }
}
//设置ID编辑框为指定的值
void setidtoeditcontrol(HWND hwnd, YINYUE200_PAIR_OF_uint64_t_uint64_t pair)
{
    WCHAR _temp_buffer[100];
    swprintf(_temp_buffer, 100, L"%llu;%llu", pair.Item1, pair.Item2);
    SendMessage(GetDlgItem(hwnd, ID_EDIT_ID), WM_SETTEXT, 0, _temp_buffer);
}
//设置编辑控件初值
void edititemwindow_initctrl(HWND hwnd, YINYUE200_TRAINPLANRECORD_PTR productrecord, YINYUE200_EDITITEMWINDOWDATA* windata)
{
    freeTrainPlanRecord_RoutePoints(&windata->Route);

    if (productrecord == NULL)
    {
        vector_init(&windata->Route);
    }
    else
    {
        //将 productrecord->RoutePoints 的内容深复制到 windowdata->Route

        vector* srcvec = &windata->Route;
        vector* orivec = &productrecord->RoutePoints;
        deepcopy_TrainPlanRecord_RoutePoints(srcvec, orivec);
    }

    SETNULLORPRODUCTINFOMEMBERDATA(ID_EDIT_NAME, Name);
    SETNULLORPRODUCTINFOMEMBERDATA(ID_EDIT_TYPE, Type);
    SETNULLORPRODUCTINFOMEMBERDATA(ID_EDIT_STATE, State);
    SETNULLORPRODUCTINFOMEMBERINTDATA(ID_EDIT_REPEAT, Repeat);
    SETNULLORPRODUCTINFOMEMBERINT32DATA_EXPR(ID_EDIT_BUSINESSCLASSSEATCOUNT, Yinyue200_GetTrainPlanRecordSeatCount(productrecord, TRAINTICKETTYPE_BUSINESSCLASS));
    SETNULLORPRODUCTINFOMEMBERINT32DATA_EXPR(ID_EDIT_FIRSTCLASSSEATCOUNT, Yinyue200_GetTrainPlanRecordSeatCount(productrecord, TRAINTICKETTYPE_FIRSTCLASS));
    SETNULLORPRODUCTINFOMEMBERINT32DATA_EXPR(ID_EDIT_SECONDCLASSSEATCOUNT, Yinyue200_GetTrainPlanRecordSeatCount(productrecord, TRAINTICKETTYPE_SECONDCLASS));

    if (productrecord == NULL)
    {
        SYSTEMTIME time;
        GetLocalTime(&time);//时间的呈现始终呈现本地时间，存储使用 UTC 时间，因此此处获取本地时间
        DateTime_SetSystemtime(GetDlgItem(hwnd, ID_EDIT_STARTDATE), GDT_VALID, &time);
        DateTime_SetSystemtime(GetDlgItem(hwnd, ID_EDIT_STARTTIME), GDT_VALID, &time);

        {
            GUID guid;
            CheckHResult(CoCreateGuid(&guid));
            setidtoeditcontrol(hwnd, Yinyue200_ConvertToUint64PairFromGuid(guid));//生成新的 ID
        }
    }
    else
    {
        FILETIME filetime = Yinyue200_ConvertToFileTimeFromUINT64(productrecord->StartTimePoint);
        FILETIME localfiletime;
        Yinyue200_FileTimeToLocalFileTime(&filetime, &localfiletime);//lpLocalFileTime cannot be the same as the lpFileTime parameter.
        SYSTEMTIME systime;
        FileTimeToSystemTime(&localfiletime, &systime);
        DateTime_SetSystemtime(GetDlgItem(hwnd, ID_EDIT_STARTDATE), GDT_VALID, &systime);
        DateTime_SetSystemtime(GetDlgItem(hwnd, ID_EDIT_STARTTIME), GDT_VALID, &systime);

        setidtoeditcontrol(hwnd, productrecord->ID);
    }

    EditItemWindow_InsertListViewItems(Yinyue200_GetChildControlById(hwnd, ID_LISTVIEW_ROUTE), vector_total(&windata->Route));
}
int EditItemWindow_RouteListViewNotify(HWND hwnd, LPARAM lParam)
{
    LPNMHDR lpnmh = (LPNMHDR)lParam;
    HWND hwndListView = GetDlgItem(hwnd, ID_LISTVIEW_ROUTE);
    YINYUE200_EDITITEMWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
    if (windata)
    {
        switch (lpnmh->code)
        {
        case LVN_GETDISPINFO:
        {
            LV_DISPINFO* lpdi = (LV_DISPINFO*)lParam;
            TCHAR szString[MAX_PATH];
            YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR record = VECTOR_GET(windata->Route, YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR, lpdi->item.iItem);
            if (record)
            {
                if (lpdi->item.mask & LVIF_TEXT)
                {
                    switch (lpdi->item.iSubItem)
                    {
                    case 0:
                        wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax, record->Station.DisplayName, _TRUNCATE);//站点信息显示
                        break;
                    case 1:
                        swprintf(lpdi->item.pszText, lpdi->item.cchTextMax, L"%lf", record->Distance / 1000.0);//里程信息显示
                        break;
                    case 2:
                        swprintf(lpdi->item.pszText, lpdi->item.cchTextMax, L"%lf", Yinyue200_ConvertToTotalSecondFromUINT64(record->RouteRunTimeSpan) / 60.0);//运行时间信息显示
                        break;
                    case 3:
                        swprintf(lpdi->item.pszText, lpdi->item.cchTextMax, L"%lf", Yinyue200_ConvertToTotalSecondFromUINT64(record->ResidenceTime) / 60.0);//停留时间信息显示
                        break;
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

        }
        case NM_DBLCLK:
        {
            LPNMITEMACTIVATE lpnmitem = lParam;
            if (lpnmitem->iItem >= 0)
            {
                YINYUE200_EDITITEMWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
                EnableWindow(hwnd, false);
                HWND hListView = GetDlgItem(hwnd, ID_LISTVIEW_ROUTE);
                YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR ptr;
                int iPos = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
                if (iPos >= 0)
                {
                    ptr = vector_get(&windata->Route, iPos);

                    YINYUE200_EDITITEMWINDOW_ROUTEPOINTADDOREDIT_CALLBACK_CONTEXT* callbackcontext = CreateYinyue200_EditItemWindow_RoutePointAddOrEdit_Callback_Context();
                    callbackcontext->hwnd = hwnd;

                    CreateRoutePointEditWindow(ptr, windata->enablesave, editwindowaddoreditroutepointcallback, callbackcontext);
                }

            }
            break;
        }
        }

        return 0;
    }
    else
    {
        return 0;
    }
}
YINYUE200_EDITITEMWINDOW_ROUTEPOINTADDOREDIT_CALLBACK_CONTEXT* CreateYinyue200_EditItemWindow_RoutePointAddOrEdit_Callback_Context()
{
    return yinyue200_safemallocandclear(sizeof(YINYUE200_EDITITEMWINDOW_ROUTEPOINTADDOREDIT_CALLBACK_CONTEXT));
}
BOOL EditItemWindow_InsertListViewItems(HWND hwndListView, size_t count)
{
    //empty the list
    ListView_DeleteAllItems(hwndListView);

    //set the number of items in the list
    ListView_SetItemCount(hwndListView, count);

    return TRUE;
}
void editwindowaddoreditroutepointcallback(YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR data, void* context)
{
    YINYUE200_EDITITEMWINDOW_ROUTEPOINTADDOREDIT_CALLBACK_CONTEXT* callbackcontext = context;
    HWND hwnd = callbackcontext->hwnd;
    EnableWindow(hwnd, true);
    if (data != NULL)
    {
        YINYUE200_EDITITEMWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if (windowdata)
        {
            if (callbackcontext->add)
            {
                vector_add(&windowdata->Route, data);
            }

            EditItemWindow_InsertListViewItems(Yinyue200_GetChildControlById(hwnd, ID_LISTVIEW_ROUTE), vector_total(&windowdata->Route));
        }
        else
        {
            free(data);
        }
    }
    free(context);
}
int __cdecl CompareRoutePoint(void* context, void const* left, void const* right)
{
    YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR* leftobj = left;
    YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR* rightobj = right;
    return (*rightobj)->RouteRunTimeSpan - (*leftobj)->RouteRunTimeSpan;
}
YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR Yinyue200_EditItemWindow_GetSelectedRouted(HWND hwnd, YINYUE200_EDITITEMWINDOWDATA* windata)
{
    HWND hListView = GetDlgItem(hwnd, ID_LISTVIEW_ROUTE);
    int iPos = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
    if (iPos == -1)
    {
        MessageBox(hwnd, L"请至少选择一项", NULL, 0);
        return NULL;
    }
    else
    {
        int nextpos = ListView_GetNextItem(hListView, iPos, LVNI_SELECTED);
        if (nextpos != -1)
        {
            MessageBox(hwnd, L"只能选择一项", NULL, 0);
            return NULL;
        }
        else
        {
            return vector_get(&windata->Route, iPos);
        }
    }
}
void Yinyue200_EditItemWindow_SetPasss(HWND hwnd, YINYUE200_EDITITEMWINDOWDATA* windata)
{
    // Add string to combobox.
    SendMessage(hwnd, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"不选");
    for (int k = 0; k < vector_total(&windata->passengers); k += 1)
    {
        YINYUE200_PASSENGERINFO_PTR passinfo = vector_get(&windata->passengers, k);

        int size = Yinyue200_wcslenWithNull(passinfo->IDType) + Yinyue200_wcslenWithNull(passinfo->IDNumber) + Yinyue200_wcslenWithNull(passinfo->FullName) + 100;

        PWSTR buffer = CreateWSTR(size);

        swprintf(buffer, size, L"%s (%s : %s)", Yinyue200_GetPWSTRWithoutNull(passinfo->FullName), Yinyue200_GetPWSTRWithoutNull(passinfo->IDType), Yinyue200_GetPWSTRWithoutNull(passinfo->IDNumber));

        // Add string to combobox.
        SendMessage(hwnd, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)buffer);

        free(buffer);
    }
}
bool Yinyue200_EditItemWindow_BeforeTicketCheck(HWND hwnd, YINYUE200_EDITITEMWINDOWDATA* windowdata, SYSTEMTIME date)
{
    if (windowdata->TrainPlanRecord->State && wcscmp(windowdata->TrainPlanRecord->State, L"停运") == 0)
    {
        MessageBox(hwnd, L"当前车次已停运，您无法预定本车次车票", NULL, 0);
        return false;
    }
    else if (windowdata->startstation == NULL || windowdata->endstation == NULL)
    {
        MessageBox(hwnd, L"请先在左侧设置订单的起点和终点信息", NULL, 0);
        return false;
    }
    else
    {
        enum Yinyue200_TicketRefuseReason reason = Yinyue200_CheckTrainPlanRecordDateWithBookLimit(windowdata->TrainPlanRecord, date.wYear, date.wMonth, date.wDay, windowdata->startstation, windowdata->endstation);
        switch (reason)
        {
        case YINYUE200_TICKETREFUSERESON_NOREFUSE:
        {
            return true;
        }
        break;
        case YINYUE200_TICKETREFUSERESON_NOPASTTICKET:
            MessageBox(hwnd, L"不能购买过去的车票哦", NULL, 0);
            return false;
        case YINYUE200_TICKETREFUSERESON_TOOLATE:
            MessageBox(hwnd, L"开车前10分钟停止售票哦", NULL, 0);
            return false;
        case YINYUE200_TICKETREFUSERESON_TOOEARLY:
            MessageBox(hwnd, L"不可以预定两个月之后的车票哦", NULL, 0);
            return false;
        case YINYUE200_TICKETREFUSERESON_NOTRAIN:
            MessageBox(hwnd, L"选定的日期不开行指定车次，请换一个日期再试", NULL, 0);
            return false;
        default:
            MessageBox(hwnd, L"订票系统未知错误", NULL, 0);
            return false;
        }
    }
}
LRESULT CALLBACK EditItemWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
        UINT dpi = yinyue200_GetDpiForWindow(hwnd);


        YINYUE200_EDITITEMWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        windowdata->lastfont = yinyue200_CreateDefaultFont(hwnd);
        windowdata->largefont = Yinyue200_CreateFont(15, dpi);

        ADDLABELANDEDIT(NAME, L"车次");
        ADDLABELANDEDIT(ID, L"ID");
        ADDLABELANDEDIT(TYPE, L"类型");
        HWND Hwnd_STATE_Label = Yinyue200_FastCreateLabelControl(hwnd, ID_LABEL_STATE, L"状态");
        HWND hwnd_STATE_Edit = Yinyue200_FastCreateComboBoxControl(hwnd, ID_EDIT_STATE);
        do {
            TCHAR States[2][10] =
            {
                TRAUBPLANSTATEWSTR_NORMAL, TRAUBPLANSTATEWSTR_STOP
            };

            TCHAR A[16];
            int  k = 0;

            memset(&A, 0, sizeof(A));
            for (k = 0; k < 2; k += 1)
            {
                wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)States[k]);

                // Add string to combobox.
                SendMessage(hwnd_STATE_Edit, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);
            }
        } while (0);

        ADDLABELANDEDIT(REPEAT, L"该车次发出的频率（单位：天）");
        ADDLABELANDEDIT(BUSINESSCLASSSEATCOUNT, L"商务座数量");
        ADDLABELANDEDIT(FIRSTCLASSSEATCOUNT, L"一等座数量");
        ADDLABELANDEDIT(SECONDCLASSSEATCOUNT, L"二等座数量");

        HWND hwnd_STARTDATE_Label = Yinyue200_FastCreateLabelControl(hwnd, ID_LABEL_STARTDATE, L"首次发车日期");
        HWND hwnd_STARTDATE_Edit = Yinyue200_FastCreateDatePickControl(hwnd, ID_EDIT_STARTDATE);
        HWND hwnd_STARTTIME_Label = Yinyue200_FastCreateLabelControl(hwnd, ID_LABEL_STARTTIME, windowdata->bookmode ? L"发车时间" : L"首次发车时间");
        HWND hwnd_STARTTIME_Edit = Yinyue200_FastCreateTimePickControl(hwnd, ID_EDIT_STARTTIME);
        HWND hwnd_ROUTEEDITNOTICE_Label = Yinyue200_FastCreateLabelControl(hwnd, ID_LABEL_ROUTEEDITNOTICE, L"路线编辑");

        HWND hwnd_ROUTE_Edit = Yinyue200_FastCreateListViewControl(hwnd, ID_LISTVIEW_ROUTE);


        {
            LV_COLUMN   lvColumn;
            //initialize the columns
            lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
            lvColumn.fmt = LVCFMT_LEFT;
            lvColumn.cx = YINYUE200_LOGICTOPHYBYDPI(120, dpi);
            LPWSTR szString[EDITITEMWINDOW_COLUMNCOUNT] = {
                L"车站",
                L"里程（千米）",
                L"到达时间（分钟）",
                L"停留时间（分钟）"
            };
            for (int i = 0; i < EDITITEMWINDOW_COLUMNCOUNT; i++)
            {
                lvColumn.pszText = szString[i];
                ListView_InsertColumn(hwnd_ROUTE_Edit, i, &lvColumn);
            }
        }

        HWND hwnd_ROUTE_ADDBTN = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_ROUTEADD, windowdata->bookmode ? L"设置为起点" : L"添加路径点");
        HWND hwnd_ROUTE_DELBTN = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_ROUTEDELETE, windowdata->bookmode ? L"设置为终点" : L"删除路径点");

        HWND hwndButton_Save = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_SAVE, L"保存");
        HWND hwndButton_SaveAndNext = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_SAVEANDNEXT, L"保存并添加下一个");
        HWND hwndButton_Cancel = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_CANCEL, L"取消");

        YINYUE200_TRAINPLANRECORD_PTR productrecord = windowdata->TrainPlanRecord;

        edititemwindow_initctrl(hwnd, productrecord, windowdata);

        if (!windowdata->enablesave)
        {
            EnableWindow(hwndButton_Save, false);
            EnableWindow(hwndButton_SaveAndNext, false);
        }

        if (productrecord)
        {
            EnableWindow(hwnd_ID_Edit, false);
            EnableWindow(hwnd_NAME_Edit, false);

            if (windowdata->bookmode)
            {
                ShowWindow(hwndButton_Save, SW_HIDE);
                ShowWindow(hwndButton_SaveAndNext, SW_HIDE);

                EnableWindow(hwnd_STARTDATE_Edit, false);
                EnableWindow(hwnd_STARTTIME_Edit, false);
                EnableWindow(hwnd_STATE_Edit, false);
                EnableWindow(hwnd_TYPE_Edit, false);
                EnableWindow(hwnd_REPEAT_Edit, false);
                EnableWindow(hwnd_BUSINESSCLASSSEATCOUNT_Edit, false);
                EnableWindow(hwnd_FIRSTCLASSSEATCOUNT_Edit, false);
                EnableWindow(hwnd_SECONDCLASSSEATCOUNT_Edit, false);

                Yinyue200_FastCreateLabelControlWithoutContent(hwnd, ID_LABEL_TICKETINFO);
                Yinyue200_EditItemWindow_UpdateTicketInfo(hwnd, windowdata);

                Yinyue200_FastCreateDatePickControl(hwnd, ID_EDIT_BOOKTICKETDATESELECTION);

                Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_SEARCHTICKETS, L"查询可用的车票");
                Yinyue200_FastCreateLabelControlWithoutContent(hwnd, ID_LABEL_TICKETDATA);

                HWND hwnd_BOOKTICKETTYPE_Edit = Yinyue200_FastCreateComboBoxDropListControl(hwnd, ID_EDIT_BOOKTICKETTYPE);

                do
                {
                    TCHAR States[3][10] =
                    {
                        L"商务座", L"一等座", L"二等座"
                    };

                    TCHAR A[16];
                    int  k = 0;

                    memset(&A, 0, sizeof(A));
                    for (k = 0; k < 3; k += 1)
                    {
                        wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)States[k]);

                        // Add string to combobox.
                        SendMessage(hwnd_BOOKTICKETTYPE_Edit, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);
                    }
                } while (0);

                ShowWindow(hwnd_BOOKTICKETTYPE_Edit, SW_HIDE);

                windowdata->passengers = CreateFullListOfPassengerInfoRefWithOwner(GetNowLoginedUserName());

                HWND passlabel = Yinyue200_FastCreateLabelControl(hwnd, ID_LABEL_PASSENGERSELECTION, L"选择乘客（最少1人，最多3人）：");

                HWND pass1 = Yinyue200_FastCreateComboBoxDropListControl(hwnd, ID_EDIT_PASSENGERSELECTION_1);
                HWND pass2 = Yinyue200_FastCreateComboBoxDropListControl(hwnd, ID_EDIT_PASSENGERSELECTION_2);
                HWND pass3 = Yinyue200_FastCreateComboBoxDropListControl(hwnd, ID_EDIT_PASSENGERSELECTION_3);
                ShowWindow(pass1, SW_HIDE);
                ShowWindow(pass2, SW_HIDE);
                ShowWindow(pass3, SW_HIDE);
                ShowWindow(passlabel, SW_HIDE);
                Yinyue200_EditItemWindow_SetPasss(pass1, windowdata);
                Yinyue200_EditItemWindow_SetPasss(pass2, windowdata);
                Yinyue200_EditItemWindow_SetPasss(pass3, windowdata);

                HWND bookticketbutton = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_BOOKTICKETS, L"下单");
                ShowWindow(bookticketbutton, SW_HIDE);
            }
        }


        Yinyue200_SetWindowSize(hwnd, windowdata->bookmode ? 1100 : 700, 780, dpi);

        SIZE winsize = Yinyue200_GetWindowClientAreaSize(hwnd);
        LayoutControls_EditItemWindow(hwnd, dpi, windowdata);

    }
    return 0;
    case WM_COMMAND:
    {
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
        {
            switch (LOWORD(wParam))
            {
            case ID_BUTTON_SEARCHTICKETS:
                //当前一定在bookmode下
            {
                YINYUE200_EDITITEMWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);

                SYSTEMTIME date;
                DateTime_GetSystemtime(GetDlgItem(hwnd, ID_EDIT_BOOKTICKETDATESELECTION), &date);

                if (Yinyue200_EditItemWindow_BeforeTicketCheck(hwnd, windowdata, date))
                {
                    double businessprice = Yinyue200_TicketManage_GetPrice(windowdata->TrainPlanRecord, windowdata->startstation, windowdata->endstation, TRAINTICKETTYPE_BUSINESSCLASS) / 100.0;
                    double firstprice = Yinyue200_TicketManage_GetPrice(windowdata->TrainPlanRecord, windowdata->startstation, windowdata->endstation, TRAINTICKETTYPE_FIRSTCLASS) / 100.0;
                    double secondprice = Yinyue200_TicketManage_GetPrice(windowdata->TrainPlanRecord, windowdata->startstation, windowdata->endstation, TRAINTICKETTYPE_SECONDCLASS) / 100.0;

                    uint64_t uint64date = Yinyue200_ConvertToUINT64FromFileTime(ConvertDateToLocalFILETIME(date.wYear, date.wMonth, date.wDay));

                    YINYUE200_SEATINFOCACHE_PTR seatinfo = Yinyue200_GetUsedTicketCount(windowdata->TrainPlanRecord, uint64date);
                    BITVECTOR seatvec = Yinyue200_GetSeatUsability(windowdata->TrainPlanRecord, uint64date, windowdata->startstation, windowdata->endstation, seatinfo);

                    int32_t businesscount = Yinyue200_GetUseableSeatsNumber(windowdata->TrainPlanRecord, &seatvec, TRAINTICKETTYPE_BUSINESSCLASS, seatinfo);
                    int32_t firstcount = Yinyue200_GetUseableSeatsNumber(windowdata->TrainPlanRecord, &seatvec, TRAINTICKETTYPE_FIRSTCLASS, seatinfo);
                    int32_t secondcount = Yinyue200_GetUseableSeatsNumber(windowdata->TrainPlanRecord, &seatvec, TRAINTICKETTYPE_SECONDCLASS, seatinfo);

                    PWSTR buffer = CreateWSTR(300);

                    swprintf(buffer, 300, L"商务座（ %.2lf 元）：%d 张\r\n一等座（ %.2lf 元）：%d 张\r\n二等座（ %.2lf 元）：%d 张\r\n\r\n请选择你要预定的座位类别", businessprice, businesscount, firstprice, firstcount, secondprice, secondcount);
                    SendMessage(Yinyue200_GetChildControlById(hwnd, ID_LABEL_TICKETDATA), WM_SETTEXT, 0, buffer);

                    ShowWindow(Yinyue200_GetChildControlById(hwnd, ID_EDIT_BOOKTICKETTYPE), SW_SHOW);

                    ShowWindow(Yinyue200_GetChildControlById(hwnd, ID_EDIT_PASSENGERSELECTION_1), SW_SHOW);
                    ShowWindow(Yinyue200_GetChildControlById(hwnd, ID_EDIT_PASSENGERSELECTION_2), SW_SHOW);
                    ShowWindow(Yinyue200_GetChildControlById(hwnd, ID_EDIT_PASSENGERSELECTION_3), SW_SHOW);
                    ShowWindow(Yinyue200_GetChildControlById(hwnd, ID_LABEL_PASSENGERSELECTION), SW_SHOW);
                    ShowWindow(Yinyue200_GetChildControlById(hwnd, ID_BUTTON_BOOKTICKETS), SW_SHOW);



                    free(buffer);
                    BitVector_Free(&seatvec);
                }
                break;
            case ID_BUTTON_BOOKTICKETS:
            {
                YINYUE200_EDITITEMWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);

                SYSTEMTIME date;
                DateTime_GetSystemtime(GetDlgItem(hwnd, ID_EDIT_BOOKTICKETDATESELECTION), &date);

                if (Yinyue200_EditItemWindow_BeforeTicketCheck(hwnd, windowdata, date))
                {
                    //首先检查乘客信息
                    vector selectindexs;
                    vector_init_int(&selectindexs);
                    int sel1 = SendMessage(Yinyue200_GetChildControlById(hwnd, ID_EDIT_PASSENGERSELECTION_1), (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                    int sel2 = SendMessage(Yinyue200_GetChildControlById(hwnd, ID_EDIT_PASSENGERSELECTION_2), (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                    int sel3 = SendMessage(Yinyue200_GetChildControlById(hwnd, ID_EDIT_PASSENGERSELECTION_3), (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                    int seatlevel = SendMessage(Yinyue200_GetChildControlById(hwnd, ID_EDIT_BOOKTICKETTYPE), (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                    enum TrainSeatType seattype = TRAINTICKETTYPE_UNKNOWN;
                    switch (seatlevel)
                    {
                    case 0:
                        seattype = TRAINTICKETTYPE_BUSINESSCLASS;
                        break;
                    case 1:
                        seattype = TRAINTICKETTYPE_FIRSTCLASS;
                        break;
                    case 2:
                        seattype = TRAINTICKETTYPE_SECONDCLASS;
                        break;
                    default:
                        break;
                    }
                    int sels[3] = { sel1,sel2,sel3 };
                    for (int i = 0; i < 3; i++)
                    {
                        int one = sels[i];
                        if (sels[i] != -1 && sels[i] != 0)
                        {
                            for (int j = 0; j < vector_total_int(&selectindexs); j++)
                            {
                                int innerone = vector_get_int(&selectindexs, j);
                                if (innerone == one)
                                {
                                    MessageBox(hwnd, L"乘客不能相同", NULL, 0);
                                    goto exitbookticket;
                                }
                            }
                            vector_add_int(&selectindexs, one);
                        }
                    }
                    for (int j = 0; j < vector_total_int(&selectindexs); j++)
                    {
                        (*vector_getPointer_int(&selectindexs, j))--;
                    }

                    if (vector_total_int(&selectindexs) > 0)
                    {
                        uint64_t uint64date = Yinyue200_ConvertToUINT64FromFileTime(ConvertDateToLocalFILETIME(date.wYear, date.wMonth, date.wDay));
                        YINYUE200_SEATINFOCACHE_PTR seatinfo = Yinyue200_GetUsedTicketCount(windowdata->TrainPlanRecord, uint64date);
                        BITVECTOR seatvec = Yinyue200_GetSeatUsability(windowdata->TrainPlanRecord, uint64date, windowdata->startstation, windowdata->endstation, seatinfo);

                        int32_t seatcount = Yinyue200_GetUseableSeatsNumber(windowdata->TrainPlanRecord, &seatvec, seattype, seatinfo);

                        if (seatcount >= vector_total_int(&selectindexs))//检查余票是否能订这么多张
                        {
                            int succcount = 0;
                            int failedcount = 0;
                            int32_t price = 0;
                            for (int i = 0; i < vector_total_int(&selectindexs); i++)
                            {
                                int one = vector_get_int(&selectindexs, i);
                                YINYUE200_PASSENGERINFO_PTR passinfo = vector_get(&windowdata->passengers, one);
                                YINYUE200_TICKET_PTR ticket =
                                    Yinyue200_BookTickets(windowdata->TrainPlanRecord, passinfo, date.wYear, date.wMonth, date.wDay,
                                        windowdata->startstation, windowdata->endstation, seattype, GetNowLoginedUserName());

                                if (ticket == NULL)
                                {
                                    failedcount++;
                                }
                                else
                                {
                                    succcount++;
                                    price += ticket->Price;
                                }
                            }
                            double doubleprice = price / 100.0;
                            PWSTR buffer = CreateWSTR(100);
                            swprintf(buffer, 100, L"操作完成，购买成功 %d 张，购买失败 %d 张，共 %.2lf 元", succcount, failedcount, doubleprice);
                            MessageBox(hwnd, buffer, L"提示", 0);
                            free(buffer);
                        }
                        else
                        {
                            MessageBox(hwnd, seatcount == 1 ? L"好像没票了" : L"余票不足", NULL, 0);
                        }

                        BitVector_Free(&seatvec);
                    }
                    else
                    {
                        MessageBox(hwnd, L"请先选择乘客", NULL, 0);
                    }
                exitbookticket:;
                    vector_free_int(&selectindexs);
                }

            }
            break;
            case ID_BUTTON_SAVEANDNEXT:
            case ID_BUTTON_SAVE:
            {
                YINYUE200_EDITITEMWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);

                YINYUE200_TRAINPLANRECORD_PTR productrecord = windowdata->TrainPlanRecord == NULL ?
                    CreateTrainPlanRecord() : windowdata->TrainPlanRecord;
                SAVEPRODUCTINFOMEMBERDATA(ID_EDIT_NAME, Name);
                SAVEPRODUCTINFOMEMBERPAIROFUINT64DATA(ID_EDIT_ID, ID);
                SAVEPRODUCTINFOMEMBERDATA(ID_EDIT_TYPE, Type);
                SAVEPRODUCTINFOMEMBERDATA(ID_EDIT_STATE, State);
                SAVEPRODUCTINFOMEMBERINTDATA(ID_EDIT_REPEAT, Repeat, 1);
                SAVEPRODUCTINFOMEMBERINT32DATA(ID_EDIT_BUSINESSCLASSSEATCOUNT, BUSINESSCLASSSEATCOUNT, *Yinyue200_GetTrainPlanRecordSeatCountPointer(productrecord, TRAINTICKETTYPE_BUSINESSCLASS), 0);
                SAVEPRODUCTINFOMEMBERINT32DATA(ID_EDIT_FIRSTCLASSSEATCOUNT, FIRSTCLASSSEATCOUNT, *Yinyue200_GetTrainPlanRecordSeatCountPointer(productrecord, TRAINTICKETTYPE_FIRSTCLASS), 0);
                SAVEPRODUCTINFOMEMBERINT32DATA(ID_EDIT_SECONDCLASSSEATCOUNT, SECONDCLASSSEATCOUNT, *Yinyue200_GetTrainPlanRecordSeatCountPointer(productrecord, TRAINTICKETTYPE_SECONDCLASS), 0);


                {
                    SYSTEMTIME date;
                    SYSTEMTIME time;
                    DateTime_GetSystemtime(GetDlgItem(hwnd, ID_EDIT_STARTDATE), &date);
                    DateTime_GetSystemtime(GetDlgItem(hwnd, ID_EDIT_STARTTIME), &time);
                    date.wHour = time.wHour;
                    date.wMinute = time.wMinute;
                    date.wSecond = time.wSecond;
                    date.wMilliseconds = time.wMilliseconds;
                    FILETIME filetime;
                    SystemTimeToFileTime(&date, &filetime);
                    FILETIME utcfiletime;
                    Yinyue200_LocalFileTimeToFileTime(&filetime, &utcfiletime);
                    productrecord->StartTimePoint = Yinyue200_ConvertToUINT64FromFileTime(utcfiletime);
                }

                freeTrainPlanRecord_RoutePoints(&productrecord->RoutePoints);

                //排序points
                vector_qsort(&windowdata->Route, CompareRoutePoint, NULL);

                deepcopy_TrainPlanRecord_RoutePoints(&productrecord->RoutePoints, &windowdata->Route);

                if (windowdata->TrainPlanRecord == NULL)
                {
                    AddTrainPlanRecord(productrecord);
                }

                if (LOWORD(wParam) == ID_BUTTON_SAVEANDNEXT)
                {
                    windowdata->TrainPlanRecord = NULL;
                    edititemwindow_initctrl(hwnd, NULL, windowdata);
                }
                else
                {
                    SendMessage(hwnd, WM_CLOSE, NULL, NULL);
                }
                break;
            }
            case ID_BUTTON_CANCEL:
                SendMessage(hwnd, WM_CLOSE, NULL, NULL);
                break;
            case ID_BUTTON_ROUTEADD:
            {
                YINYUE200_EDITITEMWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
                if (windata->bookmode)
                {
                    YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR route = Yinyue200_EditItemWindow_GetSelectedRouted(hwnd, windata);
                    if (route)
                    {
                        free(windata->startstation);
                        windata->startstation = CreateWstrFromWstr(route->Station.DisplayName);
                        Yinyue200_EditItemWindow_UpdateTicketInfo(hwnd, windata);
                    }
                }
                else
                {
                    EnableWindow(hwnd, false);

                    YINYUE200_EDITITEMWINDOW_ROUTEPOINTADDOREDIT_CALLBACK_CONTEXT* callbackcontext = CreateYinyue200_EditItemWindow_RoutePointAddOrEdit_Callback_Context();
                    callbackcontext->hwnd = hwnd;
                    callbackcontext->add = true;

                    CreateRoutePointEditWindow(NULL, windata->enablesave, editwindowaddoreditroutepointcallback, callbackcontext);
                }

            }
            break;
            case ID_BUTTON_ROUTEDELETE:
            {
                YINYUE200_EDITITEMWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
                if (windata->bookmode)
                {
                    YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR route = Yinyue200_EditItemWindow_GetSelectedRouted(hwnd, windata);
                    if (route)
                    {
                        free(windata->endstation);
                        windata->endstation = CreateWstrFromWstr(route->Station.DisplayName);
                        Yinyue200_EditItemWindow_UpdateTicketInfo(hwnd, windata);
                    }
                }
                else
                {
                    HWND hListView = GetDlgItem(hwnd, ID_LISTVIEW_ROUTE);
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
                                YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR tobedel = vector_get(&windata->Route, tobedelindex);
                                free(tobedel->Station.DisplayName);
                                free(tobedel);
                                vector_delete_int(&windata->Route, tobedelindex);
                            }
                        }
                        vector_free_int(vec);
                        free(vec);
                    }

                    EditItemWindow_InsertListViewItems(hListView, vector_total(&windata->Route));
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
        return 0;
    }
        case WM_DESTROY:
        {
            YINYUE200_EDITITEMWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);

            freeTrainPlanRecord_RoutePoints(&windowdata->Route);

            yinyue200_DeleteFont(windowdata->largefont);
            yinyue200_DeleteFont(windowdata->lastfont);
            if (windowdata != NULL)
            {
                free(windowdata->startstation);
                free(windowdata->endstation);
                free(windowdata);
            }
            RemoveProp(hwnd, YINYUE200_WINDOW_DATA);
            DecreaseWindowCount();
            CheckIfNoWindowAndQuit();
            return 0;
        }
        case WM_DPICHANGED:
        {
            YINYUE200_EDITITEMWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
            if (windowdata && windowdata->lastfont)
            {
                yinyue200_DeleteFont(windowdata->lastfont);
                windowdata->lastfont = yinyue200_CreateDefaultFont(hwnd);
                LayoutControls_EditItemWindow(hwnd, HIWORD(wParam), windowdata);
            }
            break;
        }
        case WM_NOTIFY:
        {
            if (LOWORD(wParam) == ID_LISTVIEW_ROUTE)
            {
                return EditItemWindow_RouteListViewNotify(hwnd, lParam);
            }
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