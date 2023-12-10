
// Hardplace705++Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Hardplace705++.h"
#include "Hardplace705++Dlg.h"
#include "afxdialogex.h"

#pragma comment (lib, "OneCore.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

constexpr auto IDM_REBOOT = 0x0020;
constexpr auto IDM_ONTOP = 0x0030;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CHardplace705Dlg dialog



CHardplace705Dlg::CHardplace705Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HARDPLACE705_DIALOG, pParent),
	m_iPTT(-1), m_fPlacementSet(false), m_cIOFailures(0), m_IOPingPong(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHardplace705Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMMPORT, m_CommPort);
	DDX_Control(pDX, IDC_SLIDER, m_PwrCtrl);
	DDX_Radio(pDX, IDC_PTT, m_iPTT);
}

BEGIN_MESSAGE_MAP(CHardplace705Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOPEN, &CHardplace705Dlg::OnOpenComPort)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SET_INITIAL_PWR, &CHardplace705Dlg::OnSetInitialPwr)
	ON_BN_CLICKED(IDC_SET_MAXIMUM_PWR, &CHardplace705Dlg::OnSetMaximumPwr)
	ON_BN_CLICKED(IDC_RESET_PWR, &CHardplace705Dlg::OnResetPwr)
	ON_WM_VSCROLL()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_PTT, IDC_QRP, &CHardplace705Dlg::OnClickedPtt)
	ON_BN_CLICKED(IDC_TUNE, &CHardplace705Dlg::OnTune)
	ON_MESSAGE(WM_APP, &CHardplace705Dlg::OnAppInit)
END_MESSAGE_MAP()


// CHardplace705Dlg message handlers

