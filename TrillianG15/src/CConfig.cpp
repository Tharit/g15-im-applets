#include "StdAfx.h"
#include "CConfig.h"

#include "CAppletManager.h"

#define FONTF_BOLD 1
#define FONTF_ITALIC 2

bool CConfig::m_bInitializingDialog = false;
bool CConfig::m_abBoolSettings[BOOL_SETTINGS];
bool CConfig::m_bDialogsInitialized = false;
int CConfig::m_aiIntSettings[INT_SETTINGS];
LOGFONT CConfig::m_logfont[FONT_SETTINGS];
LOGFONT CConfig::m_templogfont[FONT_SETTINGS];
HANDLE CConfig::m_hSampleFont[FONT_SETTINGS];
TEXTMETRIC CConfig::m_tmFontInfo[FONT_SETTINGS];
vector<CProtoFilter*> CConfig::m_FilterList;
vector<CProtoFilter*> CConfig::m_TempFilterList;

// Dialogs
HWND CConfig::m_hwndPreferencesDlg = NULL;
HWND CConfig::m_hwndNotificationsDlg = NULL;
HWND CConfig::m_hwndAppearanceDlg = NULL;
HWND CConfig::m_hwndContactlistDlg = NULL;
HWND CConfig::m_hwndChatsessionsDlg = NULL;

void CConfig::Initialize()
{
	CConfig::LoadSettings();
}

void CConfig::Shutdown()
{
	if(m_hSampleFont)
		DeleteObject(m_hSampleFont);
}

void CConfig::DestroyConfigDialog(HWND hwndDialog)
{
	if(hwndDialog == NULL)
		return;

	struct dialog_entry_t		det;
	
	trillianInitialize(det);
	det.hwnd = hwndDialog;

	g_plugin_send(APP_GUID, "dialogRemove", &det);

	DestroyWindow(hwndDialog);
}

