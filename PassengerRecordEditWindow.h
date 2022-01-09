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
#include "PassengersManage.h"
#pragma once
typedef void (*PassengerInfoEditFinishCallback)(YINYUE200_PASSENGERINFO_PTR data, void* context);
void CreatePassengerRecordEditWindow(YINYUE200_PASSENGERINFO_PTR passenger, PassengerInfoEditFinishCallback callback, void* callbackcontext);