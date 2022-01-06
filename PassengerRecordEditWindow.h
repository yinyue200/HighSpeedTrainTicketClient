#include "PassengersManage.h"
#pragma once
typedef void (*PassengerInfoEditFinishCallback)(YINYUE200_PASSENGERINFO_PTR data, void* context);
void CreatePassengerRecordEditWindow(YINYUE200_PASSENGERINFO_PTR productrecord, PassengerInfoEditFinishCallback callback, void* callbackcontext);