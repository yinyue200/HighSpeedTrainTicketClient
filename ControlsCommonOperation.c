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

#include "ControlsCommonOperation.h"
#include "DpiHelper.h"
#include <CommCtrl.h>
#include "common.h"
void Yinyue200_SetWindowFont(HWND hwnd, HFONT font)
{
    SendMessage(hwnd, WM_SETFONT, (WPARAM)font, TRUE);
}
HWND Yinyue200_GetChildControlById(HWND hwnd, int id)
{
    return GetDlgItem(hwnd, id);
}
//结合当前DPI信息设置控件位置
LRESULT Yinyue200_SetControlPos(HWND hwnd, double x, double y, double cx, double cy, UINT DPI)
{
    SetWindowPos(hwnd, NULL,
        YINYUE200_LOGICTOPHYBYDPI_DOUBLE(x, DPI),
        YINYUE200_LOGICTOPHYBYDPI_DOUBLE(y, DPI),
        YINYUE200_LOGICTOPHYBYDPI_DOUBLE(cx, DPI),
        YINYUE200_LOGICTOPHYBYDPI_DOUBLE(cy, DPI),
        SWP_NOZORDER | SWP_NOACTIVATE);
}
//结合当前DPI信息设置窗口位置
LRESULT Yinyue200_SetWindowSize(HWND hwnd, double cx, double cy, UINT DPI)
{
    SetWindowPos(hwnd, NULL,
        0, 0,
        YINYUE200_LOGICTOPHYBYDPI_DOUBLE(cx, DPI),
        YINYUE200_LOGICTOPHYBYDPI_DOUBLE(cy, DPI),
        SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}
//获取窗口客户区域大小
SIZE Yinyue200_GetWindowClientAreaSize(HWND window)
{
    RECT rect;
    GetClientRect(window, &rect);
    SIZE size = { rect.right - rect.left, rect.bottom - rect.top };
    return size;
}
HWND Yinyue200_FastCreateControl(LPCTSTR type, HWND parent, HMENU id, DWORD style, LPCTSTR title)
{
    return CreateWindow(
        type,   // predefined class 
        title,         // window title 
        style,
        0, 0, 0, 0, //这里不设置大小和位置，后续进行设置，这是考虑到高DIP适配和窗口大小改变等问题
        parent,         // parent window 
        id,   // control ID 
        (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE),
        NULL);
}
//快速创建文本输入框控件
HWND Yinyue200_FastCreateEditControl(HWND parent, HMENU id)
{
    return Yinyue200_FastCreateControl(L"EDIT", parent, id, WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT, NULL);
}
//快速创建按钮控件
HWND Yinyue200_FastCreateButtonControl(HWND parent, HMENU id, LPCTSTR title)
{
    return Yinyue200_FastCreateControl(L"BUTTON", parent, id, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        title);
}
//快速创建静态文本控件
HWND Yinyue200_FastCreateLabelControl(HWND parent, HMENU id, LPCTSTR content)
{
    HWND hwnd = Yinyue200_FastCreateLabelControlWithoutContent(parent, id);
    SendMessage(hwnd, WM_SETTEXT, 0, content);
    return hwnd;
}
//快速创建静态文本控件
HWND Yinyue200_FastCreateLabelControlWithoutContent(HWND parent, HMENU id)
{
    HWND hwnd = Yinyue200_FastCreateControl(L"STATIC", parent, id, WS_CHILD | WS_VISIBLE, NULL);
    return hwnd;
}
//快速创建复选框控件
HWND Yinyue200_FastCreateCheckBoxControl(HWND parent, HMENU id, LPCTSTR title)
{
    return Yinyue200_FastCreateControl(L"BUTTON", parent, id, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, title);
}
//  Yinyue200_FastCreateDatePickControl creates a DTP control
//  Returns the handle to the new DTP control if successful, or NULL 
//  otherwise.
// 
//    hwnd - The handle to the window.
HWND Yinyue200_FastCreateDatePickControl(HWND hwnd, HMENU id)
{
    HWND hwndDP = CreateWindowEx(0,
            DATETIMEPICK_CLASS,
            TEXT("DateTime"),
            WS_BORDER | WS_CHILD | WS_VISIBLE | DTS_LONGDATEFORMAT,
            0,0,0,0,
            hwnd,
            id,
            yinyue200_hInstance,
            NULL);

    return (hwndDP);
}
//  Yinyue200_FastCreateDatePickControl creates a DTP control
//  Returns the handle to the new DTP control if successful, or NULL 
//  otherwise.
// 
//    hwnd - The handle to the window.
HWND Yinyue200_FastCreateTimePickControl(HWND hwnd, HMENU id)
{
    HWND hwndDP = CreateWindowEx(0,
        DATETIMEPICK_CLASS,
        TEXT("DateTime"),
        WS_BORDER | WS_CHILD | WS_VISIBLE | DTS_TIMEFORMAT,
        0, 0, 0, 0,
        hwnd,
        id,
        yinyue200_hInstance,
        NULL);

    return (hwndDP);
}
HWND Yinyue200_FastCreateListViewControl(HWND hwnd, HMENU id)
{
    DWORD       dwStyle;
    HWND        hwndListView;

    dwStyle = WS_TABSTOP |
        WS_CHILD |
        WS_BORDER |
        WS_VISIBLE |
        LVS_AUTOARRANGE |
        LVS_REPORT |
        LVS_OWNERDATA;

    hwndListView = CreateWindowEx(WS_EX_CLIENTEDGE,          // ex style
        WC_LISTVIEW,               // class name - defined in commctrl.h
        TEXT(""),                        // dummy text
        dwStyle,                   // style
        0,                         // x position
        0,                         // y position
        0,                         // width
        0,                         // height
        hwnd,                // parent
        id,        // ID
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),  // instance
        NULL);                     // no extra data

    return hwndListView;
}
//将字符串 str 转换为 double 并赋值给 setto
int Yinyue200_EditWindowParseFromString(LPWSTR str,double *setto)
{
    double ddata;
    if (swscanf(str, L"%lf", &ddata) == 1)
    {
        *setto = ddata;//只有成功读取才进行赋值
        return 1;
    }
    else  if (str[0] != 0)//如果str不是空字符串
    {
        return -1;
    }
    return 0;
}
int Yinyue200_EditWindowParseFromStringAndFree(LPWSTR str, double* setto)
{
    int ret = Yinyue200_EditWindowParseFromString(str, setto);
    free(str);
    return ret;
}
HWND Yinyue200_FastCreateComboBoxControl(HWND hwnd, HMENU id)
{
    return Yinyue200_FastCreateControl(WC_COMBOBOX, hwnd, id, CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, L"");
}
HWND Yinyue200_FastCreateComboBoxDropListControl(HWND hwnd, HMENU id)
{
    return Yinyue200_FastCreateControl(WC_COMBOBOX, hwnd, id, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, L"");
}
HWND Yinyue200_FastCreateListBoxControl(HWND hwnd, HMENU id)
{
    return Yinyue200_FastCreateControl(WC_LISTBOX, hwnd, id, WS_VSCROLL|LBS_MULTIPLESEL | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, L"");
}