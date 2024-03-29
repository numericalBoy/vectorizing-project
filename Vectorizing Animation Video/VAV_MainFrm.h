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

// VAV_MainFrm.h : VAV_MainFrame 類別的介面
//

#pragma once
#include "FileView.h"
#include "ClassView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "vavImage.h"
#include "Patch.h"
#include "ReadVideo.h"
#include "DiffusionFrame.h"
#include "MySLIC.h"
#include "GenerateDiffusion.h"

typedef std::vector<cv::Mat> Mats;
typedef std::vector<cv::Vec2f> Vec2fs;

class VAV_View;
class VAV_MainFrame : public CFrameWndEx
{
protected: // 僅從序列化建立
	VAV_MainFrame();
	DECLARE_DYNCREATE(VAV_MainFrame)

// 屬性
public:
	Vec2fs		m_Moves;
	Mats		m_Video;
	vavImages	m_VideoTexture;
	vavImage    m_vavImage;
	vavImage    m_cannyImage;
	Lines		m_BlackLine;
	Lines		m_BLineWidth;
	ReadVideo	m_ReadVideo;
	DiffusionFrames	m_DiffusionFrames;
	BackGround	m_BackGround;
	FrameInfos	m_FrameInfos;
	float movex, movey;
	// use for block
	Lines		m_BlackLine2;
	// slic
	MySLIC*		m_SLIC;
	cv::Mat		m_SLIC_Img;
	uints		m_SLIC_Contour;
	uints		m_Buff;
	MySLIC::SLICLabels	m_Labels;
	MySLIC::SLICLabelPatchs m_LPatchs;
	ColorConstraints	m_ColorConstraints;
	ints	m_LabelsMap;
	int		m_NumLabels;
	// slic end
	int     m_PatchTransparency;
	int     m_SelectPatchTransparency;
	int     m_BlackRegionThreshold;
	int     m_TriangleLineTransparency;
	int     m_PictureTransparency;
	int     m_LineTransparency;
	int     m_LineSkeletonTransparency;
	bool	m_DRAW_PATCH;
	bool	m_DRAW_SEPARATE_PATCH;
	bool	m_DRAW_CONTOUR;
	bool	m_DRAW_CONTOUR_CONTROL_POINT;
	bool	m_DRAW_CONTOUR_SKELETON_POINT;
	bool	m_DRAW_ISOSURFACE;
	bool	m_DRAW_CURVE_EXTRACTION;
	bool	m_DRAW_CANNY_EXTRACTION;
	// new
	bool	m_CONSTRAINT_CURVES_PARAMETER_1;
	bool	m_CONSTRAINT_CURVES_PARAMETER_2;
	bool	m_ISOSURFACE_REGION;
	bool	m_ISOSURFACE_CONSTRAINT;
	bool	m_ISOSURFACE_CONSTRAINT_VECTORIZATION;
	bool	m_DECORATIVE_CURVES;
	bool	m_DECORATIVE_CURVES_CONSTRAINT;
	bool	m_BOUNDARY_CURVES;
	bool	m_BOUNDARY_CURVES_CONSTRAINT;
	bool	m_REGION_GROWING;
	bool	m_BLACK_LINE_VECTORIZATION;
	bool	m_BOUNDARY_CURVES_VECTORIZATION;

