// 這個 MFC 範例原始程式碼會示範如何使用 MFC Microsoft Office Fluent 使用者介面
// (「Fluent UI」)，並且僅提供為參考資料，做為
// MFC 參考及 MFC C++ 程式庫軟體
// 隨附相關電子文件的補充。
// 關於 Fluent UI 之複製、使用或散發的授權條款則分別提供。
// 如需 Fluent UI 授權計劃的詳細資訊，請造訪
// http://go.microsoft.com/fwlink/?LinkId=238214。
//
// Copyright (C) Microsoft Corporation
// 著作權所有，並保留一切權利。

// stdafx.h : 可在此標頭檔中包含標準的系統 Include 檔，
// 或是經常使用卻很少變更的
// 專案專用 Include 檔案

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 從 Windows 標頭排除不常使用的成員
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 明確定義部分的 CString 建構函式

// 關閉 MFC 隱藏一些常見或可忽略警告訊息的功能
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心與標準元件
#include <afxext.h>         // MFC 擴充功能


#include <afxdisp.h>        // MFC Automation 類別



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 支援的 Internet Explorer 4 通用控制項
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 支援的 Windows 通用控制項
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能區和控制列的 MFC 支援
#define _USE_MATH_DEFINES
#include <cmath>
#include <d3d9.h>
#include <d3dx9.h>
#include <d3d11.h>
#include <windows.h>
#include <d3dx11.h>

#include <dxerr.h>
#include <cassert>
#include <string>
#include <iostream>
#include <ctime>
#include <cstdio>
#include <vector>
#include <sstream>
#include <tchar.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vtkSphereSource.h>
#include <vtkMath.h>
#include <vtkDoubleArray.h>
#include <vtkFieldData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkXYPlotActor.h>
#include <vtkCubeSource.h>
#include <vtkPolyData.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkTesting.h>

#define VTK_SMART_POINTER(x) \
	typedef vtkSmartPointer< x >    x##_Sptr; \
	typedef std::vector< x##_Sptr > x##_Sptrs;
VTK_SMART_POINTER(vtkXYPlotActor);
VTK_SMART_POINTER(vtkRenderWindow);
VTK_SMART_POINTER(vtkRenderWindowInteractor);

template<typename T, typename U> class has_member_Initialize_tester
{
private:
	template<U> struct helper;
	template<typename T> static char check(helper < &T::Initialize >*);
	template<typename T> static char(&check(...))[2];
public:
	enum { value = (sizeof(check<T> (0)) == sizeof(char)) };
};
template<char Doit, class T> struct static_Check_To_Initialize
{
	static void Do(T& ic)   { ic; }
};
template<class T> struct static_Check_To_Initialize<1, T>
{
	static void Do(T& ic)   { ic->Initialize(); }
};
static struct
{
	template<class T> operator vtkSmartPointer<T> ()
	{
		vtkSmartPointer<T> ptr = vtkSmartPointer<T>::New();
		static_Check_To_Initialize<has_member_Initialize_tester<T, void(T::*)()>::value, vtkSmartPointer<T> >::Do(
		    ptr);
		return ptr;
	}
}
vtkSmartNew;
static struct
{
	template<class T> operator vtkSmartPointer<T> ()
	{
		return vtkSmartPointer<T>::New();
	}
}
vtkOnlyNew;

//全域巨集
#define ReleaseCOM(x) { if(x){ x->Release();x = 0; }}

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)                                      \
	{                                                  \
		HRESULT hr = x;                                \
		if(FAILED(hr))                                 \
		{                                              \
			DXTrace(__FILE__, __LINE__, hr, _T(#x), TRUE); \
		}                                              \
	}
#endif
#else
#ifndef HR
#define HR(x) x;
#endif
#endif

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


