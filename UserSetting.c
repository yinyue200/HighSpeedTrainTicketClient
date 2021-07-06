#include "UserSetting.h"

static PWCHAR yinyue200_configFilePath = L"data.tsv";
static PWCHAR yinyue200_userConfigFilePath = L"userinfo.tsv";

PWCHAR yinyue200_GetConfigFilePath()
{
	return yinyue200_configFilePath;
}

PWCHAR yinyue200_GetUserConfigFilePath()
{
	return yinyue200_userConfigFilePath;
}