HWND CConfig::CreateConfigDialog(int iX,int iY,int iTemplate,HWND parent,DLGPROC dlgproc)
{
	struct dialog_entry_t		det;

	trillianInitialize(det);
					
	det.hwnd = CreateDialog(hInstance, MAKEINTRESOURCE(iTemplate), parent, (DLGPROC)dlgproc);

	g_plugin_send(APP_GUID, "dialogAdd", &det);   
	
	SetWindowPos(det.hwnd, 0, iX,iY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	return det.hwnd;
}

void CConfig::HandlePreferencesDialogs(plugin_prefs_show_t	*pps)
{
	strcpy(pps->prefs_info.name, "TrillianG15");
	strcpy(pps->prefs_info.description, "Applet for the Logitech G15 keyboard");

	/* This can be an HBITMAP if you've got a pretty icon for your preferences screen.
	 */

	pps->prefs_info.bitmap = 0;

	/* Show or hide?
	 */

	if (pps->show == 1) 
	{

		if(!CConfig::m_bDialogsInitialized)
		{
			CConfig::InitializeTempFilters();
			CConfig::m_bDialogsInitialized = true;
		}
		if (!pps->sub_entry)
		{
			if(CConfig::m_hwndPreferencesDlg == NULL)
				CConfig::m_hwndPreferencesDlg = CConfig::CreateConfigDialog(pps->x,pps->y, IDD_PREFS, pps->hwnd, (DLGPROC)CConfig::PreferencesDlgProc);
			ShowWindow(CConfig::m_hwndPreferencesDlg, SW_SHOW);
		}
		else if(!strcmp(pps->sub_entry,"Appearance"))
		{
			if(CConfig::m_hwndAppearanceDlg == NULL)
				CConfig::m_hwndAppearanceDlg = CConfig::CreateConfigDialog(pps->x,pps->y, IDD_FONTS, pps->hwnd, (DLGPROC)CConfig::AppearanceDlgProc);
			ShowWindow(CConfig::m_hwndAppearanceDlg, SW_SHOW);
			for(int i=0;i<FONT_SETTINGS;i++)
				SendDlgItemMessage(m_hwndAppearanceDlg,CConfig::GetSampleField(i),WM_SETFONT,(WPARAM)m_hSampleFont[i],(LPARAM)true);
		}
		else if(!strcmp(pps->sub_entry,"Notifications"))
		{
			if(CConfig::m_hwndNotificationsDlg == NULL)
				CConfig::m_hwndNotificationsDlg = CConfig::CreateConfigDialog(pps->x,pps->y, IDD_NOTIFICATIONS, pps->hwnd, (DLGPROC)CConfig::NotificationsDlgProc);
			ShowWindow(CConfig::m_hwndNotificationsDlg, SW_SHOW);
		}
		else if(!strcmp(pps->sub_entry,"Chatsessions"))
		{
			if(CConfig::m_hwndChatsessionsDlg == NULL)
				CConfig::m_hwndChatsessionsDlg = CConfig::CreateConfigDialog(pps->x,pps->y, IDD_CHAT, pps->hwnd, (DLGPROC)CConfig::ChatDlgProc);
			ShowWindow(CConfig::m_hwndChatsessionsDlg, SW_SHOW);
		}
		else if(!strcmp(pps->sub_entry,"Contactlist"))
		{
			if(CConfig::m_hwndContactlistDlg == NULL)
				CConfig::m_hwndContactlistDlg = CConfig::CreateConfigDialog(pps->x,pps->y, IDD_CLIST, pps->hwnd, (DLGPROC)CConfig::ContactlistDlgProc);
			ShowWindow(CConfig::m_hwndContactlistDlg, SW_SHOW);
		}
	} 
	else if (pps->show == 0)
	{
		if (!pps->sub_entry)
		{
			if(CConfig::m_hwndPreferencesDlg != NULL)
				ShowWindow(CConfig::m_hwndPreferencesDlg, SW_HIDE);
		}
		else if(!strcmp(pps->sub_entry,"Appearance"))
		{
			if(CConfig::m_hwndAppearanceDlg != NULL)
				ShowWindow(CConfig::m_hwndAppearanceDlg, SW_HIDE);
		}
		else if(!strcmp(pps->sub_entry,"Notifications"))
		{
			if(CConfig::m_hwndNotificationsDlg != NULL)
				ShowWindow(CConfig::m_hwndNotificationsDlg, SW_HIDE);
		}
		else if(!strcmp(pps->sub_entry,"Chatsessions"))
		{
			if(CConfig::m_hwndChatsessionsDlg != NULL)
				ShowWindow(CConfig::m_hwndChatsessionsDlg, SW_HIDE);
		}
		else if(!strcmp(pps->sub_entry,"Contactlist"))
		{
			if(CConfig::m_hwndContactlistDlg != NULL)
				ShowWindow(CConfig::m_hwndContactlistDlg, SW_HIDE);
		}
	}
}

void CConfig::HandlePreferencesActions(plugin_prefs_action_t *ppa)
{
	/* Take action:
		 *
		 * 0 - Cancel
		 * 1 - Apply
		 * 2 - OK
		 */

	// Destroy windows
	if(ppa->type != 1)
	{
		CConfig::DestroyConfigDialog(CConfig::m_hwndChatsessionsDlg);
		CConfig::DestroyConfigDialog(CConfig::m_hwndContactlistDlg);
		CConfig::DestroyConfigDialog(CConfig::m_hwndNotificationsDlg);
		CConfig::DestroyConfigDialog(CConfig::m_hwndPreferencesDlg);
		CConfig::DestroyConfigDialog(CConfig::m_hwndAppearanceDlg);

		CConfig::m_hwndPreferencesDlg = NULL;
		CConfig::m_hwndNotificationsDlg = NULL;
		CConfig::m_hwndAppearanceDlg = NULL;
		CConfig::m_hwndContactlistDlg = NULL;
		CConfig::m_hwndChatsessionsDlg = NULL;

		CConfig::m_bDialogsInitialized = false;
	}

	// save config
	if(ppa->type != 0)
	{
		CConfig::SaveSettings();
	}
}

void CConfig::LoadDefaults()
{
	for(int i=0;i<BOOL_SETTINGS;i++)
		m_abBoolSettings[i] = false;

	for(int i=0;i<INT_SETTINGS;i++)
		m_aiIntSettings[i] = 1;
	
	for(int i=0;i<FONT_SETTINGS;i++)
	{
		m_logfont[i].lfOutPrecision = OUT_DEFAULT_PRECIS;
		m_logfont[i].lfClipPrecision = CLIP_DEFAULT_PRECIS;
		m_logfont[i].lfQuality = DEFAULT_QUALITY;
		m_logfont[i].lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
		m_logfont[i].lfHeight = -MulDiv(6, 96, 72);
		// Style
		m_logfont[i].lfWeight = 0 & FONTF_BOLD ? FW_BOLD : FW_NORMAL;
		m_logfont[i].lfItalic = 0 & FONTF_ITALIC ? 1 : 0;
		// Charset
		m_logfont[i].lfCharSet = DEFAULT_CHARSET;
		// Name
		lstrcpy(m_logfont[i].lfFaceName,_T("Small Fonts"));
		
		UpdateFontSettings(i);
	}

	// Bool defaults
	m_abBoolSettings[CLIST_HIDEOFFLINE] = true;
	m_abBoolSettings[NOTIFY_MESSAGES] = true;
	m_abBoolSettings[NOTIFY_SIGNON] = true;
	m_abBoolSettings[NOTIFY_SIGNOFF] = true;
	m_abBoolSettings[NOTIFY_PROTO_STATUS] = true;
	m_abBoolSettings[SHOW_LABELS] = true;
	m_abBoolSettings[CLIST_USEGROUPS] = true;
	m_abBoolSettings[NOTIFY_PROTO_SIGNON] = true;
	m_abBoolSettings[NOTIFY_PROTO_SIGNOFF] = true;
	m_abBoolSettings[CLIST_SHOWPROTO] = true;
	m_abBoolSettings[SESSION_SYMBOLS] = true;
	m_abBoolSettings[CLIST_COUNTERS] = true;
	m_abBoolSettings[CLIST_DRAWLINES] = true;

	m_abBoolSettings[NOTIFY_SKIP_MESSAGES] = true;
	m_abBoolSettings[NOTIFY_SKIP_STATUS] = true;
	// Int defaults
	m_aiIntSettings[CLIST_GA] = CLIST_GA_NONE;

	m_aiIntSettings[NOTIFY_NICKCUTOFF_OFFSET] = 10;
	m_aiIntSettings[SESSION_LOGSIZE] = 10;
	m_aiIntSettings[NOTIFY_LOGSIZE] = 10;
	m_aiIntSettings[NOTIFY_DURATION] = 5;
	m_aiIntSettings[NOTIFY_TITLE] = NOTIFY_TITLE_NAME;
	m_aiIntSettings[SESSION_AUTOSCROLL] = SESSION_AUTOSCROLL_FIRST;
}

void CConfig::LoadSettings()
{
	LoadDefaults();

	char buffer[256];
	sprintf(buffer,"%s%s",g_home_directory,"trilliang15.xml");

	xml_tree_t xmltree;
	xmltree.filename = buffer;
	xmltree.data = NULL;
	
	int iRes = g_plugin_send(APP_GUID, "xmlGenerateTree", &xmltree);
	
	// invalid configuration or file doesn't exist - use defaults
	if(iRes < 0 || stricmp(xmltree.root_tag->children->text,"trilliang15"))
		return;

	xml_tag_t *tag = xmltree.root_tag->children->children;
	xml_attribute_t *attribute = NULL;

	while(tag != NULL)
	{
		if(!stricmp(tag->text,"bools"))
		{
			xml_tag_t *booltag = tag->children;
			int iID = 0,iValue =0;
			// loop through the setting tags
			while(booltag != NULL)
			{
				if(!stricmp(booltag->text,"setting"))
				{
					attribute = booltag->attributes;
					// loop through the attributes
					while(attribute != NULL)
					{
						if(!stricmp(attribute->name,"id"))
							iID = atoi(attribute->value);
						else if(!stricmp(attribute->name,"value"))
							iValue = atoi(attribute->value);
						attribute = attribute->next_attribute;
					}
					if(iID >= 0 && iID < BOOL_SETTINGS)
						m_abBoolSettings[iID] = iValue;
				}
				booltag = booltag->next_tag;
			}
		}
		else if(!stricmp(tag->text,"integers"))
		{
			xml_tag_t *integertag = tag->children;
			int iID = 0,iValue =0;
			// loop through the setting tags
			while(integertag != NULL)
			{
				if(!stricmp(integertag->text,"setting"))
				{
					attribute = integertag->attributes;
					// loop through the attributes
					while(attribute != NULL)
					{
						if(!stricmp(attribute->name,"id"))
							iID = atoi(attribute->value);
						else if(!stricmp(attribute->name,"value"))
							iValue = atoi(attribute->value);
						attribute = attribute->next_attribute;
					}
					if(iID >= 0 && iID < INT_SETTINGS)
						m_aiIntSettings[iID] = iValue;
				}
				integertag = integertag->next_tag;
			}
		}
		else if(!stricmp(tag->text,"filters"))
		{
			xml_tag_t *filtertag = tag->children;
			char *medium = NULL;
			int iValue = 1,iValue2 = 1;
			// loop through the setting tags
			while(filtertag != NULL)
			{
				iValue = 1;
				iValue2 = 1;
				if(!stricmp(filtertag->text,"filter"))
				{
					attribute = filtertag->attributes;
					// loop through the attributes
					while(attribute != NULL)
					{
						if(!stricmp(attribute->name,"medium"))
							medium = attribute->value;
						else if(!stricmp(attribute->name,"value"))
							iValue = atoi(attribute->value);
						else if(!stricmp(attribute->name,"value2"))
							iValue2 = atoi(attribute->value);
						attribute = attribute->next_attribute;
					}
					SetFilterValue(toTstring(medium),iValue!=0,iValue2!=0);
				}
				filtertag = filtertag->next_tag;
			}
		}
		else if(!stricmp(tag->text,"fonts"))
		{
			xml_tag_t *fonttag = tag->children;
			char *facename = NULL;
			int iFont = 0;
			int iHeight = -MulDiv(6, 96, 72);
			int iStyle = 0;
			int iCharset = DEFAULT_CHARSET;
			
			// loop through the setting tags
			while(fonttag != NULL)
			{
				if(!stricmp(fonttag->text,"font"))
				{
					attribute = fonttag->attributes;
					// loop through the attributes
					while(attribute != NULL)
					{
						if(!stricmp(attribute->name,"id"))
							iFont = atoi(attribute->value);
						else if(!stricmp(attribute->name,"height"))
							iHeight = atoi(attribute->value);
						else if(!stricmp(attribute->name,"style"))
							iStyle = atoi(attribute->value);
						else if(!stricmp(attribute->name,"charset"))
							iCharset = atoi(attribute->value);
						else if(!stricmp(attribute->name,"facename"))
							facename = attribute->value;
						attribute = attribute->next_attribute;
					}
					if(iFont >= 0 && iFont <FONT_SETTINGS)
					{
						m_logfont[iFont].lfOutPrecision = OUT_DEFAULT_PRECIS;
						m_logfont[iFont].lfClipPrecision = CLIP_DEFAULT_PRECIS;
						m_logfont[iFont].lfQuality = DEFAULT_QUALITY;
						m_logfont[iFont].lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
						m_logfont[iFont].lfHeight = iHeight;
						// Style
						m_logfont[iFont].lfWeight = iStyle & FONTF_BOLD ? FW_BOLD : FW_NORMAL;
						m_logfont[iFont].lfItalic = iStyle & FONTF_ITALIC ? 1 : 0;
						// Charset
						m_logfont[iFont].lfCharSet = iCharset;
						// Name
						if(facename == NULL)
							lstrcpy(m_logfont[iFont].lfFaceName,_T("Small Fonts"));
						else
							lstrcpy(m_logfont[iFont].lfFaceName,toTstring(facename).c_str());

						UpdateFontSettings(iFont);
					}
				}
				fonttag = fonttag->next_tag;
			}
		}
		tag = tag->next_tag;
	}
}

void CConfig::SaveSettings()
{
	// read settings from dialogs
	char buf[256];

	// copy temp filter values to the real list
	vector<CProtoFilter*>::iterator iter = m_TempFilterList.begin();
	while(iter != m_TempFilterList.end())
	{
		CConfig::SetFilterValue((*iter)->strName,(*iter)->bNotificationFilter,(*iter)->bContactlistFilter);
		iter++;
	}

	// Appearance dialog
	if(m_hwndAppearanceDlg != NULL)
	{
		m_abBoolSettings[TRANSITIONS] =  IsDlgButtonChecked(m_hwndAppearanceDlg,IDC_TRANSITIONS) == BST_CHECKED ? true : false;
		m_abBoolSettings[SHOW_LABELS] =  IsDlgButtonChecked(m_hwndAppearanceDlg,IDC_SHOW_LABELS) == BST_CHECKED ? true : false;
		m_abBoolSettings[CONTROL_BACKLIGHTS] =  IsDlgButtonChecked(m_hwndAppearanceDlg,IDC_CONTROL_BACKLIGHTS) == BST_CHECKED ? true : false;
		m_abBoolSettings[HOOK_VOLUMEWHEEL] =  IsDlgButtonChecked(m_hwndAppearanceDlg,IDC_HOOK_VOLUMEWHEEL) == BST_CHECKED ? true : false;
		m_abBoolSettings[TIMESTAMP_SECONDS] =  IsDlgButtonChecked(m_hwndAppearanceDlg,IDC_TIMESTAMP_SECONDS) == BST_CHECKED ? true : false;

		for(int i=0;i<FONT_SETTINGS;i++)
			m_logfont[i] = m_templogfont[i];
		SendMessage(m_hwndAppearanceDlg, WM_INITDIALOG, 0, 0);
	}

	// Notifications dialog		
	if(m_hwndNotificationsDlg != NULL)
	{
		m_abBoolSettings[NOTIFY_TIMESTAMPS] =		IsDlgButtonChecked(m_hwndNotificationsDlg,IDC_NOTIFY_TIMESTAMPS) == BST_CHECKED ? true : false;
		
		m_abBoolSettings[NOTIFY_SHOWPROTO] =		IsDlgButtonChecked(m_hwndNotificationsDlg,IDC_NOTIFY_SHOWPROTO) == BST_CHECKED ? true : false;
		m_abBoolSettings[NOTIFY_NICKCUTOFF] =		IsDlgButtonChecked(m_hwndNotificationsDlg,IDC_NOTIFY_NICKCUTOFF) == BST_CHECKED ? true : false;
		m_abBoolSettings[NOTIFY_PROTO_STATUS] =		IsDlgButtonChecked(m_hwndNotificationsDlg,IDC_NOTIFY_PROTO_STATUS) == BST_CHECKED ? true : false;
		m_abBoolSettings[NOTIFY_PROTO_SIGNON] =		IsDlgButtonChecked(m_hwndNotificationsDlg,IDC_NOTIFY_PROTO_SIGNON) == BST_CHECKED ? true : false;
		m_abBoolSettings[NOTIFY_PROTO_SIGNOFF] =	IsDlgButtonChecked(m_hwndNotificationsDlg,IDC_NOTIFY_PROTO_SIGNOFF) == BST_CHECKED ? true : false;
		m_abBoolSettings[NOTIFY_MESSAGES] =		IsDlgButtonChecked(m_hwndNotificationsDlg,IDC_NOTIFY_MESSAGES) == BST_CHECKED ? true : false;
		m_abBoolSettings[NOTIFY_SIGNON] =		IsDlgButtonChecked(m_hwndNotificationsDlg,IDC_NOTIFY_SIGNON) == BST_CHECKED ? true : false;
		m_abBoolSettings[NOTIFY_SIGNOFF] =		IsDlgButtonChecked(m_hwndNotificationsDlg,IDC_NOTIFY_SIGNOFF) == BST_CHECKED ? true : false;
		m_abBoolSettings[NOTIFY_STATUS] =		IsDlgButtonChecked(m_hwndNotificationsDlg,IDC_NOTIFY_STATUS) == BST_CHECKED ? true : false;
		m_abBoolSettings[NOTIFY_SECTION] =		IsDlgButtonChecked(m_hwndNotificationsDlg,IDC_NOTIFY_SECTION) == BST_CHECKED ? true : false;

		m_abBoolSettings[NOTIFY_SKIP_MESSAGES] =		IsDlgButtonChecked(m_hwndNotificationsDlg,IDC_NOTIFY_SKIP_MESSAGES) == BST_CHECKED ? true : false;
		m_abBoolSettings[NOTIFY_SKIP_SIGNON] =		IsDlgButtonChecked(m_hwndNotificationsDlg,IDC_NOTIFY_SKIP_SIGNON) == BST_CHECKED ? true : false;
		m_abBoolSettings[NOTIFY_SKIP_SIGNOFF] =		IsDlgButtonChecked(m_hwndNotificationsDlg,IDC_NOTIFY_SKIP_SIGNOFF) == BST_CHECKED ? true : false;
		m_abBoolSettings[NOTIFY_SKIP_STATUS] =		IsDlgButtonChecked(m_hwndNotificationsDlg,IDC_NOTIFY_SKIP_STATUS) == BST_CHECKED ? true : false;
		

		GetDlgItemTextA(m_hwndNotificationsDlg,IDC_NOTIFY_DURATION,buf,256);
		m_aiIntSettings[NOTIFY_DURATION] = atoi(buf) > 0 ? atoi(buf):1;

		GetDlgItemTextA(m_hwndNotificationsDlg,IDC_NOTIFY_LOGSIZE,buf,256);
		m_aiIntSettings[NOTIFY_LOGSIZE] = atoi(buf) > 0 ? atoi(buf):1;

		GetDlgItemTextA(m_hwndNotificationsDlg,IDC_NOTIFY_NICKCUTOFF_OFFSET,buf,256);
		m_aiIntSettings[NOTIFY_NICKCUTOFF_OFFSET] = atoi(buf) > 0 ? atoi(buf):1;

		if(IsDlgButtonChecked(m_hwndNotificationsDlg,IDC_NOTIFY_TITLEHIDE) == BST_CHECKED)
			m_aiIntSettings[NOTIFY_TITLE] = NOTIFY_TITLE_HIDE;
		else if(IsDlgButtonChecked(m_hwndNotificationsDlg,IDC_NOTIFY_TITLENAME) == BST_CHECKED)
			m_aiIntSettings[NOTIFY_TITLE] = NOTIFY_TITLE_NAME;
		else
			m_aiIntSettings[NOTIFY_TITLE] = NOTIFY_TITLE_INFO;

		SendMessage(m_hwndNotificationsDlg, WM_INITDIALOG, 0, 0);
	}

	// Chat dialog
	if(m_hwndChatsessionsDlg != NULL)
	{
		m_abBoolSettings[SESSION_TIMESTAMPS] =	IsDlgButtonChecked(m_hwndChatsessionsDlg,IDC_SESSION_TIMESTAMPS) == BST_CHECKED ? true : false;
		
		m_abBoolSettings[SESSION_SCROLL_MAXIMIZED] =	IsDlgButtonChecked(m_hwndChatsessionsDlg,IDC_SESSION_SCROLL_MAXIMIZED) == BST_CHECKED ? true : false;
		m_abBoolSettings[SESSION_REPLY_MAXIMIZED] =		IsDlgButtonChecked(m_hwndChatsessionsDlg,IDC_SESSION_REPLY_MAXIMIZED) == BST_CHECKED ? true : false;
		m_abBoolSettings[SESSION_SENDRETURN] =			IsDlgButtonChecked(m_hwndChatsessionsDlg,IDC_SESSION_SENDRETURN) == BST_CHECKED ? true : false;
		m_abBoolSettings[SESSION_SYMBOLS] =				IsDlgButtonChecked(m_hwndChatsessionsDlg,IDC_SESSION_SYMBOLS) == BST_CHECKED ? true : false;

		if(IsDlgButtonChecked(m_hwndChatsessionsDlg,IDC_SESSION_SCROLLNONE) == BST_CHECKED)
			m_aiIntSettings[SESSION_AUTOSCROLL] = SESSION_AUTOSCROLL_NONE;
		else if(IsDlgButtonChecked(m_hwndChatsessionsDlg,IDC_SESSION_SCROLLFIRST) == BST_CHECKED)
			m_aiIntSettings[SESSION_AUTOSCROLL] = SESSION_AUTOSCROLL_FIRST;
		else
			m_aiIntSettings[SESSION_AUTOSCROLL] = SESSION_AUTOSCROLL_LAST;

		m_abBoolSettings[MAXIMIZED_TITLE] =  IsDlgButtonChecked(m_hwndChatsessionsDlg,IDC_MAXIMIZED_TITLE) == BST_UNCHECKED ? true : false;
		m_abBoolSettings[MAXIMIZED_LABELS] =  IsDlgButtonChecked(m_hwndChatsessionsDlg,IDC_MAXIMIZED_LABELS) == BST_UNCHECKED ? true : false;

		GetDlgItemTextA(m_hwndChatsessionsDlg,IDC_SESSION_LOGSIZE,buf,256);
		m_aiIntSettings[SESSION_LOGSIZE] = atoi(buf) > 0 ? atoi(buf):1;
		SendMessage(m_hwndChatsessionsDlg, WM_INITDIALOG, 0, 0);

	}

	// Contactlist dialog	
	if(m_hwndContactlistDlg != NULL)
	{
		m_abBoolSettings[CLIST_COLUMNS] =	IsDlgButtonChecked(m_hwndContactlistDlg,IDC_CLIST_COLUMNS) == BST_CHECKED ? true : false;
		m_abBoolSettings[CLIST_HIDEOFFLINE] =	IsDlgButtonChecked(m_hwndContactlistDlg,IDC_CLIST_HIDEOFFLINE) == BST_CHECKED ? true : false;
		m_abBoolSettings[CLIST_USEGROUPS] =	IsDlgButtonChecked(m_hwndContactlistDlg,IDC_CLIST_USEGROUPS) == BST_CHECKED ? true : false;
		m_abBoolSettings[CLIST_SHOWPROTO] =	IsDlgButtonChecked(m_hwndContactlistDlg,IDC_CLIST_SHOWPROTO) == BST_CHECKED ? true : false;
		m_abBoolSettings[CLIST_DRAWLINES] =	IsDlgButtonChecked(m_hwndContactlistDlg,IDC_CLIST_DRAWLINES) == BST_CHECKED ? true : false;
		m_abBoolSettings[CLIST_POSITION] =	IsDlgButtonChecked(m_hwndContactlistDlg,IDC_CLIST_POSITION) == BST_CHECKED ? true : false;
		m_abBoolSettings[CLIST_COUNTERS] =	IsDlgButtonChecked(m_hwndContactlistDlg,IDC_CLIST_COUNTERS) == BST_CHECKED ? true : false;
		
		if(IsDlgButtonChecked(m_hwndContactlistDlg,IDC_CLIST_GA_NONE) == BST_CHECKED)
			m_aiIntSettings[CLIST_GA] = CLIST_GA_NONE;
		else if(IsDlgButtonChecked(m_hwndContactlistDlg,IDC_CLIST_GA_COLLAPSE) == BST_CHECKED)
			m_aiIntSettings[CLIST_GA] = CLIST_GA_COLLAPSE;
		else
			m_aiIntSettings[CLIST_GA] = CLIST_GA_EXPAND;

		SendMessage(m_hwndContactlistDlg, WM_INITDIALOG, 0, 0);
	}

	// write xml file

	char buffer[256];
	sprintf(buffer,"%s%s",g_home_directory,"trilliang15.xml");

	FILE *fp = fopen(buffer,"w");
	fprintf(fp,"<?xml version=\"1.0\" ?>\n");
	fprintf(fp,"<trilliang15>\n");

	// Save all bool settings
	fprintf(fp,"<bools>\n");
	for(int i=0;i<BOOL_SETTINGS;i++)
	{
		fprintf(fp,"<setting id=\"%i\" value=\"%i\" />\n",i,m_abBoolSettings[i]);
	}
	fprintf(fp,"</bools>\n");

	// Save all int settings
	fprintf(fp,"<integers>\n");
	for(int i=0;i<INT_SETTINGS;i++)
	{
		fprintf(fp,"<setting id=\"%i\" value=\"%i\" />\n",i,m_aiIntSettings[i]);
	}
	fprintf(fp,"</integers>\n");

	// Save filter settings
	fprintf(fp,"<filters>\n");
	iter = m_FilterList.begin();
	while(iter != m_FilterList.end())
	{
		fprintf(fp,"<filter medium=\"%s\" value=\"%d\" value2=\"%d\" />\n",toNarrowString((*iter)->strName).c_str(),(int)(*iter)->bNotificationFilter,(int)(*iter)->bContactlistFilter);
		iter++;
	}
	fprintf(fp,"</filters>\n");

	// Save font settings
	fprintf(fp,"<fonts>\n");
	for(int i=0;i<FONT_SETTINGS;i++)
	{
		int style = 0;
		if(m_logfont[i].lfWeight == FW_BOLD)
			style |= FONTF_BOLD;
		if(m_logfont[i].lfItalic)
			style |= FONTF_ITALIC;

		fprintf(fp,"<font id=\"%i\" height=\"%i\" style=\"%i\" charset=\"%i\" facename=\"%s\" />\n",
				i,
				m_logfont[i].lfHeight,
				style,
				m_logfont[i].lfCharSet,
				toNarrowString(m_logfont[i].lfFaceName).c_str());

		UpdateFontSettings(i);
	}

	fprintf(fp,"</fonts>\n");

	fprintf(fp,"</trilliang15>\n");
	fclose(fp);

	if(CAppletManager::GetInstance()->IsInitialized())
		CAppletManager::GetInstance()->OnConfigChanged();
}

void CConfig::UpdateFontSettings(int iFont)
{
	if(m_hSampleFont[iFont])
		DeleteObject(m_hSampleFont[iFont]);
	m_hSampleFont[iFont] =  CreateFontIndirect(&m_logfont[iFont]);
	HDC hDC = CreateCompatibleDC(NULL);
	SelectObject(hDC, m_hSampleFont[iFont]);   
	GetTextMetrics(hDC,&m_tmFontInfo[iFont]);
	DeleteObject(hDC);
}

LOGFONT &CConfig::GetFont(int iFont)
{
	return m_logfont[iFont];
}

int CConfig::GetFontHeight(int iFont)
{
	return m_tmFontInfo[iFont].tmHeight;
}

int CConfig::GetSampleField(int iFont)
{
	switch(iFont)
	{
	case 0: return IDC_SAMPLE1;
	case 1: return IDC_SAMPLE2;
	case 2: return IDC_SAMPLE3;
	case 3: return IDC_SAMPLE4;
	case 4: return IDC_SAMPLE5;
	};
	return 0;
}

int CALLBACK CConfig::PreferencesDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_COMMAND:
		{
			if(LOWORD(wParam) == IDC_REFRESHLIST)
				CAppletManager::GetInstance()->RegenerateContactData();
			else if(LOWORD(wParam) == IDC_CREDITS)
				CAppletManager::GetInstance()->ActivateCreditsScreen();
		}
	}
	return 0;
}

