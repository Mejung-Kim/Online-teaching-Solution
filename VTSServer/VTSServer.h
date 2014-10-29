
// VTSServer.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.


// CVTSServerApp:
// 이 클래스의 구현에 대해서는 VTSServer.cpp을 참조하십시오.
//

class CVTSServerApp : public CWinApp
{
public:
	CVTSServerApp();

// 재정의입니다.
public:
	virtual BOOL InitInstance();
	HANDLE h_FindUserThread;
	SOCKET hServSock;
// 구현입니다.
public:
	int isTeacher;
	DECLARE_MESSAGE_MAP()
	CvvImage Board;
};

extern CVTSServerApp theApp;
