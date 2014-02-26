#include "ipmsg/ipmsg.h"
#include "MainDlg.h"
#include "resource.h"
#include "Graphics/JRenderSystem.h"
#include "base/JuiReader.h"
#include "controls/JuiButton.h"
#include "controls/JuiListBox.h"

#include "user.h"

#include <process.h>


MainDlg::MainDlg()
{
	m_FriendList = NULL;
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

	JRenderSystem::GetInstance().CreateRenderer("GDI", (uint32_t)m_hWnd);

	JuiReader reader;
	reader.LoadScript(this, "res/default/mainframe.txt");

	m_FriendList = dynamic_cast<JuiListBox*>(FindControl("friends"));
	if(m_FriendList != NULL)
	{
		JuiControl* pItem = FindControl("friend_item");
		m_FriendList->SetItemModel(pItem);
	}

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
	const char *groupName;

	if(user->group_name[0])
		groupName = user->group_name;
	else
		groupName = "noneGroup";

	if(m_FriendList != NULL)
	{
		m_FriendList->PushDefaultItem();
	}
}