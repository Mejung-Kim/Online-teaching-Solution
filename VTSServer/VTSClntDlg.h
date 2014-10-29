#pragma once
#include "resource.h"

// CVTSClntDlg 대화 상자입니다.

class CVTSClntDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CVTSClntDlg)

public:
	CVTSClntDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CVTSClntDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VTSSERVER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	int m_x;
	int m_y;
	int m_bDown;
	SOCKET *DataSock;
	SOCKET *CamSock;
//	afx_msg void OnChangeFilepath();
	afx_msg void OnEnChangeFilepath();
	afx_msg void OnClickedMsgsend();
	CString FilePath;
	afx_msg void OnClickedErase();
};
