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
#include "PassengersManage.h"
#include "ControlsCommonOperation.h"
#include "DpiHelper.h"
#include "PassengerRecordEditWindow.h"
#include "LoginWindow.h"
#define ID_BUTTON_SAVE 1
#define ID_BUTTON_CANCEL 2
#define ID_EDIT_IDTYPE 3
#define ID_EDIT_IDNUMBER 4
#define ID_EDIT_FULLNAME 6
#define ID_EDIT_OWNER 7
#define ID_EDIT_PHONENUM 8
#define ID_EDIT_EMERGENCYCONTCATPERSONFULLNAME 9
#define ID_EDIT_EMERGENCYCONTCATPERSONPHONENUMBER 10
#define ID_LABEL_IDTYPE 11
#define ID_LABEL_IDNUMBER 12
#define ID_LABEL_FULLNAME 13
#define ID_LABEL_OWNER 14
#define ID_LABEL_PHONENUM 15
#define ID_LABEL_EMERGENCYCONTCATPERSONFULLNAME 16
#define ID_LABEL_EMERGENCYCONTCATPERSONPHONENUMBER 17

typedef struct Yinyue200_PassengerInfoEditWindowData
{
    YINYUE200_PASSENGERINFO_PTR PassengerInfo;
    PassengerInfoEditFinishCallback callback;
    void* callbackcontext;
    HFONT lastfont;
    bool callbacksent;
} YINYUE200_PASSENGERINFOEDITWINDOWDATA;
LRESULT CALLBACK PassengerRecordEditWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreatePassengerRecordEditWindow(YINYUE200_PASSENGERINFO_PTR productrecord, PassengerInfoEditFinishCallback callback, void* callbackcontext)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"yinyue200.HighSpeedTrainTicketClient.PassengerRecordEditWindow";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = PassengerRecordEditWindowProc;
    wc.hInstance = yinyue200_hInstance;
    wc.lpszClassName = CLASS_NAME;

    WORD result = RegisterClass(&wc);
    //printf("%d", result);

    YINYUE200_PASSENGERINFOEDITWINDOWDATA* windowdata = yinyue200_safemallocandclear(sizeof(YINYUE200_PASSENGERINFOEDITWINDOWDATA));
    windowdata->PassengerInfo = productrecord;
    windowdata->callback = callback;
    windowdata->callbackcontext = callbackcontext;

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"乘客信息",    // Window text
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
//确定各控件位置
void LayoutControls_PassengerRecordEditWindow(HWND hwnd, UINT dpi, YINYUE200_PASSENGERINFOEDITWINDOWDATA* windata)
{
    Yinyue200_SetWindowSize(hwnd, 600, 500, dpi);
    if (windata->lastfont)
    {
        HFONT font = windata->lastfont;
        int lasty = 10;
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(IDTYPE);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(IDNUMBER);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(FULLNAME);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(OWNER);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(PHONENUM);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(EMERGENCYCONTCATPERSONFULLNAME);
        YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(EMERGENCYCONTCATPERSONPHONENUMBER);


        lasty += 10;
        YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_SAVE, 10, lasty, 100, 50);
        YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_CANCEL, 10 + 100 + 10, lasty, 100, 50);
    }

}
#define SETNULLORPRODUCTINFOMEMBERDATA(chwnd,member) SendMessage(GetDlgItem(hwnd,chwnd), WM_SETTEXT, 0, productrecord==NULL?L"":productrecord->##member);
//初始化编辑框初值
void passengerrecordeditwindow_initctrl(HWND hwnd, YINYUE200_PASSENGERINFO_PTR productrecord)
{
    
    SETNULLORPRODUCTINFOMEMBERDATA(ID_EDIT_IDTYPE, IDType);
    SETNULLORPRODUCTINFOMEMBERDATA(ID_EDIT_IDNUMBER, IDNumber);
    SETNULLORPRODUCTINFOMEMBERDATA(ID_EDIT_FULLNAME, FullName);
    SETNULLORPRODUCTINFOMEMBERDATA(ID_EDIT_OWNER, Owner);
    SETNULLORPRODUCTINFOMEMBERDATA(ID_EDIT_PHONENUM, PhoneNum);
    SETNULLORPRODUCTINFOMEMBERDATA(ID_EDIT_EMERGENCYCONTCATPERSONFULLNAME, EmergencyContactPersonFullName);
    SETNULLORPRODUCTINFOMEMBERDATA(ID_EDIT_EMERGENCYCONTCATPERSONPHONENUMBER, EmergencyContactPhoneNumber);
}
#define SAVEPASSENGERINFOMEMBERDATA(memberid,member) do\
{\
    free(ptr->##member);\
    ptr->##member=CreateWstrForWindowText(GetDlgItem(hwnd,memberid));\
}while(0);
//乘客信息编辑窗口消息处理函数
LRESULT CALLBACK PassengerRecordEditWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

        YINYUE200_PASSENGERINFOEDITWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        windata->lastfont = yinyue200_CreateDefaultFont(hwnd);

        Yinyue200_FastCreateLabelControl(hwnd, ID_LABEL_IDTYPE, L"证件类型");
        HWND hwnd_IDTYPE_Edit = Yinyue200_FastCreateComboBoxControl(hwnd, ID_EDIT_IDTYPE);

        do
        {
            TCHAR States[6][20] =
            {
                L"中国居民身份证", L"港澳居民来往内地通行证", L"台湾居民来往大陆通行证", L"护照", L"外国人永久居留身份证", L"港澳台居民居住证"
            };

            TCHAR A[16];
            int  k = 0;

            memset(&A, 0, sizeof(A));
            for (k = 0; k < 6; k += 1)
            {
                wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)States[k]);

                // Add string to combobox.
                SendMessage(hwnd_IDTYPE_Edit, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);
            }
        } while (0);


        ADDLABELANDEDIT(IDNUMBER, L"证件号");
        ADDLABELANDEDIT(FULLNAME, L"姓名");
        ADDLABELANDEDIT(OWNER, L"创建人");
        ADDLABELANDEDIT(PHONENUM, L"手机号");
        ADDLABELANDEDIT(EMERGENCYCONTCATPERSONFULLNAME, L"紧急联系人姓名");
        ADDLABELANDEDIT(EMERGENCYCONTCATPERSONPHONENUMBER, L"紧急联系人手机号");

        EnableWindow(hwnd_OWNER_Edit, false);
        if (windata->PassengerInfo != NULL)
        {
            EnableWindow(hwnd_IDTYPE_Edit, false);//不允许证件类型修改
            EnableWindow(hwnd_IDNUMBER_Edit, false);//不允许证件号修改
        }
        else
        {
            SendMessage(hwnd_OWNER_Edit, WM_SETTEXT, 0, GetNowLoginedUserName());
        }

        HWND hwnd_okbutton = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_SAVE, L"确定");
        HWND hwnd_cancelbutton = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_CANCEL, L"取消");

        passengerrecordeditwindow_initctrl(hwnd, windata->PassengerInfo);//初始化编辑框初值

        LayoutControls_PassengerRecordEditWindow(hwnd, yinyue200_GetDpiForWindow(hwnd), windata);
    }
    break;
    case WM_DESTROY:
    {
        YINYUE200_PASSENGERINFOEDITWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
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
        YINYUE200_PASSENGERINFOEDITWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
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
                YINYUE200_PASSENGERINFOEDITWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);

                windowdata->callback(NULL, windowdata->callbackcontext);//取消修改时返回 NULL 代表操作被取消
                windowdata->callbacksent = true;

                SendMessage(hwnd, WM_CLOSE, NULL, NULL);
                break;
            case ID_BUTTON_SAVE:
            {
                YINYUE200_PASSENGERINFOEDITWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);

                YINYUE200_PASSENGERINFO_PTR ptr = windowdata->PassengerInfo;
                if (ptr == NULL)
                {
                    ptr = CreatePassengerInfo();//创建一个新的 YINYUE200_PASSENGERINFO
                }

                SAVEPASSENGERINFOMEMBERDATA(ID_EDIT_IDTYPE, IDType);
                SAVEPASSENGERINFOMEMBERDATA(ID_EDIT_IDNUMBER, IDNumber);
                SAVEPASSENGERINFOMEMBERDATA(ID_EDIT_FULLNAME, FullName);
                SAVEPASSENGERINFOMEMBERDATA(ID_EDIT_OWNER, Owner);
                SAVEPASSENGERINFOMEMBERDATA(ID_EDIT_PHONENUM, PhoneNum);
                SAVEPASSENGERINFOMEMBERDATA(ID_EDIT_EMERGENCYCONTCATPERSONFULLNAME, EmergencyContactPersonFullName);
                SAVEPASSENGERINFOMEMBERDATA(ID_EDIT_EMERGENCYCONTCATPERSONPHONENUMBER, EmergencyContactPhoneNumber);
                

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