// Single Sign-On integration for Windows, Mac, and Linux.
// (C) 2013 CubicleSoft.  All Rights Reserved.

#include "main.h"

IMPLEMENT_APP(MainFrmApp)

wxString GetAppDir()
{
	wxString Result;
	wxStandardPaths TempPaths;

	Result = TempPaths.GetExecutablePath();
	Result.Replace(wxT("\\"), wxT("/"));

	return Result.BeforeLast(wxT('/'));
}

wxString GetUserDir()
{
	wxString Result;
	wxStandardPaths TempPaths;

	Result = TempPaths.GetUserDataDir();
	Result.Replace(wxT("\\"), wxT("/"));

	return Result;
}

URLParamMap GetURLParameters(wxString URL)
{
	int Pos;
	wxArrayString Values;
	URLParamMap Result;

	Pos = URL.Find(wxT("?"));
	if (Pos == wxNOT_FOUND)  Values.Add(URL);
	else
	{
		Values.Add(URL.Left(Pos));
		URL = URL.Mid(Pos + 1);
	}

	Result[""] = Values;

	wxStringTokenizer Tokens(URL, wxT("&"));
	while (Tokens.HasMoreTokens())
	{
		wxString Token = Tokens.GetNextToken();
		if (!Token.empty())
		{
			wxString Name, Value;

			Pos = Token.Find(wxT("="));
			if (Pos == wxNOT_FOUND)  Name = wxURI::Unescape(Token);
			else
			{
				Name = wxURI::Unescape(Token.Left(Pos));
				Value = wxURI::Unescape(Token.Mid(Pos + 1));
			}

			if (!Name.empty())
			{
				Values = Result[Name];
				Values.Add(Value);
				Result[Name] = Values;
			}
		}
	}

	return Result;
}

wxString URLEscape(CURL *Curl, wxString Str)
{
	char *TempEsc;
	wxString Result;

	TempEsc = curl_easy_escape(Curl, Str.mb_str(), 0);
	Result = TempEsc;
	curl_free(TempEsc);

	return Result;
}

wxString MakeMozillaCookie(wxString Domain, bool Subdomains, wxString Path, bool SSLOnly, int Timeout, wxString Name, wxString Value)
{
	wxString Result;

	Result = Domain + wxT("\t");
	Result += (Subdomains ? wxT("TRUE\t") : wxT("FALSE\t"));
	Result += Path + wxT("\t");
	Result += (SSLOnly ? wxT("TRUE\t") : wxT("FALSE\t"));
	Result += wxString::Format(wxT("%i"), Timeout) + wxT("\t");
	Result += Name + wxT("\t");
	Result += Value;

	return Result;
}


MainFrmApp::~MainFrmApp()
{
}

