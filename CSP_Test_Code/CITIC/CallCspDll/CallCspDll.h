// CallCspDll.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCallCspDllApp:
// �йش����ʵ�֣������ CallCspDll.cpp
//

class CCallCspDllApp : public CWinApp
{
public:
	CCallCspDllApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCallCspDllApp theApp;