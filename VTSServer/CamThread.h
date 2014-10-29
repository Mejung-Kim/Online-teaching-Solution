#pragma once
#include "stdafx.h"
typedef struct _CamThreadArg
{
	SOCKET *m_pChild;
	CWnd *dlg;
}CamThreadArg;

typedef struct _CamThreadArg_recv
{
	SOCKET *m_pChild;
	CWnd *dlg;
}CamThreadArg_recv;

