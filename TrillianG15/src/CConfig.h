#ifndef _CCONFIG_H_
#define _CCONFIG_H_

// Tabs
#define TAB_OPTIONS 0
#define TAB_NOTIFICATIONS 1
#define TAB_CHAT 2
#define TAB_CLIST 3

// Fonts
#define FONT_LOG 0
#define FONT_SESSION 1
#define FONT_CLIST 2
#define FONT_TITLE 3
#define FONT_NOTIFICATION 4

#define FONT_SETTINGS 5

// Bool settings
#define CLIST_HIDEOFFLINE 0
#define NOTIFY_MESSAGES 1
#define NOTIFY_SIGNON 2
#define NOTIFY_SIGNOFF 3
#define NOTIFY_STATUS 4
#define SHOW_LABELS 5
#define SESSION_SENDRETURN 6
#define NOTIFY_PROTO_STATUS 7
#define SESSION_SCROLL_MAXIMIZED 8
#define SESSION_REPLY_MAXIMIZED 9
#define MAXIMIZED_TITLE 10
#define MAXIMIZED_LABELS 11
#define CLIST_USEGROUPS 12
#define NOTIFY_PROTO_SIGNON 13
#define NOTIFY_PROTO_SIGNOFF 14
#define CLIST_SHOWPROTO 15
#define SESSION_SYMBOLS 16
#define CLIST_COUNTERS 17
#define CLIST_DRAWLINES 18
#define CLIST_POSITION 19
// #define CLIST_COLLAPSE 20
#define NOTIFY_SECTION 21
#define CONTROL_BACKLIGHTS 22
#define HOOK_VOLUMEWHEEL 23
#define NOTIFY_SKIP_MESSAGES 24
#define NOTIFY_SKIP_STATUS 25
#define NOTIFY_SKIP_SIGNON 26
#define NOTIFY_SKIP_SIGNOFF 27
#define SESSION_TIMESTAMPS 28
#define NOTIFY_TIMESTAMPS 29
#define TIMESTAMP_SECONDS 30
#define CLIST_COLUMNS 31
#define TRANSITIONS 32
#define NOTIFY_NICKCUTOFF 33
#define NOTIFY_SHOWPROTO 34

#define BOOL_SETTINGS 35

// Int Settings
#define SESSION_LOGSIZE 0
#define NOTIFY_LOGSIZE 1
#define NOTIFY_DURATION 2
#define NOTIFY_TITLE 3
#define SESSION_AUTOSCROLL 4
#define CLIST_GA 5
#define NOTIFY_NICKCUTOFF_OFFSET 6

#define INT_SETTINGS 7

// Enums
#define CLIST_GA_NONE 0
#define CLIST_GA_EXPAND 1
#define CLIST_GA_COLLAPSE 2

#define NOTIFY_TITLE_HIDE 0
#define NOTIFY_TITLE_INFO 1
#define NOTIFY_TITLE_NAME 2

#define SESSION_AUTOSCROLL_NONE 0
#define SESSION_AUTOSCROLL_FIRST 1
#define SESSION_AUTOSCROLL_LAST 2

struct CProtoFilter
{
	tstring strName;
	bool bNotificationFilter;
	bool bContactlistFilter;
};

class CConfig
{
public:
	static void Initialize();
	static void Shutdown();

	static void LoadSettings();
	static void LoadDefaults();

	static void SaveSettings();
	
	static void HandlePreferencesDialogs(plugin_prefs_show_t	*pps);
	static void HandlePreferencesActions(plugin_prefs_action_t *ppa);

	// InitDialogHook Callback
	static int InitOptionsDialog(WPARAM wParam, LPARAM lParam);
	// Dialog WndProc
	static int CALLBACK PreferencesDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static int CALLBACK AppearanceDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static int CALLBACK ChatDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static int CALLBACK NotificationsDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static int CALLBACK ContactlistDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static bool GetBoolSetting(int iSetting);
	static int GetIntSetting(int iSetting);

	static bool GetProtocolNotificationFilter(tstring strProtocol);
	static bool GetProtocolContactlistFilter(tstring strProtocol);

	static LOGFONT &GetFont(int iFont);
	static int GetFontHeight(int iFont);
private:
	static HWND CreateConfigDialog(int iX,int iY,int iTemplate,HWND parent,DLGPROC dlgproc);
	static void DestroyConfigDialog(HWND hwndDialog);

	static int GetSampleField(int iFont);
	static void ClearTree(HWND hTree);
	static void FillTree(HWND hTree,bool bClist = false);

	static vector<CProtoFilter*> m_TempFilterList;
	static vector<CProtoFilter*> m_FilterList;

	static HANDLE m_hSampleFont[FONT_SETTINGS];
	static TEXTMETRIC m_tmFontInfo[FONT_SETTINGS];
	static LOGFONT m_logfont[FONT_SETTINGS];
	static LOGFONT m_templogfont[FONT_SETTINGS];

	static void InitializeTempFilters();
	static void SetTempFilterValue(tstring strMedium,bool bNotificationFilter,bool bContactlistFilter);
	static void SetFilterValue(tstring strMedium,bool bFilter,bool bContactlistFilter);

	static void UpdateFontSettings(int iFont);

	static bool m_abBoolSettings[BOOL_SETTINGS];
	static int m_aiIntSettings[INT_SETTINGS];
	static bool m_bInitializingDialog;
	
	static HWND m_hwndPreferencesDlg;
	static HWND m_hwndNotificationsDlg;
	static HWND m_hwndAppearanceDlg;
	static HWND m_hwndContactlistDlg;
	static HWND m_hwndChatsessionsDlg;

	static bool m_bDialogsInitialized;
};

#endif