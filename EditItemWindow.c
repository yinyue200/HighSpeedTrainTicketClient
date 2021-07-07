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
#define ID_EDIT_STATE 6
#define ID_EDIT_DATE 7
#define ID_EDIT_PROVIDEBY 8
#define ID_EDIT_RECIEVEDBY 9
#define ID_EDIT_RESENTBY 10
#define ID_EDIT_COUNT 11
#define ID_EDIT_COST 12
#define ID_EDIT_PRICE 13
#define ID_EDIT_RESENTPRICE 14
#define ID_EDIT_SIGNER 15
#define ID_BUTTON_SAVEANDNEXT 16
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
        CW_USEDEFAULT, CW_USEDEFAULT, 700, 770,

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
else\
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
else\
{\
    MessageBox(hwnd, TEXT(#member) L"格式错误", NULL, 0);\
}\
free(_temp_int64_str);\
}
#define FORCESPACE  
#define ADDLABELANDEDIT(ctrl_id,CTRL_EDIT_ID,displaylabel) HWND Hwnd_##ctrl_id##_Label = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE, 10, lasty, 500, 25 , hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);\
lasty += 25;\
HWND hwnd_##ctrl_id##_Edit = CreateWindow(L"EDIT", NULL, WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT, 10, lasty, 500, 25, hwnd, (HMENU)CTRL_EDIT_ID, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL); \
lasty += 25; \
SendMessage(Hwnd_##ctrl_id##_Label,WM_SETTEXT,0,displaylabel);
void edititemwindow_initctrl(HWND hwnd,PRODUCTRECORD_PTR productrecord)
{
    SETNULLORPRODUCTINFOMEMBERDATA(ID_EDIT_NAME, Name);
    SETNULLORPRODUCTINFOMEMBERINTDATA(ID_EDIT_ID, ID);
    SETNULLORPRODUCTINFOMEMBERDATA(ID_EDIT_TYPE, Type);
    SETNULLORPRODUCTINFOMEMBERDATA(ID_EDIT_STATE, State);
    SETNULLORPRODUCTINFOMEMBERINTDATA(ID_EDIT_DATE, Date);
    SETNULLORPRODUCTINFOMEMBERDATA(ID_EDIT_PROVIDEBY, ProvideBy);
    SETNULLORPRODUCTINFOMEMBERDATA(ID_EDIT_RECIEVEDBY, RecievedBy);
    SETNULLORPRODUCTINFOMEMBERDATA(ID_EDIT_RESENTBY, ResentBy);
    SETNULLORPRODUCTINFOMEMBERINTDATA(ID_EDIT_COUNT, Count);
    SETNULLORPRODUCTINFOMEMBERPRICEDATA(ID_EDIT_COST, Cost);
    SETNULLORPRODUCTINFOMEMBERPRICEDATA(ID_EDIT_PRICE, Price);
    SETNULLORPRODUCTINFOMEMBERPRICEDATA(ID_EDIT_RESENTPRICE, ResentPrice);
    SETNULLORPRODUCTINFOMEMBERDATA(ID_EDIT_SIGNER, Signer);
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

        ADDLABELANDEDIT(TYPE, ID_EDIT_TYPE,L"类型");
        ADDLABELANDEDIT(STATE, ID_EDIT_STATE, L"状态");
        ADDLABELANDEDIT(DATE, ID_EDIT_DATE, L"日期");
        ADDLABELANDEDIT(PROVIDEBY, ID_EDIT_PROVIDEBY, L"供货商");
        ADDLABELANDEDIT(RECIEVEDBY, ID_EDIT_RECIEVEDBY, L"收货商");
        ADDLABELANDEDIT(RESENTBY, ID_EDIT_RESENTBY, L"退货商");
        ADDLABELANDEDIT(COUNT, ID_EDIT_COUNT, L"数量");
        ADDLABELANDEDIT(COST, ID_EDIT_COST, L"进价");
        ADDLABELANDEDIT(PRICE, ID_EDIT_PRICE, L"销售价");
        ADDLABELANDEDIT(RESENTPRICE, ID_EDIT_RESENTPRICE, L"退货价");
        ADDLABELANDEDIT(SIGNER, ID_EDIT_SIGNER, L"经手人");

        lasty += 10;
        HWND hwndButton_Save = CreateWindow(//see https://docs.microsoft.com/en-us/windows/win32/controls/buttons
            L"BUTTON",
            L"保存",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            10, lasty,100,50,
            hwnd, (HMENU)ID_BUTTON_SAVE, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),NULL);
        HWND hwndButton_SaveAndNext = CreateWindow(
            L"BUTTON",
            L"保存并添加下一个",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            10 + 100 + 10, lasty, 200, 50,
            hwnd, (HMENU)ID_BUTTON_SAVEANDNEXT, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        HWND hwndButton_Cancel = CreateWindow(
            L"BUTTON",
            L"取消",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            10+300+20, lasty, 100, 50,
            hwnd, (HMENU)ID_BUTTON_CANCEL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        lasty += 50;
        PRODUCTRECORD_PTR productrecord = windowdata->ProductRecord;
        
        SendMessage(NameLabelHwnd, WM_SETTEXT, 0, L"名称");
        SendMessage(NameLabelId, WM_SETTEXT, 0, L"ID");

        edititemwindow_initctrl(hwnd, productrecord);

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
            switch (LOWORD(wParam))
            {
            case ID_BUTTON_SAVEANDNEXT:
            case ID_BUTTON_SAVE:
            {
                EDITITEMWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);

                PRODUCTRECORD_PTR productrecord = windowdata->ProductRecord == NULL ?
                    CreateProductRecord() : windowdata->ProductRecord;
                SAVEPRODUCTINFOMEMBERDATA(ID_EDIT_NAME, Name);
                SAVEPRODUCTINFOMEMBERINTDATA(ID_EDIT_ID, ID);
                SAVEPRODUCTINFOMEMBERDATA(ID_EDIT_TYPE, Type);
                SAVEPRODUCTINFOMEMBERDATA(ID_EDIT_STATE, State);
                SAVEPRODUCTINFOMEMBERINTDATA(ID_EDIT_DATE, Date);
                SAVEPRODUCTINFOMEMBERDATA(ID_EDIT_PROVIDEBY, ProvideBy);
                SAVEPRODUCTINFOMEMBERDATA(ID_EDIT_RECIEVEDBY, RecievedBy);
                SAVEPRODUCTINFOMEMBERDATA(ID_EDIT_RESENTBY, ResentBy);
                SAVEPRODUCTINFOMEMBERINTDATA(ID_EDIT_COUNT, Count);
                SAVEPRODUCTINFOMEMBERPRICEDATA(ID_EDIT_COST, Cost);
                SAVEPRODUCTINFOMEMBERPRICEDATA(ID_EDIT_PRICE, Price);
                SAVEPRODUCTINFOMEMBERPRICEDATA(ID_EDIT_RESENTPRICE, ResentPrice);
                SAVEPRODUCTINFOMEMBERDATA(ID_EDIT_SIGNER, Signer);

                if (windowdata->ProductRecord == NULL)
                    VECTOR_ADD(yinyue200_ProductList, productrecord);

                if (LOWORD(wParam) == ID_BUTTON_SAVEANDNEXT)
                {
                    windowdata->ProductRecord = NULL;
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