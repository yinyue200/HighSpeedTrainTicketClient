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
LPWORD lpwAlign(LPWORD lpIn)
{
	ULONG ul;

	ul = (ULONG)lpIn;
	ul++;
	ul >>= 1;
	ul <<= 1;
	return (ULONG)ul;
}
PWCHAR CreateWSTR(size_t length)
{
	return malloc(length * sizeof(wchar_t));
}
PWCHAR CreateWstrForWindowText(HWND hwnd)
{
	size_t size = GetWindowTextLength(hwnd) + 1;
	PWCHAR p = CreateWSTR(size);
	GetWindowText(hwnd, p, size);
	return p;
}
__declspec(noreturn) void UnrecoveryableFailed()
{
	MessageBox(NULL, L"不可恢复的错误", NULL, 0);
	exit(1234);//检测到不可恢复的错误后直接退出程序
}
void CheckIfNoWindowAndQuit()
{
	if (CheckIfNoWindow())
		PostQuitMessage(0);
}
void FailedIfFalse(bool state)
{
	if (state)
	{
		;
	}
	else
	{
		UnrecoveryableFailed();
	}
}
void* yinyue200_safemalloc(size_t size)
{
	void* p = malloc(size);
	if (p)
	{
		return p;
	}
	else
	{
		UnrecoveryableFailed();
		return NULL;
	}
}
