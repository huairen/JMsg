#ifndef MAINDLG_H_
#define MAINDLG_H_

#include "base/win32/JuiFrame.h"

struct user_info;
class JuiListBox;

class MainDlg : public JuiFrame
{
public:
	MainDlg();
	~MainDlg();

	void OnControlEvent(JuiControl* sender, int message, int param);

	void OnRecvMsg();
	static void ThreadFunc(void *arg);

protected:
	bool HandleCreate(LPCREATESTRUCT lpCS);
	bool HandleSysCommand(UINT uCmdType, POINTS pt);

	void OnFriendOnline(user_info *user);

private:
	JuiListBox* m_FriendList;
};

#endif