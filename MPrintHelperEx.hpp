#ifndef MPRINT_HELPER_EX_HPP_
#define MPRINT_HELPER_EX_HPP_

#include <windows.h>
#include "MPrintHelper.hpp"

class MBlakerPapersApp;

class MPrintHelperEx : public MPrintHelper
{
public:
    MPrintHelperEx();
    virtual ~MPrintHelperEx();
    virtual BOOL DoPrintPages(HWND hwnd, HDC hDC, LPCWSTR pszDocName);
};

#endif  // ndef MPRINT_HELPER_EX_HPP_
