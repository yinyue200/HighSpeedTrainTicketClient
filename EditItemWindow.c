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
#define ID_EDIT_NAME 1
#define ID_BUTTON_SAVE 2
#define ID_BUTTON_CANCEL 3
#define ID_EDIT_ID 4
#define ID_EDIT_TYPE 5
typedef struct EditItemWindowData
{
    PRODUCTRECORD_PTR ProductRecord;
    bool enablesave;
} EDITITEMWINDOWDATA;
LRESULT CALLBACK EditItemWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateEditItemWindow(PRODUCTRECORD_PTR productrecord,bool enablesave)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"yinyue200.SimpleStoreErp.EditItemWindow";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = EditItemWindowProc;
    wc.hInstance = yinyue200_hInstance;
    wc.lpszClassName = CLASS_NAME;

    WORD result = RegisterClass(&wc);
    //printf("%d", result);

    EDITITEMWINDOWDATA* windowdata = yinyue200_safemalloc(sizeof(EDITITEMWINDOWDATA));
    windowdata->ProductRecord = productrecord;
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

#define SETNULLORPRODUCTINFOMEMBERDATA(chwnd,member) SendMessage(chwnd, WM_SETTEXT, 0, productrecord==NULL?L"":productrecord->##member);
#define SETNULLORPRODUCTINFOMEMBERINTDATA(chwnd,member) if(productrecord==NULL)SendMessage(chwnd, WM_SETTEXT, 0, L"");else{ WCHAR _temp_buffer[30];swprintf(_temp_buffer,30, L"%lld", productrecord->##member); SendMessage(chwnd, WM_SETTEXT, 0, _temp_buffer);}
#define SAVEPRODUCTINFOMEMBERDATA(memberid,member) productrecord->##member=CreateWstrForWindowText(GetDlgItem(hwnd,memberid));
#define SAVEPRODUCTINFOMEMBERINTDATA(memberid,member) {PWCHAR _temp_int64_str = CreateWstrForWindowText(GetDlgItem(hwnd,memberid));\
int64_t _temp_int64;\
if (swscanf(_temp_int64_str, L"%lld", &_temp_int64) == 1)\
{\
    productrecord->##member = _temp_int64;\
}\
else\
{\
    MessageBox(hwnd, TEXT(#member) L"格式错误", NULL, 0);\
}\
free(_temp_int64_str);\
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

        EDITITEMWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);

        HWND NameLabelHwnd = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE, 10, lasty, 500, 25
            , hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 25;
        HWND hwndEdit_Name = CreateWindow(
            L"EDIT",   // predefined class 
            NULL,         // no window title 
            WS_BORDER | WS_CHILD | WS_VISIBLE |
            ES_LEFT ,
            10, lasty, 500, 25,   //MAYBE set size in WM_SIZE message 
            hwnd,         // parent window 
            (HMENU)ID_EDIT_NAME,   // edit control ID 
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);        // pointer not needed 
        lasty += 25;
        HWND NameLabelId = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE, 10, lasty, 500, 25
            , hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 25;
        HWND hwndEdit_ID = CreateWindow(
            L"EDIT",   // predefined class 
            NULL,         // no window title 
            WS_BORDER | WS_CHILD | WS_VISIBLE |
            ES_LEFT,
            10, lasty, 500, 25,
            hwnd,         // parent window 
            (HMENU)ID_EDIT_ID,   // edit control ID 
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);        // pointer not needed
        lasty += 25;
        HWND NameLabelType = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE, 10, lasty, 500, 25
            , hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 25;
        HWND hwndEdit_Type = CreateWindow(
            L"EDIT",
            NULL,
            WS_BORDER | WS_CHILD | WS_VISIBLE |
            ES_LEFT,
            10, lasty, 500, 25,
            hwnd,
            (HMENU)ID_EDIT_TYPE,
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);
        lasty += 25;
        HWND hwndButton_Save = CreateWindow(//see https://docs.microsoft.com/en-us/windows/win32/controls/buttons
            L"BUTTON",
            L"保存",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            10, lasty,100,50,
            hwnd, (HMENU)ID_BUTTON_SAVE, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),NULL);
        HWND hwndButton_Cancel = CreateWindow(
            L"BUTTON",
            L"取消",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            10+100+10, lasty, 100, 50,
            hwnd, (HMENU)ID_BUTTON_CANCEL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 50;
        PRODUCTRECORD_PTR productrecord = windowdata->ProductRecord;
        
        SendMessage(NameLabelHwnd, WM_SETTEXT, 0, L"名称");
        SETNULLORPRODUCTINFOMEMBERDATA(hwndEdit_Name, Name);
        SendMessage(NameLabelId, WM_SETTEXT, 0, L"ID");
        SETNULLORPRODUCTINFOMEMBERINTDATA(hwndEdit_ID, ID);
        SendMessage(NameLabelType, WM_SETTEXT, 0, L"类型");

        if (!windowdata->enablesave)
        {
            EnableWindow(hwndButton_Save, false);
        }
        
    }
    return 0;
    case WM_COMMAND:
    {
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
        {
            if (LOWORD(wParam) == ID_BUTTON_SAVE)
            {
                EDITITEMWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
                
                PRODUCTRECORD_PTR productrecord = windowdata->ProductRecord == NULL ? 
                    CreateProductRecord() : windowdata->ProductRecord;
                SAVEPRODUCTINFOMEMBERDATA(ID_EDIT_NAME, Name);
                SAVEPRODUCTINFOMEMBERINTDATA(ID_EDIT_ID, ID);

                if(windowdata->ProductRecord == NULL)
                    VECTOR_ADD(yinyue200_ProductList, productrecord);
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
        HANDLE windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if(windowdata!=NULL)
            free(windowdata);
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