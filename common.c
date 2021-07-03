#include "common.h"
#include "cJSON.h"
PWCHAR CreateWSTR(size_t length)
{
	return malloc(length * sizeof(wchar_t));
}
PWCHAR CreateWstrForWindowText(HWND hwnd)
{
	size_t size = GetWindowTextLength(hwnd + 1);
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
