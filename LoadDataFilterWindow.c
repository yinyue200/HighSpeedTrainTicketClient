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
#define ID_EDIT_NAME 1
#define ID_BUTTON_SAVE 2
#define ID_BUTTON_CANCEL 3
#define ID_EDIT_ID 4
#define ID_EDIT_TYPE 5
#define ID_CHK_ID 6
#define ID_CHK_NAME 7
#define ID_CHK_TYPE 8
#define ID_EDIT_STATE 9
#define ID_EDIT_DATE 10
#define ID_EDIT_PROVIDEBY 11
#define ID_EDIT_RECIEVEDBY 12
#define ID_EDIT_RESENTBY 13
#define ID_EDIT_COUNT 14
#define ID_EDIT_COST 15
#define ID_EDIT_PRICE 16
#define ID_EDIT_RESENTPRICE 17
#define ID_EDIT_SIGNER 18
#define ID_CHK_STATE 19
#define ID_CHK_DATE 20
#define ID_CHK_PROVIDEBY 21
#define ID_CHK_RECIEVEDBY 22
#define ID_CHK_RESENTBY 23
#define ID_CHK_COUNT 24
#define ID_CHK_COST 25
#define ID_CHK_PRICE 26
#define ID_CHK_RESENTPRICE 27
#define ID_CHK_SIGNER 28
typedef struct Yinyue200_LoadDataFilterWindowData
{
    YINYUE200_MAINWINDOWDATA* mainwindowdata;
} YINYUE200_LOADDATAFILTERWINDOWDATA;
LRESULT CALLBACK LoadDataFilterWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateLoadDataFilterWindow(YINYUE200_MAINWINDOWDATA *mainwindowdata)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"yinyue200.SimpleStoreErp.LoadDataFilterWindow";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = LoadDataFilterWindowProc;
    wc.hInstance = yinyue200_hInstance;
    wc.lpszClassName = CLASS_NAME;

    WORD result = RegisterClass(&wc);
    //printf("%d", result);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"选择筛选条件-我的小店出入库管理系统",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 850, 800,

        NULL,       // Parent window    
        NULL,       // Menu
        yinyue200_hInstance,  // Instance handle
        NULL        // Additional application data
    );

    YINYUE200_LOADDATAFILTERWINDOWDATA* windata = yinyue200_safemalloc(sizeof(YINYUE200_LOADDATAFILTERWINDOWDATA));
    memset(windata, 0, sizeof(YINYUE200_LOADDATAFILTERWINDOWDATA));
    windata->mainwindowdata = mainwindowdata;
    SetProp(hwnd, YINYUE200_WINDOW_DATA, windata);

    if (hwnd == NULL)
    {
        int a = GetLastError();
        return 0;
    }

    ShowWindow(hwnd, yinyue200_nCmdShow);


    return 0;
}
#define PREDEFINELOADDATAFILTERPROC(name,uppid)  HWND Field##name##Ctrl = GetDlgItem(hwnd, ID_EDIT_##uppid);\
BOOL Is##name##FieldChk = IsDlgButtonChecked(hwnd, ID_CHK_##uppid);\
PWCHAR Field##name##Text = CreateWstrForWindowText(Field##name##Ctrl);
#define PREDEFINELOADDATAFILTERPROC_INT(name,uppid,displayname)  HWND Field##name##Ctrl = GetDlgItem(hwnd, ID_EDIT_##uppid);\
BOOL Is##name##FieldChk = IsDlgButtonChecked(hwnd, ID_CHK_##uppid);\
PWCHAR Field##name##Text = CreateWstrForWindowText(Field##name##Ctrl);\
int64_t Field##name##Int_Top;int64_t Field##name##Int_Bottom;\
if(Is##name##FieldChk){\
int _temp_ret_sscanf= swscanf(Field##name##Text, L"%lld-%lld", &Field##name##Int_Bottom,&Field##name##Int_Top);\
if(Field##name##Int_Bottom>Field##name##Int_Top){\
int64_t _temp_PREDEFINELOADDATAFILTERPROC_INT_swap=Field##name##Int_Bottom;Field##name##Int_Bottom=Field##name##Int_Top;\
Field##name##Int_Top=_temp_PREDEFINELOADDATAFILTERPROC_INT_swap;}\
if (_temp_ret_sscanf==1)\
{\
    Field##name##Int_Bottom=Field##name##Int_Top;\
}\
else if(_temp_ret_sscanf==2){}\
else\
{\
    MessageBox(hwnd, displayname L" 格式错误", NULL, 0);\
    Is##name##FieldChk = false;\
}\
}\
free(Field##name##Text);
#define PREDEFINELOADDATAFILTERPROC_DOUBLE(name,uppid,displayname)  HWND Field##name##Ctrl = GetDlgItem(hwnd, ID_EDIT_##uppid);\
BOOL Is##name##FieldChk = IsDlgButtonChecked(hwnd, ID_CHK_##uppid);\
PWCHAR Field##name##Text = CreateWstrForWindowText(Field##name##Ctrl);\
double Field##name##Int_Top;double Field##name##Int_Bottom;\
if(Is##name##FieldChk){\
int _temp_ret_sscanf= swscanf(Field##name##Text, L"%lf-%lf", &Field##name##Int_Bottom,&Field##name##Int_Top);\
if(Field##name##Int_Bottom>Field##name##Int_Top){\
double _temp_PREDEFINELOADDATAFILTERPROC_INT_swap=Field##name##Int_Bottom;Field##name##Int_Bottom=Field##name##Int_Top;\
Field##name##Int_Top=_temp_PREDEFINELOADDATAFILTERPROC_INT_swap;}\
if (_temp_ret_sscanf==1)\
{\
    Field##name##Int_Bottom=Field##name##Int_Top;\
}\
else if(_temp_ret_sscanf==2){}\
else\
{\
    MessageBox(hwnd, displayname L" 格式错误", NULL, 0);\
    Is##name##FieldChk = false;\
}\
}\
free(Field##name##Text);
#define LOADDATAFILTERPROC_FLITER_WSTR(name) if (Is##name##FieldChk)\
{\
    if (wcscmp(record->name, Field##name##Text))\
    {\
        SHOULDREV &= TRUE;\
    }\
    else\
    {\
        SHOULDREV &= FALSE;\
    }\
}
#define LOADDATAFILTERPROC_FLITER_INT(name) if (Is##name##FieldChk)\
{\
    if (record->name >= Field##name##Int_Bottom&&record->name <= Field##name##Int_Top)\
    {\
        SHOULDREV &= TRUE;\
    }\
    else\
    {\
        SHOULDREV &= FALSE;\
    }\
}
#define FREE_LOADDATAFILTERPROC_FLITER_WSTR(name) free(Field##name##Text)
#define CREATECTRL(name,label)  HWND Hwnd_##name##_Label = CreateWindow(L"BUTTON", label, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, lasty, 500, 25, hwnd, ID_CHK_##name, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);\
lasty += 25;\
HWND hwnd_##name##_Edit = CreateWindow(L"EDIT",NULL, WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT,10, lasty, 500, 25,hwnd,ID_EDIT_##name,(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),NULL);\
lasty += 25;
LRESULT CALLBACK LoadDataFilterWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {

        AddWindowCount();
       
        int lasty = 10;
        HWND WindowLabel = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE, 10, lasty, 500, 25
            , hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 25;
        CREATECTRL(NAME,L"名称")
        CREATECTRL(ID, L"ID")
        CREATECTRL(TYPE, L"类型")
        CREATECTRL(STATE, L"状态")
        CREATECTRL(DATE, L"日期")
        CREATECTRL(PROVIDEBY, L"供货商")
        CREATECTRL(RECIEVEDBY, L"收货商")
        CREATECTRL(RESENTBY, L"退货商")
        CREATECTRL(COUNT, L"数量")
        CREATECTRL(COST, L"进价")
        CREATECTRL(PRICE, L"销售价")
        CREATECTRL(RESENTPRICE, L"退货价")
        CREATECTRL(SIGNER, L"经手人")
        lasty += 5;
        HWND hwndButton_Save = CreateWindow(
            L"BUTTON",
            L"确定",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            10, lasty, 100, 50,
            hwnd, ID_BUTTON_SAVE, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        HWND hwndButton_Cancel = CreateWindow(
            L"BUTTON",
            L"取消",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            10 + 100 + 10, lasty, 100, 50,
            hwnd, ID_BUTTON_CANCEL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 50;
        SendMessage(WindowLabel, WM_SETTEXT, 0, L"填写筛选条件，部分字段可用“-”表示范围");


    }
    return 0;
    case WM_NOTIFY:
    {

        return 0;
    }
    case WM_COMMAND:
    {
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
        {
            if (LOWORD(wParam) == ID_BUTTON_SAVE)
            {
                YINYUE200_LOADDATAFILTERWINDOWDATA *ldfwindow = GetProp(hwnd, YINYUE200_WINDOW_DATA);
                YINYUE200_MAINWINDOWDATA* mainwindow = ldfwindow->mainwindowdata;

                PREDEFINELOADDATAFILTERPROC(Name,NAME)
                PREDEFINELOADDATAFILTERPROC_INT(ID,ID, L"ID")
                PREDEFINELOADDATAFILTERPROC(Type,TYPE)
                PREDEFINELOADDATAFILTERPROC(State, STATE)
                PREDEFINELOADDATAFILTERPROC_INT(Date,DATE, L"日期")
                PREDEFINELOADDATAFILTERPROC(ProvideBy,PROVIDEBY)
                PREDEFINELOADDATAFILTERPROC(RecievedBy,RECIEVEDBY)
                PREDEFINELOADDATAFILTERPROC(ResentBy,RESENTBY)
                PREDEFINELOADDATAFILTERPROC_INT(Count,COUNT, L"数量")
                PREDEFINELOADDATAFILTERPROC_DOUBLE(Cost,COST, L"进价")
                PREDEFINELOADDATAFILTERPROC_DOUBLE(Price,PRICE, L"销售价")
                PREDEFINELOADDATAFILTERPROC_DOUBLE(ResentPrice,RESENTPRICE, L"退货价")
                PREDEFINELOADDATAFILTERPROC(Signer,SIGNER)

                for (size_t i = 0; i < VECTOR_TOTAL(mainwindow->UnsortedNowList); i++)
                {
                    PRODUCTRECORD* record = VECTOR_GET(mainwindow->UnsortedNowList, PRODUCTRECORD*, i);
                    BOOL SHOULDREV = TRUE;
                    LOADDATAFILTERPROC_FLITER_WSTR(Name)
                    LOADDATAFILTERPROC_FLITER_INT(ID)
                    LOADDATAFILTERPROC_FLITER_WSTR(Type)
                    LOADDATAFILTERPROC_FLITER_WSTR(State)
                    LOADDATAFILTERPROC_FLITER_INT(Date)
                    LOADDATAFILTERPROC_FLITER_WSTR(ProvideBy)
                    LOADDATAFILTERPROC_FLITER_WSTR(RecievedBy)
                    LOADDATAFILTERPROC_FLITER_WSTR(ResentBy)
                    LOADDATAFILTERPROC_FLITER_INT(Count)
                    LOADDATAFILTERPROC_FLITER_INT(Cost)
                    LOADDATAFILTERPROC_FLITER_INT(Price)
                    LOADDATAFILTERPROC_FLITER_INT(ResentPrice)
                    LOADDATAFILTERPROC_FLITER_WSTR(Signer)
                    

                    if (!SHOULDREV)
                    {
                        VECTOR_DELETE(mainwindow->UnsortedNowList, i);
                        i--;
                    }
                }


                FREE_LOADDATAFILTERPROC_FLITER_WSTR(Name);
                FREE_LOADDATAFILTERPROC_FLITER_WSTR(Type);
                FREE_LOADDATAFILTERPROC_FLITER_WSTR(State);
                FREE_LOADDATAFILTERPROC_FLITER_WSTR(ProvideBy);
                FREE_LOADDATAFILTERPROC_FLITER_WSTR(RecievedBy);
                FREE_LOADDATAFILTERPROC_FLITER_WSTR(ResentBy);
                FREE_LOADDATAFILTERPROC_FLITER_WSTR(Signer);
                mainwindow->sortstate = 0;
                Yinyue200_Main_UpdateListViewData(mainwindow->WindowHwnd);
                SendMessage(hwnd, WM_CLOSE, NULL, NULL);
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
        YINYUE200_LOADDATAFILTERWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if (windata)
        {
            free(windata);
        }
        RemoveProp(hwnd, YINYUE200_WINDOW_DATA);
        DecreaseWindowCount();
        CheckIfNoWindowAndQuit();
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