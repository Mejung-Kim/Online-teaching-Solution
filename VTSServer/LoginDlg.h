#pragma once
// CLoginDlg 대화 상자입니다.
#include "resource.h"
#include "stdafx.h"
class CLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLoginDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LoginDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString username;
	CButton B_Teacher;
	CButton B_Student;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	int *isTeacher;
	CString IP;
};