int CALLBACK CConfig::AppearanceDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char buf[128];

	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			m_bInitializingDialog = true;

			CheckDlgButton(hwndDlg, IDC_TRANSITIONS,			m_abBoolSettings[TRANSITIONS]		? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_SHOW_LABELS,			m_abBoolSettings[SHOW_LABELS]		? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_CONTROL_BACKLIGHTS,		m_abBoolSettings[CONTROL_BACKLIGHTS]		? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_HOOK_VOLUMEWHEEL,		m_abBoolSettings[HOOK_VOLUMEWHEEL]		? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_TIMESTAMP_SECONDS,		m_abBoolSettings[TIMESTAMP_SECONDS]		? BST_CHECKED	: BST_UNCHECKED);
			
			for(int i=0;i<FONT_SETTINGS;i++)
			{
				m_templogfont[i] = m_logfont[i];
				SendDlgItemMessage(hwndDlg,CConfig::GetSampleField(i),WM_SETFONT,(WPARAM)m_hSampleFont[i],(LPARAM)true);
			}

			m_bInitializingDialog = false;
			break;
		}
		case WM_COMMAND:
		{
			int iFont = LOWORD(wParam);
			switch(iFont)
			{
			case IDC_CHOOSEFONT1: iFont = 0; break;
			case IDC_CHOOSEFONT2: iFont = 1; break;
			case IDC_CHOOSEFONT3: iFont = 2; break;
			case IDC_CHOOSEFONT4: iFont = 3; break;
			case IDC_CHOOSEFONT5: iFont = 4; break;
			default: iFont = -1;
			};

			if(iFont >= 0)
			{
				CHOOSEFONT cf = { 0 };
				cf.lStructSize = sizeof(cf);
				cf.hwndOwner = hwndDlg;
				cf.lpLogFont = &m_templogfont[iFont];
				cf.Flags = CF_FORCEFONTEXIST | CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
				if (ChooseFont(&cf))
				{
					if(m_hSampleFont[iFont])
						DeleteObject(m_hSampleFont[iFont]);
					m_hSampleFont[iFont] =  CreateFontIndirect(&m_templogfont[iFont]);
					TRACE(_T("Font selected!"));
				}

				for(int i=0;i<FONT_SETTINGS;i++)
					SendDlgItemMessage(hwndDlg,CConfig::GetSampleField(iFont),WM_SETFONT,(WPARAM)m_hSampleFont[iFont],(LPARAM)true);
			}

			// Activate the apply button
			if (!m_bInitializingDialog && ((HIWORD(wParam) == EN_CHANGE) || (HIWORD(wParam) == BN_CLICKED)))
				SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);

			break;
		}
	}
	return 0;
}