bool MainFrmApp::OnInit()
{
	GxInfo.MxAppDir = ::GetAppDir();
	GxInfo.MxUserDir = ::GetUserDir();
	wxFileName::Mkdir(GxInfo.MxUserDir, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

	wxLocale locale(wxLANGUAGE_DEFAULT, wxLOCALE_LOAD_DEFAULT);
	GxInfo.MxLang = locale.GetCanonicalName();
	GxInfo.MxLang.Replace(wxT("_"), wxT("-"));

	// Global initialization.
//	SSL_load_error_strings();
//	SSL_library_init();
	curl_global_init(CURL_GLOBAL_ALL);

	GxInfo.MxCurl = curl_easy_init();

	// Start the main UI.
	MainFrm *frame = new MainFrm(NULL);
	SetTopWindow(frame);
	frame->Show();

	return true;
}

int MainFrmApp::OnExit()
{
	curl_easy_cleanup(GxInfo.MxCurl);

	return 0;
}


BEGIN_EVENT_TABLE(MainFrm, wxFrame)
	EVT_CLOSE(MainFrm::OnClose)
	EVT_MENU(wxID_EXIT, MainFrm::OnFileExit)
	EVT_MENU(ID_MENU_HELP_ABOUT, MainFrm::OnAbout)
	EVT_BUTTON(ID_BUTTON_TEST_API, MainFrm::OnTestAPIButton)
	EVT_BUTTON(ID_BUTTON_LOGOUT, MainFrm::OnLogoutButton)

	EVT_THREAD(EVT_API_MSGBOX, MainFrm::OnAPIMsgBox)
	EVT_THREAD(EVT_API_LOGIN, MainFrm::OnAPILogin)
	EVT_THREAD(EVT_API_CALLBACK, MainFrm::OnAPICallback)
END_EVENT_TABLE()

MainFrm::MainFrm(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize &size, long style)
: wxFrame(parent, id, title, position, size, style)
{
	GxInfo.MxFrame = this;

	// Main menu.
	wxMenuBar *WxMenuBar = new wxMenuBar;
	wxMenu *OptionsMenu = new wxMenu;
	OptionsMenu->Append(wxID_EXIT, wxT("E&xit\tAlt+F4"), wxT("Exit the application."), wxITEM_NORMAL);
	WxMenuBar->Append(OptionsMenu, wxT("&Options"));

	wxMenu *HelpMenu = new wxMenu;
	HelpMenu->Append(ID_MENU_HELP_ABOUT, wxT("&About..."), wxT("About SSO Client."), wxITEM_NORMAL);
	WxMenuBar->Append(HelpMenu, wxT("&Help"));
	SetMenuBar(WxMenuBar);

	// Create a panel that fills the entire window.
	// Otherwise the wxFrame gets an ugly background color.
	wxPanel *Panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("MainPanel"));

	// Buttons.
	wxBoxSizer *BoxSizer = new wxBoxSizer(wxVERTICAL);
	BoxSizer->AddStretchSpacer();
	wxButton *Button = new wxButton(Panel, ID_BUTTON_TEST_API, "Test API");
	BoxSizer->Add(Button, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_HORIZONTAL, 4);
	Button = new wxButton(Panel, ID_BUTTON_LOGOUT, "Logout");
	BoxSizer->Add(Button, 0, wxLEFT | wxRIGHT | wxTOP | wxALIGN_CENTER_HORIZONTAL, 4);
	BoxSizer->AddStretchSpacer();
	Panel->SetSizer(BoxSizer);

	SetSize(0, 0, 400, 320);
	SetMinSize(wxSize(400, 320));
	Center();
}

MainFrm::~MainFrm()
{
}

void MainFrm::OnClose(wxCloseEvent &Event)
{
	if (GxInfo.MxAPI.InProgress())  wxMessageBox(wxT("SSO Client is busy at the moment.  Please try again later."), wxT("Thread still running"), wxOK | wxICON_EXCLAMATION);
	else  Destroy();
}

void MainFrm::OnFileExit(wxCommandEvent &Event)
{
	Close();
}

void MainFrm::OnAbout(wxCommandEvent &Event)
{
	wxMessageBox(wxT("Single Sign-On integration for Windows, Mac, and Linux.\n(C) 2013 CubicleSoft.  All Rights Reserved."), wxT("About SSO Client"), wxOK | wxICON_INFORMATION);
}

void MainFrm::OnTestAPIButton(wxCommandEvent &Event)
{
	if (!GxInfo.MxAPI.Test(this, "test", NULL))  APICallInProgress();
}

void MainFrm::OnLogoutButton(wxCommandEvent &Event)
{
	if (!GxInfo.MxAPI.Logout(this, "logout", NULL))  APICallInProgress();
}

void MainFrm::APICallInProgress()
{
	wxMessageBox(wxT("An API call is still in progress.  Try again in a few moments."), wxT("API call in progress"), wxOK | wxICON_EXCLAMATION);
}

void MainFrm::OnAPIMsgBox(wxThreadEvent &Event)
{
	API::MsgBox TempMsgBox = Event.GetPayload<API::MsgBox>();

	wxMessageBox(TempMsgBox.MxMessage, TempMsgBox.MxCaption, TempMsgBox.MxStyle);
}

void MainFrm::OnAPILogin(wxThreadEvent &Event)
{
	if (wxMessageBox(wxT("This action requires an active login session but you are not signed in.  Would you like to sign in?"), wxT("Not signed in"), wxYES_NO) == wxYES)
	{
		SSOLoginDlg Dialog(this);
		Dialog.ShowModal();
	}
}

void MainFrm::OnAPICallback(wxThreadEvent &Event)
{
	API::Callback TempCallback = Event.GetPayload<API::Callback>();

	wxMessageBox(TempCallback.MxResult["msg"].AsString(), wxT("Message from server for '") + TempCallback.MxType + wxT("'"), wxOK | wxICON_INFORMATION);
}


APIThread::~APIThread()
{
	wxCriticalSectionLocker enter(MxAPIPtr->MxThreadLock);
	MxAPIPtr->MxThread = NULL;

	wxCriticalSectionLocker enter2(MxAPIPtr->MxDataLock);
	delete MxAPIPtr->MxThreadInfo;
	MxAPIPtr->MxThreadInfo = NULL;
}

