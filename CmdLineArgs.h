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
        const CString szToken(_T("PORT=COM"));

        Args.MakeUpper();
        if (Args.Find(szToken) == 0) {
            m_Port = static_cast<UINT>(_ttoi(Args.Mid(szToken.GetLength())));
        }
    }
public:
    UINT getPort(void) const {
        return m_Port;
    }
protected:
    UINT m_Port;
};

