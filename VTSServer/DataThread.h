#pragma once
#include "stdafx.h"
typedef struct ops
{
	int x;
	int y;
	int b_Down;
}ops;

typedef struct DrawArg
{
	SOCKET * server;
	CWnd * dlg;
}DrawArg;;

typedef struct SMArg
{
	SOCKET *server;
	char *msgbuf;
	int *len;
}SMArg;