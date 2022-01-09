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

#include "FastSearchingTrainWindow.h"
#include "common.h"
#include "DpiHelper.h"
#include "ControlsCommonOperation.h"
#include "MainWindow.h"
#include "TrainPlanRecord.h"
#include "TicketsManage.h"
#include <CommCtrl.h>
#define ID_BUTTON_OK 1
#define ID_BUTTON_CANCEL 2
#define ID_LABEL_STARTSTATION 3
#define ID_EDIT_STARTSTATION 4
#define ID_LABEL_ENDSTATION 5
#define ID_EDIT_ENDSTATION 6
#define ID_LABEL_DATE 7
#define ID_EDIT_DATE 8

typedef struct Yinyue200_FastSearchingTrainWindowData
{
    YINYUE200_MAINWINDOWDATA* mainwindowdata;
    HFONT lastfont;
} YINYUE200_FASTSEARCHINGTRAINWINDOWDATA;
LRESULT CALLBACK FastSearchingTrainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateFastSearchingTrainWindow(YINYUE200_MAINWINDOWDATA* mainwindowdata)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"yinyue200.HighSpeedTrainTicketClient.FastSearchingTrainWindow";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = FastSearchingTrainWindowProc;
    wc.hInstance = yinyue200_hInstance;
    wc.lpszClassName = CLASS_NAME;

    WORD result = RegisterClass(&wc);
    //printf("%d", result);

    // Create the window.

    YINYUE200_FASTSEARCHINGTRAINWINDOWDATA* windata = yinyue200_safemallocandclear(sizeof(YINYUE200_FASTSEARCHINGTRAINWINDOWDATA));
    memset(windata, 0, sizeof(YINYUE200_FASTSEARCHINGTRAINWINDOWDATA));
    windata->mainwindowdata = mainwindowdata;

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"车次快速查询",    // Window text
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
#define YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(tag) do{\
YINYUE200_SETCONTROLPOSANDFONT(ID_LABEL_##tag, 10, lasty, 500, 20);\
lasty+=25;\
YINYUE200_SETCONTROLPOSANDFONT(ID_EDIT_##tag, 10, lasty, 500, 20);\
lasty+=25;\
}while (0)
#define FREE_LOADDATAFILTERPROC_FLITER_WSTR(name) free(Field##name##Text)
void LayoutControls_FastSearchingTrainWindow(HWND hwnd, UINT dpi, YINYUE200_FASTSEARCHINGTRAINWINDOWDATA* windata)
{
    int lasty = 20;
    HFONT font = windata->lastfont;
    if (font)
    {
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(DATE);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(STARTSTATION);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(ENDSTATION);

        lasty += 10;
        YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_OK, 10, lasty, 100, 50);
        YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_CANCEL, 10 + 100 + 10, lasty, 100, 50);
        lasty += 50;
    }
}
#define PREDEFINELOADDATAFILTERPROC(name,uppid) HWND Field##name##Ctrl = GetDlgItem(hwnd, ID_EDIT_##uppid); PWCHAR Field##name##Text = CreateWstrForWindowText(Field##name##Ctrl)
LRESULT CALLBACK FastSearchingTrainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

        Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_CANCEL, L"取消");
        Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_OK, L"查询");
        Yinyue200_FastCreateLabelControl(hwnd, ID_LABEL_DATE, L"乘车日期");
        Yinyue200_FastCreateLabelControl(hwnd, ID_LABEL_STARTSTATION, L"行程起点");
        Yinyue200_FastCreateLabelControl(hwnd, ID_LABEL_ENDSTATION, L"行程终点");
        Yinyue200_FastCreateDatePickControl(hwnd, ID_EDIT_DATE);
        Yinyue200_FastCreateEditControl(hwnd, ID_EDIT_STARTSTATION);
        Yinyue200_FastCreateEditControl(hwnd, ID_EDIT_ENDSTATION);

        YINYUE200_FASTSEARCHINGTRAINWINDOWDATA* ldfwindow = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        ldfwindow->lastfont = yinyue200_CreateDefaultFont(hwnd);

        UINT dpi = yinyue200_GetDpiForWindow(hwnd);

        Yinyue200_SetWindowSize(hwnd, 600, 300, dpi);

        LayoutControls_FastSearchingTrainWindow(hwnd, dpi, ldfwindow);
    }
    return 0;
    case WM_NOTIFY:
    {

        return 0;
    }
    case WM_DPICHANGED:
    {
        YINYUE200_FASTSEARCHINGTRAINWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if (windowdata && windowdata->lastfont)
        {
            yinyue200_DeleteFont(windowdata->lastfont);
            windowdata->lastfont = yinyue200_CreateDefaultFont(hwnd);
            LayoutControls_FastSearchingTrainWindow(hwnd, HIWORD(wParam), windowdata);
        }
        break;
    }
    case WM_COMMAND:
    {
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
        {
            if (LOWORD(wParam) == ID_BUTTON_OK)
            {
                YINYUE200_FASTSEARCHINGTRAINWINDOWDATA* ldfwindow = GetProp(hwnd, YINYUE200_WINDOW_DATA);
                YINYUE200_MAINWINDOWDATA* mainwindow = ldfwindow->mainwindowdata;


                PREDEFINELOADDATAFILTERPROC(StartStation, STARTSTATION);//始发站
                PREDEFINELOADDATAFILTERPROC(EndStation, ENDSTATION);//终到站

                free(mainwindow->startstation);
                free(mainwindow->endstation);

                mainwindow->startstation = FieldStartStationText;
                mainwindow->endstation = FieldEndStationText;

                SYSTEMTIME date;
                DateTime_GetSystemtime(GetDlgItem(hwnd, ID_EDIT_DATE), &date);
                uint64_t localdateint64 = Yinyue200_ConvertToUINT64FromFileTime(ConvertDateToLocalFILETIME(date.wYear, date.wMonth, date.wDay));

                mainwindow->searchdate = localdateint64;

                vector nnowlist;
                vector_initwithcap(&nnowlist, yinyue200_ProductList.total);

                for (int i = 0; i < VECTOR_TOTAL(yinyue200_ProductList); i++)
                {
                    YINYUE200_TRAINPLANRECORD_PTR record = VECTOR_GET(yinyue200_ProductList, YINYUE200_TRAINPLANRECORD_PTR, i);
                    bool SHOULDREV = true;
                    do
                    {
                        bool findfinal = false;
                        vector* vecpoints = &record->RoutePoints;
                        bool findstart = false;
                        for (int j = 0; j < vector_total(vecpoints); j++)
                        {
                            YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR one = vector_get(vecpoints, j);
                            if (findstart)
                            {
                                if (wcscmp(one->Station.DisplayName, FieldEndStationText) == 0)
                                {
                                    findfinal = true;
                                    break;
                                }
                            }
                            else if (wcscmp(one->Station.DisplayName, FieldStartStationText) == 0)
                            {
                                findstart = true;
                            }
                        }
                        SHOULDREV &= findfinal;
                    } while (0);

                    if (SHOULDREV)
                    {
                        uint64_t outdate;
                        enum Yinyue200_TicketRefuseReason reason = Yinyue200_CheckTrainPlanRecordDateWithBookLimit(record, localdateint64, FieldStartStationText, FieldEndStationText, &outdate);
                        SHOULDREV &= reason == YINYUE200_TICKETREFUSERESON_NOREFUSE;
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
        YINYUE200_FASTSEARCHINGTRAINWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
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