// VisualCardMonitorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VisualCardMonitor.h"
#include "VisualCardMonitorDlg.h"

#include <DBT.h>
#include <afxwin.h>
#include <Wincrypt.h>
#include "UKeyAPI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVisualCardMonitorDlg dialog

extern "C" const GUID GUID_DEVINTERFACE_HEDUSBDEVICE = {0x50dd5230, 0xba8a, 0x11d1, {0xbf, 0x5d,0x00, 0x00, 0xf8, 0x05, 0xf5, 0x30}};

#define CERTLENGTH 2044

CVisualCardMonitorDlg::CVisualCardMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVisualCardMonitorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVisualCardMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATUS_INFO, m_edtState);
	
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

	RegisterDevNotify();

	// TODO: Add extra initialization here
	m_isNotify = FALSE;
	DetectReader();
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

bool CVisualCardMonitorDlg::RegisterDevNotify()
{
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
	HDEVNOTIFY hDevNotify;
	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter) );
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_HEDUSBDEVICE;

	// device notifications should be send to the main dialog
	hDevNotify = RegisterDeviceNotification(
		this->m_hWnd,
		&NotificationFilter,
		DEVICE_NOTIFY_WINDOW_HANDLE
		);
	if ( !hDevNotify) {
		DWORD Err = GetLastError();
		return FALSE;
	}

	return TRUE;
}

