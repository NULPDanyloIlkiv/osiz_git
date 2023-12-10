#include "helper.h"

Helper::Helper(void)
{ m_hwnd = NULL; m_regparam = { 0x0 }; }
Helper::~Helper(void)
{ /*Code...*/ }

Helper& Helper::GetInstance(void)
{ static Helper _h; return(_h); }

DWORD Helper::LoadLogLevel(void) {
	DWORD dwResult = ERROR_SUCCESS;
	DWORD dwRegDisposition = 0x0, dwBytesRead = sizeof(DWORD);
	HKEY hRegKey = NULL;

	dwResult = RegCreateKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\" REG_PARAM_NAME,
		0x0, NULL, 0x0, KEY_ALL_ACCESS, NULL, &hRegKey, &dwRegDisposition);

	if (dwResult != ERROR_SUCCESS)
	{ LogLastError(LogLevel::LOG_LVL_ERROR); goto linkExit; }

	if (dwRegDisposition == REG_CREATED_NEW_KEY)
	{ LogMessage(LogLevel::LOG_LVL_INFO,
		L"Registry Key did NOT exist; Created new Key : HKCU\\SOFTWARE\\" REG_PARAM_NAME);
	}
	else {
		LogMessage(LogLevel::LOG_LVL_INFO,
			L"Open exitsing Registry Key : HKCU\\SOFTWARE\\" REG_PARAM_NAME);
	}

	dwResult = RegGetValue(hRegKey, NULL, L"LogLevel",
		RRF_RT_DWORD, NULL, &GetInstance().m_regparam.dwLogLevel, &dwBytesRead);
	if (dwResult != ERROR_SUCCESS)
		if (dwResult == ERROR_FILE_NOT_FOUND) {
			dwResult = ERROR_SUCCESS;
			LogMessage(LogLevel::LOG_LVL_INFO,
				L"Registry value [LogLevel] NOT found! Using DEFAULT of 0x0(LOG_LVL_NULL)");
			GetInstance().m_regparam.dwLogLevel = LogLevel::LOG_LVL_NULL;
		}
		else { LogLastError(LogLevel::LOG_LVL_ERROR); goto linkExit; }

	LogMessage(LogLevel::LOG_LVL_INFO, L"[LogLevel] is : " +
		std::to_wstring(GetInstance().m_regparam.dwLogLevel));

	if (hRegKey)
		RegCloseKey(hRegKey);

linkExit:
	return dwResult;
}

