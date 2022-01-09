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
        L"��Ʊ��Ϣ",    // Window text
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
//ȷ�����ؼ�λ��
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

    swprintf(buffer, 1000, L"ĳĳ��·���˹�˾��Ʊ��Ʊ����\r\n\r\n        %s\r\n     %s �� %s",
        ticket->TrainName, ticket->StartStation, ticket->EndStation);

    DrawText(img, buffer, -1, &rc, DT_WORDBREAK);

    swprintf(buffer, 1000, L"     %.2fԪ      %d��      %s\r\n �޳˵��յ��γ�", price, ticket->SeatNumber, Yinyue200_GetSeatLevelName(ticket->SeatLevel));

    int d = YINYUE200_LOGICTOPHYBYDPI(40 * 3 + 35, dpi);
    rc.top += d;
    DrawText(img, buffer, -1, &rc, DT_WORDBREAK);
    //rc.top -= d;
    SelectObject(img, smallfont);

    swprintf(buffer, 1000, timezone > 0 ? L"   %d �� %02d �� %02d �� %02d:%02d ��UTC+%.0f����" : L"   %d �� %02d �� %02d �� %02d:%02d ��UTC%.0f����",
        systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, timezone);

    rc.top += YINYUE200_LOGICTOPHYBYDPI(40 * 2, dpi);

    DrawText(img, buffer, -1, &rc, DT_WORDBREAK);

    rc.top += YINYUE200_LOGICTOPHYBYDPI(40 * 2, dpi);

    swprintf(buffer, 1000, L" %s\r\n��%s : %s��\r\n%llu;%llu",
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
    //PD_RETURNDEFAULT ��ζ��ֱ�ӷ��ص�ǰϵͳĬ�ϵĴ�ӡ�����ã���û�������ʶ����ᵯ���Ի������û��Լ�ѡ��
    //PD_RETURNDC ��ζ�ŷ��ص���dc������ic��information context��
    //PD_ALLPAGES ָ����ȫ������ѡť�ڳ�ʼʱ��ѡ��(ȱʡ��־)
    //���ڴ����ʱ������Ҫ֪������Ĵ�����Ϣ����ִ��CommDlgError���鿴����ֵ

    //PrintDlgĿ���ǻ�ȡ��ǰϵͳ���õ�Ĭ�ϴ�ӡ�������Ϣ��������ʹ��
    if (!PrintDlg(&printInfo))
    {
        printInfo.Flags = 0;	//�����־��Ȼ��ִ�н��ᵯ���Ի������û�ѡ���ӡ��
        if (!PrintDlg(&printInfo))
        {
            MessageBox(hwnd, L"û��ѡ���ӡ����", L"����", 0);
            goto endprint;
        }
    }

    //��ȡ��ӡ��ʱ���dc��Ȼ�������dc�ϻ�ͼ���Ǵ�ӡ������������
    HDC hPrintDC = printInfo.hDC;

    //����ȫ�ֶ��󣬻�ȡ����ָ�롣 devmode���й��豸��ʼ���ʹ�ӡ����������Ϣ
    DEVMODE* devMode = (DEVMODE*)GlobalLock(printInfo.hDevMode);
    if (devMode == 0)
    {
        MessageBox(hwnd, L"��ȡ��ӡ������ʱ�����˴���", L"����", 0);
        goto endprint;
    }

    devMode->dmPaperSize = DMPAPER_A4;				//��ӡ��ֽ������ΪA4��
    devMode->dmOrientation = DMORIENT_LANDSCAPE;		//��ӡ�������óɺ����ӡ
    //�Դ�ӡ��������ã���Ӱ��hPrintDC�Ĵ�С��������Ϊ1024���߶�Ϊ300
    //���ں����ӡ��ʱ��dc��С���ǿ�1024 * ��300
    //�������ӡ��ʱ��dc��С���ǿ�300 * ��1024

    int printQuality = devMode->dmPrintQuality;			//��ȡ��ӡ���Ĵ�ӡ����
    //devMode->dmPrintQuality = DMRES_MEDIUM;
    //���ô�ӡ�����ģ���Ϊ���ر���ӡ��ֽ�ϵ�ʱ��������ת����
    //��λ��dpi����Ϊ����ÿӢ��(dots per inch)������һӢ���ֽ����
    //��ӡ���ٸ����ص㣬��ζ���������Խ�ߣ���ӡ���Խ��ϸ��Խ�ͣ�Խ�ֲ�
    //���õ����������Ǿ�����ֵ��Ҳ�����Ǻ�DMRES_MEDIUM
    //һ������ѡ��300������600��DMRES_MEDIUM = 600dpi

    //Ӧ�������޸Ĺ��������.
    ResetDC(hPrintDC, devMode);

    //����ȫ�ֶ��󣬶�ӦGlobalLock
    GlobalUnlock(printInfo.hDevMode);

    //���û�ͼģʽ���Ա�֤��ͼ���Բ�ʧ��Ļ�����ȥ����ΪStretchDIBits����Ҫ������������ܹ�����ʧ��Ļ�ͼ
    //������StretchDIBits��ͼ��������ʾ��ʱ��ͻᷢ�֣�24λͼ����û��������ã��ǻ�ʧ���
    SetStretchBltMode(hPrintDC, HALFTONE);

    //��ȡdc�Ĵ�С��ʵ���ϻ���һ��HORZRES��VERTRES���ǿ�Ⱥ͸߶ȣ������Ҳ�õ��Ľ��˵��������׼ȷ��
    //HORZSIZE ��Horizontal size in millimeters��ҳ���ȣ�ˮƽ������λ����mm
    //VERTSIZE ��Vertical size in millimeters��ҳ��߶ȣ���ֱ������λ����mm
    //LOGPIXELSX ��Logical pixels/inch in X��x������߼�����ÿӢ��.��λ pix / inch������ÿӢ��
    //LOGPIXELSY ��Logical pixels/inch in Y��y������߼�����ÿӢ��.��λ pix / inch������ÿӢ��
    //���ù��߼��Ǹ�ʲô���������������֪����λ��pix / inch������
    //1 inch = 2.54 cm������������ mm / 25.4 * pix / inch���õ��Ľ������dc��С������Ϊ��λ
    int dc_page_width = GetDeviceCaps(hPrintDC, HORZSIZE) / 25.4 * GetDeviceCaps(hPrintDC, LOGPIXELSX);
    int dc_page_height = GetDeviceCaps(hPrintDC, VERTSIZE) / 25.4 * GetDeviceCaps(hPrintDC, LOGPIXELSY);

    //���ˣ����Կ�ʼ��ӡ��
    DOCINFO doc_info = { sizeof(DOCINFO), L"��Ʊ" };
    //cbSize
    //�ṹ����ֽڴ�С
    //lpszDocName
    //ָ���ĵ������ַ���ָ�룬���ַ�����nullΪβ��
    //lpszOutput
    //ָ������ļ������Ƶ��ַ���ָ�룬���ַ�����nullΪβ�����ָ��Ϊnull����ô������ᷢ����ĳ���豸�����豸���ɴ����� StartDoc �����ġ��豸�����ľ����HDC��ָ����
    //lpszDatatype
    //ָ��ָ�����ĳ���������͵��ַ��������������ڼ�¼��ӡ���������ַ�����nullΪβ���Ϸ�ֵ��ͨ�����ú��� EnumPrintProcessorDatatypes �ɵõ������Ϊ NULL��
    //fwType
    //ָ����ӡ������������Ϣ����Ϊ0��������ֵ֮һ��
    //DI_APPBANDING
    //DI_ROPS_READ_DESTINATION

    //��ʼһ����������ӡ��ʱ���ǰ��յ��������ֵģ�������ҵ��ţ�����0��Ϊ������
    int doc_idd = StartDoc(hPrintDC, &doc_info);
    if (doc_idd <= 0)
    {
        MessageBox(hwnd, L"��ӡ����", L"����", 0);
    }
    else
    {
        for (size_t i = 0; i < ticketcount; i++)
        {
            //��ʼһ����ӡҳ�棬���ڵ���0Ϊ��ȷ
            if (StartPage(hPrintDC) < 0)
            {
                MessageBox(hwnd, L"��ӡ����", L"����", 0);
                AbortDoc(hPrintDC);	//����doc��ӡ
            }
            else
            {
                Yinyue200_DrawTicket(hPrintDC, ticket[i], 696, 464, printQuality);

                EndPage(hPrintDC);
            }
        }


        EndDoc(hPrintDC);
        MessageBox(hwnd, L"��ӡ���", L"��ʾ", 0);

    }

endprint:;
}
//�˿���Ϣ�༭������Ϣ������
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



        HWND hwnd_okbutton = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_PRINT, L"��ӡ");
        HWND hwnd_cancelbutton = Yinyue200_FastCreateButtonControl(hwnd, ID_BUTTON_CANCEL, L"�ر�");

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