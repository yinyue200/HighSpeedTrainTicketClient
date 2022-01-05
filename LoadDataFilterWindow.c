//  SimpleStoreErp
//	Copyright(C) 2021 ��Խ
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
#define ID_CHK_ID 5
#define ID_CHK_NAME 6
#define ID_EDIT_TYPE 7
#define ID_CHK_TYPE 8
#define ID_EDIT_DATE 9
#define ID_CHK_DATE 10
#define ID_EDIT_STARTTIME 11
#define ID_CHK_STARTTIME 12
#define ID_EDIT_STARTSTATION 13
#define ID_CHK_STARTSTATION 14
#define ID_EDIT_ENDSTATION 15
#define ID_CHK_ENDSTATION 16
#define ID_EDIT_STATIONS 17
#define ID_CHK_STATIONS 18
#define ID_EDIT_REPEAT 19
#define ID_CHK_REPEAT 20
#define ID_EDIT_FIRSTDATE 21
#define ID_CHK_FIRSTDATE 22
#define ID_EDIT_DISTANCE 23
#define ID_CHK_DISTANCE 24
#define ID_EDIT_RUNTIME 25
#define ID_CHK_RUNTIME 26
#define ID_EDIT_STATE 27
#define ID_CHK_STATE 28

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
        L"ѡ��ɸѡ����-�ҵ�С���������ϵͳ",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

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
    MessageBox(hwnd, displayname L" ��ʽ����", NULL, 0);\
    Is##name##FieldChk = false;\
}\
}\
free(Field##name##Text);
#define PREDEFINELOADDATAFILTERPROC_PAIRINT(name,uppid,displayname)  HWND Field##name##Ctrl = GetDlgItem(hwnd, ID_EDIT_##uppid);\
BOOL Is##name##FieldChk = IsDlgButtonChecked(hwnd, ID_CHK_##uppid);\
PWCHAR Field##name##Text = CreateWstrForWindowText(Field##name##Ctrl);\
int64_t Field##name##Int_High;int64_t Field##name##Int_Low;\
if(Is##name##FieldChk){\
int _temp_ret_sscanf= swscanf(Field##name##Text, L"%llu;%llu", &Field##name##Int_High,&Field##name##Int_Low);\
if(_temp_ret_sscanf==2){}\
else\
{\
    MessageBox(hwnd, displayname L" ��ʽ����", NULL, 0);\
    Is##name##FieldChk = false;\
}\
}\
free(Field##name##Text);
#define PREDEFINELOADDATAFILTERPROC_LOCALDATE(name,uppid,displayname)  HWND Field##name##Ctrl = GetDlgItem(hwnd, ID_EDIT_##uppid);\
BOOL Is##name##FieldChk = IsDlgButtonChecked(hwnd, ID_CHK_##uppid);\
PWCHAR Field##name##Text = CreateWstrForWindowText(Field##name##Ctrl);\
uint64_t Field##name##Int_Top;uint64_t Field##name##Int_Bottom;\
if(Is##name##FieldChk){\
int _temp_a1,_temp_b1,_temp_c1,_temp_a2,_temp_b2,_temp_c2;int _temp_ret_sscanf=2;\
int _temp_ret_sscanf_date= swscanf(Field##name##Text, L"%d/%d/%d-%d/%d/%d", &_temp_a1,&_temp_b1,&_temp_c1, &_temp_a2,&_temp_b2,&_temp_c2);\
if(_temp_ret_sscanf_date==3)\
{\
    Field##name##Int_Bottom=Yinyue200_ConvertToUINT64FromFileTime(ConvertDateToLocalFILETIME(_temp_a1,_temp_b1,_temp_c1));Field##name##Int_Top=Field##name##Int_Bottom+yinyue200_ADay-1;\
}\
else if(_temp_ret_sscanf_date==6)\
{\
    Field##name##Int_Bottom=Yinyue200_ConvertToUINT64FromFileTime(ConvertDateToLocalFILETIME(_temp_a1,_temp_b1,_temp_c1));Field##name##Int_Top=Yinyue200_ConvertToUINT64FromFileTime(ConvertDateToLocalFILETIME(_temp_a2,_temp_b2,_temp_c2));\
}\
else\
{\
    _temp_ret_sscanf = swscanf(Field##name##Text, L"%lld-%lld", &Field##name##Int_Bottom,&Field##name##Int_Top);\
}\
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
    MessageBox(hwnd, displayname L" ��ʽ����", NULL, 0);\
    Is##name##FieldChk = false;\
}\
}\
free(Field##name##Text);
#define PREDEFINELOADDATAFILTERPROC_DATE(name,uppid,displayname)  HWND Field##name##Ctrl = GetDlgItem(hwnd, ID_EDIT_##uppid);\
BOOL Is##name##FieldChk = IsDlgButtonChecked(hwnd, ID_CHK_##uppid);\
PWCHAR Field##name##Text = CreateWstrForWindowText(Field##name##Ctrl);\
uint64_t Field##name##Int_Top;uint64_t Field##name##Int_Bottom;\
if(Is##name##FieldChk){\
int _temp_a1,_temp_b1,_temp_c1,_temp_a2,_temp_b2,_temp_c2;int _temp_ret_sscanf=2;\
int _temp_ret_sscanf_date= swscanf(Field##name##Text, L"%d/%d/%d-%d/%d/%d", &_temp_a1,&_temp_b1,&_temp_c1, &_temp_a2,&_temp_b2,&_temp_c2);\
if(_temp_ret_sscanf_date==3)\
{\
    Field##name##Int_Bottom=Yinyue200_ConvertToUINT64FromFileTime(ConvertDateToUTCFILETIME(_temp_a1,_temp_b1,_temp_c1));Field##name##Int_Top=Field##name##Int_Bottom+yinyue200_ADay;\
}\
else if(_temp_ret_sscanf_date==6)\
{\
    Field##name##Int_Bottom=Yinyue200_ConvertToUINT64FromFileTime(ConvertDateToUTCFILETIME(_temp_a1,_temp_b1,_temp_c1));Field##name##Int_Top=Yinyue200_ConvertToUINT64FromFileTime(ConvertDateToUTCFILETIME(_temp_a2,_temp_b2,_temp_c2));\
}\
else\
{\
    _temp_ret_sscanf = swscanf(Field##name##Text, L"%lld-%lld", &Field##name##Int_Bottom,&Field##name##Int_Top);\
}\
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
    MessageBox(hwnd, displayname L" ��ʽ����", NULL, 0);\
    Is##name##FieldChk = false;\
}\
}\
free(Field##name##Text);
#define PREDEFINELOADDATAFILTERPROC_TIME(name,uppid,displayname)  HWND Field##name##Ctrl = GetDlgItem(hwnd, ID_EDIT_##uppid);\
BOOL Is##name##FieldChk = IsDlgButtonChecked(hwnd, ID_CHK_##uppid);\
PWCHAR Field##name##Text = CreateWstrForWindowText(Field##name##Ctrl);\
uint64_t Field##name##Int_Top;uint64_t Field##name##Int_Bottom;\
if(Is##name##FieldChk){\
int _temp_a1,_temp_b1,_temp_a2,_temp_b2;int _temp_ret_sscanf=2;\
int _temp_ret_sscanf_date= swscanf(Field##name##Text, L"%d:%d-%d:%d", &_temp_a1,&_temp_b1, &_temp_a2,&_temp_b2);\
if(_temp_ret_sscanf_date==2)\
{\
    Field##name##Int_Bottom=ConvertTimeToUINT64(_temp_a1,_temp_b1,0);Field##name##Int_Top=Field##name##Int_Bottom+Yinyue200_ConvertToUINT64FromTotalSecond(60);\
}\
else if(_temp_ret_sscanf_date==4)\
{\
    Field##name##Int_Bottom=ConvertTimeToUINT64(_temp_a1,_temp_b1,0);Field##name##Int_Top=ConvertTimeToUINT64(_temp_a2,_temp_b2,0);\
}\
else\
{\
    _temp_ret_sscanf = swscanf(Field##name##Text, L"%lld-%lld", &Field##name##Int_Bottom,&Field##name##Int_Top);\
}\
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
    MessageBox(hwnd, displayname L" ��ʽ����", NULL, 0);\
    Is##name##FieldChk = false;\
}\
}\
free(Field##name##Text);
#define PREDEFINELOADDATAFILTERPROC_DOUBLE(name,uppid,displayname)  HWND Field##name##Ctrl = GetDlgItem(hwnd, ID_EDIT_##uppid);\
BOOL Is##name##FieldChk = IsDlgButtonChecked(hwnd, ID_CHK_##uppid);\
PWCHAR Field##name##Text = CreateWstrForWindowText(Field##name##Ctrl);\
double Field##name##Double_Top=0.0;double Field##name##Double_Bottom=0.0;\
if(Is##name##FieldChk){\
int _temp_ret_sscanf= swscanf(Field##name##Text, L"%lf-%lf", &Field##name##Double_Bottom,&Field##name##Double_Top);\
if(Field##name##Double_Bottom>Field##name##Double_Top){\
double _temp_PREDEFINELOADDATAFILTERPROC_INT_swap=Field##name##Double_Bottom;Field##name##Double_Bottom=Field##name##Double_Top;\
Field##name##Double_Top=_temp_PREDEFINELOADDATAFILTERPROC_INT_swap;}\
if (_temp_ret_sscanf==1)\
{\
    Field##name##Double_Bottom=Field##name##Double_Top;\
}\
else if(_temp_ret_sscanf==2){}\
else\
{\
    MessageBox(hwnd, displayname L" ��ʽ����", NULL, 0);\
    Is##name##FieldChk = false;\
}\
}\
free(Field##name##Text);
#define LOADDATAFILTERPROC_FLITER_WSTR(name)do{ if (Is##name##FieldChk)\
{\
    if (wcscmp(record->name, Field##name##Text)==0)\
    {\
        SHOULDREV &= TRUE;\
    }\
    else\
    {\
        SHOULDREV &= FALSE;\
    }\
}}while(0)
#define LOADDATAFILTERPROC_FLITER_INT(name)do{  if (Is##name##FieldChk)\
{\
    if (record->name >= Field##name##Int_Bottom&&record->name <= Field##name##Int_Top)\
    {\
        SHOULDREV &= TRUE;\
    }\
    else\
    {\
        SHOULDREV &= FALSE;\
    }\
}}while(0)
#define LOADDATAFILTERPROC_FLITER_PAIRINT(name)do{ if (Is##name##FieldChk)\
{\
    if (record->name.Item1 == Field##name##Int_High&&record->name.Item2 == Field##name##Int_Low)\
    {\
        SHOULDREV &= TRUE;\
    }\
    else\
    {\
        SHOULDREV &= FALSE;\
    }\
}}while(0)
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
        CREATECTRL(NAME,L"����")
        CREATECTRL(ID, L"ID")
        CREATECTRL(TYPE, L"����")
        CREATECTRL(STATE, L"״̬")
        CREATECTRL(DATE, L"����")
        CREATECTRL(STARTTIME, L"����ʱ��")
        CREATECTRL(STARTSTATION, L"ʼ��վ")
        CREATECTRL(ENDSTATION, L"�յ�վ")
        CREATECTRL(STATIONS, L";��վ�����;��վ�÷ֺŷָ���")
        CREATECTRL(REPEAT, L"�����������")
        CREATECTRL(FIRSTDATE, L"�״ο�������")
        CREATECTRL(DISTANCE, L"����̣�ǧ�ף�")
        CREATECTRL(RUNTIME, L"������ʱ�䣨���ӣ�")
        lasty += 5;
        HWND hwndButton_Save = CreateWindow(
            L"BUTTON",
            L"ȷ��",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            10, lasty, 100, 50,
            hwnd, ID_BUTTON_SAVE, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        HWND hwndButton_Cancel = CreateWindow(
            L"BUTTON",
            L"ȡ��",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            10 + 100 + 10, lasty, 100, 50,
            hwnd, ID_BUTTON_CANCEL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 50;
        SendMessage(WindowLabel, WM_SETTEXT, 0, L"��дɸѡ�����������ֶο��á�-����ʾ��Χ");


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

                PREDEFINELOADDATAFILTERPROC(Name, NAME);
                PREDEFINELOADDATAFILTERPROC_PAIRINT(ID, ID, L"ID");
                PREDEFINELOADDATAFILTERPROC(Type, TYPE);
                PREDEFINELOADDATAFILTERPROC(State, STATE);
                PREDEFINELOADDATAFILTERPROC_LOCALDATE(Date, DATE, L"����");
                PREDEFINELOADDATAFILTERPROC_TIME(Time, STARTTIME, L"����ʱ��");
                PREDEFINELOADDATAFILTERPROC(StartStation, STARTSTATION);//ʼ��վ
                PREDEFINELOADDATAFILTERPROC(EndStation, ENDSTATION);//�յ�վ
                PREDEFINELOADDATAFILTERPROC(Stations, STATIONS);
                vector StationsVec = SplitStringToVectorOfString(FieldStationsText, L";");
                PREDEFINELOADDATAFILTERPROC_DATE(StartTimePoint, FIRSTDATE, L"�״ο�������");
                PREDEFINELOADDATAFILTERPROC_INT(Repeat, REPEAT, L"�����������");
                PREDEFINELOADDATAFILTERPROC_DOUBLE(Distance, DISTANCE, L"����̣�ǧ�ף���ֻ֧��������ʽ��");
                uint64_t FieldDistanceInt_Bottom = FieldDistanceDouble_Bottom * 1000;
                uint64_t FieldDistanceInt_Top = FieldDistanceDouble_Bottom * 1000;
                PREDEFINELOADDATAFILTERPROC_DOUBLE(Runtime, RUNTIME, L"������ʱ�䣨���ӣ���ֻ֧��������ʽ��");
                uint64_t FieldRuntimeInt_Bottom = Yinyue200_ConvertToUINT64FromTotalSecond(FieldRuntimeDouble_Bottom * 60);
                uint64_t FieldRuntimeInt_Top = Yinyue200_ConvertToUINT64FromTotalSecond(FieldRuntimeDouble_Bottom * 60);

                //PREDEFINELOADDATAFILTERPROC(ProvideBy,PROVIDEBY)
                //PREDEFINELOADDATAFILTERPROC(RecievedBy,RECIEVEDBY)
                //PREDEFINELOADDATAFILTERPROC(ResentBy,RESENTBY)
                //PREDEFINELOADDATAFILTERPROC_INT(Count,COUNT, L"����")
                //PREDEFINELOADDATAFILTERPROC_DOUBLE(Cost,COST, L"����")
                //PREDEFINELOADDATAFILTERPROC_DOUBLE(Price,PRICE, L"���ۼ�")
                //PREDEFINELOADDATAFILTERPROC_DOUBLE(ResentPrice,RESENTPRICE, L"�˻���")
                //PREDEFINELOADDATAFILTERPROC(Signer,SIGNER)

                vector nnowlist;
                vector_initwithcap(&nnowlist,mainwindow->UnsortedNowList.total);

                for (int i = 0; i < VECTOR_TOTAL(mainwindow->UnsortedNowList); i++)
                {
                    YINYUE200_TRAINPLANRECORD_PTR record = VECTOR_GET(mainwindow->UnsortedNowList, YINYUE200_TRAINPLANRECORD_PTR, i);
                    bool SHOULDREV = true;
                    LOADDATAFILTERPROC_FLITER_WSTR(Name);
                    LOADDATAFILTERPROC_FLITER_PAIRINT(ID);
                    LOADDATAFILTERPROC_FLITER_WSTR(Type);
                    LOADDATAFILTERPROC_FLITER_WSTR(State);
                    //����ɸѡ
                    do
                    {
                        if (IsDateFieldChk)
                        {
                            bool ret = false;
                            int date = Yinyue200_GetTrainPlanRecordCreatedTotalDateFromLocalUINT64(record, FieldDateInt_Bottom);
                            if (date >= 0)
                            {
                                int modrev = date % record->Repeat;
                                if (modrev == 0)
                                {
                                    ret = true;
                                }
                                else
                                {
                                    int spanday = (FieldDateInt_Top - FieldDateInt_Bottom) / yinyue200_ADay;
                                    if (modrev <= spanday)
                                    {
                                        ret = true;
                                    }
                                }
                            }
                            else
                            {
                                ret = false;
                            }
                            SHOULDREV &= ret;
                        }
                    } while (0);
                    //����ʱ��ɸѡ
                    do
                    {
                        if (IsTimeFieldChk)
                        {
                            uint64_t timepart = GetTimePartUINT64OFUINT64(record->StartTimePoint);
                            SHOULDREV &= (timepart >= FieldTimeInt_Bottom && timepart <= FieldDateInt_Top);
                        }
                    } while (0);
                    //���վɸѡ
                    if (IsStartStationFieldChk)
                    {
                        if (vector_total(&record->RoutePoints) > 0)
                        {
                            YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR one = vector_get(&record->RoutePoints, 0);
                            SHOULDREV &= wcscmp(FieldStartStationText, one->Station.DisplayName) == 0;
                        }
                        else
                        {
                            SHOULDREV &= false;
                        }
                    }
                    //�յ�վɸѡ
                    if (IsEndStationFieldChk)
                    {
                        if (vector_total(&record->RoutePoints) > 0)
                        {
                            YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR one = vector_get(&record->RoutePoints, vector_total(&record->RoutePoints) - 1);
                            SHOULDREV &= wcscmp(FieldStartStationText, one->Station.DisplayName) == 0;
                        }
                        else
                        {
                            SHOULDREV &= false;
                        }
                    }
                    //;����ɸѡ
                    if (IsStationsFieldChk)
                    {
                        for (int i = 0; i < vector_total(&StationsVec); i++)
                        {
                            PWSTR tobecmp = vector_get(&StationsVec, i);
                            bool chk = false;
                            vector* vecpoints = &record->RoutePoints;
                            for (int j = 0; j < vector_total(vecpoints); j++)
                            {
                                YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR one = vector_get(vecpoints, j);
                                if (wcscmp(one->Station.DisplayName, tobecmp) == 0)
                                {
                                    chk = true;
                                    break;
                                }
                            }
                            if (chk == false)
                            {
                                SHOULDREV &= false;
                                goto skipforend;
                            }
                        }
                        SHOULDREV &= true;
                    skipforend:
                        ;
                    }
                    LOADDATAFILTERPROC_FLITER_INT(StartTimePoint);
                    LOADDATAFILTERPROC_FLITER_INT(Repeat);
                    if (IsDistanceFieldChk)
                    {
                        uint64_t distance = 0;
                        if (vector_total(&record->RoutePoints) > 1)
                        {
                            YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR first = vector_get(&record->RoutePoints, 0);
                            YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR last = vector_get(&record->RoutePoints, vector_total(&record->RoutePoints) - 1);
                            distance = last->Distance - first->Distance;
                        }
                        SHOULDREV &= (distance >= FieldDistanceInt_Bottom && distance <= FieldDistanceInt_Top);
                    }
                    if (IsRuntimeFieldChk)
                    {
                        uint64_t distance = 0;
                        if (vector_total(&record->RoutePoints) > 1)
                        {
                            YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR first = vector_get(&record->RoutePoints, 0);
                            YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR last = vector_get(&record->RoutePoints, vector_total(&record->RoutePoints) - 1);
                            distance = last->RouteRunTimeSpan - first->RouteRunTimeSpan;
                        }
                        SHOULDREV &= (distance >= FieldRuntimeInt_Bottom && distance <= FieldRuntimeInt_Top);
                    }

                    if (!SHOULDREV)
                    {
                        //VECTOR_DELETE(mainwindow->UnsortedNowList, i);
                        //i--;
                    }
                    else
                    {
                        vector_add(&nnowlist, record);
                    }
                }
                vector_free(&mainwindow->UnsortedNowList);
                mainwindow->UnsortedNowList = nnowlist;
                FreeVectorOfString(&StationsVec);
                FREE_LOADDATAFILTERPROC_FLITER_WSTR(Name);
                FREE_LOADDATAFILTERPROC_FLITER_WSTR(Type);
                FREE_LOADDATAFILTERPROC_FLITER_WSTR(State);
                FREE_LOADDATAFILTERPROC_FLITER_WSTR(StartStation);
                FREE_LOADDATAFILTERPROC_FLITER_WSTR(EndStation);
                FREE_LOADDATAFILTERPROC_FLITER_WSTR(Stations);

                //FREE_LOADDATAFILTERPROC_FLITER_WSTR(ProvideBy);
                //FREE_LOADDATAFILTERPROC_FLITER_WSTR(RecievedBy);
                //FREE_LOADDATAFILTERPROC_FLITER_WSTR(ResentBy);
                //FREE_LOADDATAFILTERPROC_FLITER_WSTR(Signer);
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