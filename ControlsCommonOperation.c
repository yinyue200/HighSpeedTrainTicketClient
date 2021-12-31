//  SimpleStoreErp
//	Copyright(C) 2021 ��Խ
//
//	This program is free software : you can redistribute it and/or modify
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
#include "ControlsCommonOperation.h"
#include "DpiHelper.h"
void Yinyue200_SetWindowFont(HWND hwnd, HFONT font)
{
    SendMessage(hwnd, WM_SETFONT, (WPARAM)font, TRUE);
}
HWND Yinyue200_GetChildControlById(HWND hwnd, int id)
{
    return GetDlgItem(hwnd, id);
}
//��ϵ�ǰDPI��Ϣ���ÿؼ�λ��
LRESULT Yinyue200_SetControlPos(HWND hwnd, double x, double y, double cx, double cy, UINT DPI)
{
    SetWindowPos(hwnd, NULL,
        YINYUE200_LOGICTOPHYBYDPI_DOUBLE(x, DPI),
        YINYUE200_LOGICTOPHYBYDPI_DOUBLE(y, DPI),
        YINYUE200_LOGICTOPHYBYDPI_DOUBLE(cx, DPI),
        YINYUE200_LOGICTOPHYBYDPI_DOUBLE(cy, DPI),
        SWP_NOZORDER | SWP_NOACTIVATE);
}
//��ϵ�ǰDPI��Ϣ���ô���λ��
LRESULT Yinyue200_SetWindowSize(HWND hwnd, double cx, double cy, UINT DPI)
{
    SetWindowPos(hwnd, NULL,
        0, 0,
        YINYUE200_LOGICTOPHYBYDPI_DOUBLE(cx, DPI),
        YINYUE200_LOGICTOPHYBYDPI_DOUBLE(cy, DPI),
        SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}
HWND Yinyue200_FastCreateControl(LPCTSTR type, HWND parent, HMENU id, DWORD style, LPCTSTR title)
{
    return CreateWindow(
        type,   // predefined class 
        title,         // window title 
        style,
        0, 0, 0, 0, //���ﲻ���ô�С��λ�ã������������ã����ǿ��ǵ���DIP����ʹ��ڴ�С�ı������
        parent,         // parent window 
        id,   // control ID 
        (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE),
        NULL);
}
//���ٴ����ı������ؼ�
HWND Yinyue200_FastCreateEditControls(HWND parent, HMENU id)
{
    return Yinyue200_FastCreateControl(L"EDIT", parent, id, WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT, NULL);
}
//���ٴ�����ť�ؼ�
HWND Yinyue200_FastCreateButtonControls(HWND parent, HMENU id, LPCTSTR title)
{
    return Yinyue200_FastCreateControl(L"BUTTON", parent, id, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        title);
}
//���ٴ�����̬�ı��ؼ�
HWND Yinyue200_FastCreateLabelControls(HWND parent, HMENU id, LPCTSTR content)
{
    HWND hwnd = Yinyue200_FastCreateControl(L"STATIC", parent, id, WS_CHILD | WS_VISIBLE, NULL);
    SendMessage(hwnd, WM_SETTEXT, 0, content);
    return hwnd;
}

HWND Yinyue200_FastCreateCheckBoxControls(HWND parent, HMENU id, LPCTSTR title)
{
    return Yinyue200_FastCreateControl(L"BUTTON", parent, id, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, title);
}
