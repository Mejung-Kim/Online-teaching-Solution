#include "stdafx.h"
#include "VTSServerDlg.h"
#include "VTSClntDlg.h"
#include "LoadImage.h"
UINT LoadImage(LPVOID Arg)
{
	LIArg * pArg = (LIArg *)Arg;
	CWnd * dlg= pArg->dlg;
	char SORC = pArg->SORC;

	CvvImage LoadImage;
	CWnd *BoardWnd;
	
	if(FALSE==LoadImage.Load("problem.jpg",1)){
		AfxMessageBox(_T("읽어오기를 실패하였습니다."));
		return 0;
	}
	
	if(SORC == 'S')
		BoardWnd= ((CVTSServerDlg *)dlg)->GetDlgItem(IDC_Board);
	else
		BoardWnd= ((CVTSClntDlg *)dlg)->GetDlgItem(IDC_Board);
	
	RECT rect;
	BoardWnd->GetClientRect(&rect);
	HDC hDC = GetDC(BoardWnd->m_hWnd);
	CDC dc;
	if(!dc.Attach(hDC)){
		AfxMessageBox(_T("읽어오기를 실패하였습니다."));
		return 0;
	};

	LoadImage.DrawToHDC(dc.m_hDC, &rect);	
		
	return 0;
}

