
// VTSServerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "VTSServerDlg.h"
#include "afxdialogex.h"
#include <WinSock2.h>
#include "DataThread.h"
#include "CamThread.h"
#include "LoadImage.h"
#include "resource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define MSGSIZE 100
#define NAMESIZE 10
// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CVTSServerDlg 대화 상자




CVTSServerDlg::CVTSServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVTSServerDlg::IDD, pParent)
	, m_x(0)
	, m_y(0)
	, m_bDown(FALSE)
	, FilePath(_T(""))
	, ops_x(0)
	, ops_y(0)
	, ops_bDown(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVTSServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FilePath, FilePath);
	DDX_Control(pDX, IDC_ChatList, m_chatlist);
	DDX_Control(pDX, IDC_Board, Board);
}

BEGIN_MESSAGE_MAP(CVTSServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//	ON_STN_CLICKED(IDC_Board, &CVTSServerDlg::OnStnClickedBoard)
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
ON_EN_CHANGE(IDC_FilePath, &CVTSServerDlg::OnEnChangeFilepath)
ON_BN_CLICKED(IDC_MsgSend, &CVTSServerDlg::OnBnClickedMsgsend)
ON_BN_CLICKED(IDC_Erase, &CVTSServerDlg::OnBnClickedErase)
END_MESSAGE_MAP()


// CVTSServerDlg 메시지 처리기

BOOL CVTSServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.
	
	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_x=0;
	m_y=0;
	m_bDown=FALSE;
	int retval;
	CString str;
	//***************웹캠서버 셋팅하는 부분입니다.************//
	WSADATA wsaData;
	SOCKET *CamServSock=new SOCKET();
	SOCKADDR_IN *CamservAddr = new SOCKADDR_IN;

	if(WSAStartup(MAKEWORD(2,2),&wsaData) !=0)
		AfxMessageBox(_T("WSAStartup()에서 에러가 발생하였습니다."));
	
	*CamServSock=socket(PF_INET, SOCK_STREAM, 0);
	if(*CamServSock==INVALID_SOCKET)
		AfxMessageBox(_T("hServSock()에서 에러가 발생하였습니다."));
	
	int optval = TRUE;
	if(setsockopt(*CamServSock,SOL_SOCKET,SO_REUSEADDR,(char*)&optval, sizeof(optval))<0)
		AfxMessageBox(_T("setsockopterror"));
	
	memset(CamservAddr, 0, sizeof(*CamservAddr));
	CamservAddr->sin_family=AF_INET;
	CamservAddr->sin_addr.s_addr=inet_addr(M_IP);
	CamservAddr->sin_port=htons(PORT);
	
	if(bind(*CamServSock,(SOCKADDR *)CamservAddr, sizeof(*CamservAddr))==SOCKET_ERROR)
		AfxMessageBox(_T("bind()에서 에러가 발생했습니다."));

	if(listen(*CamServSock,1)==SOCKET_ERROR)
		AfxMessageBox(_T("listen()에서 에러가 발생했습니다."));
	
	//***************웹캠서버 셋팅 끝입니다******************//


	//*************데이터 서버 소켓 셋팅입니다.***************//
	SOCKET *DataServSock=new SOCKET();
	SOCKADDR_IN *DataservAddr = new SOCKADDR_IN;
		
	if(WSAStartup(MAKEWORD(2,2),&wsaData) !=0)
		AfxMessageBox(_T("WSAStartup()에서 에러가 발생하였습니다."));
	
	*DataServSock=socket(PF_INET, SOCK_STREAM, 0);
	if(*DataServSock==INVALID_SOCKET){
		AfxMessageBox(_T("DataSocket()에서 에러가 발생하였습니다."));
		return FALSE;
	}
	if(setsockopt(*DataServSock,SOL_SOCKET,SO_REUSEADDR,(char*)&optval, sizeof(optval))<0)
			AfxMessageBox(_T("setsockopterror"));
	
	memset(DataservAddr, 0, sizeof(*DataservAddr));
	DataservAddr->sin_family=AF_INET;
	DataservAddr->sin_addr.s_addr=inet_addr(M_IP);
	DataservAddr->sin_port=htons(PORT2);
	
	if(bind(*DataServSock,(SOCKADDR *)DataservAddr, sizeof(*DataservAddr))==SOCKET_ERROR){
		AfxMessageBox(_T("bind()에서 에러가 발생했습니다."));
		return FALSE;
	}

	if(listen(*DataServSock,1)==SOCKET_ERROR){
		AfxMessageBox(_T("listen()에서 에러가 발생했습니다."));
		return FALSE;
	}
	//********************데이터 서버 소켓 환경설정 끝입니다.*************//

	//************웹캠 소켓 연결 *********************//
	CamSock = new SOCKET();
	SOCKADDR_IN *CamAddr = new SOCKADDR_IN;
	int szCamAddr = sizeof(*CamAddr);
	*CamSock = accept(*CamServSock,(SOCKADDR*)CamAddr,&szCamAddr);
	if(*CamSock==INVALID_SOCKET){
		AfxMessageBox(_T(" 서버 : 캠 소켓 에러"));
		exit(0);
		return FALSE;
	}
	//*************웹캠 소켓 연결 끝입니다************//

	//**************웹캠 소켓 연결 잘 됐는지 클라이언트로 부터 메시지 받아옵니다.***********//
	char temp;
	recv(*CamSock,&temp,sizeof(char),MSG_WAITALL);
	if(temp!='o'){
		AfxMessageBox(_T("서버 : 클라이언트로 부터 'o'를 수신하는 과정에서 오류가 생겼습니다."));
		exit(0);
		return FALSE;
	}
	//****************************************************************//

	//**************웹캠 소켓 연결 잘 됐다고 클라이언트한테 알려줍니다..***********//
	retval = send(*CamSock,&temp,sizeof(char),0);
	if(retval!=sizeof(char)){
		AfxMessageBox(_T("서버 : 클라이언트에게 'o'를 전송하는 과정에서 오류가 생겼습니다. "));
		exit(0);
		return FALSE;
	}
	//****************************************************************//

	//********************데이터 소켓 연결입니다.*****************//
	DataSock=new SOCKET();
	SOCKADDR_IN *DataAddr = new SOCKADDR_IN;
	int szDataAddr = sizeof(*DataAddr);
	*DataSock=accept(*DataServSock,(SOCKADDR*)DataAddr,&szDataAddr);
	if(*DataSock==INVALID_SOCKET){
		AfxMessageBox(_T("서버 : 데이터 소켓 에러"));
		exit(0);
		return FALSE;
	}
	////*******************데이터 소켓 연결 끝입니다.********************//
	//
	////**************데이터 소켓 연결 잘 됐는지 클라이언트로 부터 메시지 받아옵니다.***********//

	retval=recv(*DataSock,&temp,sizeof(char),MSG_WAITALL);
	if(temp!='k'){
		str.Format(_T(" 리시브 리턴 값 : %d"),retval);
		AfxMessageBox(_T("서버 : 클라이언트로부터 'k'를 수신하는 과정에서 오류가 생겼습니다..")+str);
		exit(0);
		return FALSE;
	}
	////****************************************************************//

	////**************데이터 소켓 연결 잘 됐다고 클라이언트에게 알려줍니다.***********//
	temp='k';
	retval=send(*DataSock,&temp,sizeof(char),0);
	if(retval!=sizeof(char)){
		str.Format(_T(" 리시브 리턴 값 : %d"),retval);
		AfxMessageBox(_T("서버 : 클라이언트에게 'k'를 전송하는 과정에서 오류가 생겼습니다. /")+str);
		exit(0);
		return FALSE;
	}
	//****************************************************************//
	
	////*******************보내는 CamThread 매개변수 셋팅 및 실행입니다****************//
	CamThreadArg * _CamThreadArg = new CamThreadArg();
	_CamThreadArg->dlg=this;
	_CamThreadArg->m_pChild = CamSock;
	HANDLE hCam = AfxBeginThread(CamThread,_CamThreadArg,THREAD_PRIORITY_NORMAL, 0, 0);

	////**************** 상대방에게 서버 유저의 이름을 전송합니다.********************//
	char * namebuf=new char[NAMESIZE];
	memset(namebuf,0,NAMESIZE);
	int len;
	//데이터 형변환
	WideCharToMultiByte(CP_ACP,0,M_username,-1,namebuf,M_username.GetLength(),NULL,NULL);
	len = strlen(namebuf)+1;
	namebuf[len-1]='\0';
	M_username.ReleaseBuffer();

	retval=send(*DataSock,namebuf,NAMESIZE,0);
	if(retval!=NAMESIZE){
		AfxMessageBox(_T("이름 전송 과정에 오류가 생겼습니다."));
		exit(0);
	}
	////******************이름 전송 끝입니다.*******************************//

	////******************상대방의 이름을 수신합니다.**********************//
	memset(namebuf,0,NAMESIZE);
	WCHAR strData[NAMESIZE*2]={0};
	//메시지 크기 만큼 데이터 수신, 완전히 수신하지 않으면 재수신
	while(TRUE){
		retval=recv(*DataSock,namebuf,NAMESIZE,0);
		if(retval==NAMESIZE)
			break;
		else if(retval==-1){
			AfxMessageBox(_T("클라이언트의 이름을 수신하는 과정에서 오류가 생겼습니다."));
			exit(0);
		}
	};
	MultiByteToWideChar(CP_ACP,0,namebuf,-1,strData,NAMESIZE*2);
	M_username_client.Format(_T("%s"),strData);
	////******************이름 수신 끝입니다.***********************************//


	////*******************받는 CamThread 매개변수 셋팅및 실행입니다***************//
	CamThreadArg_recv * _CamThreadArg_recv = new CamThreadArg_recv();
	_CamThreadArg_recv->dlg=this;
	_CamThreadArg_recv->m_pChild = CamSock; 
	HANDLE hCam_recv = AfxBeginThread(CamThread_recv,_CamThreadArg_recv,THREAD_PRIORITY_NORMAL, 0, 0);
	
	//*****************DataThread 셋팅 및 실행입니다.*****************************//
	DrawArg *_DrawArg = new DrawArg();
	_DrawArg->dlg = this;
	_DrawArg->server = DataSock;
	HANDLE hDraw = AfxBeginThread(DataThread,_DrawArg,THREAD_PRIORITY_HIGHEST,0,0);

	//***************스레드의 종료를 대기합니다.**************************//
	HANDLE hArg[3];
	hArg[0]=hCam;
	hArg[1]=hCam_recv;
	hArg[2]=hDraw;
	WaitForMultipleObjects(2,hArg,TRUE,INFINITE);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CVTSServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CVTSServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
		

}
// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CVTSServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CVTSServerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bDown=true;
	m_x=point.x;
	m_y=point.y;
	
	char buf[sizeof(int)*3];
	memcpy(buf,&m_x,sizeof(int));
	memcpy(buf+sizeof(int),&m_y,sizeof(int));
	memcpy(buf+sizeof(int)*2,&m_bDown,sizeof(int));
	
	cs.Lock();
	if(sizeof(int)*3!=send(*DataSock,buf,sizeof(int)*3,0)){
		AfxMessageBox(_T("마우스 정보 전송에 이상이 생겼습니다. 프로그램을 종료합니다."));
		exit(0);
	}
	cs.Unlock();
	//CDialogEx::OnLButtonDown(nFlags, point);
}


void CVTSServerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: ¿©±â¿¡ ¸Þ½ÃÁö Ã³¸®±â ÄÚµå¸¦ Ãß°¡ ¹×/¶Ç´Â ±âº»°ªÀ» È£ÃâÇÕ´Ï´Ù.
	m_bDown=false;
	char buf[sizeof(int)*3];
	memcpy(buf,&m_x,sizeof(int));
	memcpy(buf+sizeof(int),&m_y,sizeof(int));
	memcpy(buf+sizeof(int)*2,&m_bDown,sizeof(int));

	cs.Lock();
	if(sizeof(int)*3!=send(*DataSock,buf,sizeof(int)*3,0)){
		AfxMessageBox(_T("마우스 정보 전송에 이상이 생겼습니다. 프로그램을 종료합니다."));
		exit(0);
	}
	cs.Unlock();

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CVTSServerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ¿©±â¿¡ ¸Þ½ÃÁö Ã³¸®±â ÄÚµå¸¦ Ãß°¡ ¹×/¶Ç´Â ±âº»°ªÀ» È£ÃâÇÕ´Ï´Ù.
	if(!m_bDown)
		return;
	CClientDC dc(this);
	dc.MoveTo(m_x,m_y);
	dc.LineTo(point.x,point.y);

	m_x=point.x;
	m_y=point.y;

	char buf[sizeof(int)*3];
	memcpy(buf,&m_x,sizeof(int));
	memcpy(buf+sizeof(int),&m_y,sizeof(int));
	memcpy(buf+sizeof(int)*2,&m_bDown,sizeof(int));
	
	cs.Lock();
	if(sizeof(int)*3!=send(*DataSock,buf,sizeof(int)*3,0)){
		AfxMessageBox(_T("마우스 정보 전송에 이상이 생겼습니다. 프로그램을 종료합니다."));
		exit(0);
	}
	cs.Unlock();

	CDialogEx::OnMouseMove(nFlags, point);
}



