#include "stdafx.h"
#include "DataThread.h"
#include <WinSock2.h>
#include "VTSServerDlg.h"

#define BUFSIZE sizeof(int)*3
#define MSGSIZE 100

UINT DataThread(LPVOID arg)
{
 	DrawArg * parg = (DrawArg *) arg;
	CWnd * dlg = parg->dlg;
	SOCKET * server = parg->server;
	char buf[BUFSIZE]={0};
	ops _ops;
	ops befor;
	int retval;

	_ops.x=0;
	_ops.y=0;
	_ops.b_Down=0;
	CClientDC dc(dlg);
	
	while(TRUE){
		befor.x=_ops.x;
		befor.y=_ops.y;
		befor.b_Down=_ops.b_Down;
		
		retval = recv(*server,buf, sizeof(int)*3,0);
		if(retval != sizeof(int)*3){
			AfxMessageBox(_T("마우스 정보 수신 오류났어요ㅠㅠ"));
			exit(0);
			continue;
		}
		memcpy(&_ops.x,buf,sizeof(int));
		memcpy(&_ops.y,buf+sizeof(int),sizeof(int));
		memcpy(&_ops.b_Down,buf+sizeof(int)*2,sizeof(int));

		// 채팅 메시지 데이터인 경우
		if(_ops.x<0){
			char msgbuf[MSGSIZE]={0};
			WCHAR strData[MSGSIZE]={0};
			CString strData2;
			int wlen=0;
			//메시지 크기 만큼 데이터 수신, 완전히 수신하지 않으면 재수신
			while(TRUE){
				retval=recv(*server,msgbuf,_ops.y,0);
				if(retval==_ops.y)
					break;
				else if(retval==-1){
					AfxMessageBox(_T("데이터소켓에 이상이 생겼습니다. 프로그램을 종료합니다."));
					exit(0);
				}
			};
			MultiByteToWideChar(CP_ACP,0,msgbuf,-1,strData,_ops.y*2);
			strData2.Format(M_username_client+_T(" : %s"),strData);
			((CListBox *)dlg->GetDlgItem(IDC_ChatList))->InsertString(-1,strData2);
		}

		if(_ops.b_Down&&befor.b_Down){
			dc.MoveTo(befor.x,befor.y);
			dc.LineTo(_ops.x,_ops.y);
		}
	}

}

UINT SendMsgThread(LPVOID arg)
{
	SMArg * pArg = (SMArg *)arg;
	SOCKET * DataSock = pArg->server;
	char buf[sizeof(int)*3]={0};
	char * msgbuf = pArg->msgbuf;
	int index = -1;
	int *len = pArg->len;
	int retval=0;
	
	cs.Lock();
	//시작 메시지 셋팅
	memcpy(buf,&index,sizeof(int));
	memcpy(buf+sizeof(int),len,sizeof(int));

	//시작 메시지 전송
	while(TRUE){
		retval=send(*DataSock,buf,sizeof(buf),0);
		if(retval==sizeof(buf))
			break;
	} //제대로 보낼 때 까지 반복 전송

	//본 메시지 전송
	while(TRUE){
		retval=send(*DataSock,msgbuf,*len,0);
		if(retval==*len)
			break;
	} //제대로 보낼 때 까지 반복 전송
	cs.Unlock();
	
	delete[] len;
	delete[] msgbuf;
	delete[] arg;
	return 0;
}