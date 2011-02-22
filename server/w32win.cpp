static const char *RcsId = "$Header$";
static const char *Vers = "$Revision$";

//+=============================================================================
//
// file :               w32win.cpp
//
// description :        C++ source for the main Tango device server output
//			window uses with Windows.
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// $Revision$
//
// $Log$
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//-=============================================================================

#include <w32win.h>
#include <winnt_lib/tango_static/resource.h>

namespace Tango
{

static HINSTANCE hInst;
static TCHAR szWindowClass[40] = "TangoDS_Window_Class";
static int Main_Win_width;
static int Main_Win_height;
static HDC Main_Win_MemDC;
static LOGFONT lf;
static LOGFONT user_font;

//+----------------------------------------------------------------------------
//
// method : 		W32Win::W32Win()
// 
// description : 	Constructor of the W32Win class. It mainly creates
//                      the server output main window
//
//-----------------------------------------------------------------------------


W32Win::W32Win(Util *ptr,int nCmd)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	hInst = hInstance;
	
	RegisterTangoClass(hInstance);
		
	InitInstance(hInstance,nCmd);

	pcb = new CoutBuf(hInstance,nCmd,win,ptr->get_ds_name().c_str());
}


W32Win::~W32Win()
{
}

void W32Win::RegisterTangoClass(HINSTANCE hInstance)
{
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)TangoWndProc;
	wc.cbClsExtra	= 0;
	wc.cbWndExtra	= 0;
	wc.hInstance	= hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(TANGO_ICON1));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName = (LPCSTR)TANGO_MENU1;
	wc.lpszClassName = szWindowClass;

	ATOM at = RegisterClass(&wc);
	if (at == 0)
	{
                Except::throw_exception((LPCSTR)"API_NtDebugWindowError",
				      (LPCSTR)"Can't register class for server main output window",
                                      (LPCSTR)"W32Win::RegisterTangoClass");
	}
}

void W32Win::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

//
// Build window title
//

	TCHAR szTitle[80];
	strcpy(szTitle,"Tango device server : ");
	Tango::Util *tg = Tango::Util::instance();
	strcat(szTitle,tg->get_ds_name().c_str());

//
// Create window
//

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
			    CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL,
			    hInstance, NULL);

	if (!hWnd)
	{
                Except::throw_exception((LPCSTR)"API_NtDebugWindowError",
				      (LPCSTR)"Can't create main device server window",
                                      (LPCSTR)"W32Win::InitInstance");

	}
	win = hWnd;

//
// Set background bitmap
//

	HBITMAP dancers = LoadBitmap(hInst,MAKEINTRESOURCE(TANGO_BITMAP1));
	BITMAP dancers_bm;
	GetObject(dancers,sizeof(BITMAP),&dancers_bm);

	HDC dc = GetDC(hWnd);
	HDC mem_dc = CreateCompatibleDC(dc);
	SelectObject(mem_dc,dancers);

	RECT wnd,client,calc;
	GetWindowRect(hWnd,&wnd);
	GetClientRect(hWnd,&client);
	calc.right = wnd.right - wnd.left - client.right + dancers_bm.bmWidth;
	calc.bottom = wnd.bottom - wnd.top - client.bottom + dancers_bm.bmHeight;

//
// Set window size
//

	SetWindowPos(hWnd,HWND_TOP,0,0,calc.right*X_FACTOR,
		     calc.bottom*Y_FACTOR,SWP_NOMOVE | SWP_NOZORDER);

//	SetBkMode(mem_dc,TRANSPARENT);
	SetBkMode(mem_dc,(int)1);

	Main_Win_width = dancers_bm.bmWidth;
	Main_Win_height = dancers_bm.bmHeight;
	Main_Win_MemDC = mem_dc;

//
// Release display context
//

	ReleaseDC(hWnd,dc);

//
// Display window
//

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
}

//
// Window procedure for debug window
//

LRESULT CALLBACK TangoWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	BOOL bo;
	HMENU hMenu;
	int level;
	RECT re;
	int calc_hei;
	bool too_large = false;
	Tango::Util *tg = Tango::Util::instance();

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
		switch (wmId)
		{
		case TANGO_APP_ABOUT:
			DialogBox(hInst,(LPCTSTR)TANGO_ABOUT1,hWnd,(DLGPROC)AboutProc);
			break;

		case ID_FILE_EXIT:
			DestroyWindow(hWnd);
			break;

		case ID_VIEW_CONSOLE:
			hMenu = GetMenu(hWnd);
			if (tg->get_console_window() == NULL)
			{
				tg->get_debug_object()->CreateWin(tg->get_ds_name().c_str());
			}

			MENUITEMINFO mii;

			mii.fMask = MIIM_STATE;
			mii.cbSize = sizeof(mii);
			bo = GetMenuItemInfo(hMenu,ID_VIEW_CONSOLE,FALSE,&mii);
			if (mii.fState == 0x0)
			{
				ShowWindow(tg->get_console_window(),SW_SHOW);
			}
			else
			{
				ShowWindow(tg->get_console_window(),SW_HIDE);
			}

			mii.fState ^= MFS_CHECKED;
			bo = SetMenuItemInfo(hMenu,ID_VIEW_CONSOLE,FALSE,&mii);
			break;

		case ID_DEBUG_LEVEL0:
			SetDebugLevel(0,GetMenu(hWnd));
			tg->set_trace_level(0);
			break;

		case ID_DEBUG_LEVEL1:
			SetDebugLevel(1,GetMenu(hWnd));
			tg->set_trace_level(1);
			break;

		case ID_DEBUG_LEVEL2:
			SetDebugLevel(2,GetMenu(hWnd));
			tg->set_trace_level(2);
			break;

		case ID_DEBUG_LEVEL3:
			SetDebugLevel(3,GetMenu(hWnd));
			tg->set_trace_level(3);
			break;

		case ID_DEBUG_LEVEL4:
			SetDebugLevel(4,GetMenu(hWnd));
			tg->set_trace_level(4);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
		
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc,0,0,Main_Win_width,Main_Win_height,Main_Win_MemDC,0,0,SRCCOPY);

