
// Hardplace705++Dlg.h : header file
//

#pragma once

#include "SerialPort.h"
#include <afxwin.h>

// CHardplace705Dlg dialog
class CHardplace705Dlg : public CDialogEx
{
	// Construction
public:
	CHardplace705Dlg(CWnd* pParent = nullptr);	// standard constructor

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HARDPLACE705_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Implementation
public:
	static DWORD BytesAvailable(CSerialPort& rSerial);

protected:
	void onSerialException(CSerialException& ex);
	void HardrockCmd(const char* lpszCmd, CString& rResponse, bool fUpdateText = true);
	DWORD BytesAvailable(void);
	void ClearSerial();
	void CloseSerial();
	void EnableHardrock(bool fEnable = true);
	void EnableHardplace(bool fEnable = true);
	void EnableIOTimer(void)
	{
		if (m_Serial.IsOpen())
		{
			SetWindowLongPtr(GetSafeHwnd(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
			SetTimer(IcomPoll, IcomPollInterval, IOTimerProc);
		}
	}
	void DisableIOTimer(void)
	{
		// TODO: Add your implementation code here.
		KillTimer(IcomPoll);
	}
	static void IOTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	void onIOTimer(void);

public:
	afx_msg void OnClose();
	afx_msg void OnOpenComPort();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSetInitialPwr();
	afx_msg void OnSetMaximumPwr();
	afx_msg void OnResetPwr();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClickedPtt(UINT nID);
	afx_msg void OnTune();

protected:
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnAppInit(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	CComboBox m_CommPort;
	CSerialPort m_Serial;
	CSliderCtrl m_PwrCtrl;

protected:
	HICON m_hIcon;
	const UINT_PTR HardrockStatus = 1;
	const UINT HardrockStatusInterval = 10000;
	const UINT_PTR IcomPoll = 2;
	const UINT IcomPollInterval = 500;
	int m_iPTT;

private:
	bool m_fPlacementSet;
	unsigned int m_cIOFailures;
	bool m_IOPingPong;
public:
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnEndSession(BOOL bEnding);
};

class CComFlushDialog :
	public CDialogEx
{
public:
	CComFlushDialog();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FLUSHCOMPORT };
#endif
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

protected:
	CComboBox m_CommPort;
public:
	afx_msg void OnBnClickedOk();
};
