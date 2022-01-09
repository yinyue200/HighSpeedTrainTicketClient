//  HighSpeedTrainTicketClient
//  Copyright(C) 2022 ��Խ
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
static PWCHAR yinyue200_ticketsInfoConfigFilePath = L"ticketsinfo.tsv";
//��ȡ���������ļ�·��
PWCHAR yinyue200_GetConfigFilePath()
{
    return yinyue200_configFilePath;
}
//��ȡ�û���Ϣ����·��
PWCHAR yinyue200_GetUserConfigFilePath()
{
    return yinyue200_userConfigFilePath;
}
/// <summary>
/// ��ȡ�˿���Ϣ�ļ�·��
/// </summary>
/// <returns></returns>
PWCHAR yinyue200_GetPassengerInfoConfigFilePath()
{
    return yinyue200_passengerInfoConfigFilePath;
}
/// <summary>
/// ��ȡ��Ʊ��Ϣ�ļ�·��
/// </summary>
/// <returns></returns>
PWCHAR yinyue200_GetTicketInfoConfigFilePath()
{
    return yinyue200_ticketsInfoConfigFilePath;
}

