#include "ipmsg/ipmsg.h"
#include "MainDlg.h"
#include "../build/vc110/JMsg/resource.h"
#include "graphics/gdi/JGdiRender.h"
#include "base/JuiReader.h"
#include "button/JuiButton.h"

#include "user.h"

#include <process.h>

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
	if(message == JuiControl::MSG_CLICK)
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

void MainDlg::OnRecvMsg()
{
	char buff[1024] = {0};
	uint32_t user_id;
	struct msg_packet msg;

	while(1)
	{
		msg_recv(&msg);
		user_id = process_msg(&msg);

		switch (GET_MODE(msg.command))
		{
		case IPMSG_SENDMSG:
			break;

		case IPMSG_ANSENTRY:
		case IPMSG_BR_ENTRY:
			OnFriendOnline(user_find(user_id));
			break;

		case IPMSG_BR_EXIT:
			break;
		}
	}
}

void MainDlg::ThreadFunc( void *arg )
{
	MainDlg *dlg = (MainDlg*)arg;
	dlg->OnRecvMsg();
}

bool MainDlg::HandleCreate( LPCREATESTRUCT lpCS )
{
	SetIcon(IDI_JMSG_ICO);
	g_GdiRender.SetWindowHandler(m_hWnd);

	JuiReader reader;
	reader.LoadScript(this, "resource/default/mainframe.dlg");

	ipmsg_init();
	_beginthread(&MainDlg::ThreadFunc, 0, this);

	return true;
}

bool MainDlg::HandleSysCommand( UINT uCmdType, POINTS pt )
{
	BOOL bZoomed = ::IsZoomed(m_hWnd);
	LRESULT lRes = DefaultWndProc(WM_SYSCOMMAND, uCmdType, MAKELPARAM(pt.x, pt.y));

	if (::IsZoomed(m_hWnd) != bZoomed)
	{
		JuiControl *pMaxCtrl = FindControl("max");
		JuiControl *pRestoreCtrl = FindControl("restore");
		if(bZoomed)
		{
			pRestoreCtrl->SetVisible(false);
			pMaxCtrl->SetVisible(true);
		}
		else
		{
			pMaxCtrl->SetVisible(false);
			pRestoreCtrl->SetVisible(true);
		}
	}

	return true;
}


void MainDlg::OnFriendOnline( user_info *user )
{
	JuiContainer *groupCtrl;
	const char *groupName;

	if(user->group_name[0])
		groupName = user->group_name;
	else
		groupName = "noneGroup";

	groupCtrl = dynamic_cast<JuiContainer*>(FindControl(groupName));
	if(groupCtrl == NULL)
	{
		JuiContainer *page = dynamic_cast<JuiContainer*>(FindControl("friends"));
		if(page != NULL)
		{
// 			JuiRollout *roll = new JuiRollout;
// 			roll->SetBounds(page->GetPosition(), page->GetExtent());
// 			page->AddControl(roll);

		}
	}
}