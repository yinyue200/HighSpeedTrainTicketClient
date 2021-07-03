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
	MessageBox(NULL, L"���ɻָ��Ĵ���", NULL, 0);
	exit(1234);//��⵽���ɻָ��Ĵ����ֱ���˳�����
}
void CheckIfNoWindowAndQuit()
{
	if (CheckIfNoWindow())
		PostQuitMessage(0);
}
