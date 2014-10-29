
// VTSServerDlg.h : ��� ����
//

#pragma once
#include "resource.h"
#include "cvvimage.h"
#include "DataThread.h"
// CVTSServerDlg ��ȭ ����
class CVTSServerDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CVTSServerDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_VTSSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// ���������� ����� Ŭ���̾�Ʈ�� ��û�� ���� Ŭ���̾�Ʈ ������ ����� CamThread�� �����ϴ� �Լ�
	//UINT ServerSetUp(LPVOID arg);
	// Ŭ���̾�Ʈ�� ����� �� ���ʷ� �ѹ� Ŭ���̾�Ʈ�κ��� ����� �̸��� �޾ƿ��� �Լ�
	void ReceiveUsername(SOCKET * hClntSock);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	int m_x;
	int m_y;
	BOOL m_bDown;
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnStnClickedBoard();
	CString FilePath;
	afx_msg void OnEnChangeFilepath();
	int ops_x;
	int ops_y;
	int ops_bDown;
	CListBox m_chatlist;
	SOCKET *CamSock;
	SOCKET *DataSock;
	CStatic Board;
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedMsgsend();
	afx_msg void OnBnClickedErase();
};


