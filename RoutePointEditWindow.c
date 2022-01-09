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
#include "RoutePointEditWindow.h"
#include "common.h"
#include "ControlsCommonOperation.h"
#include "ProductRecord.h"
#include "DpiHelper.h"
#define ID_EDIT_STATION 1
#define ID_LABEL_STATION 2
#define ID_LABEL_RUNTIMESPAN 3
#define ID_EDIT_RUNTIMESPAN 4
#define ID_EDIT_PRICE 5
#define ID_LABEL_PRICE 6
#define ID_LABEL_DISTANCE 7
#define ID_EDIT_DISTANCE 8
#define ID_BUTTON_SAVE 9
#define ID_BUTTON_CANCEL 10
#define ID_EDIT_RESIDENCETIME 11
#define ID_LABEL_RESIDENCETIME 12
typedef struct Yinyue200_RoutePointEditWindowData
{
    YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR RoutePoint;
    RoutePointEditFinishCallback callback;
    uint64_t TrainStartDateTime;
    uint64_t TrainStartLocalTime;
    void* callbackcontext;
    bool enablesave;
    HFONT lastfont;
    bool callbacksent;
} YINYUE200_ROUTEPOINTEDITWINDOWDATA;
LRESULT CALLBACK RoutePointEditWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
/// <summary>
/// 
/// </summary>
/// <param name="routepoint"></param>
/// <param name="enablesave"></param>
/// <param name="callback"></param>
/// <param name="callbackcontext"></param>
/// <param name="trainStartDateTime">����ʱ��</param>
void CreateRoutePointEditWindow(YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR routepoint, bool enablesave, RoutePointEditFinishCallback callback, void *callbackcontext, uint64_t trainStartLocalTime)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"yinyue200.HighSpeedTrainTicketClient.RoutePointEditWindow";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = RoutePointEditWindowProc;
    wc.hInstance = yinyue200_hInstance;
    wc.lpszClassName = CLASS_NAME;

    WORD result = RegisterClass(&wc);
    //printf("%d", result);

    YINYUE200_ROUTEPOINTEDITWINDOWDATA* windowdata = yinyue200_safemallocandclear(sizeof(YINYUE200_ROUTEPOINTEDITWINDOWDATA));
    windowdata->RoutePoint = routepoint;
    windowdata->callback = callback;
    windowdata->enablesave = enablesave;
    windowdata->callbackcontext = callbackcontext;
    windowdata->TrainStartLocalTime = GetTimePartUINT64OFUINT64(trainStartLocalTime);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"·����༭",    // Window text
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
#define ADDLABELANDEDIT(ctrl_id,displaylabel) HWND Hwnd_##ctrl_id##_Label =  Yinyue200_FastCreateLabelControl(hwnd,ID_LABEL_##ctrl_id,displaylabel);\
HWND hwnd_##ctrl_id##_Edit = Yinyue200_FastCreateEditControl(hwnd,ID_EDIT_##ctrl_id);
#define YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(tag) do{\
YINYUE200_SETCONTROLPOSANDFONT(ID_LABEL_##tag, 10, lasty, 500, 20);\
lasty+=25;\
YINYUE200_SETCONTROLPOSANDFONT(ID_EDIT_##tag, 10, lasty, 500, 20);\
lasty+=25;\
}while (0)
//ȷ�����ؼ�λ��
void LayoutControls_RoutePointEditWindow(HWND hwnd, UINT dpi, YINYUE200_ROUTEPOINTEDITWINDOWDATA *windata)
{
    Yinyue200_SetWindowSize(hwnd, 600, 500, dpi);
    if (windata->lastfont)
    {
        HFONT font = windata->lastfont;
        int lasty = 10;
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(STATION);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(DISTANCE);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(RUNTIMESPAN);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(RESIDENCETIME);
        
        lasty += 10;
        YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_SAVE, 10, lasty, 100, 50);
        YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_CANCEL, 10 + 100 + 10, lasty, 100, 50);
    }

}
//��ʼ���༭���ֵ
void routepointeditwindow_initctrl(HWND hwnd, YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR productrecord)
{
    if (productrecord != NULL)
    {
        PWSTR buffer = yinyue200_safemalloc(1000 * sizeof(WCHAR));

        SendMessage(Yinyue200_GetChildControlById(hwnd, ID_EDIT_STATION), WM_SETTEXT, 0, productrecord->Station.DisplayName);
        swprintf(buffer, 1000, L"%.3lf", productrecord->Distance / 1000.0);
        SendMessage(Yinyue200_GetChildControlById(hwnd, ID_EDIT_DISTANCE), WM_SETTEXT, 0, buffer);
        swprintf(buffer, 1000, L"%lf", Yinyue200_ConvertToTotalSecondFromUINT64(productrecord->RouteRunTimeSpan) / 60.0);
        SendMessage(Yinyue200_GetChildControlById(hwnd, ID_EDIT_RUNTIMESPAN), WM_SETTEXT, 0, buffer);
        swprintf(buffer, 1000, L"%lf", Yinyue200_ConvertToTotalSecondFromUINT64(productrecord->ResidenceTime) / 60.0);
        SendMessage(Yinyue200_GetChildControlById(hwnd, ID_EDIT_RESIDENCETIME), WM_SETTEXT, 0, buffer);
        free(buffer);
    }
}
//·����༭������Ϣ������
LRESULT CALLBACK RoutePointEditWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

        YINYUE200_ROUTEPOINTEDITWINDOWDATA *windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        windata->lastfont = yinyue200_CreateDefaultFont(hwnd);

        ADDLABELANDEDIT(STATION, L"��վ");
        ADDLABELANDEDIT(DISTANCE, L"���վ����վ�Ʒ���̣���λ��ǧ�ף�");
        ADDLABELANDEDIT(RUNTIMESPAN, L"Ԥ�Ƶ���ʱ�䣨��д��ʼ��վ����վ����ķ��ӣ�");
        ADDLABELANDEDIT(RESIDENCETIME, L"ͣ��ʱ�䣨���ӣ�");

        HWND hwnd_okbutton = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_SAVE, L"ȷ��");
        HWND hwnd_cancelbutton = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_CANCEL, L"ȡ��");

        EnableWindow(hwnd_okbutton, windata->enablesave);//��Ҫʱ����ȷ����ť

        routepointeditwindow_initctrl(hwnd, windata->RoutePoint);//��ʼ���༭���ֵ

        LayoutControls_RoutePointEditWindow(hwnd, yinyue200_GetDpiForWindow(hwnd), windata);
    }
        break;
    case WM_DESTROY:
    {
        YINYUE200_ROUTEPOINTEDITWINDOWDATA *windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if (!windowdata->callbacksent)
        {
            windowdata->callback(NULL, windowdata->callbackcontext);
        }
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
        YINYUE200_ROUTEPOINTEDITWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if (windowdata && windowdata->lastfont)
        {
            yinyue200_DeleteFont(windowdata->lastfont);
            windowdata->lastfont = yinyue200_CreateDefaultFont(hwnd);
            LayoutControls_RoutePointEditWindow(hwnd, HIWORD(wParam), windowdata);
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
    case WM_COMMAND:
    {
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
        {
            switch (LOWORD(wParam))
            {
            case ID_BUTTON_CANCEL:
                YINYUE200_ROUTEPOINTEDITWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);

                windowdata->callback(NULL, windowdata->callbackcontext);//ȡ���޸�ʱ���� NULL ���������ȡ��
                windowdata->callbacksent = true;

                SendMessage(hwnd, WM_CLOSE, NULL, NULL);
                break;
            case ID_BUTTON_SAVE:
            {
                YINYUE200_ROUTEPOINTEDITWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
#if YINYUE200_ROUTEPOINTEDITWINDOW_ALWAYS_PERFORMCHANGES_ON_NEWOBJECT
                YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR ptr = yinyue200_safemallocandclear(sizeof(YINYUE200_TRAINPLANRECORD_ROUTEPOINT));//����һ���µ� YINYUE200_TRAINPLANRECORD_ROUTEPOINT
#else
                YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR ptr = windowdata->RoutePoint;
                if (ptr == NULL)
                {
                    ptr = yinyue200_safemallocandclear(sizeof(YINYUE200_TRAINPLANRECORD_ROUTEPOINT));//����һ���µ� YINYUE200_TRAINPLANRECORD_ROUTEPOINT
                }
#endif
                free(ptr->Station.DisplayName);
                ptr->Station.DisplayName = CreateWstrForWindowText(Yinyue200_GetChildControlById(hwnd, ID_EDIT_STATION));
                PWCHAR _temp_int64_str = CreateWstrForWindowText(GetDlgItem(hwnd, ID_EDIT_DISTANCE)); 
                double disdouble = 0.0;
                int _temp_ret = Yinyue200_EditWindowParseFromStringAndFree(_temp_int64_str, &disdouble);
                if (_temp_ret < 0)
                {
                    MessageBox(hwnd, L"�����ʽ����", NULL, 0);
                }
                else
                {
                    ptr->Distance = disdouble * 1000.0;
                }
                _temp_int64_str = Yinyue200_ReplacePWSTR(CreateWstrForWindowText(Yinyue200_GetChildControlById(hwnd, ID_EDIT_RUNTIMESPAN)), L'��', L':');
                double timespan_double = 0.0;
                if (PWSTRContainChar(_temp_int64_str, L':')==false)
                {
                    if (Yinyue200_EditWindowParseFromStringAndFree(_temp_int64_str, &timespan_double) >= 0)
                    {
                        ptr->RouteRunTimeSpan = Yinyue200_ConvertToUINT64FromTotalSecond(timespan_double * 60.0);
                    }
                    else
                    {
                        MessageBox(hwnd, L"����ʱ���ʽ����", NULL, 0);
                    }
                }
                else
                {
                    int hours, minute;
                    if (swscanf(_temp_int64_str, L"%d:%d", &hours,&minute) == 2)
                    {
                        ptr->RouteRunTimeSpan = Yinyue200_ConvertToUINT64FromTotalSecond(3600.0 * hours + minute * 60.0) - windowdata->TrainStartLocalTime;
                    }
                    else
                    {
                        MessageBox(hwnd, L"����ʱ���ʽ����", NULL, 0);
                    }
                    free(_temp_int64_str);
                }
                _temp_int64_str = Yinyue200_ReplacePWSTR(CreateWstrForWindowText(Yinyue200_GetChildControlById(hwnd, ID_EDIT_RESIDENCETIME)), L'��', L':');
                if (PWSTRContainChar(_temp_int64_str, L':') == false)
                {
                    double restime_double = 0.0;
                    if (Yinyue200_EditWindowParseFromStringAndFree(_temp_int64_str, &restime_double) >= 0)
                    {
                        ptr->ResidenceTime = Yinyue200_ConvertToUINT64FromTotalSecond(restime_double * 60.0);
                    }
                    else
                    {
                        MessageBox(hwnd, L"ͣ��ʱ���ʽ����", NULL, 0);
                    }
                }
                else
                {
                    int hours, minute;
                    if (swscanf(_temp_int64_str, L"%d:%d", &hours, &minute) == 2)
                    {
                        ptr->ResidenceTime = Yinyue200_ConvertToUINT64FromTotalSecond(3600.0 * hours + minute * 60.0) - windowdata->TrainStartLocalTime;
                        ptr->ResidenceTime -= ptr->RouteRunTimeSpan;
                    }
                    else
                    {
                        MessageBox(hwnd, L"ͣ��ʱ���ʽ����", NULL, 0);
                    }
                    free(_temp_int64_str);
                }
                windowdata->callback(ptr, windowdata->callbackcontext);
                windowdata->callbacksent = true;

                SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            }
            break;
            }
        }
        break;
        }
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}