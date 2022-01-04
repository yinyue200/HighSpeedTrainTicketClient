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
#include <stdint.h>
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
#define YINYUE200_SKIP_LOGINCHECK 1 //����ʱ�Ƿ�������¼
#define YINYUE200_ROUTEPOINTEDITWINDOW_ALWAYS_PERFORMCHANGES_ON_NEWOBJECT 0
#include "UserManage.h"
#define YINYUE200_WINDOW_DATA L"YINYUE200_WINDOW_DATA"
//C ���������ú�ʵ�ֵ��෺��Ԫ��ṹ�嶨��
#define YINYUE200_DEFINE_PAIR(type1,type2) typedef struct tag_yinyue200_pair_of_##type1##_##type2 \
{\
	type1 Item1;\
	type2 Item2;\
} YINYUE200_PAIR_OF_##type1##_##type2;
YINYUE200_DEFINE_PAIR(uint64_t, uint64_t)
YINYUE200_DEFINE_PAIR(wchar_t, double)
YINYUE200_DEFINE_PAIR(wchar_t, int32_t)

extern HINSTANCE yinyue200_hInstance;//global handle to the program instance.
extern int yinyue200_nCmdShow;//specifies how the application windows should be display
extern vector yinyue200_ProductList;//ȫ�ּ�¼��Ϣ
extern USERDATAINFO_PTR yinyue200_LoganUserInfo;//��ǰ��¼�û���Ϣ

LPWORD lpwAlign(LPWORD lpIn);
//���ӵ�ǰ�Ĵ������
void AddWindowCount();
//���ٵ�ǰ�Ĵ������
void DecreaseWindowCount();
//���ص�ǰ�Ƿ����д򿪵Ĵ���
bool CheckIfNoWindow();
wchar_t* CreateWSTR(size_t length);
wchar_t* CreateWstrForWindowText(HWND hwnd);
wchar_t* CreateWstrFromWstr(wchar_t *hwnd);
//�޷��ָ��Ĵ���ֱ���˳�����
void UnrecoveryableFailed();
//��鵱ǰ�Ƿ�û�д򿨵Ĵ��ڣ����û�����˳�����
void CheckIfNoWindowAndQuit();
//��� hresult,�� hresult ֵ����ʧ��ʱ�˳�����
void CheckHResult(HRESULT hresult);
//�� state Ϊ false ʱ���� UnrecoveryableFailed()
void FailedIfFalse(bool state);
//�� malloc ʧ��ʱ���� UnrecoveryableFailed()
void* yinyue200_safemalloc(size_t size);
//����yinyue200_safemalloc�����ڴ�������0
void* yinyue200_safemallocandclear(size_t size);
GUID Yinyue200_ConvertToGuid(uint64_t high, uint64_t low);
YINYUE200_PAIR_OF_uint64_t_uint64_t Yinyue200_ConvertToUint64PairFromGuid(GUID guid);
FILETIME Yinyue200_ConvertToFileTimeFromUINT64(uint64_t time);
uint64_t Yinyue200_ConvertToUINT64FromFileTime(FILETIME time);
double Yinyue200_ConvertToTotalSecondFromUINT64(uint64_t time);
uint64_t Yinyue200_ConvertToUINT64FromTotalSecond(double time);