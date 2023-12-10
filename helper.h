#ifndef _TRIVIAL_HELPER_H_
#define _TRIVIAL_HELPER_H_

#include "framework.h"

#define HPRINT(_data_) std::wcout << _data_ << std::endl;

class Helper {
	HWND m_hwnd;
	struct sRegParam {
		DWORD dwLogLevel;
	} m_regparam;
	std::mutex m_mLogMessage;

public:
	Helper(void); ~Helper(void);

	static Helper& GetInstance(void);

	Helper(const Helper&) = delete;
	Helper& operator=(const Helper&) = delete;

	enum LogLevel {
		LOG_LVL_NULL = 0x0,
		LOG_LVL_INFO = 0x1,
		LOG_LVL_WARNING = 0x2,
		LOG_LVL_ERROR = 0x3,
		LOG_LVL_DEBUG = 0x4
	};
	static DWORD LoadLogLevel(void);

	static BOOL LogMessage(DWORD dwLogLevel,
		const std::wstring& wsMessage);
	static DWORD LogLastError(DWORD dwLogLevel);
	static DWORD LogFormatError(DWORD dwLogLevel,
		DWORD dwError);

	static DWORD ClearLog(void);



	static DWORD ShowLastError(const wchar_t* wcTitle);
	static DWORD ShowFormatError(const wchar_t* wcTitle,
		DWORD dwError);
	static void ShowMessage(const wchar_t* wcMessage,
		const wchar_t* wcTitle);

	static void LogAndShowMessage(DWORD dwLogLevel,
		const std::wstring& wsMessage);

	static void PrintDebugString(const std::wstring& s);

	static BOOL CreateConsole(void);
	static BOOL DestroyConsole(void);
};

#endif //_TRIVIAL_HELPER_H_
