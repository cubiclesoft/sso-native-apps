// Single Sign-On integration for Windows, Mac, and Linux.
// (C) 2013 CubicleSoft.  All Rights Reserved.

#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/stdpaths.h>
#include <wx/thread.h>
#include <wx/webview.h>
#include <wx/arrstr.h>
#include <wx/hashmap.h>
#include <wx/tokenzr.h>
#include <wx/uri.h>
#include <wx/filename.h>

#include <wx/jsonreader.h>
#include <wx/jsonwriter.h>
#include <curl/curl.h>
//#include <curl/types.h>
#include <curl/easy.h>
//#include <openssl/ssl.h>

enum
{
	ID_MENU_HELP_ABOUT = 1001,

	ID_BUTTON_TEST_API,
	ID_BUTTON_LOGOUT,

	ID_SSO_LOGIN_WEBVIEW,

    EVT_API_MSGBOX = wxID_HIGHEST + 1,
	EVT_API_LOGIN,
	EVT_API_CALLBACK
};

#define SSO_API_URL    wxT("https://barebonescms.com/sso/native_app/api/")
#define SSO_LOGIN_URL  wxT("https://barebonescms.com/sso/native_app/api/login.php")

class MainFrmApp : public wxApp
{
public:
	virtual ~MainFrmApp();

	virtual bool OnInit();
	virtual int OnExit();
};

class API;

// Separated from the API class so that users can't directly control the thread.
class APIThread : public wxThread
{
public:
	APIThread(API *APIPtr) : wxThread(wxTHREAD_DETACHED)
	{
		MxAPIPtr = APIPtr;
	}

	virtual ~APIThread();

protected:
	virtual ExitCode Entry();

private:
	void SendMsgBox(wxString Message, wxString Caption, long Style);

	API *MxAPIPtr;
};

class API
{
	friend class APIThread;

public:
	API() : MxThread()
	{
	}

	virtual ~API();

	bool InProgress();
	bool SaveCookie(wxString Cookie);

	// A test API call.  This is where you would expose your own API calls in a similar fashion.
	bool Test(wxEvtHandler *Handler, wxString CallbackType, void *CallbackData);
	bool Logout(wxEvtHandler *Handler, wxString CallbackType, void *CallbackData);

	class MsgBox
	{
	public:
		MsgBox()
		{
			MxStyle = 0;
		}

		MsgBox(wxString Message, wxString Caption, long Style)
		{
			MxCaption = Caption;
			MxMessage = Message;
			MxStyle = Style;
		}

		wxString MxCaption;
		wxString MxMessage;
		long MxStyle;
	};

	class Callback
	{
	public:
		wxString MxType;
		void *MxData;
		wxJSONValue MxResult;
	};

private:
	class ThreadInfo
	{
		friend class APIThread;

	public:
		ThreadInfo(wxEvtHandler *Handler, wxString CallbackType, void *CallbackData, bool HandleNoLogin = true);
		~ThreadInfo();

		wxString URLEscape(wxString Str);
		void SetGET(wxString Opts);
		void SetPOST(wxString Opts);

	private:
		wxEvtHandler *MxHandler;
		Callback MxCallback;

		CURL *MxCurl;
		struct curl_slist *MxHeaders;
		wxString MxURL, MxPostVars, MxCertFile, MxCookieFile;
		char MxErrorBuffer[CURL_ERROR_SIZE];
		bool MxHandleNoLogin;
		wxString MxCurlResult;
	};

	bool InternalExecute(ThreadInfo *ThreadInfoPtr);

	wxCriticalSection MxThreadLock;
	APIThread *MxThread;

	wxCriticalSection MxDataLock;
	ThreadInfo *MxThreadInfo;
};

class MainFrm;

// Primary globals.
class AppInfoStruct
{
public:
	AppInfoStruct()
	{
	}

	wxString MxAppDir, MxUserDir, MxLang;
	MainFrm *MxFrame;
	CURL *MxCurl;

	// Simple API management.
	API MxAPI;
} GxInfo;

size_t ProcessCurlData(void *ptr, size_t size, size_t nmemb, void *userdata);

class MainFrm : public wxFrame
{
public:
	MainFrm(wxWindow *parent, wxWindowID id = wxID_ANY, const wxString &title = wxT("SSO Client"), const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxCAPTION | wxRESIZE_BORDER | wxSYSTEM_MENU | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX);
	virtual ~MainFrm();

private:
	DECLARE_EVENT_TABLE();
	void OnFileExit(wxCommandEvent &Event);
	void OnClose(wxCloseEvent &Event);
	void OnAbout(wxCommandEvent &Event);
	void OnTestAPIButton(wxCommandEvent &Event);
	void OnLogoutButton(wxCommandEvent &Event);

	void OnAPIMsgBox(wxThreadEvent &Event);
	void OnAPILogin(wxThreadEvent &Event);
	void OnAPICallback(wxThreadEvent &Event);

	void APICallInProgress();
};

WX_DECLARE_STRING_HASH_MAP(wxArrayString, URLParamMap);

URLParamMap GetURLParameters(wxString URL);

class SSOLoginDlg : public wxDialog
{
public:
	SSOLoginDlg(wxWindow *parent, wxWindowID id = wxID_ANY, const wxString &title = wxT("SSO Client Login"), const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	virtual ~SSOLoginDlg();

private:
	wxWebView *MxWebView;

	DECLARE_EVENT_TABLE();
	void OnWebViewLoaded(wxWebViewEvent &Event);
};