wxThread::ExitCode APIThread::Entry()
{
	wxCriticalSectionLocker enter(MxAPIPtr->MxDataLock);

	CURLcode CurlResult;
	CurlResult = curl_easy_perform(MxAPIPtr->MxThreadInfo->MxCurl);
	wxString ErrorStr;

	if (CurlResult)
	{
		wxString TempStr;

		ErrorStr = MxAPIPtr->MxThreadInfo->MxErrorBuffer;
		ErrorStr += "\n\n";
		TempStr.Printf("Curl result: %d\n", CurlResult);
		ErrorStr += TempStr;

		SendMsgBox(ErrorStr, "cURL Transfer Error", wxOK | wxICON_EXCLAMATION);
	}
	else
	{
		wxJSONReader Reader;
		if (Reader.Parse(MxAPIPtr->MxThreadInfo->MxCurlResult, &MxAPIPtr->MxThreadInfo->MxCallback.MxResult) > 0)
		{
			wxArrayString::iterator ErrorIter;
			ErrorStr.Empty();
			wxArrayString JSONErrors = Reader.GetErrors();
			for (ErrorIter = JSONErrors.begin(); ErrorIter != JSONErrors.end(); ++ErrorIter)
			{
				ErrorStr += *ErrorIter;
				ErrorStr += wxT("\n");
			}

			SendMsgBox(ErrorStr + wxT("\n\n") + MxAPIPtr->MxThreadInfo->MxCurlResult, wxT("Malformed JSON data"), wxOK | wxICON_EXCLAMATION);
		}
		else if (MxAPIPtr->MxThreadInfo->MxCallback.MxResult["success"].AsBool())
		{
			wxThreadEvent TempEvent(wxEVT_THREAD, EVT_API_CALLBACK);
			TempEvent.SetPayload(MxAPIPtr->MxThreadInfo->MxCallback);
			wxQueueEvent(MxAPIPtr->MxThreadInfo->MxHandler, TempEvent.Clone());
		}
		else if (MxAPIPtr->MxThreadInfo->MxHandleNoLogin && MxAPIPtr->MxThreadInfo->MxCallback.MxResult["errorcode"].AsString() == wxT("no_login"))
		{
			wxThreadEvent TempEvent(wxEVT_THREAD, EVT_API_LOGIN);
			wxQueueEvent(MxAPIPtr->MxThreadInfo->MxHandler, TempEvent.Clone());
		}
		else
		{
			SendMsgBox(MxAPIPtr->MxThreadInfo->MxCallback.MxResult["error"].AsString() + wxT(" (") + MxAPIPtr->MxThreadInfo->MxCallback.MxResult["errorcode"].AsString() + wxT(")"), wxT("Server responded with an error"), wxOK | wxICON_EXCLAMATION);
		}
	}

	return (wxThread::ExitCode)0;
}

void APIThread::SendMsgBox(wxString Message, wxString Caption, long Style)
{
	API::MsgBox TempMsgBox(Message, Caption, Style);
	wxThreadEvent TempEvent(wxEVT_THREAD, EVT_API_MSGBOX);
	TempEvent.SetPayload(TempMsgBox);
	wxQueueEvent(MxAPIPtr->MxThreadInfo->MxHandler, TempEvent.Clone());
}


API::~API()
{
	{
		wxCriticalSectionLocker enter(MxThreadLock);

		if (MxThread)
		{
			if (MxThread->Delete() != wxTHREAD_NO_ERROR)  return;
		}
	}

	while (1)
	{
		{
			wxCriticalSectionLocker enter(MxThreadLock);
			if (MxThread == NULL)  return;
		}

		wxThread::This()->Sleep(1);
	}
}

bool API::InProgress()
{
	wxCriticalSectionLocker enter(MxThreadLock);

	return (MxThread != NULL);
}

bool API::SaveCookie(wxString Cookie)
{
	wxCriticalSectionLocker enter(MxThreadLock);

	if (MxThread != NULL)  return false;

	CURL *Curl = curl_easy_init();
	wxString CookieFile = GxInfo.MxUserDir + wxT("/curl_cookies.dat");
	curl_easy_setopt(Curl, CURLOPT_COOKIEFILE, CookieFile.mb_str().data());
	curl_easy_setopt(Curl, CURLOPT_COOKIEJAR, CookieFile.mb_str().data());
	curl_easy_setopt(Curl, CURLOPT_COOKIELIST, Cookie.mb_str().data());
	curl_easy_cleanup(Curl);

	return true;
}

