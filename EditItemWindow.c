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
#include "ProductRecord.h"
#include "ControlsCommonOperation.h"
#include "DpiHelper.h"
#include <commctrl.h>
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
typedef struct Yinyue200_EditItemWindowData
{
    YINYUE200_TRAINPLANRECORD_PTR TrainPlanRecord;
    bool enablesave;
    HFONT lastfont;
} YINYUE200_EDITITEMWINDOWDATA;
LRESULT CALLBACK EditItemWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateEditItemWindow(YINYUE200_TRAINPLANRECORD_PTR productrecord,bool enablesave)
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

#define SETNULLORPRODUCTINFOMEMBERDATA(chwnd,member) SendMessage(GetDlgItem(hwnd,chwnd), WM_SETTEXT, 0, productrecord==NULL?L"":productrecord->##member);
#define SETNULLORPRODUCTINFOMEMBERINTDATA(chwnd,member) if(productrecord==NULL)SendMessage(GetDlgItem(hwnd,chwnd), WM_SETTEXT, 0, L"");else{ WCHAR _temp_buffer[30];swprintf(_temp_buffer,30, L"%lld", productrecord->##member); SendMessage(GetDlgItem(hwnd,chwnd), WM_SETTEXT, 0, _temp_buffer);}
#define SETNULLORPRODUCTINFOMEMBERPRICEDATA(chwnd,member) if(productrecord==NULL)SendMessage(GetDlgItem(hwnd,chwnd), WM_SETTEXT, 0, L"");else{ WCHAR _temp_buffer[30];swprintf(_temp_buffer,30, L"%lf", productrecord->##member); SendMessage(GetDlgItem(hwnd,chwnd), WM_SETTEXT, 0, _temp_buffer);}
#define SAVEPRODUCTINFOMEMBERDATA(memberid,member) productrecord->##member=CreateWstrForWindowText(GetDlgItem(hwnd,memberid));
#define SAVEPRODUCTINFOMEMBERINTDATA(memberid,member) {PWCHAR _temp_int64_str = CreateWstrForWindowText(GetDlgItem(hwnd,memberid));\
int64_t _temp_int64;\
if (swscanf(_temp_int64_str, L"%lld", &_temp_int64) == 1)\
{\
    productrecord->##member = _temp_int64;\
}\
else if(_temp_int64_str[0]!=0)\
{\
    MessageBox(hwnd, TEXT(#member) L"格式错误", NULL, 0);\
}\
free(_temp_int64_str);\
}
#define SAVEPRODUCTINFOMEMBERPAIROFUINT64DATA(memberid,member) {PWCHAR _temp_int64_str = CreateWstrForWindowText(GetDlgItem(hwnd,memberid));\
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
}
#define SAVEPRODUCTINFOMEMBERDATEDATA(memberid,member) {PWCHAR _temp_int64_str = CreateWstrForWindowText(GetDlgItem(hwnd,memberid));\
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
}
#define SAVEPRODUCTINFOMEMBERPRICEDATA(memberid,member) {PWCHAR _temp_int64_str = CreateWstrForWindowText(GetDlgItem(hwnd,memberid));\
double _temp_int64;\
if (swscanf(_temp_int64_str, L"%lf", &_temp_int64) == 1)\
{\
    productrecord->##member = _temp_int64;\
}\
else  if(_temp_int64_str[0]!=0)\
{\
    MessageBox(hwnd, TEXT(#member) L"格式错误", NULL, 0);\
}\
free(_temp_int64_str);\
}
#define FORCESPACE  
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
        lasty += 10;
        YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_SAVE, 10, lasty, 100, 50);
        YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_SAVEANDNEXT, 20 + 100, lasty, 100, 50);
        YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_CANCEL, 10 + 200 + 20, lasty, 100, 50);

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
void edititemwindow_initctrl(HWND hwnd, YINYUE200_TRAINPLANRECORD_PTR productrecord)
{
    SETNULLORPRODUCTINFOMEMBERDATA(ID_EDIT_NAME, Name);
    SETNULLORPRODUCTINFOMEMBERDATA(ID_EDIT_TYPE, Type);
    SETNULLORPRODUCTINFOMEMBERDATA(ID_EDIT_STATE, State);
    SETNULLORPRODUCTINFOMEMBERINTDATA(ID_EDIT_REPEAT, Repeat);
    if (productrecord == NULL)
    {
        SYSTEMTIME time;
        GetLocalTime(&time);//时间的呈现始终呈现本地时间，存储使用 UTC 时间，因此此处获取本地时间
        DateTime_SetSystemtime(GetDlgItem(hwnd, ID_EDIT_STARTDATE), GDT_VALID, &time);
        DateTime_SetSystemtime(GetDlgItem(hwnd, ID_EDIT_STARTTIME), GDT_VALID, &time);

        {
            GUID guid;
            CheckHResult(CoCreateGuid(&guid));
            setidtoeditcontrol(hwnd, Yinyue200_ConvertToUint64PairFromGuid(guid));
        }
    }
    else
    {
        FILETIME filetime = Yinyue200_ConvertToFileTimeFromUINT64(productrecord->StartTimePoint);
        FILETIME localfiletime;
        FileTimeToLocalFileTime(&filetime, &localfiletime);//lpLocalFileTime cannot be the same as the lpFileTime parameter.
        SYSTEMTIME systime;
        FileTimeToSystemTime(&localfiletime, &systime);
        DateTime_SetSystemtime(GetDlgItem(hwnd, ID_EDIT_STARTDATE), GDT_VALID, &systime);
        DateTime_SetSystemtime(GetDlgItem(hwnd, ID_EDIT_STARTTIME), GDT_VALID, &systime);

        setidtoeditcontrol(hwnd, productrecord->ID);
    }
}
LRESULT CALLBACK EditItemWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

        YINYUE200_EDITITEMWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        windowdata->lastfont = yinyue200_CreateDefaultFont(hwnd);

        ADDLABELANDEDIT(NAME, L"车次");
        ADDLABELANDEDIT(ID, L"ID");
        ADDLABELANDEDIT(TYPE,L"类型");
        ADDLABELANDEDIT(STATE, L"状态");
        ADDLABELANDEDIT(REPEAT, L"该车次发出的频率（单位：天）");
        HWND hwnd_STARTDATE_Label = Yinyue200_FastCreateLabelControl(hwnd, ID_LABEL_STARTDATE, L"首次发车日期");
        HWND hwnd_STARTDATE_Edit = Yinyue200_FastCreateDatePickControl(hwnd, ID_EDIT_STARTDATE);
        HWND hwnd_STARTTIME_Label = Yinyue200_FastCreateLabelControl(hwnd, ID_LABEL_STARTTIME, L"首次发车时间");
        HWND hwnd_STARTTIME_Edit = Yinyue200_FastCreateTimePickControl(hwnd, ID_EDIT_STARTTIME);


        lasty += 10;
        HWND hwndButton_Save = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_SAVE, L"保存");
        HWND hwndButton_SaveAndNext = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_SAVEANDNEXT, L"保存并添加下一个");
        HWND hwndButton_Cancel = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_CANCEL, L"取消");
        lasty += 50;
        YINYUE200_TRAINPLANRECORD_PTR productrecord = windowdata->TrainPlanRecord;
        
        edititemwindow_initctrl(hwnd, productrecord);

        if (!windowdata->enablesave)
        {
            EnableWindow(hwndButton_Save, false);
        }
        UINT dpi = yinyue200_GetDpiForWindow(hwnd);

        Yinyue200_SetWindowSize(hwnd, 700, 770, dpi);

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
                    LocalFileTimeToFileTime(&filetime, &utcfiletime);
                    productrecord->StartTimePoint = Yinyue200_ConvertToUINT64FromFileTime(utcfiletime);
                }

                if (windowdata->TrainPlanRecord == NULL)
                    VECTOR_ADD(yinyue200_ProductList, productrecord);

                if (LOWORD(wParam) == ID_BUTTON_SAVEANDNEXT)
                {
                    windowdata->TrainPlanRecord = NULL;
                    edititemwindow_initctrl(hwnd, NULL);
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
            default:
                break;
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
        CheckIfNoWindowAndQuit();
        return 0;
    }
    case WM_DPICHANGED:
    {
        YINYUE200_EDITITEMWINDOWDATA *windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if (windowdata && windowdata->lastfont)
        {
            yinyue200_DeleteFont(windowdata->lastfont);
            windowdata->lastfont = yinyue200_CreateDefaultFont(hwnd);
            LayoutControls_EditItemWindow(hwnd, HIWORD(wParam), windowdata);
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
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}