BOOL Helper::LogMessage(DWORD dwLogLevel, const std::wstring& wsMessage) {
	if (dwLogLevel == LogLevel::LOG_LVL_NULL)
		{ return(FALSE); }
	std::lock_guard<std::mutex>
		lgLogMessage(GetInstance().m_mLogMessage);

	size_t sizeMessageLength = wsMessage.size();

	SYSTEMTIME sysTime = { 0x0 };

	HANDLE hLogFileHandle = INVALID_HANDLE_VALUE;
	DWORD dwEndOfFile = 0x0, dwNumberOfBytesWritten = 0x0;

	DWORD dwError = ERROR_SUCCESS;

	if (GetInstance().m_regparam.dwLogLevel < dwLogLevel) { return(FALSE); }
	if (sizeMessageLength <= NULL ||
		sizeMessageLength >= 1024) { return(FALSE); }

	//*wstring wsDateTime, wsSeverityTag, wsFormattedMessage;

	wchar_t wcDateTime[64]{};
	wchar_t wcSeverityTag[16]{};
	wchar_t wcFormattedMessage[1024];

	GetLocalTime(&sysTime);

	swprintf_s(wcDateTime, L"[%i/%i/%i %i:%i:%i.%i]",
		sysTime.wMonth, sysTime.wDay, sysTime.wYear,
		sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
		sysTime.wMilliseconds);

	switch (dwLogLevel) {
	case LogLevel::LOG_LVL_INFO:
		wcscpy_s(wcSeverityTag, L"[INFO] : ");
		break;
	case LogLevel::LOG_LVL_WARNING:
		wcscpy_s(wcSeverityTag, L"[WARNING] : ");
		break;
	case LogLevel::LOG_LVL_ERROR:
		wcscpy_s(wcSeverityTag, L"[ERROR] : ");
		break;
	case LogLevel::LOG_LVL_DEBUG:
		wcscpy_s(wcSeverityTag, L"[DEBUG] : ");
		break;

	default:
		break;
	}

	swprintf_s(wcFormattedMessage, L"%s\n", wsMessage.data());

	if ((hLogFileHandle = CreateFile(LOG_FILE_NAME, FILE_APPEND_DATA,
		FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
	{ dwError = GetLastError(); goto linkError; }

	dwEndOfFile = SetFilePointer(hLogFileHandle, 0x0, NULL, FILE_END);

	if(!WriteFile(hLogFileHandle, wcDateTime, wcslen(wcDateTime) * sizeof(wchar_t),
		&dwNumberOfBytesWritten, NULL))
	{ dwError = GetLastError(); goto linkError; }

	if(!WriteFile(hLogFileHandle, wcSeverityTag, wcslen(wcSeverityTag) * sizeof(wchar_t),
		&dwNumberOfBytesWritten, NULL))
	{ dwError = GetLastError(); goto linkError; }

	if(!WriteFile(hLogFileHandle, wcFormattedMessage, wcslen(wcFormattedMessage) * sizeof(wchar_t),
		&dwNumberOfBytesWritten, NULL))
	{ dwError = GetLastError(); goto linkError; }

	if (hLogFileHandle != INVALID_HANDLE_VALUE)
		if(!CloseHandle(hLogFileHandle))
		{ dwError = GetLastError(); goto linkExit; }

linkExit:
	return(TRUE);
linkError:
	(void)ShowFormatError
		(L"LogMessage - Error!", dwError); { return(FALSE); }
}

DWORD Helper::LogLastError(DWORD dwLogLevel) {
	DWORD dwError = ERROR_SUCCESS;
	(void)LogFormatError(dwLogLevel,
		dwError = GetLastError());
	return(dwError);
}

DWORD Helper::LogFormatError(DWORD dwLogLevel, DWORD dwError) {
	WCHAR wcError[256]{};
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), wcError, 256, NULL);
	(void)LogMessage(dwLogLevel,
		L"[" + std::to_wstring(dwError) + L"]" + wcError);
	return(dwError);
}

DWORD Helper::ClearLog(void) {
	DWORD dwError = ERROR_SUCCESS;

	if(!DeleteFile(LOG_FILE_NAME))
	{ dwError = GetLastError(); }

	return(dwError);
}



DWORD Helper::ShowLastError(const wchar_t* wcTitle) {
	DWORD dwError = ERROR_SUCCESS;
	ShowFormatError(wcTitle, dwError = GetLastError());
	return(dwError);
}
DWORD Helper::ShowFormatError(const wchar_t* wcTitle, DWORD dwError) {
	WCHAR wcError[256]{};
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), wcError, 256, NULL);

	wchar_t wcMessage[256]{};
	swprintf_s(wcMessage, L"[%d]%s", dwError, wcError);

	ShowMessage(wcMessage, wcTitle);
	return(dwError);
}
void Helper::ShowMessage(const wchar_t* wcMessage, const wchar_t* wcTitle) {
	(void)MessageBeep(MB_ICONERROR), (void)MessageBox(NULL, wcMessage, wcTitle,
		MB_OK | MB_DEFBUTTON1 | MB_APPLMODAL | MB_SETFOREGROUND);
}



void Helper::LogAndShowMessage(DWORD dwLogLevel, const std::wstring& wsMessage) {
	if(LogMessage(dwLogLevel, wsMessage))
	{ ShowMessage(wsMessage.data(),
		L"Log-Message");
	}
}



void Helper::PrintDebugString(const std::wstring& s)
{ OutputDebugString(s.data()); }



BOOL Helper::CreateConsole(void) {
	if (GetInstance().m_hwnd)
	{ ShowMessage(L"Console is already Created!",
		L"CreateConsole()"); return(FALSE);
	}
	if (!AllocConsole())
	{ exit(ShowLastError(L"AllocConsole - Failed!")); }
	GetInstance().m_hwnd = GetConsoleWindow();
	if (!GetInstance().m_hwnd)
	{ ShowMessage(L"GetConsoleWindow - Failed!",
		L"GetConsoleWindow()");
	}

	FILE* fDummy;
	_wfreopen_s(&fDummy, L"CONIN$", L"r", stdin);
	_wfreopen_s(&fDummy, L"CONOUT$", L"w", stderr);
	_wfreopen_s(&fDummy, L"CONOUT$", L"w", stdout);

	_wsetlocale(LC_ALL, L"C");
	SetConsoleTitle(L"CONSOLE");

	std::wcout << L"CONSOLE::Create()" << std::endl;

	return(TRUE);
}
BOOL Helper::DestroyConsole(void) {
	if (!GetInstance().m_hwnd)
	{ ShowMessage(L"Console is not Created!",
		L"DestroyConsole()"); return(FALSE);
	}
	if (!FreeConsole())
	{ exit(ShowLastError(L"FreeConsole - Failed!")); }

	PostMessage(GetInstance().m_hwnd, WM_CLOSE,
		0x0, 0x0), GetInstance().m_hwnd = NULL;

	return(TRUE);
}
