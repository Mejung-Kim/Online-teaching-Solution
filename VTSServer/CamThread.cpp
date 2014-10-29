#include "stdafx.h"
#include "CamThread.h"
#include <WinSock2.h>
#include "VTSClntDlg.h"


#define BUFSIZE				4096
#define DATASIZE			4096-sizeof(int)*2
#define FILE_NAME			"cam.jpg"


UINT CamThread(LPVOID arg)
{
	//매개변수 캐스팅
	CamThreadArg *pArg =(CamThreadArg *)arg;
	//소켓 분리
	SOCKET * CamSocket = pArg->m_pChild;
	CWnd *CamWnd = pArg->dlg->GetDlgItem(IDC_Cam);


	IplImage *img;
	CvvImage selfCam;
	CvCapture *capture;
	CvMat *pJpeg; 
	LPBYTE pBufferJpg; //실제 보내야할 데이터만 담기는 버퍼

	HDC hdc;
	CDC dc;
	
	//클라이언트 윈도우 얻어오기
	RECT rect;
	CamWnd->GetClientRect(&rect); 

		//핸들 얻어오기
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
	
	//카메라 연결
	capture = cvCreateCameraCapture(0);



	while(1)
	{
		//카메라로 한장 찍음
		while(TRUE){
   			img = cvQueryFrame(capture);
			if(img!=NULL)
				break;
		}
		//************* IplImage -> jpg 인코딩 부분 *********************//
		pJpeg = cvEncodeImage(".jpg",img); //찍은 사진을 jpg 형식으로 인코딩한 뒤 CvMat 형식의 pJpeg에 저장
		pBufferJpg = pJpeg->data.ptr;  //pJpeg에서 부가 정보 빼고, 데이터만 추출
		fileLength = (int)pJpeg->cols; //보내는 전체 데이터의 크기 구함

		//********************한장씩 보낼때마다 초기화할 부분*************************//
		nLeft = fileLength; //전송해야할 크기 
		nSend = 0; //보낸크기
		nData = DATASIZE; //보내는 데이터의 크기를 4KB로 초기화, 마지막에 4KB보다 적은 데이터만 남은 경우에 이값을 수정한다.

		//*********************** 시작 메시지 전송 부분 ***************************//
		nIndex = 1; //인덱스 번호 1을 줘서 파일의 시작을 알림
		ZeroMemory(buf, BUFSIZE); //버퍼를 비움
		memcpy(buf, &nIndex, sizeof(int)); //버퍼에 1을 담음
		memcpy(buf+sizeof(int),&fileLength,sizeof(int)); //파일 전체 크기를 보냄
		if(BUFSIZE!=send(*CamSocket,buf, BUFSIZE,0)){
			AfxMessageBox(_T("캠 전송 스레드에서 파일 시작 메시지 전송 에러"));
			exit(0); //파일 시작 메시지 보냄
		}
		//********************** 실제 이미지 전송 부분 *******************************//
		for( int i=2 ; i != 100 ; i++ )
		{
			//버퍼 초기화
			ZeroMemory(buf, BUFSIZE);
			ZeroMemory(data, DATASIZE);

			memcpy(data, pBufferJpg+nSend, nData);

			nLeft -= nData;
			nSend += nData;

			//읽어온 파일내용과, 패킷을 포장한다4
			nIndex = i;
			memcpy(buf, &nIndex, sizeof(int));
			memcpy(buf+sizeof(int), &nData, sizeof(int));
			memcpy(buf+sizeof(int)*2, data, nData);
			
			/* 전송하는 부분 */
			retval = send(*CamSocket,buf, BUFSIZE,0);
			
			/* 오류 처리 부분 */
			if(retval==-1){
				CString MsgErr;
				 MsgErr.Format(L"%d 에러", GetLastError()); //에러메세지를 만든다.
				 AfxMessageBox(MsgErr); //에러메세지 창을 띄운다
				 exit(0);
				 return 1;
			}
			else if(retval == 0){
				AfxMessageBox(_T("접속이 끊어졌습니다."));
				exit(0);
				return 1;
			}
			else if(retval!=BUFSIZE)
			{	
				str.Format(_T("->보낸 크기: %d"),retval);
				AfxMessageBox(_T("사진 전송 중에 동기 소켓에 이상이 있습니다.")+str);
				exit(0);
				return 1;
			}
			

			//파일 한개 전송을 마친 경우. 끝을 알리고 파일 리드 루프 종료
			if(nLeft <= 0){
				ZeroMemory(buf, BUFSIZE);

				nIndex = 0;	//파일의 끝을 알림
				memcpy(buf, &nIndex, sizeof(int));

				if(BUFSIZE!=send(*CamSocket,buf, BUFSIZE,0)){
					AfxMessageBox(_T("보내는 캠 SEND 에러"));
					exit(0);
				}
				break;
		
			}
			else if(nLeft<DATASIZE){
				nData = nLeft;
			}
		}
		
			/* 자신의 캠을 출력하는 부분 */
			//핸들 얻어오기
		
	
			selfCam.CopyOf(img,1); //IplImage -> CvvImage
			EnterCriticalSection(cs);; //임계영역 진입
			selfCam.DrawToHDC(dc.m_hDC, &rect); //그리기
			LeaveCriticalSection(cs); //임계영역 해제
			
			//프레임 길이는 0.1초로 하고 ESC를 누르면 종료되도록 합니다.
			if( cvWaitKey(10) == 27)
				break; 
	}
		
		ReleaseDC(CamWnd->m_hWnd,hdc); 
		
		cvReleaseCapture(&capture); //카메라 해제
		return 0;
}

