#pragma once
#include "common.h"
#include "TicketsManage.h"
void CreateTicketPreviewWindow(YINYUE200_TICKET_PTR productrecord);
void Yinyue200_PrintTickets(HWND hwnd, YINYUE200_TICKET_PTR* ticket, size_t ticketcount);