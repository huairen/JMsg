#ifndef MAINDLG_H_
#define MAINDLG_H_

#include "win32/JuiFrame.hxx"

class MainDlg : public JuiFrame
{
public:
	MainDlg();
	~MainDlg();

	void OnControlEvent(JuiControl* sender, int message, int param);

protected:
	bool HandleCreate(LPCREATESTRUCT lpCS);
	bool HandleSysCommand(UINT uCmdType, POINTS pt);
};

#endif