BOOL CHardplace705Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	//  In WM_SYSCOMMAND messages, the four low - order bits of the nID parameter are used internally by Windows.
	//	When an application tests the value of nID,
	//	it must combine the value 0xFFF0 with the nID value by using the bitwise - AND operator to obtain the correct result.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	ASSERT((IDM_REBOOT & 0xFFF0) == IDM_REBOOT);
	ASSERT(IDM_REBOOT < 0xF000);
	ASSERT((IDM_ONTOP & 0xFFF0) == IDM_ONTOP);
	ASSERT(IDM_ONTOP < 0xF000);


	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strOnTop;
		bNameValid = strOnTop.LoadString(IDS_ONTOP);
		ASSERT(bNameValid);
		CString strReboot;
		bNameValid = strReboot.LoadString(IDS_REBOOT);
		ASSERT(bNameValid);

		if (!strReboot.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ONTOP, strOnTop);
			pSysMenu->AppendMenu(MF_STRING, IDM_REBOOT, strReboot);
		}

		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_PwrCtrl.SetRange(0, 255);
	m_PwrCtrl.SetPageSize(m_PwrCtrl.GetRangeMax() / 10);
	m_PwrCtrl.SetPos(m_PwrCtrl.GetRangeMax());
	m_PwrCtrl.SetTicFreq(25);

	ULONG uPortsFound(0);
	if (GetCommPorts(0, 0, &uPortsFound) == ERROR_MORE_DATA)
	{
#pragma warning(push)
#pragma warning(disable : 6001 )
		CAutoPtr<ULONG> pPortBuf(new ULONG[uPortsFound]);
#pragma warning(pop)

		if (GetCommPorts(pPortBuf, uPortsFound, &uPortsFound) == ERROR_SUCCESS)
		{
			for (unsigned nIndex(0); nIndex < uPortsFound; nIndex++)
			{
				CString comPort;

				comPort.Format(_T("COM%u"), pPortBuf[nIndex]);
				if (m_CommPort.FindStringExact(-1, comPort) == CB_ERR)
				{
					int iIndex(m_CommPort.AddString(comPort));
					if (iIndex >= 0)
					{
						m_CommPort.SetItemData(iIndex, pPortBuf[nIndex]);
					}
				}
			}
			UINT uDefaultPort(theApp.GetProfileInt(_T("Settings"), _T("Port"), 0));
			CString findString;

			findString.Format(_T("COM%u"), theApp.GetProfileInt(_T("Settings"), _T("Port"), 0));
			int nIndex(m_CommPort.FindStringExact(-1, findString));

			if (nIndex != CB_ERR)
			{
				m_CommPort.SetCurSel(nIndex);
				PostMessage(WM_APP);
			}
			else
			{
				m_CommPort.SelectString(-1, _T("COM"));
			}
		}
		else
		{
			AfxMessageBox(IDS_NOTFOUND);;
		}
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHardplace705Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if ((nID & 0xFFF0) == IDM_REBOOT)
	{
		if (m_Serial.IsOpen())
		{
			if (AfxMessageBox(IDS_REBOOTQUERY, MB_YESNO) == IDYES)
			{
				CString sResp;
				CWaitCursor wait;

				DisableIOTimer();
				HardrockCmd("HPRE;", sResp);
				CloseSerial();
			}
		}
		else
		{
			AfxMessageBox(IDS_OPENPORT);
		}
	}
	else if ((nID & 0xFFF0) == IDM_ONTOP)
	{
		SetWindowPos(((GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) & WS_EX_TOPMOST) != 0) ? &wndNoTopMost : &wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHardplace705Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHardplace705Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CHardplace705Dlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	if (m_Serial.IsOpen())
	{
		CloseSerial();
	}
	CDialogEx::OnClose();
}


void CHardplace705Dlg::OnOpenComPort()
{
	// TODO: Add your control notification handler code here
	try
	{
		if (!m_Serial.IsOpen())
		{
			m_Serial.Open(int(m_CommPort.GetItemData(m_CommPort.GetCurSel())), DWORD(115200));
			if (m_Serial.IsOpen())
			{
				COMMTIMEOUTS timeOuts;
				CString sResp;

				m_Serial.GetTimeouts(timeOuts);
				timeOuts.ReadIntervalTimeout = 250;
				timeOuts.ReadTotalTimeoutConstant = 250;
				timeOuts.WriteTotalTimeoutConstant = 10000;
				m_Serial.SetTimeouts(timeOuts);

				HardrockCmd("HPVE;", sResp);
				if (sResp.Find(_T("Hardplace")) >= 0)
				{
					SetDlgItemText(IDOPEN, _T("Close"));
					EnableHardplace();

					HardrockCmd("HRMD;", sResp, false);
					if (sResp.GetLength() >= 5)
					{
						m_iPTT = sResp.GetAt(4) - '0';
						UpdateData(FALSE);
						EnableHardrock();
					}
					theApp.WriteProfileInt(
						_T("Settings"), _T("Port"), int(m_CommPort.GetItemData(m_CommPort.GetCurSel())));
					EnableIOTimer();
				}
				else
				{
					CloseSerial();
					AfxMessageBox(IDS_HPNOTFOUND);
				}
			}
			else
			{
				AfxMessageBox(IDS_PORTBUSY);
			}
		}
		else
		{
			CloseSerial();
		}
	}
	catch (CSerialException ex) {
		onSerialException(ex);
	}

}

void CHardplace705Dlg::HardrockCmd(const char* lpszCmd, CString& rResponse, bool fUpdateText)
{
	rResponse.Empty();
	try
	{
		char achRsp[80];

		ClearSerial();
		m_Serial.Write(lpszCmd, DWORD(strlen(lpszCmd)));

		DWORD dwRead(m_Serial.Read(achRsp, sizeof achRsp));
		if (dwRead)
		{
			achRsp[dwRead] = '\0';
			rResponse = CA2T(achRsp);
			if (fUpdateText)
			{
				SetDlgItemText(IDC_STATUS, CA2T(achRsp));
				SetTimer(HardrockStatus, HardrockStatusInterval, NULL);
			}
		}
	}
	catch (CSerialException ex) {
		onSerialException(ex);
	}
}

void CHardplace705Dlg::onSerialException(CSerialException& ex)
{
	TCHAR achErrorStr[80];

	ex.GetErrorMessage2(achErrorStr, sizeof achErrorStr / sizeof(TCHAR));
	try
	{
		if (m_Serial.IsOpen())
		{
			DisableIOTimer();
			CloseSerial();
		}
	}
	catch (CSerialException ex)
	{
	}
	AfxMessageBox(achErrorStr);
}

void CHardplace705Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == HardrockStatus)
	{
		SetDlgItemText(IDC_STATUS, _T(""));
		KillTimer(nIDEvent);
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CHardplace705Dlg::OnSetInitialPwr()
{
	// TODO: Add your control notification handler code here
	CString sResp;
	HardrockCmd("HPIP;", sResp);
}


void CHardplace705Dlg::OnSetMaximumPwr()
{
	// TODO: Add your control notification handler code here
	CString sResp;

	DisableIOTimer();
	HardrockCmd("HPMP;", sResp);
	if (!sResp.IsEmpty())
	{
		AfxMessageBox(IDS_PWRMAX);
	}
	HardrockCmd(";", sResp);
	EnableIOTimer();
}


void CHardplace705Dlg::OnResetPwr()
{
	// TODO: Add your control notification handler code here
	CString sResp;
	HardrockCmd("HPDP;", sResp);
}

DWORD CHardplace705Dlg::BytesAvailable(void)
{
	DWORD dwAvailable(0);
	DWORD dwErrors;
	COMSTAT Status;

	if (ClearCommError(HANDLE(m_Serial), &dwErrors, &Status))
	{
		dwAvailable = Status.cbInQue;
	}
	else
	{
		CSerialPort::ThrowSerialException();
	}
	return dwAvailable;
}

void CHardplace705Dlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if (pScrollBar == (CScrollBar*)(&m_PwrCtrl))
	{
		switch (nSBCode)
		{
		default:
			DisableIOTimer();
			break;

		case TB_ENDTRACK:
		case TB_THUMBPOSITION:
			try
			{
				if (m_Serial.IsOpen())
				{
					uint8_t achRsp[6];
					int iLevel(abs(m_PwrCtrl.GetPos() - m_PwrCtrl.GetRangeMax()));
					uint8_t  WriteRFPowerReq[] = { 0xFE, 0xFE, 0xA4, 0xE5, 0x14, 0x0A, 0x00, 0x00, 0xFD };
					unsigned uLevelLow(iLevel % 100);
					unsigned uLevelHigh(iLevel / 100);

					WriteRFPowerReq[6] = ((uLevelHigh / 10) << 4) + uLevelHigh % 10;
					WriteRFPowerReq[7] = ((uLevelLow / 10) << 4) + uLevelLow % 10;

					ClearSerial();
					m_Serial.Write(WriteRFPowerReq, DWORD(sizeof WriteRFPowerReq));
					m_Serial.Read(achRsp, sizeof achRsp);
				}
				else
				{
					MessageBeep(MB_ICONERROR);
					m_PwrCtrl.SetPos(m_PwrCtrl.GetRangeMax());
				}
			}
			catch (CSerialException ex) {
				onSerialException(ex);
			}
			EnableIOTimer();
			break;
		}
	}
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CHardplace705Dlg::ClearSerial()
{
	// TODO: Add your implementation code here.
	for (DWORD dwOctets(BytesAvailable()); (m_Serial.IsOpen() && dwOctets > 0); dwOctets = BytesAvailable())
	{
#pragma warning(push)
#pragma warning(disable : 6001 )
		CAutoPtr<uint8_t> Buf(new uint8_t[dwOctets]);
#pragma warning(pop)

		try
		{
			m_Serial.Read(Buf, dwOctets);
		}
		catch (CSerialException ex)
		{
			onSerialException(ex);
		}
	}
}


void CHardplace705Dlg::OnDestroy()
{

	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	WINDOWPLACEMENT wp = { 0 };
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(&wp);
	theApp.WriteProfileBinary(_T("Settings"), _T("Window"), (LPBYTE)&wp, wp.length);
	theApp.WriteProfileInt(_T("Settings"), _T("OnTop"), (GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) & WS_EX_TOPMOST) != 0 ? 1 : 0);
}


