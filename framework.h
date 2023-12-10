#ifndef _TRIVIAL_FRAMEWORK_H_
#define _TRIVIAL_FRAMEWORK_H_

#ifndef __cplusplus
#error Error! :: Please use the "C++"
#endif

#define UNICODE

#define _UNICODE

#include "targetver.h"
// Исключите редко используемые компоненты из заголовков Windows
#define WIN32_LEAN_AND_MEAN
// Файлы заголовков Windows
#include <windows.h>
#include <windowsx.h>

#include <shellapi.h>
#include <psapi.h>
#pragma comment(lib, "winmm.lib")
// Файлы заголовков среды выполнения C
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <inttypes.h>
// Файлы заголовков среды выполнения C++
#include <iostream>

#include <algorithm>

#include <vector>
#include <array>
#include <stack>

#include <set>

#include <string>
#include <list>
#include <map>

#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
// Файлы заголовков "Trivial"
#ifndef REG_PARAM_NAME
#define REG_PARAM_NAME L"TRIVIAL"
#endif
#ifndef LOG_FILE_NAME
#define LOG_FILE_NAME L".TrivialLog.txt"
#endif
#include "helper.h"

#include "vec2.h"
#include "vecfunctions.h"
// Точка входа "Trivial"
#ifndef TMain
#define TMain APIENTRY WinMain
#endif
#ifndef TParam
#define TParam _In_ HINSTANCE hInstance, \
_In_opt_ HINSTANCE hNULLInstance, \
_In_ LPSTR lpCmdLine, \
_In_ int nCmdShow
#endif

#ifndef wTMain
#define wTMain APIENTRY wWinMain
#endif
#ifndef wTParam
#define wTParam _In_ HINSTANCE hInstance, \
	_In_opt_ HINSTANCE hNULLInstance, \
	_In_ LPWSTR lpCmdLine, \
	_In_ int nCmdShow
#endif

#ifdef NDEBUG
#define _TRIVIAL_RELEASE_
#endif
#ifndef NDEBUG
#define _TRIVIAL_DEBUG_
#endif

#ifndef _TRIVIAL_CONFIG_

#ifdef _TRIVIAL_RELEASE_
#define _TRIVIAL_CONFIG_ L"RELEASE"
#endif

#ifdef _TRIVIAL_DEBUG_
#define _TRIVIAL_CONFIG_ L"DEBUG"
#endif

#endif

#ifdef _WIN64
#define _TRIVIAL_x64_
#endif
#ifndef _WIN64
#define _TRIVIAL_x86_
#endif

#ifndef _TRIVIAL_PLATFORM_

#ifdef _TRIVIAL_x64_
#define _TRIVIAL_PLATFORM_ L"x64"
#endif

#ifdef _TRIVIAL_x86_
#define _TRIVIAL_PLATFORM_ L"x86"
#endif

#endif

#ifndef _NULL_ID_
#define _NULL_ID_ (size_t)0x0
#endif

#ifndef WM_CREATE_DIALOG
#define WM_CREATE_DIALOG WM_APP + 0x1
#endif

#ifndef WM_DESTROY_DIALOG
#define WM_DESTROY_DIALOG WM_APP + 0x2
#endif

#ifndef WM_CALLBACK
#define WM_CALLBACK WM_APP + 0x3
#endif

#endif //_TRIVIAL_FRAMEWORK_H_
