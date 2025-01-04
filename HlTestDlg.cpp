
// HlTestDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "FileHelper.h"
#include "UtilsTools.h"
#include "UtilsFile.h"
#include "Convert.h"
#include "UtilsString.h"
#include "JsonParser.h"
#include "MacroDef.h"
#include "HlTest.h"
#include "HlTestDlg.h"
#include "JsonBuilder.h"
#include "afxdialogex.h"

#pragma comment(lib, "Netapi32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UINT WM_USER_CALLBACK = WM_USER + 1;
UINT WM_USER_DOACTION = WM_USER + 2;


void CrossThreadPostMessage(UINT msgType, WPARAM wParam, LPARAM lParam)
{
	CWnd* pMainWnd = AfxGetApp()->m_pMainWnd;
	if (nullptr == pMainWnd) return;
	HWND hMainWindow = pMainWnd->GetSafeHwnd();

	PostMessage(hMainWindow, msgType, wParam, lParam);
}

void TestSyncChrCallBack(const char* action, const char* data, char** resp)
{
	OutputDebugStringA(data);

	CWnd* pMainWnd = AfxGetApp()->m_pMainWnd;
	if (nullptr == pMainWnd) return;
	HWND hMainWindow = pMainWnd->GetSafeHwnd();

	// data
	int len = strlen(data);
	if (len == 0) return;
	len += 1;
	char* pNewData = new char[len];
	strcpy_s(pNewData, len, data);

	// action
	len = strlen(action);
	len += 1;
	char* pNewAction = new char[len];
	strcpy_s(pNewAction, len, action);

	PostMessage(hMainWindow, WM_USER_CALLBACK, WPARAM(pNewAction), LPARAM(pNewData));
}

CHlTestDlg::CHlTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HLTEST_DIALOG, pParent)
	, m_actionType(0)
	, m_strInput(_T(""))
	, m_strOutput(_T(""))
	, m_strMsg(_T(""))
	, m_strCustomAction(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHlTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_ui, m_actionType);
	DDX_Control(pDX, IDC_ActionList, m_cbActionList);
	DDX_Text(pDX, IDC_Input, m_strInput);
	DDX_Text(pDX, IDC_Output, m_strOutput);
	DDX_Text(pDX, IDC_Msg, m_strMsg);
	DDX_Text(pDX, IDC_edtAction, m_strCustomAction);
	DDX_Control(pDX, IDC_lstBox, m_lstBox);
}

BEGIN_MESSAGE_MAP(CHlTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_Action, &CHlTestDlg::OnAction)
	ON_BN_CLICKED(IDC_ui, &CHlTestDlg::OnClickedRadio)
	ON_BN_CLICKED(IDC_Style, &CHlTestDlg::OnClickedRadio)
	ON_BN_CLICKED(IDC_Auto, &CHlTestDlg::OnClickedRadio)
	ON_BN_CLICKED(IDC_Manual, &CHlTestDlg::OnClickedRadio)
	ON_BN_CLICKED(IDC_MyTest, &CHlTestDlg::OnClickedRadio)
	ON_BN_CLICKED(IDC_VideoDb, &CHlTestDlg::OnClickedRadio)
	
	ON_CBN_SELCHANGE(IDC_ActionList, &CHlTestDlg::OnSelchangeActionlist)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BtnClear, &CHlTestDlg::OnClickedClear)
	ON_MESSAGE(WM_USER_CALLBACK, OnAgentCallBack)
	ON_MESSAGE(WM_USER_DOACTION, OnActionCallBack)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_Test, &CHlTestDlg::OnBnClickedTest)
	ON_BN_CLICKED(IDC_FileExist, &CHlTestDlg::OnBnClickedFileexist)
END_MESSAGE_MAP()


// CHlTestDlg message handlers

