//  SimpleStoreErp
//	Copyright(C) 2021 殷越
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
#pragma once
#include <Windows.h>
#define YINYUE200_SETCONTROLPOSANDFONT(ID,X,Y,cx,cy) do{\
HWND chwnd=Yinyue200_GetChildControlById(hwnd, (ID));\
Yinyue200_SetControlPos(chwnd,(X),(Y),(cx),(cy),dpi);\
Yinyue200_SetWindowFont(chwnd,font);\
}while(0)
void Yinyue200_SetWindowFont(HWND hwnd, HFONT font);
HWND Yinyue200_GetChildControlById(HWND hwnd, int id);
//结合当前DPI信息设置控件位置
LRESULT Yinyue200_SetControlPos(HWND hwnd, double x, double y, double cx, double cy, UINT DPI);
//结合当前DPI信息设置窗口位置
LRESULT Yinyue200_SetWindowSize(HWND hwnd, double cx, double cy, UINT DPI);
HWND Yinyue200_FastCreateControl(LPCTSTR type, HWND parent, HMENU id, DWORD style, LPCTSTR title);
//快速创建文本输入框控件
HWND Yinyue200_FastCreateEditControls(HWND parent, HMENU id);
//快速创建按钮控件
HWND Yinyue200_FastCreateButtonControls(HWND parent, HMENU id, LPCTSTR title);
//快速创建静态文本控件
HWND Yinyue200_FastCreateLabelControls(HWND parent, HMENU id, LPCTSTR content);
//快速创建复选框控件
HWND Yinyue200_FastCreateCheckBoxControls(HWND parent, HMENU id, LPCTSTR content);