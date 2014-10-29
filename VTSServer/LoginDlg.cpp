// LoginDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "LoginDlg.h"
#include "afxdialogex.h"


#include "resource.h"

// CLoginDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoginDlg::IDD, pParent)
	, username(_T(""))
	, isTeacher(NULL)
	, IP(_T(""))
{

}


CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_USERNAME, username);
	DDV_MaxChars(pDX, username, 10);
	DDX_Control(pDX, IDC_TEACHER, B_Teacher);
	DDX_Control(pDX, IDC_STUDENT, B_Student);
	DDX_Text(pDX, IDC_IP, IP);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLoginDlg �޽��� ó�����Դϴ�.


void CLoginDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);

	if(username.GetLength()==0){
		AfxMessageBox(_T("����� �̸��� �Է����ּ���."));
		return;
	}
	if(IP.GetLength()==0){
		AfxMessageBox(_T("IP�� �Է����ּ���."));
		return;
	}
	
	M_username = username;
	M_IP = new char[11];
	int len = IP.GetLength();
	WideCharToMultiByte(CP_ACP,0,IP,-1,M_IP,len,NULL,NULL);
	M_IP[len]='\0';
	//�������� ��� 1�� �����Ѵ�.
	if(B_Teacher.GetCheck()){
		*isTeacher = 1;
	}

	CDialogEx::OnOK();
}


BOOL CLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CButton *b_check;
	b_check= (CButton *)(GetDlgItem(IDC_TEACHER));
	b_check->SetCheck(TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}