int CALLBACK CConfig::ChatDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char buf[128];

	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			m_bInitializingDialog = true;
			
			CheckDlgButton(hwndDlg, IDC_SESSION_TIMESTAMPS,			m_abBoolSettings[SESSION_TIMESTAMPS]	? BST_CHECKED	: BST_UNCHECKED);
			
			CheckDlgButton(hwndDlg, IDC_MAXIMIZED_TITLE,			m_abBoolSettings[MAXIMIZED_TITLE]	? BST_UNCHECKED	: BST_CHECKED);
			CheckDlgButton(hwndDlg, IDC_MAXIMIZED_LABELS,			m_abBoolSettings[MAXIMIZED_LABELS]	? BST_UNCHECKED	: BST_CHECKED);
			CheckDlgButton(hwndDlg, IDC_SESSION_REPLY_MAXIMIZED,	m_abBoolSettings[SESSION_REPLY_MAXIMIZED]? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_SESSION_SCROLL_MAXIMIZED,	m_abBoolSettings[SESSION_SCROLL_MAXIMIZED]? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_SESSION_SYMBOLS,			m_abBoolSettings[SESSION_SYMBOLS]? BST_CHECKED	: BST_UNCHECKED);

			CheckDlgButton(hwndDlg, IDC_SESSION_SENDRETURN,	m_abBoolSettings[SESSION_SENDRETURN]? BST_CHECKED	: BST_UNCHECKED);
			
			CheckDlgButton(hwndDlg, IDC_SESSION_SCROLLNONE,		m_aiIntSettings[SESSION_AUTOSCROLL] == SESSION_AUTOSCROLL_NONE? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_SESSION_SCROLLFIRST,		m_aiIntSettings[SESSION_AUTOSCROLL] == SESSION_AUTOSCROLL_FIRST? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_SESSION_SCROLLLAST,		m_aiIntSettings[SESSION_AUTOSCROLL] == SESSION_AUTOSCROLL_LAST? BST_CHECKED	: BST_UNCHECKED);
			
			itoa(m_aiIntSettings[SESSION_LOGSIZE], buf, 10);
			SetDlgItemTextA(hwndDlg, IDC_SESSION_LOGSIZE, buf);

			m_bInitializingDialog = false;
			break;
		}
		case WM_COMMAND:
		{
			// Activate the apply button
			if (!m_bInitializingDialog && ((HIWORD(wParam) == EN_CHANGE) || (HIWORD(wParam) == BN_CLICKED)))
				SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);

			break;
		}
	}
	return 0;
}

