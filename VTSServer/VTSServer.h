
// VTSServer.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CVTSServerApp:
// �� Ŭ������ ������ ���ؼ��� VTSServer.cpp�� �����Ͻʽÿ�.
//

class CVTSServerApp : public CWinApp
{
public:
	CVTSServerApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	HANDLE h_FindUserThread;
	SOCKET hServSock;
// �����Դϴ�.
public:
	int isTeacher;
	DECLARE_MESSAGE_MAP()
	CvvImage Board;
};

extern CVTSServerApp theApp;
