//  SimpleStoreErp
//	Copyright(C) 2021 “Û‘Ω
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
