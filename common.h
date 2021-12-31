//  SimpleStoreErp
//	Copyright(C) 2021 ��Խ
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
#pragma once
#include <windows.h>
#include <windowsx.h>
#include <stdbool.h>
#include "vector.h"
#include <string.h>
//  SimpleStoreErp
//	Copyright(C) 2021 ��Խ
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
#include <stdio.h>
#include "UserManage.h"

#define YINYUE200_WINDOW_DATA L"YINYUE200_WINDOW_DATA"

//���̱�ʶ
extern HINSTANCE yinyue200_hInstance;
//specifies how the application windows should be display
extern int yinyue200_nCmdShow;
//ȫ�ּ�¼��Ϣ
extern vector yinyue200_ProductList;
//��ǰ��¼�û���Ϣ
extern USERDATAINFO_PTR yinyue200_LoganUserInfo;

LPWORD lpwAlign(LPWORD lpIn);
//���ӵ�ǰ�Ĵ������
void AddWindowCount();
//���ٵ�ǰ�Ĵ������
void DecreaseWindowCount();
//���ص�ǰ�Ƿ����д򿪵Ĵ���
bool CheckIfNoWindow();
wchar_t* CreateWSTR(size_t length);
wchar_t* CreateWstrForWindowText(HWND hwnd);
//�޷��ָ��Ĵ���ֱ���˳�����
void UnrecoveryableFailed();
//��鵱ǰ�Ƿ�û�д򿨵Ĵ��ڣ����û�����˳�����
void CheckIfNoWindowAndQuit();
//�� state Ϊ false ʱ���� UnrecoveryableFailed()
void FailedIfFalse(bool state);
//�� malloc ʧ��ʱ���� UnrecoveryableFailed()
void* yinyue200_safemalloc(size_t size);
void* yinyue200_safemallocandclear(size_t size);