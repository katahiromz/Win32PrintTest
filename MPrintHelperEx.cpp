#include "MPrintHelperEx.hpp"

MPrintHelperEx::MPrintHelperEx()
{
}

MPrintHelperEx::~MPrintHelperEx()
{
}

BOOL MPrintHelperEx::DoPrintPages(HWND hwnd, HDC hDC, LPCWSTR pszDocName)
{
    if (pszDocName)
    {
        if (::StartPage(hDC) <= 0)
            return FALSE;
    }

    // physical paper size in millimeters
    const int nHorzSize = ::GetDeviceCaps(hDC, HORZSIZE);
    const int nVertSize = ::GetDeviceCaps(hDC, VERTSIZE);

    // DPI (dots per inch)
    const int nLogPixelX = ::GetDeviceCaps(hDC, LOGPIXELSX);
    const int nLogPixelY = ::GetDeviceCaps(hDC, LOGPIXELSY);

    // width and height of paper in pixels
    const int cxPaper = ::GetDeviceCaps(hDC, HORZRES);
    const int cyPaper = ::GetDeviceCaps(hDC, VERTRES);

    // margin and paper size in 1000th inches
    RECT rtMargin = m_psd.rtMargin;
    POINT ptPaperSize = m_psd.ptPaperSize;
    if (!(PageFlags() & PSD_INTHOUSANDTHSOFINCHES))
    {
        rtMargin.left = Inch1000FromMM100(rtMargin.left);
        rtMargin.top = Inch1000FromMM100(rtMargin.top);
        rtMargin.right = Inch1000FromMM100(rtMargin.right);
        rtMargin.bottom = Inch1000FromMM100(rtMargin.bottom);

        ptPaperSize.x = Inch1000FromMM100(ptPaperSize.x);
        ptPaperSize.y = Inch1000FromMM100(ptPaperSize.y);
    }

    // printable area in pixels
    RECT rcPrintArea;
    rcPrintArea.left = (nLogPixelX * rtMargin.left) / 1000;
    rcPrintArea.right = cxPaper - (nLogPixelX * rtMargin.right) / 1000;
    rcPrintArea.top = (nLogPixelY * rtMargin.top) / 1000;
    rcPrintArea.bottom = cyPaper - (nLogPixelY * rtMargin.bottom) / 1000;

    SIZE sizPrintArea;
    sizPrintArea.cx = rcPrintArea.right - rcPrintArea.left;
    sizPrintArea.cy = rcPrintArea.bottom - rcPrintArea.top;

    // draw rectangle
    SelectObject(hDC, GetStockObject(NULL_BRUSH));
    ::Rectangle(hDC, rcPrintArea.left, rcPrintArea.top, rcPrintArea.right, rcPrintArea.bottom);

    INT nPenWidth = 20;
    HPEN hPen = CreatePen(PS_SOLID, nPenWidth, RGB(0, 0, 0));  // black
    HBRUSH hbr = CreateSolidBrush(RGB(255, 255, 0));    // yellow
    HGDIOBJ hPenOld = SelectObject(hDC, hPen);
    HGDIOBJ hbrOld = SelectObject(hDC, hbr);
    {
        // 2-inch circle
        INT x = rcPrintArea.left + nPenWidth / 2;
        INT y = rcPrintArea.top + nPenWidth / 2;
        INT cx = nLogPixelX * 2;
        INT cy = nLogPixelY * 2;
        ::Ellipse(hDC, x, y, x + cx, y + cy);
    }
    SelectObject(hDC, hbrOld);
    SelectObject(hDC, hPenOld);
    DeleteObject(hbr);
    DeleteObject(hPen);

    INT nCharSize;
    if (sizPrintArea.cx < sizPrintArea.cy)
        nCharSize = sizPrintArea.cx;
    else
        nCharSize = sizPrintArea.cy;

    if (pszDocName)
    {
        LOGFONTW lf;
        ZeroMemory(&lf, sizeof(lf));
        lf.lfHeight = -nCharSize;
        lf.lfCharSet = DEFAULT_CHARSET;
        lstrcpyW(lf.lfFaceName, L"Tahoma");
        if (HFONT hFont = ::CreateFontIndirectW(&lf))
        {
            HGDIOBJ hFontOld = SelectObject(hDC, hFont);
            {
                // Print red "A"
                ::SetBkMode(hDC, TRANSPARENT);
                ::SetTextColor(hDC, RGB(255, 0, 0));
                ::DrawTextW(hDC, L"A", 1, &rcPrintArea,
                            DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);
            }
            SelectObject(hDC, hFontOld);
            DeleteObject(hFont);
        }

        ::EndPage(hDC);
    }

    return TRUE;
}