	CvPatchs    m_CvPatchs;
	void    ShowPatch(double x, double y);
// 作業
public:
	void AddFrame(cv::Mat img);
	void OnFileOpenVideo();
	void OnFileOpenVideo2();
	void OnFileOpenPicture();
	cv::Mat Do_SLIC(double m_compatcness , double m_spcount, cv::Mat img);
	VAV_View* GetVavView();
// 覆寫
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 程式碼實作
public:
	virtual ~VAV_MainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控制列內嵌的成員
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	CFileView         m_wndFileView;
	CClassView        m_wndClassView;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;
	CMFCCaptionBar    m_wndCaptionBar;

// 產生的訊息對應函式
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnViewCaptionBar();
	afx_msg void OnUpdateViewCaptionBar(CCmdUI* pCmdUI);
	afx_msg void OnOptions();
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	BOOL CreateCaptionBar();
public:
	afx_msg void OnButtonCanny();
	afx_msg void OnSpinCannyThreshold1();
	afx_msg void OnSpinCannyThreshold2();
	afx_msg void OnSpinAperturesize();
	afx_msg void OnButtonControlPointInitialize();
	afx_msg void OnButtonCGALTriangulation();
	afx_msg void OnButtonSkeleton();
	afx_msg void OnButtonSobel();
	afx_msg void OnButtonLaplace();
	afx_msg void OnSpinTransparencySelectPatch();
	afx_msg void OnUpdateSpinTransparencySelectPatch(CCmdUI* pCmdUI);
	afx_msg void OnSpinTransparencyPatch();
	afx_msg void OnUpdateSpinTransparencyPatch(CCmdUI* pCmdUI);
	afx_msg void OnSpinTransparencyline();
	afx_msg void OnUpdateSpinTransparencyline(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpinTransparencypicture(CCmdUI* pCmdUI);
	afx_msg void OnSpinTransparencypicture();
	afx_msg void OnSpinBlackregionthreshold();
	afx_msg void OnUpdateSpinBlackregionthreshold(CCmdUI* pCmdUI);
	afx_msg void OnSpinTransparencytriangleline();
	afx_msg void OnUpdateSpinTransparencytriangleline(CCmdUI* pCmdUI);
	afx_msg void OnSpinTransparencylineskeleton();
	afx_msg void OnUpdateSpinTransparencylineskeleton(CCmdUI* pCmdUI);
	afx_msg void OnCheckDrawPatch();
	afx_msg void OnUpdateCheckDrawPatch(CCmdUI *pCmdUI);
	afx_msg void OnCheckDrawSeparatePatch();
	afx_msg void OnUpdateCheckDrawSeparatePatch(CCmdUI *pCmdUI);
	afx_msg void OnCheckDrawContour();
	afx_msg void OnUpdateCheckDrawContour(CCmdUI *pCmdUI);
	afx_msg void OnCheckDrawContourControlPoint();
	afx_msg void OnUpdateCheckDrawContourControlPoint(CCmdUI *pCmdUI);
	afx_msg void OnCheckDrawContourSkeletonPoint();
	afx_msg void OnUpdateCheckDrawContourSkeletonPoint(CCmdUI *pCmdUI);
	afx_msg void OnCheckDrawIsosurface();
	afx_msg void OnUpdateCheckDrawIsosurface(CCmdUI *pCmdUI);
	afx_msg void OnCheckDrawCurveExtraction();
	afx_msg void OnUpdateCheckDrawCurveExtraction(CCmdUI *pCmdUI);
	afx_msg void OnCheckDrawCannyExtraction();
	afx_msg void OnUpdateCheckDrawCannyExtraction(CCmdUI *pCmdUI);
	afx_msg void OnCheck_ConstraintCurvesParameter1();
	afx_msg void OnUpdateCheck_ConstraintCurvesParameter1(CCmdUI *pCmdUI);
	afx_msg void OnCheck_ConstraintCurvesParameter2();
	afx_msg void OnUpdateCheck_ConstraintCurvesParameter2(CCmdUI *pCmdUI);
	afx_msg void OnCheck_IsosurfaceConstraint();
	afx_msg void OnUpdateCheck_IsosurfaceConstraint(CCmdUI *pCmdUI);
	afx_msg void OnCheck_DecorativeCurves();
	afx_msg void OnUpdateCheck_DecorativeCurves(CCmdUI *pCmdUI);
	afx_msg void OnCheck_BoundaryCurves();
	afx_msg void OnUpdateCheck_BoundaryCurves(CCmdUI *pCmdUI);
	afx_msg void OnCheck_RegionGrowing();
	afx_msg void OnUpdateCheck_RegionGrowing(CCmdUI *pCmdUI);
	afx_msg void OnCheck_BlackLineVectorization();
	afx_msg void OnUpdateCheck_BlackLineVectorization(CCmdUI *pCmdUI);
	afx_msg void OnCheck_IsosurfaceConstraintVectorization();
	afx_msg void OnUpdateCheck_IsosurfaceConstraintVectorization(CCmdUI *pCmdUI);
	afx_msg void OnButton_BuildVectorization();
	afx_msg void OnCheck_DecorativeCurvesConstraint();
	afx_msg void OnUpdateCheck_DecorativeCurvesConstraint(CCmdUI *pCmdUI);
	afx_msg void OnCheck_BoundaryCurvesConstraint();
	afx_msg void OnUpdateCheck_BoundaryCurvesConstraint(CCmdUI *pCmdUI);
	afx_msg void OnCheck_BoundaryCurvesVectorization();
	afx_msg void OnUpdateCheck_BoundaryCurvesVectorization(CCmdUI *pCmdUI);
	afx_msg void OnCheck_IsosurfaceRegion();
	afx_msg void OnUpdateCheck_IsosurfaceRegion(CCmdUI *pCmdUI);
	
	afx_msg void OnButton_Curvematching();
	afx_msg void OnButton_Zhang09();
};


