
// HlTestDlg.h : header file
//

#pragma once

#include "GuiInterface.h"
#include "ParseDef.h"
#include "SEHelper.h"
#include "LogParser.h"

#include "GameDetectPlugin.h"

#include <future>
#include <thread>
#include <chrono>

#include "AtomDef.h"

// CHlTestDlg dialog
class CHlTestDlg : public CDialogEx
{
// Construction
public:
	CHlTestDlg(CWnd* pParent = nullptr);	// standard constructor
	void AppendBoxString(const CString& strInfo);
	void GetBoxSelected(std::vector<CString>& vecData);

	void AsyncAction(const std::string& strAction, const std::string& strJson);
	void DoAction(const std::string& strAction, const std::string& strJson);

	bool IsGuiRunning() { return m_bRunning.load(); }

	/** 生产者、消费者线程处理
	*/
	void ProduceGuiData(const AsnycPipeData& data);
	void ConsumeGuiData();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HLTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	//void OnDropFiles(HDROP hDropInfo);


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAction();
	afx_msg void OnClickedRadio();
	afx_msg void OnSelchangeActionlist();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg LRESULT OnAgentCallBack(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnActionCallBack(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClickedClear();
	afx_msg void OnClose();
	afx_msg void OnBnClickedTest();
	afx_msg void OnBnClickedFileexist();
	afx_msg void OnBnClickedTestSecond();

	DECLARE_MESSAGE_MAP()
private:
	bool Init();
	bool IsMyTest();
	void AsyncAppendMsg(const std::string& strMsg);
	void AsyncAppendMsg(const std::wstring& strMsg);
	void AppendMsg(const std::string& strMsg);
	void AppendMsg(const std::wstring& strMsg);
	void OnOK() {};
	void AdjustMsgWnd();

	void StartGuiThreads();
	void StopHLThreads();
	/** 线程函数
	*/
	static unsigned __stdcall GuiThreadProc(void* pThis);

	/** 根据单选框获取当前选择的JsonPath
	*/
	std::wstring GetSelectedJsonPath();

	void FreshActionList();
	std::wstring GetActionTitle();
	std::string GetActionContent();
	
	/** 
	* \n 转成 \r\n    \r\n 转 \n
	*/
	CString JsonToControlText(const std::string& strJson);
	std::string ControlTextToJson(const CString& strText);

	void ProcessLogData(const LogData& data, int64_t qGap);

	/** 单元测试
	*/
	void UtCancelGenVideo();
	void CancelVideo();
	void ShowGenVideoList(const VideoGenInfoVec& vecData);
	void TestLangDLList();
	void TestStyleDLList();
	void TestGameDetect();
	std::string TestSha256();

private:
	CComboBox m_cbActionList;
    CString m_strInput;
    CString m_strOutput;
    CString m_strMsg;
	CString m_strCustomAction;
	CListBox m_lstBox;
	int m_actionType = 0;
	ActionMap	m_mapJsonAction;
	CLogParser	m_logParser;
	BOOL	m_bSaveParam;

	std::mutex		m_mtx;		
	std::future<void>	m_fParseLog;
	addin_interface_default_impl	m_gameDetect;

	HANDLE			m_handle = nullptr;
	unsigned int	m_Id = 0;
	std::atomic_bool m_bRunning = false;
	AsyncQueueType	m_guiQueueData;
};
