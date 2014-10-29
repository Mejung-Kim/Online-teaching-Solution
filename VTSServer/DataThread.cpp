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
			AfxMessageBox(_T("���콺 ���� ���� ���������Ф�"));
			exit(0);
			continue;
		}
		memcpy(&_ops.x,buf,sizeof(int));
		memcpy(&_ops.y,buf+sizeof(int),sizeof(int));
		memcpy(&_ops.b_Down,buf+sizeof(int)*2,sizeof(int));

		// ä�� �޽��� �������� ���
		if(_ops.x<0){
			char msgbuf[MSGSIZE]={0};
			WCHAR strData[MSGSIZE]={0};
			CString strData2;
			int wlen=0;
			//�޽��� ũ�� ��ŭ ������ ����, ������ �������� ������ �����
			while(TRUE){
				retval=recv(*server,msgbuf,_ops.y,0);
				if(retval==_ops.y)
					break;
				else if(retval==-1){
					AfxMessageBox(_T("�����ͼ��Ͽ� �̻��� ������ϴ�. ���α׷��� �����մϴ�."));
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
	//���� �޽��� ����
	memcpy(buf,&index,sizeof(int));
	memcpy(buf+sizeof(int),len,sizeof(int));

	//���� �޽��� ����
	while(TRUE){
		retval=send(*DataSock,buf,sizeof(buf),0);
		if(retval==sizeof(buf))
			break;
	} //����� ���� �� ���� �ݺ� ����

	//�� �޽��� ����
	while(TRUE){
		retval=send(*DataSock,msgbuf,*len,0);
		if(retval==*len)
			break;
	} //����� ���� �� ���� �ݺ� ����
	cs.Unlock();
	
	delete[] len;
	delete[] msgbuf;
	delete[] arg;
	return 0;
}