int CALLBACK CConfig::NotificationsDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char buf[128];

	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			m_bInitializingDialog = true;

			// Initialize the protocol filter list
			SetWindowLong(GetDlgItem(hwndDlg,IDC_PROTOCOLS),GWL_STYLE,GetWindowLong(GetDlgItem(hwndDlg,IDC_PROTOCOLS),GWL_STYLE)|TVS_NOHSCROLL);
			int iRes = 0;
			
			HIMAGELIST himlCheckBoxes;
			himlCheckBoxes=ImageList_Create(GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),ILC_COLOR32|ILC_MASK,2,2);
			iRes = ImageList_AddIcon(himlCheckBoxes,LoadIcon(hInstance,MAKEINTRESOURCE(IDI_NOTICK)));
			iRes = ImageList_AddIcon(himlCheckBoxes,LoadIcon(hInstance,MAKEINTRESOURCE(IDI_TICK)));
			TreeView_SetImageList(GetDlgItem(hwndDlg,IDC_PROTOCOLS),himlCheckBoxes,TVSIL_NORMAL);
		
			FillTree(GetDlgItem(hwndDlg,IDC_PROTOCOLS));
			
			CheckDlgButton(hwndDlg, IDC_NOTIFY_SHOWPROTO,			m_abBoolSettings[NOTIFY_SHOWPROTO]		? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_NOTIFY_NICKCUTOFF,			m_abBoolSettings[NOTIFY_NICKCUTOFF]		? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_NOTIFY_TIMESTAMPS,			m_abBoolSettings[NOTIFY_TIMESTAMPS]		? BST_CHECKED	: BST_UNCHECKED);
			
			CheckDlgButton(hwndDlg, IDC_NOTIFY_PROTO_STATUS,			m_abBoolSettings[NOTIFY_PROTO_STATUS]		? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_NOTIFY_PROTO_SIGNON,			m_abBoolSettings[NOTIFY_PROTO_SIGNON]		? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_NOTIFY_PROTO_SIGNOFF,			m_abBoolSettings[NOTIFY_PROTO_SIGNOFF]		? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_NOTIFY_SIGNON,		m_abBoolSettings[NOTIFY_SIGNON]		? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_NOTIFY_SIGNOFF,		m_abBoolSettings[NOTIFY_SIGNOFF]		? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_NOTIFY_STATUS,		m_abBoolSettings[NOTIFY_STATUS]				? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_NOTIFY_MESSAGES,		m_abBoolSettings[NOTIFY_MESSAGES]			? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_NOTIFY_SECTION,		m_abBoolSettings[NOTIFY_SECTION]			? BST_CHECKED	: BST_UNCHECKED);

			CheckDlgButton(hwndDlg, IDC_NOTIFY_SKIP_SIGNON,		m_abBoolSettings[NOTIFY_SKIP_SIGNON]		? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_NOTIFY_SKIP_SIGNOFF,	m_abBoolSettings[NOTIFY_SKIP_SIGNOFF]		? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_NOTIFY_SKIP_STATUS,		m_abBoolSettings[NOTIFY_SKIP_STATUS]				? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_NOTIFY_SKIP_MESSAGES,	m_abBoolSettings[NOTIFY_SKIP_MESSAGES]			? BST_CHECKED	: BST_UNCHECKED);
			
			itoa(m_aiIntSettings[NOTIFY_NICKCUTOFF_OFFSET], buf, 10);
			SetDlgItemTextA(hwndDlg, IDC_NOTIFY_NICKCUTOFF_OFFSET, buf);

			itoa(m_aiIntSettings[NOTIFY_LOGSIZE], buf, 10);
			SetDlgItemTextA(hwndDlg, IDC_NOTIFY_LOGSIZE, buf);
			
			itoa(m_aiIntSettings[NOTIFY_DURATION], buf, 10);
			SetDlgItemTextA(hwndDlg, IDC_NOTIFY_DURATION, buf);

			CheckDlgButton(hwndDlg, IDC_NOTIFY_TITLEHIDE,		m_aiIntSettings[NOTIFY_TITLE] == NOTIFY_TITLE_HIDE? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_NOTIFY_TITLENAME,		m_aiIntSettings[NOTIFY_TITLE] == NOTIFY_TITLE_NAME? BST_CHECKED	: BST_UNCHECKED);
			
			m_bInitializingDialog = false;
			break;
		}
		case WM_NOTIFY:
		{
			if(((LPNMHDR)lParam)->idFrom == IDC_PROTOCOLS)
			{
				if(((LPNMHDR)lParam)->code != NM_CLICK)
					return 0;

				TVHITTESTINFO hti;
				hti.pt.x=(short)LOWORD(GetMessagePos());
				hti.pt.y=(short)HIWORD(GetMessagePos());
				ScreenToClient(((LPNMHDR)lParam)->hwndFrom,&hti.pt);
				if(TreeView_HitTest(((LPNMHDR)lParam)->hwndFrom,&hti))
					if(hti.flags&TVHT_ONITEMICON) {
						TVITEM tvi;
						tvi.mask=TVIF_HANDLE|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
						tvi.hItem=hti.hItem;
						TreeView_GetItem(((LPNMHDR)lParam)->hwndFrom,&tvi);
						tvi.iImage=tvi.iSelectedImage=!tvi.iImage;
						((CProtoFilter *)tvi.lParam)->bNotificationFilter=tvi.iImage;
						TreeView_SetItem(((LPNMHDR)lParam)->hwndFrom,&tvi);
						SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
						
					}
				break;
			}
			break;
		}
		case WM_COMMAND:
		{
			// Activate the apply button
			if (!m_bInitializingDialog && ((HIWORD(wParam) == EN_CHANGE) || (HIWORD(wParam) == BN_CLICKED)))
				SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);

			break;
		}
	}
	return 0;
}

