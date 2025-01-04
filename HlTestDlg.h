
// HlTestDlg.h : header file
//

#pragma once

#include "GuiInterface.h"
#include "ParseDef.h"
#include "SEHelper.h"
#include "LogParser.h"

#include <future>
#include <thread>
#include <chrono>

// CHlTestDlg dialog
class CHlTestDlg : public CDialogEx
{
// Construction
public:
	CHlTestDlg(CWnd* pParent = nullptr);	// standard constructor
	void AppendBoxString(const CString& strInfo);
	void GetBoxSelected(std::vector<CString>& vecData);

	void DoAction(const std::string& strAction, const std::string& strJson);

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

	DECLARE_MESSAGE_MAP()
private:
	bool Init();
	bool IsMyTest();
	void AppendMsg(const std::string& strMsg);
	void AppendMsg(const std::wstring& strMsg);
	void OnOK() {};
	void AdjustMsgWnd();

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
	void LangDLList();
	void StyleDLList();
	std::string TestSha256();

private:
	CComboBox m_cbActionList;
    CString m_strInput;
    CString m_strOutput;
    CString m_strMsg;
	CString m_strCustomAction;
	CListBox m_lstBox;
	int m_actionType;
	ActionMap	m_mapJsonAction;
	CLogParser	m_logParser;

	std::future<void>	m_f;
};
