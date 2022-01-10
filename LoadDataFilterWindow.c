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
#include "ControlsCommonOperation.h"
#include "MainWindow.h"
#include "EditItemWindow.h"
#include "TrainPlanRecord.h"
#include "UserSetting.h"
#include "DpiHelper.h"
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
#define ID_LABEL_WINDOWNOTICE 29

typedef struct Yinyue200_LoadDataFilterWindowData
{
    YINYUE200_MAINWINDOWDATA* mainwindowdata;
    HFONT lastfont;
    vector* source;
} YINYUE200_LOADDATAFILTERWINDOWDATA;
LRESULT CALLBACK LoadDataFilterWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateLoadDataFilterWindow(YINYUE200_MAINWINDOWDATA *mainwindowdata, int source)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"yinyue200.HighSpeedTrainTicketClient.LoadDataFilterWindow";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = LoadDataFilterWindowProc;
    wc.hInstance = yinyue200_hInstance;
    wc.lpszClassName = CLASS_NAME;

    WORD result = RegisterClass(&wc);
    //printf("%d", result);

    // Create the window.

    YINYUE200_LOADDATAFILTERWINDOWDATA* windata = yinyue200_safemallocandclear(sizeof(YINYUE200_LOADDATAFILTERWINDOWDATA));
    memset(windata, 0, sizeof(YINYUE200_LOADDATAFILTERWINDOWDATA));
    windata->mainwindowdata = mainwindowdata;
    windata->source = source == 1 ? &yinyue200_ProductList : &mainwindowdata->UnsortedNowList;

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"ѡ��ɸѡ��������",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        yinyue200_hInstance,  // Instance handle
        windata        // Additional application data
    );

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
    if (ComparePWSTR(record->name, Field##name##Text))\
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
#define CREATECTRL(name,label)  HWND Hwnd_##name##_Label = Yinyue200_FastCreateCheckBoxControl(hwnd,ID_CHK_##name,label);\
HWND hwnd_##name##_Edit = Yinyue200_FastCreateEditControl(hwnd,ID_EDIT_##name);
#define YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(tag) do{\
YINYUE200_SETCONTROLPOSANDFONT(ID_CHK_##tag, 10, lasty, 500, 20);\
lasty+=25;\
YINYUE200_SETCONTROLPOSANDFONT(ID_EDIT_##tag, 10, lasty, 500, 20);\
lasty+=25;\
}while (0)
void LayoutControls_LoadDataFilterWindow(HWND hwnd, UINT dpi, YINYUE200_LOADDATAFILTERWINDOWDATA *windata)
{
    int lasty = 30;
    HFONT font = windata->lastfont;
    if (font)
    {
        YINYUE200_SETCONTROLPOSANDFONT(ID_LABEL_WINDOWNOTICE, 10, 0, 500, 25);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(NAME);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(ID);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(TYPE);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(STATE);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(DATE);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(STARTTIME);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(STARTSTATION);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(ENDSTATION);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(STATIONS);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(REPEAT);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(FIRSTDATE);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(DISTANCE);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(RUNTIME);
        lasty += 10;
        YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_SAVE, 10, lasty, 100, 50);
        YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_CANCEL, 20 + 100, lasty, 100, 50);

    }
}
LRESULT CALLBACK LoadDataFilterWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
       
        HWND WindowLabel = Yinyue200_FastCreateLabelControl(hwnd, ID_LABEL_WINDOWNOTICE, L"��дɸѡ�����������ֶο��á�-����ʾ��Χ");
        CREATECTRL(NAME,L"����")
        CREATECTRL(ID, L"ID")
        CREATECTRL(TYPE, L"����")
        CREATECTRL(STATE, L"״̬")
        CREATECTRL(DATE, L"����")
        CREATECTRL(STARTTIME, L"����ʱ��")
        CREATECTRL(STARTSTATION, L"ʼ��վ")
        CREATECTRL(ENDSTATION, L"�յ�վ")
        CREATECTRL(STATIONS, L";��վ�����;��վ��Ӣ�ķֺŷָ���")
        CREATECTRL(REPEAT, L"�����������")
        CREATECTRL(FIRSTDATE, L"�״ο�������")
        CREATECTRL(DISTANCE, L"����̣�ǧ�ף�")
        CREATECTRL(RUNTIME, L"������ʱ�䣨���ӣ�")
        HWND hwndButton_Save = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_SAVE, L"ȷ��");
        HWND hwndButton_Cancel = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_CANCEL, L"ȡ��");
        
        YINYUE200_LOADDATAFILTERWINDOWDATA* ldfwindow = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        ldfwindow->lastfont = yinyue200_CreateDefaultFont(hwnd);

        UINT dpi = yinyue200_GetDpiForWindow(hwnd);

        Yinyue200_SetWindowSize(hwnd, 700, 770, dpi);

        LayoutControls_LoadDataFilterWindow(hwnd, dpi, ldfwindow);
    }
    return 0;
    case WM_NOTIFY:
    {

        return 0;
    }
    case WM_DPICHANGED:
    {
        YINYUE200_LOADDATAFILTERWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if (windowdata && windowdata->lastfont)
        {
            yinyue200_DeleteFont(windowdata->lastfont);
            windowdata->lastfont = yinyue200_CreateDefaultFont(hwnd);
            LayoutControls_LoadDataFilterWindow(hwnd, HIWORD(wParam), windowdata);
        }
        break;
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

                vector nnowlist;
                vector_initwithcap(&nnowlist, ldfwindow->source->total);

                for (int i = 0; i < VECTOR_TOTAL(*ldfwindow->source); i++)
                {
                    YINYUE200_TRAINPLANRECORD_PTR record = VECTOR_GET(*ldfwindow->source, YINYUE200_TRAINPLANRECORD_PTR, i);
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
                            int date =  Yinyue200_GetTrainPlanRecordCreatedTotalDateFromLocalUINT64(record, FieldDateInt_Bottom);
                            if (date >= 0 && record->Repeat > 0)
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
                            uint64_t timepart = GetLocalTimePartUINT64OFUINT64(record->StartTimePoint);
                            SHOULDREV &= (timepart >= FieldTimeInt_Bottom && timepart <= FieldTimeInt_Top);
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
                            SHOULDREV &= wcscmp(FieldEndStationText, one->Station.DisplayName) == 0;
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
                if (ldfwindow == &mainwindow->UnsortedNowList)
                {
                    vector_free(&mainwindow->UnsortedNowList);
                }
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
                Yinyue200_Main_UpdateListViewData(mainwindow->WindowHwnd, yinyue200_GetDpiForWindow(mainwindow->WindowHwnd));
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
            yinyue200_DeleteFont(windata->lastfont);
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