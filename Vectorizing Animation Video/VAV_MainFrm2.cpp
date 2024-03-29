#include "stdafx.h"
#include "VAV_MainFrm.h"
#include "VAV_View.h"
#include "ConvStr.h"
#include "VoronoiCgal_Patch.h"
#include "CvExtenstion.h"
#include "CvExtenstion2.h"
#include "IFExtenstion.h"
#include "CmCurveEx.h"
#include "Line.h"
#include "CvExtenstion0.h"
#include "zip/ZipFolder.h"
#include "resource.h"

#ifdef USE_CGAL
#include "TriangulationCgal_Sideline.h"
#include "TriangulationCgal_Patch.h"
#endif // USE_CGAL


void VAV_MainFrame::OnButtonCGALTriangulation()
{
	D3DApp& d3dApp = GetVavView()->GetD3DApp();
	ImageSpline is;
	// contour
#ifdef USE_CGAL
	TriangulationCgal_Sideline cgal_contour;
#endif // USE_CGAL
	ImageSpline is2;
	cv::Mat curveExtration;
#ifdef USE_CGAL
	if (m_DRAW_CONTOUR)
	{
		
	}
#endif // USE_CGAL
	if (m_DRAW_ISOSURFACE)
	{
		cv::imshow("vavImage", (cv::Mat)m_vavImage);
		cv::Mat isoimg = MakeIsoSurfaceImg(m_vavImage, 32);
		FloodFillReColor(isoimg);
		cv::imshow("isoimg", isoimg);
	}
	if (m_DRAW_PATCH || m_DRAW_SEPARATE_PATCH)
	{
		cv::imshow("vavImage", (cv::Mat)m_vavImage);
		cv::Mat tmpimg = m_vavImage.Clone();
		cv::GaussianBlur(tmpimg, tmpimg, cv::Size(5, 5), 0, 0);
		cv::Mat isoimg = MakeIsoSurfaceImg(tmpimg, 64);
		is = S3GetPatchs(isoimg, m_BlackRegionThreshold * 0.01, tmpimg);
		cv::imshow("isoimg", isoimg);
	}
	if (m_DRAW_PATCH)
	{
		Lines tmp;
		Vector3s2d colors;
		for (int i = 0; i < is.m_LineFragments.size(); ++i)
		{
			Line& cps = is.m_LineFragments[i].m_Points;
			for (int j = 0; j < cps.size(); ++j)
			{
				cps[j].x -= 0.5;
				cps[j].y -= 0.5;
			}
			if (cps.size() > 5)
			{
				tmp.push_back(cps);
			}
		}
		colors = GetLinesColor(m_vavImage, tmp);
		colors = SmoothingLen5(colors, 0, 10);
		d3dApp.AddDiffusionLines(tmp, colors);
	}
#ifdef USE_CGAL
	// separate patch
	if (m_DRAW_SEPARATE_PATCH)
	{
		for (int i = 0; i < is.m_CvPatchs.size(); ++i)
		{
			TriangulationCgal_Patch cgal_patch;
			cgal_patch.SetSize(m_vavImage.GetWidth(), m_vavImage.GetHeight());
			Patch t_patch = ToPatch(is.m_CvPatchs[i]);
			//t_patch.SmoothPatch();
			cgal_patch.AddPatch(t_patch);
			is.m_CvPatchs[i].SetImage(m_vavImage);
			ColorConstraint_sptr constraint_sptr = is.m_CvPatchs[i].GetColorConstraint3();
			cgal_patch.AddColorConstraint(constraint_sptr);
			cgal_patch.SetCriteria(0.0, 10);
			cgal_patch.Compute();
			d3dApp.AddColorTriangles(cgal_patch.GetTriangles());
			d3dApp.AddTrianglesLine(cgal_patch.GetTriangles());
		}
	}
#endif // USE_CGAL
	if (m_DRAW_CURVE_EXTRACTION)
	{
		vavImage vImage = m_vavImage.Clone();
		vImage.ToExpImage();
		cv::Mat imgf, show3u = cv::Mat::zeros(vImage.GetCvMat().size(), CV_8UC3);
		imgf = vImage;
// 		for (int i = 1; i <= 2; i = i + 2)
// 		{
// 			bilateralFilter(imgf.clone(), imgf, i, i * 2, i / 2);
// 		}
// 		for (int i = 1; i <= 2; i = i + 2)
// 		{
// 			bilateralFilter(show3u.clone(), show3u, i, i * 2, i / 2);
// 		}
		cvtColor(imgf, imgf, CV_BGR2GRAY);
		imgf.convertTo(imgf, CV_32F, 1.0 / 255);
		CmCurveEx dEdge(imgf);
		dEdge.CalSecDer(5, 0.0001f);
		dEdge.Link();
		CvLines tpnts2d;
		const CEdges& edges = dEdge.GetEdges();
		for (size_t i = 0; i < edges.size(); i++)
		{
			cv::Vec3b color(rand() % 155 + 100, rand() % 155 + 100, rand() % 155 + 100);
			const std::vector<cv::Point>& pnts = edges[i].pnts;
			for (size_t j = 0; j < pnts.size(); j++)
			{
				show3u.at<cv::Vec3b>(pnts[j]) = color;
			}
			tpnts2d.push_back(edges[i].pnts);
		}
		m_BlackLine = GetLines(tpnts2d, 0.5, 0.5);
		m_BlackLine = SmoothingLen5(m_BlackLine);
		Lines showLines;
		Lines BLineWidth(m_BlackLine.size());
		Lines normals = GetNormalsLen2(m_BlackLine);
		const double blackRadio = 0.5;
		for (int idx1 = 0; idx1 < m_BlackLine.size(); ++idx1)
		{
			const Line& nowLine = m_BlackLine[idx1];
			const Line& nowNormals = normals[idx1];
			Line& lineWidths = BLineWidth[idx1];
			lineWidths.clear();
			for (int idx2 = 0; idx2 < nowLine.size() - 1; ++idx2)
			{
				const double LINE_WIDTH = 5;
				Vector2 start(nowLine[idx2] - nowNormals[idx2] * LINE_WIDTH);
				Vector2 end(nowLine[idx2] + nowNormals[idx2] * LINE_WIDTH);
				Vector2 start2(nowLine[idx2 + 1] - nowNormals[idx2 + 1] * LINE_WIDTH);
				Vector2 end2(nowLine[idx2 + 1] + nowNormals[idx2 + 1] * LINE_WIDTH);
				double_vector line1 = vImage.GetLineLight(start.x, start.y, end.x, end.y,
									  360);
				double_vector line2 = vImage.GetLineLight(start2.x, start2.y, end2.x, end2.y,
									  360);
				double_vector width1 = GetLineWidth(ConvertToSquareWave(ConvertToAngle(line1),
													15, 50), LINE_WIDTH * 2);
				double_vector width2 = GetLineWidth(ConvertToSquareWave(ConvertToAngle(line2),
													15, 50), LINE_WIDTH * 2);
				if (width1.size() >= 2 && width2.size() >= 2 && abs(width2[0] - width2[1]) < 1)
				{
					Line line1;
					line1.push_back(nowLine[idx2] - nowNormals[idx2] * width1[0] * blackRadio);
					line1.push_back(nowLine[idx2 + 1] - nowNormals[idx2 + 1] * width2[0] *
									blackRadio);
					line1 = GetLine(line1, 0.5, 0.5);
					Line line2;
					line2.push_back(nowLine[idx2] + nowNormals[idx2] * width1[1] * blackRadio);
					line2.push_back(nowLine[idx2 + 1] + nowNormals[idx2 + 1] * width2[1] *
									blackRadio);
					line2 = GetLine(line2, 0.5, 0.5);
					showLines.push_back(line1);
					showLines.push_back(line2);
					// save line width
					lineWidths.push_back(Vector2(width1[0] * blackRadio, width1[1] * blackRadio));
				}
				else
				{
					lineWidths.push_back(Vector2());
				}
			}
			lineWidths.push_back(Vector2());
		}
		m_BLineWidth = BLineWidth;
		m_BlackLine = GetLines(tpnts2d, 0.5, 0.5);
		m_BlackLine = SmoothingLen5(m_BlackLine, 0.9, 5);
		LineEnds les = GetLineEnds(m_BlackLine);
		LinkLineEnds180(les, 5, 20);
		ConnectLineEnds(les, m_BlackLine, m_BLineWidth);
		IncreaseDensity(m_BlackLine, m_BLineWidth);
		les = GetLineEnds(m_BlackLine);
		ConnectNearestLines_Width(les, m_BlackLine, m_BLineWidth, 15, 15);
		m_BLineWidth = CleanOrphanedLineWidths(m_BLineWidth, 5);
		m_BLineWidth = FixLineWidths(m_BLineWidth, 50);
		//m_BLineWidth = FixLineWidths(m_BLineWidth, 100);
		ClearLineWidthByPercent(m_BLineWidth, 0.4);
		m_BLineWidth = FixLineWidths(m_BLineWidth, 200);
		m_BLineWidth = SmoothingHas0Len5(m_BLineWidth, 0, 15);
		m_BlackLine = SmoothingLen5(m_BlackLine, 0, 5);
		Vector3s2d lineColors;
		lineColors = GetLinesColor(m_vavImage, m_BlackLine);
		lineColors = SmoothingLen5(lineColors, 0, 10);
		m_BlackLine = SmoothingLen5(m_BlackLine, 0, 10);
		d3dApp.AddLinesWidth(m_BlackLine, m_BLineWidth, lineColors);
		// block line
		dEdge.CalSecDer2(7, 0.001f);
		dEdge.Link();
		tpnts2d.clear();
		const CEdges& edges2 = dEdge.GetEdges();
		for (size_t i = 0; i < edges2.size(); i++)
		{
			cv::Vec3b color(rand() % 155 + 100, rand() % 155 + 100, rand() % 155 + 100);
			const std::vector<cv::Point>& pnts = edges2[i].pnts;
			for (size_t j = 0; j < pnts.size(); j++)
			{
				show3u.at<cv::Vec3b>(pnts[j]) = color;
			}
			tpnts2d.push_back(edges2[i].pnts);
		}
		m_BlackLine2 = GetLines(tpnts2d, 0.5, 0.5);
		Lines tmp_width = m_BlackLine2;
		//m_BlackLine2 = SmoothingLen5(m_BlackLine2, 0.5, 5);
		m_BlackLine2 = SmoothingLen5(m_BlackLine2, 0, 15);
		les = GetLineEnds(m_BlackLine2);
		LinkLineEnds180(les, 5, 20);
		ConnectLineEnds(les, m_BlackLine2, tmp_width);
		IncreaseDensity(m_BlackLine2, tmp_width);
		les = GetLineEnds(m_BlackLine2);
		ConnectNearestLines_Width(les, m_BlackLine2, tmp_width, 5, 20);
		d3dApp.AddLines(m_BlackLine2);
		d3dApp.SetScaleTemporary(1);
		d3dApp.BuildPoint();
		d3dApp.InterSetRenderTransparencyOutput2();
		cv::Mat simg = d3dApp.DrawSceneToCvMat(), stmp;
		d3dApp.SetScaleRecovery();
		d3dApp.ClearSkeletonLines();
		stmp = simg.clone();
		//stmp = TrapBallMaskAll(stmp);
		
		cvtColor(simg, simg, CV_BGR2GRAY);
		curveExtration = simg.clone();
		Dilation(curveExtration, 2, 1);
		cvtColor(curveExtration, curveExtration, CV_GRAY2BGR);
		cv::Mat tmpimg = m_vavImage.Clone();
		cv::Mat sampleimg = m_vavImage.Clone();
		cv::Mat   isoimg = MakeIsoSurfaceImg(tmpimg, 32);
		for (int i = 0; i < curveExtration.rows; i++)
		{
			for (int j = 0; j < curveExtration.cols ; j++)
			{
				cv::Vec3b& v = curveExtration.at<cv::Vec3b>(i, j);
				if (v[0] > 0)
				{
					cv::Vec3b& sam = sampleimg.at<cv::Vec3b>(i, j);
					sam[0] = 0;
					sam[1] = 0;
					sam[2] = 0;
				}
			}
		}
		cvtColor(curveExtration, curveExtration, CV_BGR2GRAY);
		Dilation(curveExtration, 2, 2);
		cvtColor(curveExtration, curveExtration, CV_GRAY2BGR);
		for (int i = 0; i < curveExtration.rows - 1; i++)
		{
			for (int j = 0; j < curveExtration.cols - 1; j++)
			{
				cv::Vec3b& v = curveExtration.at<cv::Vec3b>(i, j);
				if (v[0] > 0)
				{
					cv::Vec3b& dst = isoimg.at<cv::Vec3b>(i , j);
					dst[0] = 1;
					dst[1] = 1;
					dst[2] = 1;
				}
			}
		}
		Lines ColorWidth;
		Color2Side color2s;
		color2s = GetLinesColor2SideSmart3(m_vavImage, sampleimg, m_BlackLine2, 30);
		//color2s = GetLinesColor2Side(m_vavImage, m_BlackLine2, 3);
		color2s.left = FixLineColors(color2s.left, 600, 10);
		color2s.right = FixLineColors(color2s.right, 600, 10);
//      ClearJointArea(les, m_BlackLine2, color2s, 3);
//      les = GetLineEnds(m_BlackLine2);
//      ConnectNearestLines(les, m_BlackLine2, color2s, 10, 6, 15);
//      les = GetLineEnds(m_BlackLine2);
//      LinkLineEnds(les, 5, 20);
//      ConnectSimilarColor2Side(les, m_BlackLine2, color2s);
//      color2s.left = FixLineColors(color2s.left, 600, 1);
//      color2s.right = FixLineColors(color2s.right, 600, 1);
//      color2s.left = MedianLen(color2s.left, 10, 3);
//      color2s.right = MedianLen(color2s.right, 10, 3);
//      color2s.left = SmoothingLen5(color2s.left, 0, 5);
//      color2s.right = SmoothingLen5(color2s.right, 0, 5);
//      d3dApp.AddDiffusionLines(m_BlackLine2, color2s);
		{
			d3dApp.AddLines(m_BlackLine2);
			d3dApp.SetScaleTemporary(1);
			d3dApp.BuildPoint();
			d3dApp.InterSetRenderTransparencyOutput2();
			cv::Mat simg = d3dApp.DrawSceneToCvMat(), stmp;
			d3dApp.SetScaleRecovery();
			d3dApp.ClearSkeletonLines();
			stmp = simg.clone();
			stmp = TrapBallMaskAll(stmp, m_vavImage);
			ColorConstraints ccms;
			stmp = ConvertToIndex(stmp, m_vavImage.Clone(), ccms);
			Index2Side i2s = GetLinesIndex2Side(stmp, m_BlackLine2, 2);
			i2s.left = FixIndexs(i2s.left, 10, ccms.size());
			i2s.right = FixIndexs(i2s.right, 10, ccms.size());
			Color2Side color2s2 = LinesIndex2Color(m_BlackLine2, i2s, ccms);
			d3dApp.AddDiffusionLines(m_BlackLine2, color2s2);
			d3dApp.AddLines(m_BlackLine2);
		}
		Lines normals2 = GetNormalsLen2(m_BlackLine2);
		GetVavView()->m_FeatureLines = m_BlackLine2;
		GetVavView()->m_FeatureNormals = normals2;
// edge extraction
		Lines diffusionConstrant = S6GetPatchs(isoimg, 0, 0);
		//diffusionConstrant = SmoothingLen5(diffusionConstrant, 0, 1);
		Color2Side color2s2;
		color2s2 = GetLinesColor2Side(m_vavImage, diffusionConstrant, 0.5);
		Vector3s2d colors;
		colors = GetLinesColor(m_vavImage, diffusionConstrant);
//      123
		colors = FixLineColors(colors, 400, 10);
		colors = MedianLen(colors, 20, 3);
		colors = SmoothingLen5(colors, 0, 2);
		d3dApp.AddDiffusionLines(diffusionConstrant, colors);
// draw red line
		//d3dApp.AddLines(m_BlackLine);
		//d3dApp.AddLines(m_BlackLine2);
		//d3dApp.AddLines(diffusionConstrant);
//      {
//          std::ofstream ofs("curve.txt", std::ios::binary);
//          if (ofs.is_open())
//          {
//              boost::archive::binary_oarchive oa(ofs);
//              oa << m_BlackLine;
//              oa << m_BLineWidth;
//              oa << lineColors;
//              oa << diffusionConstrant;
//              oa << colors;
//              oa << m_BlackLine2;
//              oa << color2s;
//          }
//          ofs.close();
//      }
//		m_cannyImage = sampleimg;
//      GetVavView()->SetPictureSize(m_cannyImage.GetWidth(), m_cannyImage.GetHeight());
//      GetVavView()->SetTexture(m_cannyImage.GetDx11Texture());
	}
	if (m_DRAW_CANNY_EXTRACTION)
	{
		int t1 = 0, t2 = 30, a = 3;
		CMFCRibbonEdit* re;
		CMFCRibbonBaseElement* tmp_ui = 0;
		for (int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
		{
			tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
						 ID_SPIN_CannyThreshold1);
			if (tmp_ui != 0)
			{
				break;
			}
		}
		re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
		if (NULL != re)
		{
			t1 = atoi(ConvStr::GetStr(re->GetEditText().GetString()).c_str());
		}
		for (int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
		{
			tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
						 ID_SPIN_CannyThreshold2);
			if (tmp_ui != 0)
			{
				break;
			}
		}
		re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
		if (NULL != re)
		{
			t2 = atoi(ConvStr::GetStr(re->GetEditText().GetString()).c_str());
		}
		for (int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
		{
			tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
						 ID_SPIN_ApertureSize);
			if (tmp_ui != 0)
			{
				break;
			}
		}
		re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
		if (NULL != re)
		{
			a = atoi(ConvStr::GetStr(re->GetEditText().GetString()).c_str());
		}
		cv::Mat simg = m_vavImage.Clone();
		simg = CannyEdge(simg, t1, t2, a);
		cvtColor(simg, simg, CV_BGR2GRAY);
		AddEdge(simg);
		std::vector<cv::Vec4i> hierarchy;
		std::vector<std::vector<cv::Point>> contours;
		findContours(simg, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE,
					 cv::Point(0, 0));
		cv::Mat drawing = cv::Mat::zeros(simg.size(), CV_8UC3);
		cv::RNG rng(12345);
		for (int i = 0; i < contours.size(); i++)
		{
			cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
										  rng.uniform(50, 255));
			drawContours(drawing, contours, i, color, -1);
		}
		imshow("FillContours2", drawing);
	}
	d3dApp.BuildPoint();
	d3dApp.DrawScene();
}