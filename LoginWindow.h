#pragma once
#include "common.h"
void CreateLoginWindow(LPWSTR username, void (*callback)(void*), void* callbackcontext);
LPWSTR GetNowLoginedUserName();