// A test API call.  This is where you would expose your own API calls in a similar fashion.
bool API::Test(wxEvtHandler *Handler, wxString CallbackType, void *CallbackData)
{
	ThreadInfo *ThreadInfoPtr = new ThreadInfo(Handler, CallbackType, CallbackData);

	ThreadInfoPtr->SetGET("action=test");

	wxString PostOpts = "opt1=";
	PostOpts += ThreadInfoPtr->URLEscape("test");
	PostOpts += "&opt2=";
	PostOpts += ThreadInfoPtr->URLEscape("test2");
	ThreadInfoPtr->SetPOST(PostOpts);

	return InternalExecute(ThreadInfoPtr);
}

bool API::Logout(wxEvtHandler *Handler, wxString CallbackType, void *CallbackData)
{
	ThreadInfo *ThreadInfoPtr = new ThreadInfo(Handler, CallbackType, CallbackData, false);

	ThreadInfoPtr->SetGET("action=logout");

	return InternalExecute(ThreadInfoPtr);
}

bool API::InternalExecute(ThreadInfo *ThreadInfoPtr)
{
	wxCriticalSectionLocker enter(MxThreadLock);

	if (MxThread != NULL)
	{
		delete ThreadInfoPtr;

		return false;
	}

	MxThread = new APIThread(this);

	wxCriticalSectionLocker enter2(MxDataLock);
	MxThreadInfo = ThreadInfoPtr;

	return (MxThread->Run() == wxTHREAD_NO_ERROR);
}


API::ThreadInfo::ThreadInfo(wxEvtHandler *Handler, wxString CallbackType, void *CallbackData, bool HandleNoLogin)
{
	MxHandler = Handler;
	MxCallback.MxType = CallbackType;
	MxCallback.MxData = CallbackData;

	MxCurl = curl_easy_init();
	MxHeaders = NULL;

	MxCertFile = GxInfo.MxAppDir + wxT("/cacert.pem");
	MxCookieFile = GxInfo.MxUserDir + wxT("/curl_cookies.dat");
	curl_easy_setopt(MxCurl, CURLOPT_CAINFO, MxCertFile.mb_str().data());
	curl_easy_setopt(MxCurl, CURLOPT_COOKIEFILE, MxCookieFile.mb_str().data());
	curl_easy_setopt(MxCurl, CURLOPT_COOKIEJAR, MxCookieFile.mb_str().data());
	curl_easy_setopt(MxCurl, CURLOPT_ERRORBUFFER, MxErrorBuffer);
	curl_easy_setopt(MxCurl, CURLOPT_WRITEFUNCTION, ProcessCurlData);
	curl_easy_setopt(MxCurl, CURLOPT_WRITEDATA, &MxCurlResult);
	MxHeaders = curl_slist_append(MxHeaders, "Expect:");
	curl_easy_setopt(MxCurl, CURLOPT_HTTPHEADER, MxHeaders);

	MxHandleNoLogin = HandleNoLogin;
}

API::ThreadInfo::~ThreadInfo()
{
	// Cleanup.
	curl_easy_cleanup(MxCurl);
	curl_slist_free_all(MxHeaders);
}

wxString API::ThreadInfo::URLEscape(wxString Str)
{
	return ::URLEscape(MxCurl, Str);
}

void API::ThreadInfo::SetGET(wxString Opts)
{
	MxURL = SSO_API_URL;
	MxURL += (MxURL.Find(wxT('?')) == wxNOT_FOUND ? wxT("?") : wxT("&"));
	MxURL += Opts;

	curl_easy_setopt(MxCurl, CURLOPT_URL, MxURL.mb_str().data());
}

void API::ThreadInfo::SetPOST(wxString Opts)
{
	MxPostVars = Opts;

	curl_easy_setopt(MxCurl, CURLOPT_POST, 1);
	curl_easy_setopt(MxCurl, CURLOPT_POSTFIELDS, MxPostVars.mb_str().data());
	curl_easy_setopt(MxCurl, CURLOPT_POSTFIELDSIZE, MxPostVars.mb_str().length());
}


size_t ProcessCurlData(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	*((wxString *)userdata) += wxString::From8BitData((char *)ptr, size * nmemb);

	return size * nmemb;
}


BEGIN_EVENT_TABLE(SSOLoginDlg, wxDialog)
	EVT_WEBVIEW_LOADED(ID_SSO_LOGIN_WEBVIEW, SSOLoginDlg::OnWebViewLoaded)