BOOL CHlTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if (!Init())
	{
		return FALSE;
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHlTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void CHlTestDlg::OnBnClickedTest()
{
	//std::vector<std::wstring> vecData = UtilsTools::GetUserLocalGroups();
	int64_t space = UtilsTools::GetDiskFreeSpace();

	space = space / 1024;

	std::wstring strInfo = UtilsString::FormatString(L"free space :%d", space);

	//LangDLList();
	StyleDLList();

	//UtilsTools::GetMemInfo();
	//bool ret = UtilsTools::IsUserAdmin();
	//while (true)
	//{
	//	DoAction(ACTION_ABILITY, "");
	//	Sleep(100);
	//}

	//m_f = std::async(std::launch::async, [&] {
	//	while (true)
	//	{
	//		//CrossThreadPostMessage(WM_USER_DOACTION, NULL, NULL);
	//		Sleep(1000);
	//	}
	//});

	//m_f.valid();

	AppendMsg("-------------");
	AppendMsg(strInfo.c_str());
	
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHlTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHlTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CHlTestDlg::Init()
{
	if (-1 == seinvoke_init(TestSyncChrCallBack))
	{
		AppendMsg("seinvoke_init failed");
		return false;
	}
    FreshActionList();
    DoAction(ACTION_GAMINGAI_ENTER, "");

	//DoAction(ACTION_GET_STYLEDL_LIST, "");	

	return true;
}

bool CHlTestDlg::IsMyTest()
{
	UpdateData(TRUE);

	return m_actionType == 4;
}

void CHlTestDlg::AppendMsg(const std::string& strMsg)
{
	std::string strMsgW = strMsg + ControlEnter;
	m_strMsg += Utility::utf8_2_unicode(strMsgW).c_str();

	AdjustMsgWnd();
	UpdateData(FALSE);
}

void CHlTestDlg::AppendMsg(const std::wstring& strMsg)
{
	m_strMsg += strMsg.c_str();
	m_strMsg += CString(ControlEnter);
	
	AdjustMsgWnd();
	UpdateData(FALSE);
}

void CHlTestDlg::AdjustMsgWnd()
{
	CWnd* pMsgWnd = GetDlgItem(IDC_Msg);
	if (nullptr != pMsgWnd)
	{
		pMsgWnd->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}
}

std::wstring CHlTestDlg::GetSelectedJsonPath()
{
    UpdateData(TRUE);

    std::wstring strPath = UtilsFile::GetPath() + HlData + JsonData;
    int actionType = m_actionType;
    switch (actionType)
    {
    case 0:
        strPath += JsonFramework;
        break;
    case 1:
        strPath += JsonStyle;
        break;
    case 2:
        strPath += JsonAutoRecord;
        break;
    case 3:
        strPath += JsonManualRecord;
        break;
    case 4:
        strPath += JsonMyTest;
        break;
    case 5:
        strPath += JsonVideoDb;
        break;
    default:
		return L"";
        break;
    }

	return strPath;
}

void CHlTestDlg::FreshActionList()
{
	std::wstring strPath = GetSelectedJsonPath();

	JsonParser::LoadJson(strPath, m_mapJsonAction);
	m_cbActionList.ResetContent();
	for (auto it : m_mapJsonAction)
	{
		CString strFileName = it.first.c_str();
		m_cbActionList.AddString(strFileName);
	}
	if (m_cbActionList.GetCount() > 0)
	{
		m_cbActionList.SetCurSel(0);
	}

	UpdateData(FALSE);
}

void CHlTestDlg::OnAction()
{
	UpdateData(TRUE);
	std::string strAction;
	std::string strJson = ControlTextToJson(m_strInput);

	if (!m_strCustomAction.IsEmpty())
    {
        //手动输入参数
        strAction = Utility::unicode_2_utf8(m_strCustomAction);
        AppendMsg("manual action");
    }
    else
    {
        //使用组合框
		std::wstring strTitle = GetActionTitle();
		if (IsMyTest())
			strAction = ACTION_MYTEST;
		else
			strAction = Utility::unicode_2_utf8(strTitle);
        
		std::wstring strFullName = GetSelectedJsonPath() + L"/" + strTitle + L".json";
		//落盘
		UtilsTools::SaveLocalData(strFullName, strJson);
    }

	DoAction(strAction, strJson);
}

std::wstring CHlTestDlg::GetActionTitle()
{
    int nIdx = m_cbActionList.GetCurSel();
    CString strCBText;
    m_cbActionList.GetLBText(nIdx, strCBText);

	return strCBText;
}

std::string CHlTestDlg::GetActionContent()
{
	std::wstring strTitle = GetActionTitle();
	auto it = m_mapJsonAction.find(strTitle);
	if (it == m_mapJsonAction.end())
	{
		return "";
	}

	return it->second;
}

void CHlTestDlg::DoAction(const std::string& strAction, const std::string& strJson)
{
    char szResponse[1024] = { 0 };
    int ret = seinvoke_send(ModuleId, strAction.c_str(), strJson.c_str(), szResponse, 1024);
	m_strOutput = Utility::utf8_2_unicode(szResponse).c_str();
	UpdateData(FALSE);
}

CString CHlTestDlg::JsonToControlText(const std::string& strJson)
{
	std::string strText = UtilsTools::ReplaceString(strJson, TextEnter, ControlEnter);
	std::wstring strTextW = Utility::utf8_2_unicode(strText);

	return strTextW.c_str();
}

std::string CHlTestDlg::ControlTextToJson(const CString& strText)
{
	std::string strJson = Utility::unicode_2_utf8(strText);
	strJson = UtilsTools::ReplaceString(strJson, ControlEnter, TextEnter);
	return strJson;
}

void CHlTestDlg::OnClickedRadio()
{
	FreshActionList();
}

void CHlTestDlg::OnSelchangeActionlist()
{
	m_strInput = JsonToControlText(GetActionContent());
	m_strCustomAction = L"";
	UpdateData(FALSE);
}

LRESULT CHlTestDlg::OnAgentCallBack(WPARAM wParam, LPARAM lParam)
{
	char* pAction = (char*)wParam;
	char* pData = (char*)lParam;
	if (nullptr == pData)
	{
		return 0;
	}

	std::string strAction = std::string(pAction);
	std::string strData = std::string(pData);

	//std::string strMsg = std::string(pAction) + " -> " + std::string(pData);
	std::string strMsg = strAction + " -> " + strData;
	AppendMsg(strMsg);
	delete[]pAction;
	delete[]pData;

	//parse action
	if (strAction == std::string(ACTION_NOTIFY_HL_GVP))
	{
		VideoGenInfoVec GenList;
		JsonParser::ParseGenVideoList(strData, GenList);
		ShowGenVideoList(GenList);
	}

	return 0;
}

LRESULT CHlTestDlg::OnActionCallBack(WPARAM wParam, LPARAM lParam)
{
	std::wstring strMsg = std::to_wstring(rand());
	AppendMsg(strMsg);
	return 0;
}

void CHlTestDlg::OnDropFiles(HDROP hDropInfo)
{
    WCHAR szFileName[MAX_PATH] = L"";
    UINT nFileNum = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);//DragQueryFile第二参数为为 0xFFFFFFFF的时候返回拖曳的文件个数
    DragQueryFile(hDropInfo, nFileNum - 1, szFileName, MAX_PATH);//获得拖曳的最后一个文件的文件名
    DragFinish(hDropInfo); //用完要记得释放掉hDropInfo

	AppendMsg(UtilsString::FormatString(L"onDropFile:%s", szFileName));

	m_logParser.ParseLog(szFileName);
    ServicesVec& services = m_logParser.GetServices();
    for (auto& itSvr : services)
    {
        for (auto& it : itSvr.m_vecData)
        {
			ProcessLogData(it, itSvr.m_timeGap);
        }
    }

	AppendMsg("finish DropFile");
	CDialogEx::OnDropFiles(hDropInfo);
}

void CHlTestDlg::ProcessLogData(const LogData& data, int64_t qGap)
{
    while (1) {
        if ( UtilsTools::GetPastTime(data.m_time) > qGap)
        {
            //AppendMsg("k is:" + QString::number(k) + " date time:" + QString::number(data.m_time.toMSecsSinceEpoch()));
            break;
        }

		std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    if (data.m_type == TriggerType::TrigHL)
    {
        std::string strJson = CJsonBuilder::ImageDataToJson(data.m_imageData);
        AppendMsg("-----------------json start--------------------------------------");
        AppendMsg(strJson);
        AppendMsg("------------------json end --------------------------------------");
        DoAction(ACTION_MYTEST, strJson);
    }
    //AppendMsg("Received signal with value, oper type:" + QString::number((int)data.m_type));
}

void CHlTestDlg::UtCancelGenVideo()
{
	std::string strJson = CJsonBuilder::BuildCancelGenVideo();
	DoAction(ACTION_MYTEST, strJson);
	AppendMsg(L"cancle genvideo");
}

void CHlTestDlg::CancelVideo()
{
	int nSel = m_lstBox.GetCurSel();
	if (nSel < 0) return;

	CString strText;
	m_lstBox.GetText(nSel, strText);
	std::string strRecordTime = Utility::unicode_2_utf8(strText);

	if (!strRecordTime.empty())
	{
		std::string strJson = CJsonBuilder::BuildCancelGenVideo(strRecordTime);
		DoAction(ACTION_CANCEL_VIDEO, strJson);
	}
}

void CHlTestDlg::ShowGenVideoList(const VideoGenInfoVec& vecData)
{
	for (auto& it : vecData)
	{
		std::wstring strRTW = Utility::utf8_2_unicode(it.m_info.m_strRecordTime);
		CString strRecordTime = strRTW.c_str();
		AppendBoxString(strRecordTime);
	}
}

void CHlTestDlg::LangDLList()
{
	for (int i = 8; i < 12; ++ i)
	{
		std::string strJson = CJsonBuilder::BuildLangDL(i);
		DoAction(ACTION_DOWNLOAD_LANG_PACKAGE, strJson);
	}
}

void CHlTestDlg::StyleDLList()
{
	for (int i = 2; i <= 8; ++i)
	{
		std::string strJson = CJsonBuilder::BuildStyleDL(i);
		DoAction(ACTION_SYNC_STYLE_DATA, strJson);
	}
}

std::string CHlTestDlg::TestSha256()
{
	BYTE hashBuffer[32] = { 0 };

	std::string strSha256;
	if (UtilsTools::GetFileHash256("d:\\tmp\\kr.zip", hashBuffer)) {
		for (int i = 0; i < 32; ++i)
		{
			//printf("%02x", hashBuffer[i]);
			std::string strTmp = UtilsString::FormatString("%02x", hashBuffer[i]);
			strSha256 = UtilsString::FormatString("%s%s", strSha256.c_str(), strTmp.c_str());
		}
		printf("\n");
	}
	else {
		std::cerr << "Failed to calculate SHA-256 hash of the file." << std::endl;
	}

	return strSha256;
}

void CHlTestDlg::OnClickedClear()
{
	m_strInput = "";
	m_strOutput = "";
	m_strMsg = "";
	m_lstBox.ResetContent();

	UpdateData(FALSE);
}

BOOL CHlTestDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		OnAction();
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CHlTestDlg::OnClose()
{
	//DoAction(Action_Exit, "");
	CDialogEx::OnClose();
}

void CHlTestDlg::AppendBoxString(const CString& strInfo)
{
	int cnt = m_lstBox.GetCount();
	m_lstBox.InsertString(cnt, strInfo);
}

//for multile selected
void CHlTestDlg::GetBoxSelected(std::vector<CString>& vecData)
{
	//获得当前选中的行数
	int list_sel = m_lstBox.GetSelCount();
	if (list_sel > 0) {
		//新建一个变量来作为缓冲区来存放
		int* p = new int[list_sel];
		//获取选中的数据
		//第一个参数是数量，第二个参数是缓冲区
		m_lstBox.GetSelItems(list_sel, p);

		for (int i = 0; i < list_sel; i++) {
			//获取选中的序列号
			int i_sel = p[i];
			CString strInfo;
			m_lstBox.GetText(i_sel, strInfo);
			vecData.push_back(strInfo);
		}

		delete []p;
	}
}

void CHlTestDlg::OnBnClickedFileexist()
{
	UpdateData(TRUE);
	std::wstring strFileName = m_strInput;
	CString strTip;
	strTip.Format(_T("文件是否存在：%d"), UtilsFile::IsFileExists(strFileName));
	::MessageBox(0, strTip, L"title", 0);

	UpdateData(FALSE);
}
