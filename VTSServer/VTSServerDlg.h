
// VTSServerDlg.h : 헤더 파일
//

#pragma once
#include "resource.h"
#include "cvvimage.h"
#include "DataThread.h"
// CVTSServerDlg 대화 상자
class CVTSServerDlg : public CDialogEx
{
// 생성입니다.
public:
	CVTSServerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VTSSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// 서버소켓을 만들고 클라이언트의 요청이 오면 클라이언트 소켓을 만든뒤 CamThread를 생성하는 함수
	//UINT ServerSetUp(LPVOID arg);
	// 클라이언트와 연결될 때 최초로 한번 클라이언트로부터 사용자 이름을 받아오는 함수
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


