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
#pragma once
#include "common.h"
//获取数据文件路径
PWCHAR yinyue200_GetConfigFilePath();
//获取用户信息配置路径
PWCHAR yinyue200_GetUserConfigFilePath();
/// <summary>
/// 获取乘客信息文件路径
/// </summary>
/// <returns></returns>
PWCHAR yinyue200_GetPassengerInfoConfigFilePath();
/// <summary>
/// 获取车票信息文件路径
/// </summary>
/// <returns></returns>
PWCHAR yinyue200_GetTicketInfoConfigFilePath();