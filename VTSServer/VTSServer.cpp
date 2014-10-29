
// VTSServer.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "VTSServer.h"
#include "VTSServerDlg.h"
#include <process.h>
#include "VTSClntDlg.h"
#include "LoginDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString M_username;
CString M_username_client;
char *M_IP;
CCriticalSection cs;
char *filepath;

// CVTSServerApp

BEGIN_MESSAGE_MAP(CVTSServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CVTSServerApp ����

CVTSServerApp::CVTSServerApp()
{
	// �ٽ� ���� ������ ����
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}


// ������ CVTSServerApp ��ü�Դϴ�.

CVTSServerApp theApp;


// CVTSServerApp �ʱ�ȭ

BOOL CVTSServerApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�.
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}


	AfxEnableControlContainer();

	// ��ȭ ���ڿ� �� Ʈ�� �� �Ǵ�
	// �� ��� �� ��Ʈ���� ���ԵǾ� �ִ� ��� �� �����ڸ� ����ϴ�.
	CShellManager *pShellManager = new CShellManager;

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));
	filepath = new char[200]; //���� ��� �ʱ�ȭ
	isTeacher = 0; //LoginDlg���� ���������� �л������� ���� ������ �޾ƿ´�.��
	
	CLoginDlg LoginDlg;
	LoginDlg.isTeacher = &isTeacher;
	INT_PTR nResponse = LoginDlg.DoModal();
	
	if(nResponse == IDOK){
		
		if(isTeacher){
			AfxMessageBox(_T("������ �������� �α����մϴ�."));
			CVTSServerDlg dlg;
			m_pMainWnd = &dlg;
			nResponse = dlg.DoModal();	
		}
		else{
			AfxMessageBox(_T("�л� �������� �α����մϴ�."));
			CVTSClntDlg clntdlg;
			m_pMainWnd = &clntdlg;
			nResponse = clntdlg.DoModal();
		}
	}

	else if(nResponse == IDCANCEL)
		return 0;
	
	
		
	if (nResponse == IDOK)
	{
		// TODO: ���⿡ [Ȯ��]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ���⿡ [���]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}

	// ������ ���� �� �����ڸ� �����մϴ�.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE��
	// ��ȯ�մϴ�.
	return FALSE;
}

