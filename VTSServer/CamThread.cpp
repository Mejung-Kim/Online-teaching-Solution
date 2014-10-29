#include "stdafx.h"
#include "CamThread.h"
#include <WinSock2.h>
#include "VTSClntDlg.h"


#define BUFSIZE				4096
#define DATASIZE			4096-sizeof(int)*2
#define FILE_NAME			"cam.jpg"


UINT CamThread(LPVOID arg)
{
	//�Ű����� ĳ����
	CamThreadArg *pArg =(CamThreadArg *)arg;
	//���� �и�
	SOCKET * CamSocket = pArg->m_pChild;
	CWnd *CamWnd = pArg->dlg->GetDlgItem(IDC_Cam);


	IplImage *img;
	CvvImage selfCam;
	CvCapture *capture;
	CvMat *pJpeg; 
	LPBYTE pBufferJpg; //���� �������� �����͸� ���� ����

	HDC hdc;
	CDC dc;
	
	//Ŭ���̾�Ʈ ������ ������
	RECT rect;
	CamWnd->GetClientRect(&rect); 

		//�ڵ� ������
	hdc = GetDC(CamWnd->m_hWnd);
	dc.Attach(hdc);
	char buf[BUFSIZE];
	char data[DATASIZE];
	int nIndex=0;
	int nData=0;
	int nLeft=0;
	int bytes=0;
	int retval=0;
	int nSend=0;
	int fileLength=0;
	CString str;
	
	//ī�޶� ����
	capture = cvCreateCameraCapture(0);



	while(1)
	{
		//ī�޶�� ���� ����
		while(TRUE){
   			img = cvQueryFrame(capture);
			if(img!=NULL)
				break;
		}
		//************* IplImage -> jpg ���ڵ� �κ� *********************//
		pJpeg = cvEncodeImage(".jpg",img); //���� ������ jpg �������� ���ڵ��� �� CvMat ������ pJpeg�� ����
		pBufferJpg = pJpeg->data.ptr;  //pJpeg���� �ΰ� ���� ����, �����͸� ����
		fileLength = (int)pJpeg->cols; //������ ��ü �������� ũ�� ����

		//********************���徿 ���������� �ʱ�ȭ�� �κ�*************************//
		nLeft = fileLength; //�����ؾ��� ũ�� 
		nSend = 0; //����ũ��
		nData = DATASIZE; //������ �������� ũ�⸦ 4KB�� �ʱ�ȭ, �������� 4KB���� ���� �����͸� ���� ��쿡 �̰��� �����Ѵ�.

		//*********************** ���� �޽��� ���� �κ� ***************************//
		nIndex = 1; //�ε��� ��ȣ 1�� �༭ ������ ������ �˸�
		ZeroMemory(buf, BUFSIZE); //���۸� ���
		memcpy(buf, &nIndex, sizeof(int)); //���ۿ� 1�� ����
		memcpy(buf+sizeof(int),&fileLength,sizeof(int)); //���� ��ü ũ�⸦ ����
		if(BUFSIZE!=send(*CamSocket,buf, BUFSIZE,0)){
			AfxMessageBox(_T("ķ ���� �����忡�� ���� ���� �޽��� ���� ����"));
			exit(0); //���� ���� �޽��� ����
		}
		//********************** ���� �̹��� ���� �κ� *******************************//
		for( int i=2 ; i != 100 ; i++ )
		{
			//���� �ʱ�ȭ
			ZeroMemory(buf, BUFSIZE);
			ZeroMemory(data, DATASIZE);

			memcpy(data, pBufferJpg+nSend, nData);

			nLeft -= nData;
			nSend += nData;

			//�о�� ���ϳ����, ��Ŷ�� �����Ѵ�4
			nIndex = i;
			memcpy(buf, &nIndex, sizeof(int));
			memcpy(buf+sizeof(int), &nData, sizeof(int));
			memcpy(buf+sizeof(int)*2, data, nData);
			
			/* �����ϴ� �κ� */
			retval = send(*CamSocket,buf, BUFSIZE,0);
			
			/* ���� ó�� �κ� */
			if(retval==-1){
				CString MsgErr;
				 MsgErr.Format(L"%d ����", GetLastError()); //�����޼����� �����.
				 AfxMessageBox(MsgErr); //�����޼��� â�� ����
				 exit(0);
				 return 1;
			}
			else if(retval == 0){
				AfxMessageBox(_T("������ ���������ϴ�."));
				exit(0);
				return 1;
			}
			else if(retval!=BUFSIZE)
			{	
				str.Format(_T("->���� ũ��: %d"),retval);
				AfxMessageBox(_T("���� ���� �߿� ���� ���Ͽ� �̻��� �ֽ��ϴ�.")+str);
				exit(0);
				return 1;
			}
			

			//���� �Ѱ� ������ ��ģ ���. ���� �˸��� ���� ���� ���� ����
			if(nLeft <= 0){
				ZeroMemory(buf, BUFSIZE);

				nIndex = 0;	//������ ���� �˸�
				memcpy(buf, &nIndex, sizeof(int));

				if(BUFSIZE!=send(*CamSocket,buf, BUFSIZE,0)){
					AfxMessageBox(_T("������ ķ SEND ����"));
					exit(0);
				}
				break;
		
			}
			else if(nLeft<DATASIZE){
				nData = nLeft;
			}
		}
		
			/* �ڽ��� ķ�� ����ϴ� �κ� */
			//�ڵ� ������
		
	
			selfCam.CopyOf(img,1); //IplImage -> CvvImage
			EnterCriticalSection(cs);; //�Ӱ迵�� ����
			selfCam.DrawToHDC(dc.m_hDC, &rect); //�׸���
			LeaveCriticalSection(cs); //�Ӱ迵�� ����
			
			//������ ���̴� 0.1�ʷ� �ϰ� ESC�� ������ ����ǵ��� �մϴ�.
			if( cvWaitKey(10) == 27)
				break; 
	}
		
		ReleaseDC(CamWnd->m_hWnd,hdc); 
		
		cvReleaseCapture(&capture); //ī�޶� ����
		return 0;
}