int CALLBACK CConfig::ContactlistDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char buf[128];

	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			m_bInitializingDialog = true;

			CheckDlgButton(hwndDlg, IDC_CLIST_COLUMNS,	m_abBoolSettings[CLIST_COLUMNS]			? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_CLIST_HIDEOFFLINE,	m_abBoolSettings[CLIST_HIDEOFFLINE]			? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_CLIST_USEGROUPS,	m_abBoolSettings[CLIST_USEGROUPS]			? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_CLIST_SHOWPROTO,	m_abBoolSettings[CLIST_SHOWPROTO]? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_CLIST_DRAWLINES,	m_abBoolSettings[CLIST_DRAWLINES]? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_CLIST_POSITION,		m_abBoolSettings[CLIST_POSITION]? BST_CHECKED	: BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_CLIST_COUNTERS,		m_abBoolSettings[CLIST_COUNTERS]? BST_CHECKED	: BST_UNCHECKED);

			if(m_aiIntSettings[CLIST_GA] == CLIST_GA_NONE)
				CheckDlgButton(hwndDlg, IDC_CLIST_GA_NONE, BST_CHECKED);
			else if(m_aiIntSettings[CLIST_GA] == CLIST_GA_COLLAPSE)
				CheckDlgButton(hwndDlg, IDC_CLIST_GA_COLLAPSE, BST_CHECKED);
			else
				CheckDlgButton(hwndDlg, IDC_CLIST_GA_EXPAND, BST_CHECKED);

			// Initialize the protocol filter list
			SetWindowLong(GetDlgItem(hwndDlg,IDC_CLIST_PROTOFILTER),GWL_STYLE,GetWindowLong(GetDlgItem(hwndDlg,IDC_CLIST_PROTOFILTER),GWL_STYLE)|TVS_NOHSCROLL);
			int iRes = 0;
			
			HIMAGELIST himlCheckBoxes;
			himlCheckBoxes=ImageList_Create(GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),ILC_COLOR32|ILC_MASK,2,2);
			iRes = ImageList_AddIcon(himlCheckBoxes,LoadIcon(hInstance,MAKEINTRESOURCE(IDI_NOTICK)));
			iRes = ImageList_AddIcon(himlCheckBoxes,LoadIcon(hInstance,MAKEINTRESOURCE(IDI_TICK)));
			TreeView_SetImageList(GetDlgItem(hwndDlg,IDC_CLIST_PROTOFILTER),himlCheckBoxes,TVSIL_NORMAL);
		
			FillTree(GetDlgItem(hwndDlg,IDC_CLIST_PROTOFILTER),true);

			m_bInitializingDialog = false;
			break;
		}
		case WM_NOTIFY:
		{
			if(((LPNMHDR)lParam)->idFrom == IDC_CLIST_PROTOFILTER)
			{
				if(((LPNMHDR)lParam)->code != NM_CLICK)
					return 0;

				TVHITTESTINFO hti;
				hti.pt.x=(short)LOWORD(GetMessagePos());
				hti.pt.y=(short)HIWORD(GetMessagePos());
				ScreenToClient(((LPNMHDR)lParam)->hwndFrom,&hti.pt);
				if(TreeView_HitTest(((LPNMHDR)lParam)->hwndFrom,&hti))
					if(hti.flags&TVHT_ONITEMICON) {
						TVITEM tvi;
						tvi.mask=TVIF_HANDLE|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
						tvi.hItem=hti.hItem;
						TreeView_GetItem(((LPNMHDR)lParam)->hwndFrom,&tvi);
						tvi.iImage=tvi.iSelectedImage=!tvi.iImage;
						((CProtoFilter *)tvi.lParam)->bContactlistFilter=tvi.iImage;
						TreeView_SetItem(((LPNMHDR)lParam)->hwndFrom,&tvi);
						SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
						
					}
				break;
			}
			break;
		}
		case WM_COMMAND:
		{
			// Activate the apply button
			if (!m_bInitializingDialog && ((HIWORD(wParam) == EN_CHANGE) || (HIWORD(wParam) == BN_CLICKED)))
				SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);

			break;
		}
	}
	return 0;
}