LRESULT CVisualCardMonitorDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_DEVICECHANGE)
	{
		
		switch (wParam)
		{

		case DBT_DEVICEARRIVAL:
			if (m_isNotify)  
			{		
				strcpy(m_nid.szTip, "UKey �Ѳ���");
				DetectReader();
			}
			break;

		case DBT_DEVICEREMOVECOMPLETE:
			if (m_isNotify)  
			{		
				strcpy(m_nid.szTip, "UKey �Ѱγ�");
				if (m_hUKey != 0)
				{
					UKeyClose(m_hUKey);	
					m_hUKey = 0;
				}
				DeleteRootCert();
				m_edtState.SetWindowText("�������Ϳ��Ѱγ���֤����ע��");
				
			}
			break;
		default:
			;
		}
		
		if (m_isNotify)  
		{  
			m_isNotify = TRUE;
			m_nid.uID=IDR_MAINFRAME;  
			m_nid.uFlags=NIF_INFO; 			 
			strcpy(m_nid.szInfoTitle, "��ʾ");
			Shell_NotifyIcon(NIM_MODIFY,&m_nid);//�����������ͼ��   
			SetTimer(1, 2000, NULL);
		}
			
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

bool CVisualCardMonitorDlg::DetectReader()
{
	if (!UKeyOpen(&m_hUKey))
	{
		m_hUKey = 0;
		m_edtState.SetWindowText("������δ���룬������������");
		return FALSE;
	}
	else
	{
		if(ImportRootCert())
		{
			m_edtState.SetWindowText("�Ѽ�⵽�������Ϳ��ӿ���ע�Ῠ��֤��ɹ���");
			return TRUE;
		}
		else
		{
			m_edtState.SetWindowText("�򿪶�����ʧ�ܣ�");
			return FALSE;
		}
		
	}
	
}

bool CVisualCardMonitorDlg::ImportRootCert()
{
	LPBYTE lpCert;
	lpCert = new BYTE[CERTLENGTH];
	UKeyGetCert(m_hUKey, NULL, lpCert, CERTLENGTH);
	HCERTSTORE hSystemStore = CertOpenSystemStore(NULL, "MY");
	if (hSystemStore == NULL)
	{
		return FALSE;
	}
	
	char* szPwd = "111111";
	LPWSTR pWideChar = NULL;
	int nWideChar = 0;
	nWideChar = MultiByteToWideChar(CP_ACP, 0, szPwd, -1, pWideChar, nWideChar);
	pWideChar = new WCHAR[nWideChar];
	memset(pWideChar, 0, sizeof(WCHAR)*nWideChar);
	MultiByteToWideChar(CP_ACP, 0, szPwd, -1, pWideChar, nWideChar);
	

	CRYPT_DATA_BLOB blob;
	memset(&blob, 0, sizeof(blob));
	blob.pbData = lpCert;
	blob.cbData = CERTLENGTH;
	HCERTSTORE hCertStore = NULL;
	hCertStore = PFXImportCertStore(&blob, pWideChar, CRYPT_EXPORTABLE);

	PCCERT_CONTEXT pCertContext = CertFindCertificateInStore(hCertStore, X509_ASN_ENCODING|PKCS_7_ASN_ENCODING, 0, CERT_FIND_ANY, NULL, NULL);

	if (pCertContext == NULL)
	{
		return FALSE;
	}
	if (!CertAddCertificateContextToStore(hSystemStore, pCertContext, CERT_STORE_ADD_REPLACE_EXISTING, NULL))
		return FALSE;
	
	CertFreeCertificateContext(pCertContext);
	CertCloseStore(hSystemStore, CERT_CLOSE_STORE_FORCE_FLAG);
	delete []lpCert;
	delete []pWideChar;
	return TRUE;
}

void CVisualCardMonitorDlg::DeleteRootCert()
{
	HANDLE          hStoreHandle;
	PCCERT_CONTEXT  pCertContext=NULL;   
	PCCERT_CONTEXT  pDupCertContext; 
	PCERT_PUBLIC_KEY_INFO pOldPubKey = NULL;
	PCERT_PUBLIC_KEY_INFO pNewPubKey; 
	
	char pszNameString[256];

	char fResponse ='n';  
	char x;


	//-------------------------------------------------------------------
	// Open a system certificate store.

	hStoreHandle = CertOpenSystemStore(NULL, "MY");
	//-------------------------------------------------------------------
	// Find the certificates in the system store. 

	while(pCertContext= CertEnumCertificatesInStore(
		hStoreHandle,
		pCertContext)) // on the first call to the function,
                   //  this parameter is NULL
                   // on all subsequent
                   //  calls, it is the last pointer returned by 
                   //  the function
	{
	//-------------------------------------------------------------------
	// Get and display the name of the subject of the certificate.

		CertGetNameString(   
			pCertContext,   
			CERT_NAME_SIMPLE_DISPLAY_TYPE,   
			0,
			NULL,   
			pszNameString,   
			128);

	//-------------------------------------------------------------------
	// Check to determine whether the issuer 
	// and the subject are the same.

		CertCompareCertificateName(
			PKCS_7_ASN_ENCODING | X509_ASN_ENCODING,
			&(pCertContext->pCertInfo->Issuer),
			&(pCertContext->pCertInfo->Subject));
		//--------------------------------------------------------------------
		// Determine whether this certificate's public key matches 
		// the public key of the last certificate.

		pNewPubKey = &(pCertContext->pCertInfo->SubjectPublicKeyInfo);
		if(pOldPubKey)
		{
			CertComparePublicKeyInfo(
				PKCS_7_ASN_ENCODING | X509_ASN_ENCODING,
				pOldPubKey,
				pNewPubKey);
		}
		//-------------------------------------------------------------------
		// Reset the old key.

		pOldPubKey = pNewPubKey;

		//----------------------------------------------------------------
		// Create a duplicate pointer to the certificate to be 
		// deleted. In this way, the original pointer is not freed 
		// when the certificate is deleted from the store 
		// and the enumeration of the certificates in the store can
		// continue. If the original pointer is used, after the 
		// certificate is deleted, the enumeration loop stops.

		pDupCertContext = CertDuplicateCertificateContext(
			pCertContext);
		//-------------------------------------------------------------------
		// Compare the pCertInfo members of the two certificates 
		// to determine whether they are identical.

		CertCompareCertificate(
			X509_ASN_ENCODING,
			pDupCertContext->pCertInfo, 
			pCertContext->pCertInfo);
		//-------------------------------------------------------------------
		// Delete the certificate.
		CertDeleteCertificateFromStore(
			pDupCertContext);

	} // end while

	//-------------------------------------------------------------------
	// Clean up.

	CertCloseStore(
		hStoreHandle,
		0);

}


