// 這個 MFC 範例原始程式碼會示範如何使用 MFC Microsoft Office Fluent 使用者介面
// (「Fluent UI」)，並且僅提供為參考資料，做為
// MFC 參考及 MFC C++ 程式庫軟體
// 隨附相關電子文件的補充。
// 關於 Fluent UI 之複製、使用或散發的授權條款則分別提供。
// 如需 Fluent UI 授權計劃的詳細資訊，請造訪
// http://msdn.microsoft.com/officeui。
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// VAV_View.h : VAV_View 類別的介面
//

#pragma once
#include <windows.h>
#include "DX11/d3dApp.h"
#include "math/Vector2.h"
#include "LineDef.h"
//#include "vtkShowHistogramTimerCallback.h"
#include "Patch.h"

class VAV_Doc;
class VAV_MainFrame;

class VAV_View : public CView
{
protected: // 僅從序列化建立
	D3DApp      m_D3DApp;
	VAV_View();
	DECLARE_DYNCREATE(VAV_View)

// 屬性
public:
	VAV_Doc* GetDocument() const;

// 作業
public:
	CPalette    m_cPalette;
	CPalette*   m_pOldPalette;
	CRect       m_oldRect;
	float       m_fRadius;
	CClientDC*  m_pDC;
	float       m_Scale;
	CPoint      m_MouseDown,
	            m_MouseUp,
	            m_MouseMove;
	Vector2     m_LookCenter;
	Vector2     m_LookDown;
	HWND        m_hWndDX11;
	Lines		m_FeatureLines;
	Lines		m_FeatureNormals;
	int			m_PicW;
	int			m_PicH;
	
	bool        m_MButtonDown;
	bool        m_LButtonDown;

	CvPatchs	m_patchs;
	Color2Side	m_patchcolor;
	Lines		m_patchlines;
	cv::Mat		m_indexImg;
	bool		m_HoldCtrl;

	HANDLE	m_thread;
// 	vtkShowHistogramTimerCallback_Sptr m_TimerCallback;
// 	vtkXYPlotActor_Sptr m_plot;
// 	vtkRenderWindow_Sptr m_renderWindow;
// 	vtkRenderWindowInteractor_Sptr m_interactor;
	vavImage	m_ExpImage;
// 覆寫
public:
	virtual void OnDraw(CDC* pDC);  // 覆寫以描繪此檢視
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	D3DApp& GetD3DApp();
	void SetPictureSize(int w, int h);
	static unsigned __stdcall MyThreadFunc(LPVOID lpParam);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	void	ShowLineNormal();
// 程式碼實作
public:
	virtual ~VAV_View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void InitDx11(HWND hWnd);
	VAV_MainFrame* GetMainFrame();
	vavImage* GetImage();
	void SetTexture(ID3D11ShaderResourceView* tex);
protected:

// 產生的訊息對應函式
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCaptureChanged(CWnd* pWnd);
	afx_msg void OnMouseLeave();
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcMButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // VAV_View.cpp 中的偵錯版本
inline VAV_Doc* VAV_View::GetDocument() const
{ return reinterpret_cast<VAV_Doc*>(m_pDocument); }
#endif

