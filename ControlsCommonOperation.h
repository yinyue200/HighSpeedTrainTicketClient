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
#pragma once
#include <Windows.h>
#define YINYUE200_SETCONTROLPOSANDFONT(ID,X,Y,cx,cy) do{\
HWND chwnd=Yinyue200_GetChildControlById(hwnd, (ID));\
Yinyue200_SetControlPos(chwnd,(X),(Y),(cx),(cy),dpi);\
Yinyue200_SetWindowFont(chwnd,font);\
}while(0)
void Yinyue200_SetWindowFont(HWND hwnd, HFONT font);
HWND Yinyue200_GetChildControlById(HWND hwnd, int id);
//��ϵ�ǰDPI��Ϣ���ÿؼ�λ��
LRESULT Yinyue200_SetControlPos(HWND hwnd, double x, double y, double cx, double cy, UINT DPI);
//��ϵ�ǰDPI��Ϣ���ô���λ��
LRESULT Yinyue200_SetWindowSize(HWND hwnd, double cx, double cy, UINT DPI);
//��ȡ���ڿͻ������С
SIZE Yinyue200_GetWindowClientAreaSize(HWND window);
HWND Yinyue200_FastCreateControl(LPCTSTR type, HWND parent, HMENU id, DWORD style, LPCTSTR title);
//���ٴ����ı������ؼ�
HWND Yinyue200_FastCreateEditControl(HWND parent, HMENU id);
//���ٴ�����ť�ؼ�
HWND Yinyue200_FastCreateButtonControl(HWND parent, HMENU id, LPCTSTR title);
//���ٴ�����̬�ı��ؼ�
HWND Yinyue200_FastCreateLabelControl(HWND parent, HMENU id, LPCTSTR content);
//���ٴ�����̬�ı��ؼ�
HWND Yinyue200_FastCreateCheckBoxControl(HWND parent, HMENU id, LPCTSTR content);
//���ٴ�����ѡ��ؼ�
HWND Yinyue200_FastCreateLabelControlWithoutContent(HWND parent, HMENU id);
//  Yinyue200_FastCreateDatePickControl creates a DTP control
//  Returns the handle to the new DTP control if successful, or NULL 
//  otherwise.
// 
//    hwnd - The handle to the window.
//    id - https://stackoverflow.com/questions/20533217/what-is-winapi-hmenu-and-how-do-i-use-it
HWND Yinyue200_FastCreateDatePickControl(HWND hwnd, HMENU id);
//  Yinyue200_FastCreateDatePickControl creates a DTP control
//  Returns the handle to the new DTP control if successful, or NULL 
//  otherwise.
// 
//    hwnd - The handle to the window.
//    id - https://stackoverflow.com/questions/20533217/what-is-winapi-hmenu-and-how-do-i-use-it
HWND Yinyue200_FastCreateTimePickControl(HWND hwnd, HMENU id);
//  Yinyue200_FastCreateDatePickControl creates a ListView control
//  Returns the handle to the new ListView control if successful, or NULL 
//  otherwise.
// 
//    hwnd - The handle to the window.
//    id - https://stackoverflow.com/questions/20533217/what-is-winapi-hmenu-and-how-do-i-use-it
HWND Yinyue200_FastCreateListViewControl(HWND hwnd, HMENU id);
int Yinyue200_EditWindowParseFromString(LPWSTR str, double* setto);
int Yinyue200_EditWindowParseFromStringAndFree(LPWSTR str, double* setto);