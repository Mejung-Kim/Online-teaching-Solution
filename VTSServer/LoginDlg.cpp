// LoginDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LoginDlg.h"
#include "afxdialogex.h"


#include "resource.h"

// CLoginDlg 대화 상자입니다.

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


// CLoginDlg 메시지 처리기입니다.


void CLoginDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if(username.GetLength()==0){
		AfxMessageBox(_T("사용자 이름을 입력해주세요."));
		return;
	}
	if(IP.GetLength()==0){
		AfxMessageBox(_T("IP를 입력해주세요."));
		return;
	}
	
	M_username = username;
	M_IP = new char[11];
	int len = IP.GetLength();
	WideCharToMultiByte(CP_ACP,0,IP,-1,M_IP,len,NULL,NULL);
	M_IP[len]='\0';
	//선생님인 경우 1을 대입한다.
	if(B_Teacher.GetCheck()){
		*isTeacher = 1;
	}

	CDialogEx::OnOK();
}


BOOL CLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CButton *b_check;
	b_check= (CButton *)(GetDlgItem(IDC_TEACHER));
	b_check->SetCheck(TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
