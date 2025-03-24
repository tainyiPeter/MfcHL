#ifndef _DATADEFINE_H_
#define _DATADEFINE_H_

#include <TCHAR.H>

#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

#ifdef _UNICODE
typedef	wstring			TSTRING;

#define TOLOWER			towlower
#define TOUPPER			towupper
#define STRCPY			wcscpy
#define STRCAT			wcscat

#define STRTOINT		_wtoi
#define INTTOSTR		_itow
#define STRLEN			wcslen

#define SPRINTF			swprintf

#define OFSTREAM		wofstream
#define STRINGSTREAM	wstringstream

#else
typedef	string			TSTRING;

#define TOLOWER			tolower
#define TOUPPER			toupper
#define STRCPY			strcpy
#define STRCAT			strcat

#define STRTOINT		atoi
#define INTTOSTR		_itoa
#define STRLEN			strlen

#define SPRINTF			sprintf

#define OFSTREAM		ofstream
#define STRINGSTREAM	stringstream

#endif

//string find result
#define NPOS  -1

//------------------------------------------------------------------------------------------------------
//												file
//------------------------------------------------------------------------------------------------------
#define TIME_LEN 16
#define MAX_FILEINDEX 1000

#define MAX_PATH_IN_BYTES (MAX_PATH*2) //#299277 by Y.tsukamoto

#define ID_TAGNAME_RUN_TYPE									_T("run_type")
#define ID_TAGNAME_SETREFCOUNTONLY							_T("SetRefCountOnly")

#define ID_TAGVALUE_ALL										_T("$(all)")
#define ID_TAGVALUE_CAPTURE									_T("capture")
#define ID_TAGVALUE_TRUE									_T("true")
#define ID_TAGVALUE_FALSE									_T("false")

//password
#define ID_TAGNAME_PASSWORD									_T("controlcard.Password")
#define ID_TAGNAME_PLAINPASSWORD							_T("PlainPassword")

//misc
#define ID_TAGNAME_MISC										_T("controlcard.MISC")
#define ID_TAGNAME_TEMP_FILE_LOCATION						_T("temp_file_location")
#define ID_TAGNAME_LOG_FILE_LOCATION						_T("log_file_location")
#define ID_TAGNAME_FILE_SPAN_SIZE							_T("file_span_size")
#define ID_TAGNAME_GLOBAL_PASSWORD							_T("global_password")	// 332427,332428  2006/06/06  hana  // SMA Kure  Global password
#define ID_TAGNAME_CANCEL_LOGON_USER						_T("cancel_logon_user")	// 2006/06/22 SMA Kure CancelFGUser hana  

//Application
#define ID_TAGNAME_APPLICATIONS								_T("controlcard.Applications")
#define ID_TAGNAME_APPLICATION								_T("Application")

//file filter
#define ID_TAGNAME_CONTROLCARD_FILEANDFOLDER				_T("controlcard.FilesAndFolders")
#define ID_TAGNAME_CONTROLCARD_FILEANDFOLDER_RUN			_T("run")
#define ID_TAGNAME_CONTROLCARD_INCLUSIONS					_T("controlcard.Inclusions")
#define ID_TAGNAME_INCDESCRIPTION_FILEDESCRIPTION			_T("IncDescription.Description")
#define ID_TAGNAME_INCDESCRIPTION_DEST						_T("IncDescription.Dest")
#define ID_TAGNAME_INCDESCRIPTION_DATECOMPARE_OPERAND		_T("IncDescription.DateCompare.Operand")
#define ID_TAGNAME_INCDESCRIPTION_DATECOMPARE_DATE			_T("IncDescription.DateCompare.Date")
#define ID_TAGNAME_INCDESCRIPTION_SIZECOMPARE_OPERAND		_T("IncDescription.SizeCompare.Operand")
#define ID_TAGNAME_INCDESCRIPTION_SIZECOMPARE_SIZE			_T("IncDescription.SizeCompare.Size")
#define ID_TAGNAME_INCDESCRIPTION_OPERATION					_T("IncDescription.Operation")

#define ID_TAGNAME_CONTROLCARD_EXCLUSIONS					_T("controlcard.Exclusions")
#define ID_TAGNAME_EXCDESCRIPTION_FILEDESCRIPTION			_T("ExcDescription.Description")
#define ID_TAGNAME_EXCDESCRIPTION_DEST						_T("ExcDescription.Dest")
#define ID_TAGNAME_EXCDESCRIPTION_DATECOMPARE_OPERAND		_T("ExcDescription.DateCompare.Operand")
#define ID_TAGNAME_EXCDESCRIPTION_DATECOMPARE_DATE			_T("ExcDescription.DateCompare.Date")
#define ID_TAGNAME_EXCDESCRIPTION_SIZECOMPARE_OPERAND		_T("ExcDescription.SizeCompare.Operand")
#define ID_TAGNAME_EXCDESCRIPTION_SIZECOMPARE_SIZE			_T("ExcDescription.SizeCompare.Size")
#define ID_TAGNAME_EXCDESCRIPTION_OPERATION					_T("ExcDescription.Operation")

#define ID_TAGNAME_CONTROLCARD_EXCLUDEDRIVES				_T("controlcard.ExcludeDrives")
#define ID_TAGNAME_EXCLUDEDRIVE_DRIVE						_T("Drive")

#define ID_TAGVALUE_NEWER									_T("newer")
#define ID_TAGVALUE_OLDER									_T("older")
#define ID_TAGVALUE_LARGER									_T("larger")
#define ID_TAGVALUE_SMALLER									_T("smaller")

#define ID_TAGVALUE_PRESERVED								_T("p")
#define ID_TAGVALUE_REMOVED									_T("r")

// SMA Version
#define ID_TAGNAME_VERSION_INFO								_T("controlcard.SMA_Version")   //341510, by Judy, 09/12/2006  //_T("Controlcard.SMA_Version")	// 339905 by Gupta, 29AUG06
#define ID_TAGNAME_VERSION									_T("SMA_Version")			    // 339905 by Gupta, 29AUG06

#define ID_SMA_LOG_FILE_NAME								_T("sma.log")
#define ID_SMA_DEBUG_LOG_FILE_NAME							_T("smadebug.log")

//------------------------------------------------------------------------------------------------------
//												specstring
//------------------------------------------------------------------------------------------------------
#define ID_SPECSTR_FILENAME									_T("ID_SpecStr_FileName")
#define ID_SPECSTR_SUBPATH									_T("ID_SpecStr_SubPath")
#define ID_SPECSTR_ISFOLDER									_T("ID_SpecStr_IsFolder")

//value
#define ID_SPECSTR_VALUE_TRUE								_T("true")
#define ID_SPECSTR_VALUE_FALSE								_T("false")

#endif
