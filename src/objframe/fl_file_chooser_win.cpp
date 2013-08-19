#include "fl_file_chooser_win.H"

#ifdef WIN32
#include <Windows.h>
#endif

#ifdef HAVE_CONFIG_H
#include <force_config.h>
#endif

#ifdef HAVE_CSTDIO
#include <cstdio>
#else
#include <stdio.h>
#endif

#include <FL/Fl.H>
#include <FL/x.H>

char *fl_file_chooser(const char *message,const char *pat,const char *fname)
{
#ifdef WIN32

	OPENFILENAME ofn;       // common dialog box structure
	char szTitle[260]; 
	char szFilter[260];
	char szDefExt[3];

	static Fl_Window* topWindow = Fl::first_window();

	strcpy(g_szFile, fname);
	strcpy(szTitle, message);
	strncpy(szDefExt, pat+1, 3);
	// *.fpd files
	// 012345678901
	ZeroMemory(szFilter, sizeof(szFilter));
	sprintf(szFilter, "%s files %s\0\0", pat, pat);
	szFilter[11] = 0;

	
	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = fl_xid(topWindow) ;
	ofn.lpstrFile = g_szFile;
	ofn.nMaxFile = sizeof(g_szFile);
	ofn.lpstrFilter = szFilter;
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrTitle = szTitle;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrDefExt = szDefExt;
	ofn.Flags = OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT;
	
	// Display the Open/save dialog box. 

	if (strlen(fname)==0)
	{
		if (GetOpenFileName(&ofn)==TRUE) 
		{
			return g_szFile;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		if (GetSaveFileName(&ofn)==TRUE) 
		{
			return g_szFile;
		}
		else
		{
			return NULL;
		}
	}
	return NULL;
#else
	return NULL;
#endif
}