bool CConfig::GetProtocolNotificationFilter(tstring strProtocol)
{
	vector<CProtoFilter*>::iterator iter = m_FilterList.begin();
	while(iter != m_FilterList.end())
	{
		if((*iter)->strName == strProtocol)
			return (*iter)->bNotificationFilter;
		iter++;
	}
	return true;
}

bool CConfig::GetProtocolContactlistFilter(tstring strProtocol)
{
	vector<CProtoFilter*>::iterator iter = m_FilterList.begin();
	while(iter != m_FilterList.end())
	{
		if((*iter)->strName == strProtocol)
			return (*iter)->bContactlistFilter;
		iter++;
	}
	return true;
}

void CConfig::InitializeTempFilters()
{
	vector<CProtoFilter*>::iterator listiter = m_TempFilterList.begin();
	while(listiter != m_TempFilterList.end())
	{
		delete (*listiter);
		listiter++;
	}
	m_TempFilterList.clear();

	vector<CConnectionData*> *pvConnectionData = CAppletManager::GetInstance()->GetConnectionDataVector();
	vector<CConnectionData*>::iterator iter = pvConnectionData->begin();
	vector<CProtoFilter*>::iterator iter2 =  m_FilterList.begin();

	// copy current connections to the temp list
	while(iter != pvConnectionData->end())
	{
		CConfig::SetTempFilterValue((*iter)->strMedium,true,true);
		iter++;
	}
	// copy saved filters to the temp list
	while(iter2 != m_FilterList.end())
	{
		CConfig::SetTempFilterValue((*iter2)->strName,(*iter2)->bNotificationFilter,(*iter2)->bContactlistFilter);
		iter2++;
	}
}