UINT CamThread_recv(LPVOID arg)
{
	CamThreadArg_recv *pArg =(CamThreadArg_recv *)arg; //�Ű����� ĳ����
	SOCKET * CamSocket = pArg->m_pChild; 	//���� �и�
	CWnd *CamWnd = pArg->dlg->GetDlgItem(IDC_Cam2);
	
	//������Ʈ�� �簢�� ����
	RECT rect;
	CamWnd->GetClientRect(&rect);
	HDC hDC;
	CDC dc;
	CvvImage recvCam;	
	CvMat *matJpg; 
	IplImage* pImageFull;
	hDC = GetDC(CamWnd->m_hWnd);
	dc.Attach(hDC);

	//�ʿ��� ������
	int nIndex			=0;
	int nData			=0;
	int retval			=0;
	int fileLength		=0;
	int nRecv=0;
	char buf[BUFSIZE]; // �����͸� �޾ƿ� ����  
	char data[DATASIZE]; //buf���� ���� �����͸� �и��� �� ����

	CString str;
	//*****************���� ���� �κ�*************************//
	for(int i=0; i<1000 ;i++)
	{
		retval = recv(*CamSocket,buf, BUFSIZE,MSG_WAITALL); //��Ŷ �ϳ� ����
		//����ó��
		if(retval == 0)
		{	
			AfxMessageBox(_T("�������� ������ ����Ǿ����ϴ�."));
			exit(0);
			return 1;
		}
		else if(retval != BUFSIZE)
		{	
			str.Format(_T("->���� ũ��: %d"),retval);
			AfxMessageBox(_T("SORC : ���� ���� �߿� ���� ���Ͽ� �̻��� �ֽ��ϴ�.")+str);
			exit(0);
			return 1;
		}

		//���۸� �˸°� �и��մϴ�.
		memcpy(&nIndex,buf,sizeof(int)); //�ε����� ���ɴϴ�.
		if(nIndex==1) //���� �޽������ ���� ���̸� �޾ƿɴϴ�.
			memcpy(&fileLength,buf+sizeof(int),sizeof(int));

		else{
			memcpy(&nData,buf+sizeof(int),sizeof(int));
			memcpy(data,buf+sizeof(int)*2,nData);
		}

		//�Ѱ��� jpg���� ������ ���� �ε����� 0���� �ϰ�, �۾��� ����ϴ�.	
		if(nIndex == 0)
		{	
			pImageFull = cvDecodeImage(matJpg);
			cvReleaseMat(&matJpg);
			 
			recvCam.CopyOf( pImageFull,1);


			EnterCriticalSection(cs);
			recvCam.DrawToHDC(dc.m_hDC, &rect);		
					
			LeaveCriticalSection(cs);
			cvReleaseImage(&pImageFull);

			//***�̹��� �ϳ� ���� ������ �ʱ�ȭ �� �κ� ****//
			nRecv=0;
			i=0;
		}

		else if(nIndex==1)
		{
			matJpg = cvCreateMat( 1, fileLength, CV_8UC1 );
		}
		else
		{
			memcpy(matJpg->data.ptr+nRecv,data,nData);
			nRecv += nData;
		}
	}
	ReleaseDC(CamWnd->m_hWnd,hDC);
}
