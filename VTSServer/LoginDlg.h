#pragma once
// CLoginDlg ��ȭ �����Դϴ�.
#include "resource.h"
#include "stdafx.h"
class CLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CLoginDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_LoginDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
