#pragma once
#include "ProductRecord.h"
typedef void (*RoutePointEditFinishCallback)(YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR data);
void CreateRoutePointEditWindow(YINYUE200_TRAINPLANRECORD_ROUTEPOINT_PTR routepoint, bool enablesave, RoutePointEditFinishCallback callback);