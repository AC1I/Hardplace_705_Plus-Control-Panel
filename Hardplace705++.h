
// Hardplace705++.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CHardplace705App:
// See Hardplace705++.cpp for the implementation of this class
//

class CHardplace705App : public CWinApp
{
public:
	CHardplace705App();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CHardplace705App theApp;
