#include "MainDlg.hxx"
#include "graphics/gdi/JGdiRender.hxx"
#include "core/script/JuiCreater.hxx"
#include "../build/vc110/JMsg/resource.hxx"

JGdiRender g_GdiRender;

MainDlg::MainDlg()
{
	sm_pRender = &g_GdiRender;
}

MainDlg::~MainDlg()
{

}

void MainDlg::OnControlEvent( JuiControl* sender, int message, int param )
{
	if(message == CTRL_MSG_CLICK)
	{
		if(_stricmp(sender->GetName(), "close") == 0)
			Close(0);
		else if(_stricmp(sender->GetName(), "min") == 0)
			SendMessage(m_hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
		else if(_stricmp(sender->GetName(), "max") == 0)
			SendMessage(m_hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		else if(_stricmp(sender->GetName(), "restore") == 0)
			SendMessage(m_hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
	}
}

bool MainDlg::HandleCreate( LPCREATESTRUCT lpCS )
{
	SetIcon(IDI_JMSG_ICO);
	g_GdiRender.SetWindowHandler(m_hWnd);

	JuiCreater::LoadScript(this, "resource/default/mainframe.dlg");
	return true;
}

bool MainDlg::HandleSysCommand( UINT uCmdType, POINTS pt )
{
	BOOL bZoomed = ::IsZoomed(m_hWnd);
	LRESULT lRes = DefaultWndProc(WM_SYSCOMMAND, uCmdType, MAKELPARAM(pt.x, pt.y));

	if (::IsZoomed(m_hWnd) != bZoomed)
	{
		JuiControl *pMaxCtrl = FileControl("max");
		JuiControl *pRestoreCtrl = FileControl("restore");
		if(bZoomed)
		{
			pRestoreCtrl->RemoveFlag(CTRL_FLAG_VISIBLE);
			pMaxCtrl->AddFlag(CTRL_FLAG_VISIBLE);
		}
		else
		{
			pMaxCtrl->RemoveFlag(CTRL_FLAG_VISIBLE);
			pRestoreCtrl->AddFlag(CTRL_FLAG_VISIBLE);
		}
	}

	return true;
}
