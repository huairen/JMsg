#include "MainDlg.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	MainDlg wnd;
	if(!wnd.Create("JMsg", WS_POPUP))
		return 0;

	wnd.MoveToCenter();
	wnd.Show(SW_SHOWDEFAULT, true);

	return MainDlg::MessageLoop();
}