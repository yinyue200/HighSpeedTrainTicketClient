#pragma once
#include "common.h"
typedef void (*InputBoxCallbackFunc)(void*, LPWSTR);
void CreateInputBoxWindow(LPWSTR msg, InputBoxCallbackFunc callback, void* callbackcontext, bool ispwd);
