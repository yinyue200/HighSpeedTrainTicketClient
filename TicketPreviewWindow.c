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

#include "common.h"
#include "DpiHelper.h"
#include "ControlsCommonOperation.h"
#include "TicketsManage.h"
#define ID_BUTTON_PRINT 1
#define ID_BUTTON_CANCEL 2

typedef struct Yinyue200_TicketPreviewWindowData
{
    YINYUE200_TICKET_PTR TicketInfo;
    HFONT lastfont;
} YINYUE200_TICKETPREVIEWWINDOWDATA;
LRESULT CALLBACK TicketPreviewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateTicketPreviewWindow(YINYUE200_TICKET_PTR record)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"yinyue200.HighSpeedTrainTicketClient.TicketPreviewWindow";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = TicketPreviewWindowProc;
    wc.hInstance = yinyue200_hInstance;
    wc.lpszClassName = CLASS_NAME;

    WORD result = RegisterClass(&wc);
    //printf("%d", result);

    YINYUE200_TICKETPREVIEWWINDOWDATA* windowdata = yinyue200_safemallocandclear(sizeof(YINYUE200_TICKETPREVIEWWINDOWDATA));
    windowdata->TicketInfo = record;

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"车票信息",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        yinyue200_hInstance,  // Instance handle
        windowdata        // Additional application data
    );

    if (hwnd == NULL)
    {
        int a = GetLastError();
        return 0;
    }

    ShowWindow(hwnd, yinyue200_nCmdShow);


    return 0;
}
#define ADDLABELANDEDIT(ctrl_id,displaylabel) HWND Hwnd_##ctrl_id##_Label =  Yinyue200_FastCreateLabelControl(hwnd,ID_LABEL_##ctrl_id,displaylabel);\
HWND hwnd_##ctrl_id##_Edit = Yinyue200_FastCreateEditControl(hwnd,ID_EDIT_##ctrl_id);
#define YINYUE200_SETCONTROLPOSANDFONTFORLABELANDEDIT(tag) do{\
YINYUE200_SETCONTROLPOSANDFONT(ID_LABEL_##tag, 10, lasty, 500, 20);\
lasty+=25;\
YINYUE200_SETCONTROLPOSANDFONT(ID_EDIT_##tag, 10, lasty, 500, 20);\
lasty+=25;\
}while (0)
//确定各控件位置
void LayoutControls_TicketPreviewWindow(HWND hwnd, UINT dpi, YINYUE200_TICKETPREVIEWWINDOWDATA* windata)
{
    Yinyue200_SetWindowSize(hwnd, 600, 500, dpi);
    if (windata->lastfont)
    {
        HFONT font = windata->lastfont;
        int lasty = 400;



        lasty += 10;
        YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_CANCEL, 10, lasty, 100, 50);
        YINYUE200_SETCONTROLPOSANDFONT(ID_BUTTON_PRINT, 10 + 100 + 10, lasty, 100, 50);
    }

}
#define SETNULLORPRODUCTINFOMEMBERDATA(chwnd,member) SendMessage(GetDlgItem(hwnd,chwnd), WM_SETTEXT, 0, record==NULL?L"":record->##member);
#define SAVEPASSENGERINFOMEMBERDATA(memberid,member) do\
{\
    free(ptr->##member);\
    ptr->##member=CreateWstrForWindowText(GetDlgItem(hwnd,memberid));\
}while(0);
void Yinyue200_DrawTicket(HDC img,YINYUE200_TICKET_PTR ticket, int x, int y,UINT dpi)
{
    FILETIME trainutctime = Yinyue200_ConvertToFileTimeFromUINT64(ticket->TrainTime);
    FILETIME localtraintime;
    Yinyue200_FileTimeToLocalFileTime(&trainutctime, &localtraintime);
    SYSTEMTIME systime;
    FileTimeToSystemTime(&localtraintime, &systime);
    double timezone = Yinyue200_GetTimeZone() / 60.0;

    RECT rc;
    rc.top = y;
    rc.left = x;
    rc.right = x + YINYUE200_LOGICTOPHYBYDPI(690, dpi);
    rc.bottom = y + YINYUE200_LOGICTOPHYBYDPI(464, dpi);

    {
        HBITMAP hBitmap = (HBITMAP)LoadImage(yinyue200_hInstance, L"ticketbackground.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (hBitmap)
        {
            BITMAP bitmap;
            HDC hdcMem = CreateCompatibleDC(img);
            HGDIOBJ oldBitmap = SelectObject(hdcMem, hBitmap);

            GetObject(hBitmap, sizeof(bitmap), &bitmap);
            SetStretchBltMode(img, HALFTONE);
            StretchBlt(img, x, y, YINYUE200_LOGICTOPHYBYDPI(696, dpi), YINYUE200_LOGICTOPHYBYDPI(464, dpi), hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

            SelectObject(hdcMem, oldBitmap);
            DeleteDC(hdcMem);

            DeleteBitmap(hBitmap);
        }
    }

    SetBkMode(img, TRANSPARENT);

    rc.top += YINYUE200_LOGICTOPHYBYDPI(25, dpi);
    rc.left += YINYUE200_LOGICTOPHYBYDPI(25, dpi);

    double price = ticket->Price;
    price /= 100.0;

    //SelectObject(img, GetStockObject(NULL_PEN));
    //Rectangle(img, x, y, x + YINYUE200_LOGICTOPHYBYDPI(696, dpi), y + YINYUE200_LOGICTOPHYBYDPI(464, dpi));

    HFONT font = Yinyue200_CreateFont(25, dpi);
    HFONT smallfont = Yinyue200_CreateFont(20, dpi);
    //HFONT moresmallfont = Yinyue200_CreateFont(15, dpi);

    HFONT oldfont = SelectObject(img, font);

    PWSTR buffer = CreateWSTR(1000);

    swprintf(buffer, 1000, L"某某铁路联运公司车票（票样）\r\n\r\n        %s\r\n     %s → %s",
        ticket->TrainName, ticket->StartStation, ticket->EndStation);

    DrawText(img, buffer, -1, &rc, DT_WORDBREAK);

    swprintf(buffer, 1000, L"     %.2f元      %d号      %s\r\n 限乘当日当次车", price, ticket->SeatNumber, Yinyue200_GetSeatLevelName(ticket->SeatLevel));

    int d = YINYUE200_LOGICTOPHYBYDPI(40 * 3 + 35, dpi);
    rc.top += d;
    DrawText(img, buffer, -1, &rc, DT_WORDBREAK);
    //rc.top -= d;
    SelectObject(img, smallfont);

    swprintf(buffer, 1000, timezone > 0 ? L"   %d 年 %02d 月 %02d 日 %02d:%02d （UTC+%.0f）开" : L"   %d 年 %02d 月 %02d 日 %02d:%02d （UTC%.0f）开",
        systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, timezone);

    rc.top += YINYUE200_LOGICTOPHYBYDPI(40 * 2, dpi);

    DrawText(img, buffer, -1, &rc, DT_WORDBREAK);

    rc.top += YINYUE200_LOGICTOPHYBYDPI(40 * 2, dpi);

    swprintf(buffer, 1000, L" %s\r\n（%s : %s）\r\n%llu;%llu",
        ticket->PassengerName, ticket->PassengerIDType, ticket->PassengerID, ticket->ID.Item1, ticket->ID.Item2);

    DrawText(img, buffer, -1, &rc, DT_WORDBREAK);

    
    DeleteObject(font);
    DeleteObject(smallfont);

    SelectObject(img, oldfont);

    free(buffer);
}
void Yinyue200_PrintTickets(HWND hwnd, YINYUE200_TICKET_PTR* ticket, size_t ticketcount)
{
    PRINTDLG printInfo = { 0 };
    printInfo.lStructSize = sizeof(printInfo);
    printInfo.Flags = PD_RETURNDC | PD_ALLPAGES;
    //PD_RETURNDEFAULT 意味着直接返回当前系统默认的打印机设置，若没有这个标识，则会弹出对话框让用户自己选择
    //PD_RETURNDC 意味着返回的是dc而不是ic（information context）
    //PD_ALLPAGES 指定“全部”单选钮在初始时被选中(缺省标志)
    //对于错误的时候，若需要知道更多的错误消息，请执行CommDlgError来查看返回值

    //PrintDlg目的是获取当前系统设置的默认打印机相关信息，供后面使用
    if (!PrintDlg(&printInfo))
    {
        printInfo.Flags = 0;	//清除标志，然后执行将会弹出对话框让用户选择打印机
        if (!PrintDlg(&printInfo))
        {
            MessageBox(hwnd, L"没有选择打印机。", L"错误", 0);
            goto endprint;
        }
    }

    //获取打印的时候的dc，然后往这个dc上绘图就是打印出来的样子了
    HDC hPrintDC = printInfo.hDC;

    //锁定全局对象，获取对象指针。 devmode是有关设备初始化和打印机环境的信息
    DEVMODE* devMode = (DEVMODE*)GlobalLock(printInfo.hDevMode);
    if (devMode == 0)
    {
        MessageBox(hwnd, L"获取打印机设置时发生了错误。", L"错误", 0);
        goto endprint;
    }

    devMode->dmPaperSize = DMPAPER_A4;				//打印机纸张设置为A4。
    devMode->dmOrientation = DMORIENT_LANDSCAPE;		//打印方向设置成横向打印
    //对打印方向的设置，会影响hPrintDC的大小，假设宽度为1024，高度为300
    //则在横向打印的时候dc大小会是宽1024 * 高300
    //而纵向打印的时候dc大小会是宽300 * 高1024

    int printQuality = devMode->dmPrintQuality;			//获取打印机的打印质量
    //devMode->dmPrintQuality = DMRES_MEDIUM;
    //设置打印质量的，因为像素被打印到纸上的时候是有做转换的
    //单位是dpi，意为像素每英寸(dots per inch)。就是一英寸的纸张上
    //打印多少个像素点，意味着这个质量越高，打印结果越精细，越低，越粗糙
    //设置的质量可以是具体数值，也可以是宏DMRES_MEDIUM
    //一般我们选择300，或者600，DMRES_MEDIUM = 600dpi

    //应用我们修改过后的设置.
    ResetDC(hPrintDC, devMode);

    //解锁全局对象，对应GlobalLock
    GlobalUnlock(printInfo.hDevMode);

    //设置绘图模式，以保证绘图可以不失真的绘制上去，因为StretchDIBits函数要求设置这个才能够不是失真的绘图
    //当你用StretchDIBits绘图往窗口显示的时候就会发现，24位图，若没有这个设置，是会失真的
    SetStretchBltMode(hPrintDC, HALFTONE);

    //获取dc的大小，实际上还有一种HORZRES和VERTRES就是宽度和高度，但是我查得到的结果说计算下来准确的
    //HORZSIZE 是Horizontal size in millimeters，页面宽度（水平），单位毫米mm
    //VERTSIZE 是Vertical size in millimeters，页面高度（垂直），单位毫米mm
    //LOGPIXELSX 是Logical pixels/inch in X，x方向的逻辑像素每英寸.单位 pix / inch，像素每英寸
    //LOGPIXELSY 是Logical pixels/inch in Y，y方向的逻辑像素每英寸.单位 pix / inch，像素每英寸
    //不用管逻辑是个什么东西，不理会他，知道单位是pix / inch就行了
    //1 inch = 2.54 cm，所以这里是 mm / 25.4 * pix / inch，得到的结果就是dc大小像素数为单位
    int dc_page_width = GetDeviceCaps(hPrintDC, HORZSIZE) / 25.4 * GetDeviceCaps(hPrintDC, LOGPIXELSX);
    int dc_page_height = GetDeviceCaps(hPrintDC, VERTSIZE) / 25.4 * GetDeviceCaps(hPrintDC, LOGPIXELSY);

    //好了，可以开始打印了
    DOCINFO doc_info = { sizeof(DOCINFO), L"车票" };
    //cbSize
    //结构体的字节大小
    //lpszDocName
    //指明文档名的字符串指针，该字符串以null为尾。
    //lpszOutput
    //指明输出文件的名称的字符串指针，该字符串以null为尾。如果指针为null，那么输出将会发送至某个设备，该设备将由传递至 StartDoc 函数的‘设备上下文句柄’HDC来指明。
    //lpszDatatype
    //指针指向代表某种数据类型的字符串，而数据用于记录打印工作，该字符串以null为尾。合法值可通过调用函数 EnumPrintProcessorDatatypes 可得到，亦可为 NULL。
    //fwType
    //指明打印工作的其它信息。可为0或者以下值之一：
    //DI_APPBANDING
    //DI_ROPS_READ_DESTINATION

    //开始一个档案，打印的时候是按照档案来区分的，返回作业编号（大于0的为正常）
    int doc_idd = StartDoc(hPrintDC, &doc_info);
    if (doc_idd <= 0)
    {
        MessageBox(hwnd, L"打印错误", L"错误", 0);
    }
    else
    {
        for (size_t i = 0; i < ticketcount; i++)
        {
            //开始一个打印页面，大于等于0为正确
            if (StartPage(hPrintDC) < 0)
            {
                MessageBox(hwnd, L"打印错误", L"错误", 0);
                AbortDoc(hPrintDC);	//结束doc打印
            }
            else
            {
                Yinyue200_DrawTicket(hPrintDC, ticket[i], 696, 464, printQuality);

                EndPage(hPrintDC);
            }
        }


        EndDoc(hPrintDC);
        MessageBox(hwnd, L"打印完成", L"提示", 0);

    }

endprint:;
}
//乘客信息编辑窗口消息处理函数
LRESULT CALLBACK TicketPreviewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        AddWindowCount();

        {
            CREATESTRUCT* cs = lParam;
            SetProp(hwnd, YINYUE200_WINDOW_DATA, cs->lpCreateParams);
        }

        YINYUE200_TICKETPREVIEWWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        windata->lastfont = yinyue200_CreateDefaultFont(hwnd);



        HWND hwnd_okbutton = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_PRINT, L"打印");
        HWND hwnd_cancelbutton = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_CANCEL, L"关闭");

        LayoutControls_TicketPreviewWindow(hwnd, yinyue200_GetDpiForWindow(hwnd), windata);
    }
    break;
    case WM_DESTROY:
    {
        YINYUE200_TICKETPREVIEWWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        yinyue200_DeleteFont(windowdata->lastfont);
        if (windowdata != NULL)
            free(windowdata);
        RemoveProp(hwnd, YINYUE200_WINDOW_DATA);
        DecreaseWindowCount();
        CheckIfNoWindowAndQuit();
    }
    break;
    case WM_DPICHANGED:
    {
        YINYUE200_TICKETPREVIEWWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if (windowdata && windowdata->lastfont)
        {
            yinyue200_DeleteFont(windowdata->lastfont);
            windowdata->lastfont = yinyue200_CreateDefaultFont(hwnd);
            LayoutControls_RoutePointEditWindow(hwnd, HIWORD(wParam), windowdata);
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc = { 50,50,500,500 };
        //GetClientRect(hwnd, &rc);

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        UINT dpi = yinyue200_GetDpiForWindow(hwnd);

        YINYUE200_TICKETPREVIEWWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if (windata)
        {
            Yinyue200_DrawTicket(hdc, windata->TicketInfo, YINYUE200_LOGICTOPHYBYDPI(50, dpi), YINYUE200_LOGICTOPHYBYDPI(50, dpi), dpi / 2);
        }
        


        EndPaint(hwnd, &ps);
    }
    break;
    case WM_COMMAND:
    {
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
        {
            switch (LOWORD(wParam))
            {
            case ID_BUTTON_CANCEL:
                YINYUE200_TICKETPREVIEWWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);



                SendMessage(hwnd, WM_CLOSE, NULL, NULL);
                break;
            case ID_BUTTON_PRINT:
            {
                YINYUE200_TICKETPREVIEWWINDOWDATA* windowdata = GetProp(hwnd, YINYUE200_WINDOW_DATA);

                Yinyue200_PrintTickets(hwnd, &windowdata->TicketInfo, 1);
            }
            break;
            }
        }
        break;
        }
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}