void CHardplace705Dlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	if (bShow && !IsWindowVisible() && !m_fPlacementSet)
	{
		WINDOWPLACEMENT* lwp(0);
		UINT nl;

		m_fPlacementSet = true;

		if (theApp.GetProfileBinary(_T("Settings"), _T("Window"), (LPBYTE*)&lwp, &nl))
		{
			SetWindowPlacement(lwp);
			delete[] lwp;
		}
		if (theApp.GetProfileInt(_T("Settings"), _T("OnTop"), 0))
		{
			SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}
}

void CHardplace705Dlg::CloseSerial()
{
	// TODO: Add your implementation code here.
	DisableIOTimer();
	try
	{
		if (m_Serial.IsOpen())
		{
			CWaitCursor wait;
			m_Serial.Close();
		}
	}
	catch (CSerialException ex)
	{

	}
	EnableHardplace(FALSE);
	EnableHardrock(FALSE);
}


void CHardplace705Dlg::OnClickedPtt(UINT nID)
{
	// TODO: Add your control notification handler code here
	UpdateData();
	CStringA sCmd;

	sCmd.Format("HRMD%i;", m_iPTT);
	try
	{
		if (m_Serial.IsOpen())
		{
			m_Serial.Write(sCmd, DWORD(sCmd.GetLength()));
		}
	}
	catch (CSerialException ex)
	{
		onSerialException(ex);
	}
}


void CHardplace705Dlg::OnTune()
{
	// TODO: Add your control notification handler code here
	CStringA sCmd("HRTU;");

	try
	{
		if (m_Serial.IsOpen())
		{
			m_Serial.Write(sCmd, DWORD(sCmd.GetLength()));
		}
	}
	catch (CSerialException ex)
	{
		onSerialException(ex);
	}
}


void CHardplace705Dlg::EnableHardrock(bool fEnable)
{
	// TODO: Add your implementation code here.
	GetDlgItem(IDC_PTT)->EnableWindow(fEnable);
	GetDlgItem(IDC_PTTON)->EnableWindow(fEnable);
	GetDlgItem(IDC_COR)->EnableWindow(fEnable);
	GetDlgItem(IDC_QRP)->EnableWindow(fEnable);
	GetDlgItem(IDC_TUNE)->EnableWindow(fEnable);
	if (!fEnable)
	{
		m_iPTT = -1;
		UpdateData(FALSE);
	}
}


