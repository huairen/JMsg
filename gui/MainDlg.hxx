#ifndef MAINDLG_H_
#define MAINDLG_H_

#include "win32/JuiFrame.hxx"

struct user_info;

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
};

#endif