//  HighSpeedTrainTicketClient
//  Copyright(C) 2022 殷越
//
//  This program is free software : you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program. If not, see <https://www.gnu.org/licenses/>.
#include "UserSetting.h"

static PWCHAR yinyue200_configFilePath = L"data.tsv";
static PWCHAR yinyue200_userConfigFilePath = L"userinfo.tsv";
static PWCHAR yinyue200_passengerInfoConfigFilePath = L"passengerinfo.tsv";
//获取数据文件路径
PWCHAR yinyue200_GetConfigFilePath()
{
    return yinyue200_configFilePath;
}
//获取用户信息配置路径
PWCHAR yinyue200_GetUserConfigFilePath()
{
    return yinyue200_userConfigFilePath;
}
PWCHAR yinyue200_GetPassengerInfoConfigFilePath()
{
    return yinyue200_passengerInfoConfigFilePath;
}