END_EVENT_TABLE()

SSOLoginDlg::SSOLoginDlg(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize &size, long style)
: wxDialog(parent, id, title, position, size, style)
{
	wxString URL = SSO_LOGIN_URL;
	URL += (URL.Find(wxT('?')) == wxNOT_FOUND ? wxT("?") : wxT("&"));
	URL += wxT("lang=");
	URL += ::URLEscape(GxInfo.MxCurl, GxInfo.MxLang);

	wxBoxSizer *BoxSizer = new wxBoxSizer(wxVERTICAL);
	MxWebView = wxWebView::New(this, ID_SSO_LOGIN_WEBVIEW, SSO_LOGIN_URL);
	BoxSizer->Add(MxWebView, 1, wxEXPAND);
	SetSizer(BoxSizer);

	SetSize(0, 0, 800, 600);
	SetMinSize(wxSize(500, 550));
	Center();
}

SSOLoginDlg::~SSOLoginDlg()
{
}

void SSOLoginDlg::OnWebViewLoaded(wxWebViewEvent &Event)
{
	URLParamMap Params = GetURLParameters(Event.GetURL());
	if (Params[""][0] == SSO_LOGIN_URL && Params.find("action") != Params.end())
	{
		if (Params["action"][0] == wxT("sso_done"))
		{
			wxString URL = SSO_LOGIN_URL;
			URL += (URL.Find(wxT('?')) == wxNOT_FOUND ? wxT("?") : wxT("&"));
			URL += wxT("action=sso_alt_done");
			MxWebView->LoadURL(URL);
		}
		else if (Params["action"][0] == wxT("sso_alt_done2"))
		{
			wxString ErrorStr, Result;

			Result = MxWebView->GetPageText();

			wxJSONValue Data;
			wxJSONReader Reader;
			if (Reader.Parse(Result, &Data) > 0)
			{
				wxArrayString::iterator ErrorIter;
				ErrorStr.Empty();
				wxArrayString JSONErrors = Reader.GetErrors();
				for (ErrorIter = JSONErrors.begin(); ErrorIter != JSONErrors.end(); ++ErrorIter)
				{
					ErrorStr += *ErrorIter;
					ErrorStr += wxT("\n");
				}

				wxMessageBox(ErrorStr + wxT("\n\n") + Result, wxT("Malformed JSON data"), wxOK | wxICON_EXCLAMATION);
			}
			else if (!Data["success"].AsBool())
			{
				wxMessageBox(Data["error"].AsString() + wxT(" (") + Data["errorcode"].AsString() + wxT(")"), wxT("Server responded with an error"), wxOK | wxICON_EXCLAMATION);
			}
			else
			{
				bool SSLOnly;
				wxString Name, Domain, Path, CookieC, CookieS, CookieV, Cookie;
				int CookieSTimeout, CookieVTimeout;

				// Load info.
				Name = Data["name"].AsString();
				Domain = Data["domain"].AsString();
				Path = Data["path"].AsString();
				SSLOnly = Data["ssl"].AsBool();
				CookieC = Data["c"].AsString();
				CookieS = Data["s"].AsString();
				CookieV = Data["v"].AsString();
				CookieSTimeout = Data["st"].AsInt();
				CookieVTimeout = Data["vt"].AsInt();

				// Set cookies.
				Cookie = MakeMozillaCookie(Domain, false, Path, SSLOnly, 0, Name + wxT("_c"), CookieC);
				if (!GxInfo.MxAPI.SaveCookie(Cookie))  wxMessageBox(wxT("Unable to save cookies.  Most likely cause is an API call in progress."), wxT("Cookie Error"), wxOK | wxICON_EXCLAMATION);
				Cookie = MakeMozillaCookie(Domain, false, Path, SSLOnly, CookieSTimeout, Name + wxT("_s"), CookieS);
				if (!GxInfo.MxAPI.SaveCookie(Cookie))  wxMessageBox(wxT("Unable to save cookies.  Most likely cause is an API call in progress."), wxT("Cookie Error"), wxOK | wxICON_EXCLAMATION);
				Cookie = MakeMozillaCookie(Domain, false, Path, SSLOnly, CookieVTimeout, Name + wxT("_v"), CookieV);
				if (!GxInfo.MxAPI.SaveCookie(Cookie))  wxMessageBox(wxT("Unable to save cookies.  Most likely cause is an API call in progress."), wxT("Cookie Error"), wxOK | wxICON_EXCLAMATION);
			}

			EndModal(0);
		}
	}
}