UINT CamThread_recv(LPVOID arg)
{
	CamThreadArg_recv *pArg =(CamThreadArg_recv *)arg; //매개변수 캐스팅
	SOCKET * CamSocket = pArg->m_pChild; 	//소켓 분리
	CWnd *CamWnd = pArg->dlg->GetDlgItem(IDC_Cam2);
	
	//픽쳐컨트롤 사각형 얻어옴
	RECT rect;
	CamWnd->GetClientRect(&rect);
	HDC hDC;
	CDC dc;
	CvvImage recvCam;	
	CvMat *matJpg; 
	IplImage* pImageFull;
	hDC = GetDC(CamWnd->m_hWnd);
	dc.Attach(hDC);

	//필요한 변수들
	int nIndex			=0;
	int nData			=0;
	int retval			=0;
	int fileLength		=0;
	int nRecv=0;
	char buf[BUFSIZE]; // 데이터를 받아올 버퍼  
	char data[DATASIZE]; //buf에서 실제 데이터만 분리해 올 버퍼

	CString str;
	//*****************파일 수신 부분*************************//
	for(int i=0; i<1000 ;i++)
	{
		retval = recv(*CamSocket,buf, BUFSIZE,MSG_WAITALL); //패킷 하나 수신
		//에러처리
		if(retval == 0)
		{	
			AfxMessageBox(_T("서버와의 접속이 종료되었습니다."));
			exit(0);
			return 1;
		}
		else if(retval != BUFSIZE)
		{	
			str.Format(_T("->받은 크기: %d"),retval);
			AfxMessageBox(_T("SORC : 사진 수신 중에 동기 소켓에 이상이 있습니다.")+str);
			exit(0);
			return 1;
		}

		//버퍼를 알맞게 분리합니다.
		memcpy(&nIndex,buf,sizeof(int)); //인덱스를 얻어옵니다.
		if(nIndex==1) //시작 메시지라면 파일 길이를 받아옵니다.
			memcpy(&fileLength,buf+sizeof(int),sizeof(int));

		else{
			memcpy(&nData,buf+sizeof(int),sizeof(int));
			memcpy(data,buf+sizeof(int)*2,nData);
		}

		//한개의 jpg파일 끝까지 오면 인덱스는 0으로 하고, 작업을 멈춥니다.	
		if(nIndex == 0)
		{	
			pImageFull = cvDecodeImage(matJpg);
			cvReleaseMat(&matJpg);
			 
			recvCam.CopyOf( pImageFull,1);


			EnterCriticalSection(cs);
			recvCam.DrawToHDC(dc.m_hDC, &rect);		
					
			LeaveCriticalSection(cs);
			cvReleaseImage(&pImageFull);

			//***이미지 하나 받을 때마다 초기화 할 부분 ****//
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