//
// Set font for the main text
//

		strcpy(lf.lfFaceName,"French Script MT");
		lf.lfWeight = FW_BOLD;
		lf.lfHeight = -40;
		lf.lfCharSet = DEFAULT_CHARSET;
		SelectObject(hdc,CreateFontIndirect(&lf));

//
// Display text (in red)
//

		SetTextColor(hdc,RGB(255,0,0));
		re.left = Main_Win_width;
		re.top = 0;
		re.right = Main_Win_width * 2;
		re.bottom = Main_Win_height;
		calc_hei = DrawText(hdc,TEXT(MAIN_TXT),-1,&re,
			            DT_CALCRECT | DT_CENTER);
		re.top = (Main_Win_height - calc_hei) / 2;
		re.bottom = (Main_Win_height + calc_hei) / 2;
		re.right = Main_Win_width * 2;
		re.left = Main_Win_width;
		DrawText(hdc,TEXT(MAIN_TXT),-1,&re,DT_CENTER);

//
// Change font to a more classical one
//

		user_font.lfCharSet = DEFAULT_CHARSET;
		SelectObject(hdc,CreateFontIndirect(&user_font));

//
// Display user text. If the text is too large to fit into the foreseen place
// replace it with a predefined message
//

		re.top = Main_Win_height;
		re.left = 0;
		re.right = Main_Win_width * X_FACTOR;
		re.bottom = Main_Win_height * Y_FACTOR;
		SetTextColor(hdc,RGB(0,0,255));
		calc_hei = DrawText(hdc,tg->get_main_window_text().c_str(),-1,&re,
				    DT_CALCRECT | DT_CENTER);
		if (calc_hei > ((Y_FACTOR - 1) * Main_Win_height))
		{
			too_large = true;
			re.top = Main_Win_height;
			re.bottom = Main_Win_height * Y_FACTOR;
		}
		else
		{
			re.top = Main_Win_height + (Main_Win_height * ((Y_FACTOR - 1) / 2)) - (calc_hei / 2);
			re.bottom = Main_Win_height + (Main_Win_height * ((Y_FACTOR - 1) / 2)) + (calc_hei / 2);
		}
		re.left = 0;
		re.right = Main_Win_width * X_FACTOR;

		if (too_large == true)
			DrawText(hdc,TOO_LARGE_TXT,-1,&re,
				 DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		else
			DrawText(hdc,tg->get_main_window_text().c_str(),-1,&re,DT_CENTER);


		level = tg->get_trace_level();
		SetDebugLevel(level,GetMenu(hWnd));
		EndPaint(hWnd, &ps);
		break;
		
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
        return 0;
}

void SetDebugLevel(int level,HMENU menu)
{
	MENUITEMINFO mii;
	BOOL bo;

//
// Reset all items
//

	mii.fMask = MIIM_STATE;
	mii.fState = 0;
	mii.cbSize = sizeof(mii);
	bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL0,FALSE,&mii);
	bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL1,FALSE,&mii);
	bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL2,FALSE,&mii);
	bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL3,FALSE,&mii);
	bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL4,FALSE,&mii);

//
// Set the chosen level
//

	mii.fState = MFS_CHECKED;
	switch (level)
	{
	case 0:
		bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL0,FALSE,&mii);
		break;

	case 1:
		bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL1,FALSE,&mii);
		break;
	
	case 2:
		bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL2,FALSE,&mii);
		break;
	
	case 3:
		bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL3,FALSE,&mii);
		break;
	
	case 4:
		bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL4,FALSE,&mii);
		break;
	}
}


LRESULT CALLBACK AboutProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	Util *tg = Util::instance();
	TCHAR str[80];
	string vers;
	string full_vers;
	string::size_type nb_char;

	switch (message)
	{
	case WM_INITDIALOG:
		strcpy(str,"Device server : ");
		strcat(str,tg->get_ds_name().c_str());
		SetDlgItemText(hDlg,TANGO_ABOUT_TEXT2,(LPCSTR)str);

		strcpy(str,"TANGO release : ");
		full_vers = Vers;
		if (full_vers.rfind(' ') != string::npos)
		{
			nb_char = full_vers.rfind(' ') - full_vers.find(' ');
			vers = full_vers.substr(full_vers.find(' '),nb_char);
			strcat(str,vers.c_str());
		}
		else
		{
			strcat(str,"x.y");
		}
		SetDlgItemText(hDlg,TANGO_ABOUT_TEXT3,(LPCSTR)str);

		strcpy(str,"TANGO IDL definition release : ");
		strcat(str,tg->get_version_str().c_str());
		SetDlgItemText(hDlg,TANGO_ABOUT_TEXT4,(LPCSTR)str);

		strcpy(str,"Server release : ");
		strcat(str,tg->get_server_version().c_str());
		SetDlgItemText(hDlg,TANGO_ABOUT_TEXT5,(LPCSTR)str);		
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

} // End of Tango namespace