void CVTSServerDlg::OnEnChangeFilepath()
{
	Invalidate(TRUE);
	LIArg *arg = new LIArg;
	arg->dlg=this;
	arg->SORC='S';
	int len = FilePath.GetLength();
	memset(filepath,0,200);
	WideCharToMultiByte(CP_ACP,0,FilePath,-1,filepath,len,NULL,NULL);
	filepath[len]='\0';
	AfxBeginThread(LoadImage,arg,THREAD_PRIORITY_NORMAL, 0, 0);
	return;
}

void CVTSServerDlg::OnBnClickedMsgsend()
{
	CString strData;
	
	Invalidate(TRUE);
	GetDlgItemText(IDC_Enter,strData); //메시지 입력 받아오기
	
 	
	char buf[sizeof(int)*3]={0};
	int index = -1;
	int *len = new int();
	int retval;
	
	
	char * msgbuf=new char[MSGSIZE];
	memset(msgbuf,0,MSGSIZE);
	//데이터 형변환
	WideCharToMultiByte(CP_ACP,0,strData,-1,msgbuf,strData.GetLength(),NULL,NULL);
	*len = strlen(msgbuf)+1;
	msgbuf[*len-1]='\0';
	strData.ReleaseBuffer();

	//메시지 전송 인자 셋팅
	SMArg * _SMArg = new SMArg();
	_SMArg->msgbuf=msgbuf;
	_SMArg->len=len;
	_SMArg->server=DataSock;
	//메시지 전송
	AfxBeginThread(SendMsgThread,_SMArg,THREAD_PRIORITY_NORMAL, 0, 0);

	//화면에 내 메시지 출력
	strData.Format(M_username+_T(" : ")+strData);
	((CListBox *)this->GetDlgItem(IDC_ChatList))->InsertString(-1,strData);
	SetDlgItemText(IDC_Enter,_T(""));
	
}


void CVTSServerDlg::OnBnClickedErase()
{
	LIArg *arg = new LIArg;
	arg->dlg=this;
	arg->SORC='S';
	AfxBeginThread(LoadImage,arg,THREAD_PRIORITY_NORMAL, 0, 0);
	// TODO: ¿©±â¿¡ ÄÁÆ®·Ñ ¾Ë¸² Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
}
