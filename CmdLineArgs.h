#pragma once
#include <afxwin.h>
class CCmdLineArgs :
    public CCommandLineInfo
{
public:
    CCmdLineArgs() : m_Port(0) {

    }
public:
    virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
    {
        // TODO: Add your implementation code here.
        CString Args(pszParam);

        Args.MakeUpper();
        if (Args.Find(_T("PORT=COM")) == 0) {
            m_Port = static_cast<UINT>(_ttoi(Args.Mid(8)));
        }

    }
    UINT getPort(void) {
        return m_Port;
    }
protected:
    UINT m_Port;
};