void CHardplace705Dlg::EnableHardplace(bool fEnable)
{
	// TODO: Add your implementation code here.
	GetDlgItem(IDC_SET_MAXIMUM_PWR)->EnableWindow(fEnable);
	GetDlgItem(IDC_SET_INITIAL_PWR)->EnableWindow(fEnable);
	GetDlgItem(IDC_RESET_PWR)->EnableWindow(fEnable);
	GetDlgItem(IDC_SLIDER)->EnableWindow(fEnable);
	if (!fEnable)
	{
		SetDlgItemText(IDOPEN, _T("Open"));
		m_PwrCtrl.SetPos(m_PwrCtrl.GetRangeMax());
	}
	m_cIOFailures = 0;
}

void CHardplace705Dlg::IOTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	reinterpret_cast<CHardplace705Dlg*>(GetWindowLongPtr(hWnd, GWLP_USERDATA))->onIOTimer();
}

void CHardplace705Dlg::onIOTimer(void)
{
	DisableIOTimer();
	try
	{
		if (m_Serial.IsOpen())
		{
			m_IOPingPong = !m_IOPingPong;

			if (m_IOPingPong)
			{
				const uint8_t ReadRFLevelReq[] = { 0xFE, 0xFE, 0xA4, 0xE5, 0x14, 0x0A, 0xFD };
				uint8_t achRsp[9];

				ClearSerial();
				m_Serial.Write(ReadRFLevelReq, DWORD(sizeof ReadRFLevelReq));
				DWORD dwRead(m_Serial.Read(achRsp, sizeof achRsp));
				if (dwRead)
				{
					unsigned uPower(0);

					if (achRsp[4] == 0x14
						&& dwRead > 6
						&& achRsp[5] == 0x0A)
					{
						unsigned uMultiplier(1);
						for (size_t nIndex(static_cast<size_t>(dwRead) - 2); nIndex >= 6; nIndex--)
						{
							uint8_t uchValue((((achRsp[nIndex] & 0xF0) >> 4) * 10) + (achRsp[nIndex] & 0x0F));

							uPower += static_cast<unsigned>(uchValue) * uMultiplier;
							uMultiplier *= 100;
						}
						m_PwrCtrl.SetPos(m_PwrCtrl.GetRangeMax() - int(uPower));
					}
				}
			}
			else if (m_cIOFailures % 10 < 2)
			{
				CString sResp;
				HardrockCmd("HRMD;", sResp, false);
				if (sResp.GetLength() >= 5)
				{
					m_cIOFailures = 0;
					m_iPTT = sResp.GetAt(4) - '0';
					UpdateData(FALSE);
					if (!GetDlgItem(IDC_PTT)->IsWindowEnabled())
					{
						EnableHardrock();
					}
				}
				else if (++m_cIOFailures >= 2
					&& GetDlgItem(IDC_PTT)->IsWindowEnabled())
				{
					EnableHardrock(FALSE);
				}
			}
			else if (++m_cIOFailures >= INT_MAX)
			{
				m_cIOFailures = 0;
			}
		}
		else if (m_PwrCtrl.GetPos() != m_PwrCtrl.GetRangeMax())
		{
			m_PwrCtrl.SetPos(m_PwrCtrl.GetRangeMax());
		}
	}
	catch (CSerialException ex) {
		onSerialException(ex);
	}
	EnableIOTimer();
}
afx_msg LRESULT CHardplace705Dlg::OnAppInit(WPARAM wParam, LPARAM lParam)
{
	try
	{
		if (m_CommPort.GetCount() > 0
			&& !m_Serial.IsOpen())
		{
			COMMTIMEOUTS timeOuts;
			CString sResp;

			m_Serial.Open(int(m_CommPort.GetItemData(m_CommPort.GetCurSel())), DWORD(115200));

			m_Serial.GetTimeouts(timeOuts);
			timeOuts.ReadIntervalTimeout = 250;
			timeOuts.ReadTotalTimeoutConstant = 250;
			timeOuts.WriteTotalTimeoutConstant = 10000;
			m_Serial.SetTimeouts(timeOuts);

			HardrockCmd("HPVE;", sResp);
			m_Serial.Close();
			if (sResp.Find(_T("Hardplace")) >= 0)
			{
				OnOpenComPort();
			}
		}
	}
	catch (CSerialException ex)
	{
		if (m_Serial.IsOpen())
		{
			m_Serial.Close();
		}
	}
	return 0;
}
