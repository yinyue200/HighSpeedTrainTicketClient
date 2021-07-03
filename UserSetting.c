#include "UserSetting.h"

static PWCHAR yinyue200_configFilePath = L"data.tsv";

PWCHAR yinyue200_GetConfigFilePath()
{
	return yinyue200_configFilePath;
}