void CConfig::SetFilterValue(tstring strMedium,bool bNotificationFilter,bool bContactlistFilter)
{
	vector<CProtoFilter*>::iterator iter = m_FilterList.begin();
	while(iter != m_FilterList.end())
	{
		if((*iter)->strName == strMedium)
		{
			(*iter)->bNotificationFilter = bNotificationFilter;
			(*iter)->bContactlistFilter = bContactlistFilter;
			return;
		}
		iter++;
	}
	CProtoFilter *pFilter = new CProtoFilter();
	pFilter->strName = strMedium;
	pFilter->bNotificationFilter = bNotificationFilter;
	pFilter->bContactlistFilter = bContactlistFilter;
	m_FilterList.push_back(pFilter);
}

void CConfig::SetTempFilterValue(tstring strMedium,bool bNotificationFilter,bool bContactlistFilter)
{
	vector<CProtoFilter*>::iterator iter = m_TempFilterList.begin();
	while(iter != m_TempFilterList.end())
	{
		if((*iter)->strName == strMedium)
		{
			(*iter)->bNotificationFilter = bNotificationFilter;
			(*iter)->bContactlistFilter = bContactlistFilter;
			return;
		}
		iter++;
	}
	CProtoFilter *pFilter = new CProtoFilter();
	pFilter->strName = strMedium;
	pFilter->bNotificationFilter = bNotificationFilter;
	pFilter->bContactlistFilter = bContactlistFilter;
	m_TempFilterList.push_back(pFilter);
}

bool CConfig::GetBoolSetting(int iSetting)
{
	if(iSetting >= 0 && iSetting < BOOL_SETTINGS)
		return m_abBoolSettings[iSetting];
	return false;
}

int CConfig::GetIntSetting(int iSetting)
{
	if(iSetting >= 0 && iSetting < INT_SETTINGS)
		return m_aiIntSettings[iSetting];
	return false;
}

void CConfig::ClearTree(HWND hTree)
{
	TreeView_DeleteAllItems(hTree);
}

void CConfig::FillTree(HWND hTree,bool bClist)
{
	TreeView_DeleteAllItems(hTree);

	TVINSERTSTRUCT tvis;
	tvis.hParent=NULL;
	tvis.hInsertAfter=TVI_LAST;
	tvis.item.mask=TVIF_PARAM|TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;	
	
	vector<CProtoFilter*>::iterator iter =  m_FilterList.begin();
	iter = m_TempFilterList.begin();
	while(iter != m_TempFilterList.end())
	{
		tvis.item.lParam=(LPARAM)*iter;
		tvis.item.pszText= (LPWSTR)(*iter)->strName.c_str();
		if(bClist)
			tvis.item.iImage= (*iter)->bContactlistFilter?1:0;
		else
			tvis.item.iImage= (*iter)->bNotificationFilter?1:0;
		tvis.item.iSelectedImage= tvis.item.iImage;

		TreeView_InsertItem(hTree,&tvis);

		iter++;
	}
}