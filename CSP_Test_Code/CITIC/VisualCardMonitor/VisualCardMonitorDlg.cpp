// VisualCardMonitorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VisualCardMonitor.h"
#include "VisualCardMonitorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVisualCardMonitorDlg dialog




CVisualCardMonitorDlg::CVisualCardMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVisualCardMonitorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVisualCardMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVisualCardMonitorDlg, CDialog)
	
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_SHOWTASK,onShowTask)

	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CVisualCardMonitorDlg message handlers

BOOL CVisualCardMonitorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

//	ShowWindow(SW_MINIMIZE);

	// TODO: Add extra initialization here
	m_isNotify = FALSE;
	if (!m_isNotify)  
	{  
		m_isNotify = TRUE;
		m_nid.cbSize=(DWORD)sizeof(NOTIFYICONDATA);  
		m_nid.hWnd=this->m_hWnd;  
		m_nid.uID=IDR_MAINFRAME;  
		m_nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;  
		strcpy(m_nid.szInfo, "");
		strcpy(m_nid.szTip, "");
		m_nid.uCallbackMessage=WM_SHOWTASK;//�Զ������Ϣ����,ע��:�������Ϣ���û��Զ�����Ϣ   
		m_nid.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));  
		strcpy(m_nid.szTip, "HED�๦�ܿ��ӿ����ӳ���");//��Ϣ��ʾ��Ϊ"�ƻ���������"   
		   
		Shell_NotifyIcon(NIM_ADD,&m_nid);//�����������ͼ��   
	}  

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVisualCardMonitorDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVisualCardMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//wParam���յ���ͼ���ID����lParam���յ���������Ϊ   
LRESULT CVisualCardMonitorDlg::onShowTask(WPARAM wParam,LPARAM lParam)
{  
    if(wParam!=IDR_MAINFRAME)  
		return 1;  
    switch(lParam)  
    {  
        case WM_RBUTTONUP:          //�Ҽ�����ʱ������ݲ˵�������ֻ��һ��"�ر�"   
	    {  

			LPPOINT lpoint=new tagPOINT;  
			::GetCursorPos(lpoint);         //�õ����λ��   
			CMenu menu;  
			menu.CreatePopupMenu();             //����һ������ʽ�˵�            
			menu.AppendMenu(MF_STRING,WM_DESTROY,_T("�ر�"));   //���Ӳ˵���"�ر�"�����������ϢWM_DESTROY�������ڣ������أ��������������            
			menu.TrackPopupMenu(TPM_LEFTALIGN,lpoint->x,lpoint->y,this);//ȷ������ʽ�˵���λ��             
						SetForegroundWindow();  

			HMENU hmenu=menu.Detach();   //��Դ����   
			menu.DestroyMenu();  
			delete lpoint;  
		 }break;  
		case WM_LBUTTONDBLCLK:    //˫������Ĵ���   
		{  
			this->ShowWindow(SW_SHOW);    //�򵥵���ʾ���������¶�   
		}break;  
    }  
    return 0;  
}

void CVisualCardMonitorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == SC_CLOSE)
	{
		ShowWindow(SW_HIDE);
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}