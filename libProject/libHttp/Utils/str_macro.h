#ifndef _CHAR_HELPER_MACRO_H
#define _CHAR_HELPER_MACRO_H

#define PREPARE_CHAR_PARAM(in, out)	\
{	\
	if (in == NULL)	\
	{	\
	out = NULL; \
} else { \
	size_t in##_len = wcslen(in) + 1;		\
	out = (char*)malloc(in##_len * sizeof(WCHAR));	\
	in##_len = WideCharToMultiByte(CP_ACP, 0, in, in##_len, out, in##_len, NULL, NULL);	\
}	\
}

#define PREPARE_WCHAR_PARAM(in, out)	\
{	\
	size_t in##_len = strlen(in) + 1;		\
	out = (wchar_t*)malloc(in##_len * sizeof(WCHAR));	\
	in##_len = MultiByteToWideChar(CP_ACP, 0, in, in##_len, out, in##_len);	\
}

#define RELEASE_PARAM(in)	\
	free(in);	\
	in = NULL;

#define RELEASE_WCHAR_PARAM(in)  RELEASE_PARAM(in)
#define RELEASE_CHAR_PARAM(in)	 RELEASE_PARAM(in)

#define CALL_UNICODE_FUNC_WITH_PARA_1(func, rettype, para1, ...)	\
	rettype ret;	\
	WCHAR* para1_w;	\
	PREPARE_WCHAR_PARAM(para1, para1_w);	\
	ret = func(para1_w, __VA_ARGS__);	\
	RELEASE_WCHAR_PARAM(para1_w);	\
	return ret

#define CALL_UNICODE_VOID_FUNC_WITH_PARA_1(func, para1, ...)	\
	WCHAR* para1_w;	\
	PREPARE_WCHAR_PARAM(para1, para1_w);	\
	func(para1_w, __VA_ARGS__);	\
	RELEASE_WCHAR_PARAM(para1_w)	

#define CALL_UNICODE_FUNC_WITH_PARA_2(func, rettype, para1, para2, ...)	\
	rettype ret;	\
	WCHAR* para1_w;	\
	WCHAR* para2_w;	\
	PREPARE_WCHAR_PARAM(para1, para1_w);	\
	PREPARE_WCHAR_PARAM(para2, para2_w);	\
	ret = func(para1_w, para2_w, __VA_ARGS__);	\
	RELEASE_WCHAR_PARAM(para1_w);	\
	RELEASE_WCHAR_PARAM(para2_w);	\
	return ret

#define CALL_UNICODE_VOID_FUNC_WITH_PARA_2(func, para1, para2, ...)	\
	WCHAR* para1_w;	\
	WCHAR* para2_w;	\
	PREPARE_WCHAR_PARAM(para1, para1_w);	\
	PREPARE_WCHAR_PARAM(para2, para2_w);	\
	func(para1_w, para2_w, __VA_ARGS__);	\
	RELEASE_WCHAR_PARAM(para1_w);	\
	RELEASE_WCHAR_PARAM(para2_w)	

#define CALL_MBCS_FUNC_WITH_PARA_1(func, rettype, para1, ...)	\
	rettype ret;	\
	char* para1_a;	\
	PREPARE_CHAR_PARAM(para1, para1_a);	\
	ret = func(para1_a, __VA_ARGS__);	\
	RELEASE_CHAR_PARAM(para1_a);	\
	return ret

#define CALL_MBCS_VOID_FUNC_WITH_PARA_1(func, para1, ...)	\
	char* para1_a;	\
	PREPARE_CHAR_PARAM(para1, para1_a);	\
	func(para1_a, __VA_ARGS__);	\
	RELEASE_CHAR_PARAM(para1_a)

#define CALL_MBCS_FUNC_WITH_PARA_2(func, rettype, para1, para2, ...)	\
	rettype ret;	\
	char* para1_a;	\
	char* para2_a;	\
	PREPARE_CHAR_PARAM(para1, para1_a);	\
	PREPARE_CHAR_PARAM(para2, para2_a);	\
	ret = func(para1_a, para2_a, __VA_ARGS__);	\
	RELEASE_CHAR_PARAM(para1_a);	\
	RELEASE_CHAR_PARAM(para2_a);	\
	return ret

#define CALL_MBCS_VOID_FUNC_WITH_PARA_2(func, para1, para2, ...)	\
	char* para1_a;	\
	char* para2_a;	\
	PREPARE_CHAR_PARAM(para1, para1_a);	\
	PREPARE_CHAR_PARAM(para2, para2_a);	\
	func(para1_a, para2_a, __VA_ARGS__);	\
	RELEASE_CHAR_PARAM(para1_a);	\
	RELEASE_CHAR_PARAM(para2_a);	

#define EMPTY_STRINGA					 ""
#define EMPTY_CHARA						 ''
#define DRIVE_IDENTIFICATIONA			 ':'					
#define PATH_SPLIT_WINDOW_CHARA			 '\\'					
#define PATH_SPLIT_UNIX_CHARA			 '/'

#define EMPTY_STRINGW					 L""
#define EMPTY_CHARW						 L''
#define DRIVE_IDENTIFICATIONW			 L':'					
#define PATH_SPLIT_WINDOW_CHARW			 L'\\'					
#define PATH_SPLIT_UNIX_CHARW			 L'/'					

#ifdef _UNICODE
#define EMPTY_STRING					EMPTY_STRINGW	
#define EMPTY_CHAR						EMPTY_CHARW		
#define DRIVE_IDENTIFICATION			DRIVE_IDENTIFICATIONW				
#define PATH_SPLIT_WINDOW_CHAR			PATH_SPLIT_WINDOW_CHARW					
#define PATH_SPLIT_UNIX_CHAR			PATH_SPLIT_UNIX_CHARW	
#else
#define EMPTY_STRING					EMPTY_STRINGA	
#define EMPTY_CHAR						EMPTY_CHARA		
#define DRIVE_IDENTIFICATION			DRIVE_IDENTIFICATIONA				
#define PATH_SPLIT_WINDOW_CHAR			PATH_SPLIT_WINDOW_CHARA					
#define PATH_SPLIT_UNIX_CHAR			PATH_SPLIT_UNIX_CHARA	
#endif

#define SafeStringCopy(dest, src)	ASSERT(sizeof(dest) != sizeof(char *)); StringCchCopy(dest, sizeof(dest), src)



#endif	// _CHAR_HELPER_MACRO_H