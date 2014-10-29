// VTSClntDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VTSServer.h"
#include "VTSClntDlg.h"
#include "afxdialogex.h"
#include "CamThread.h"
#include "DataThread.h"
#include "LoadImage.h"
#define MSGSIZE 100
#define NAMESIZE 10
// CVTSClntDlg 대화 상자입니다.
IMPLEMENT_DYNAMIC(CVTSClntDlg, CDialogEx)

CVTSClntDlg::CVTSClntDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVTSClntDlg::IDD, pParent)
	, m_x(0)
	, m_y(0)
	, m_bDown(0)
	, FilePath(_T(""))
{

}

CVTSClntDlg::~CVTSClntDlg()
{
}

void CVTSClntDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FilePath, FilePath);
}


BEGIN_MESSAGE_MAP(CVTSClntDlg, CDialogEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_EN_CHANGE(IDC_FilePath, &CVTSClntDlg::OnEnChangeFilepath)
	ON_BN_CLICKED(IDC_MsgSend, &CVTSClntDlg::OnClickedMsgsend)
	ON_BN_CLICKED(IDC_Erase, &CVTSClntDlg::OnClickedErase)
END_MESSAGE_MAP()


// CVTSClntDlg 메시지 처리기입니다.


BOOL CVTSClntDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	WSADATA wsaData;
	CamSock = new SOCKET();
	SOCKADDR_IN CamservAddr;
	int retval;

	//********************캠 소켓 연결 부분입니다. *********************//
	if(WSAStartup(MAKEWORD(2,2),&wsaData) !=0){
		AfxMessageBox(_T("WSAStartup()에서 에러가 발생하였습니다."));
		exit(0);
	}
	
	*CamSock=socket(PF_INET, SOCK_STREAM, 0);
	if(*CamSock==INVALID_SOCKET){
		AfxMessageBox(_T("hServSock()에서 에러가 발생하였습니다."));
		exit(0);
	}
	memset(&CamservAddr, 0, sizeof(CamservAddr));
	CamservAddr.sin_family=AF_INET;
	CamservAddr.sin_addr.s_addr=inet_addr(M_IP);
	CamservAddr.sin_port=htons(PORT);
	
	if(connect(*CamSock,(SOCKADDR *)&CamservAddr, sizeof(CamservAddr))==SOCKET_ERROR){
		AfxMessageBox(_T("클라이언트 : 서버 접속에 실패하였습니다.(캠소켓실패)"));
		exit(0);
		return FALSE;
	}
	//********************캠소켓 연결 끝입니다.***********************************//


	//****************CamSock이 연결됐다고 서버에 알려줍니다.***********//
	char temp = 'o';
	retval=send(*CamSock,&temp,sizeof(char),0);
	if(sizeof(char)!=retval){
		AfxMessageBox(_T("클라이언트 : 서버에게 'o'를 전송하지 못했습니다."));
		exit(0);
		return 0;
	}
	//****************서버로 부터 캠소켓 잘 연결됐다고 'o'를 전송받습니다***********//
	retval=recv(*CamSock,&temp,sizeof(char),0);
	if(sizeof(char)!=retval || temp!='o'){
		AfxMessageBox(_T("클라이언트 : 서버에게 'o'를 수신하지 못했습니다."));
		exit(0);
		return 0;
	}
	//*****************************************************//
	
	DataSock = new SOCKET();
	SOCKADDR_IN DataservAddr;

	if(WSAStartup(MAKEWORD(2,2),&wsaData) !=0)
		AfxMessageBox(_T("WSAStartup()에서 에러가 발생하였습니다."));
	
	*DataSock=socket(PF_INET, SOCK_STREAM, 0);
	if(*DataSock==INVALID_SOCKET){
		AfxMessageBox(_T("hServSock()에서 에러가 발생하였습니다."));
		return FALSE;
	}

	memset(&DataservAddr, 0, sizeof(DataservAddr));
	DataservAddr.sin_family=AF_INET; 
	DataservAddr.sin_addr.s_addr=inet_addr(M_IP);
	DataservAddr.sin_port=htons(PORT2);
	
	if(connect(*DataSock,(SOCKADDR *)&DataservAddr, sizeof(DataservAddr))==SOCKET_ERROR){
		AfxMessageBox(_T("서버 접속에 실패하였습니다.(데이터소켓 실패)"));
		return FALSE;
	}

	////********Data소켓이 연결됐다고 서버에 알려준다********//
	temp = 'k';
	if(sizeof(char)!=send(*DataSock,&temp,sizeof(char),0)){
		AfxMessageBox(_T("클라이언트 : 데이터소켓에 'k'를 제대로 전송하지 못했습니다."));
		exit(0);
		return 0;
	};
	////*****************************************************//

	////**********Data소켓이 잘 연결 됐다고 서버로 부터 받아옵니다.****************//
	retval = recv(*DataSock,&temp,sizeof(char),0);
	if(sizeof(char)!=retval || temp!='k'){
		AfxMessageBox(_T("클라이언트 : 데이터소켓으로부터 'k'를 제대로 수신하지 못했습니다."));
		exit(0);
		return 0;
	};;
	////**********************데이터 소켓 연결 끝입니다******************************///
	//

	////******************상대방의 이름을 수신합니다.**********************//
	char * namebuf=new char[NAMESIZE];
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


	////**************** 상대방에게 클라이언트의 이름을 전송합니다.********************//
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
	
	//받을 캠 스레드 셋팅
	CamThreadArg_recv * _CamThreadArg_recv = new CamThreadArg_recv();
	_CamThreadArg_recv->dlg=this;
	_CamThreadArg_recv->m_pChild = CamSock;
	HANDLE hCam_recv = AfxBeginThread(CamThread_recv,(void *)_CamThreadArg_recv,THREAD_PRIORITY_NORMAL, 0, 0);

	//보낼 캠 스레드 셋팅
	CamThreadArg * _CamThreadArg = new CamThreadArg();
	_CamThreadArg->dlg=this;
	_CamThreadArg->m_pChild=CamSock;
	HANDLE hCam = (HANDLE)AfxBeginThread(CamThread,_CamThreadArg,THREAD_PRIORITY_NORMAL, 0, 0);
	
	DrawArg *_DrawArg = new DrawArg();
	_DrawArg->dlg = this;
	_DrawArg->server = DataSock;
	HANDLE hDraw = (HANDLE)AfxBeginThread(DataThread,_DrawArg,THREAD_PRIORITY_HIGHEST,0,0);

	
	HANDLE hArg[3];
	hArg[0]=hCam;
	hArg[1]=hCam_recv;
	hArg[2]=hDraw; 
	WaitForMultipleObjects(3,hArg,TRUE,INFINITE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CVTSClntDlg::OnLButtonDown(UINT nFlags, CPoint point)
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


void CVTSClntDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
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


void CVTSClntDlg::OnMouseMove(UINT nFlags, CPoint point)
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

void CVTSClntDlg::OnEnChangeFilepath()
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


void CVTSClntDlg::OnClickedMsgsend()
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


void CVTSClntDlg::OnClickedErase()
{
	LIArg *arg = new LIArg;
	arg->dlg=this;
	arg->SORC='S';
	AfxBeginThread(LoadImage,arg,THREAD_PRIORITY_NORMAL, 0, 0);
	// TODO: ¿©±â¿¡ ÄÁÆ®·Ñ ¾Ë¸² Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
}

