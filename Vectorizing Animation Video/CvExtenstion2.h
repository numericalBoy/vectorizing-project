#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "LineDef.h"
#include "ColorConstraint.h"
#include "ImageSpline.h"
#include "Patch.h"
#include "LineWidthConstraint.h"

ColorConstraint_sptrs MakeColors(int regions, const cv::Mat& mask,
								 const cv::Mat& img);
cv::Vec3b& GetColor(cv::Mat& image, int x, int y);
Vector3s2d GetLinesColor(cv::Mat img, const Lines& lines);
Vector3s GetLinesColor(cv::Mat img, const Line& lines);
Color2Side GetLinesColor(const Lines& lines, ColorConstraints& ccms);
Lines GetLinesFromCvPatch(CvPatchs& cps);

cv::Mat ShowColorToLine(const Line& line, const cv::Mat img);

Color2Side GetLinesColor2Side(cv::Mat img, const Lines& lines, double normal_len);
Index2Side GetLinesIndex2Side(cv::Mat img, const Lines& lines, double normal_len);
Index2Side GetLinesIndex2SideCV(const Lines& lines, cv::Mat img);
Index2Side GetLinesIndex2SideSmart(const Lines& lines, cv::Mat img);
Index2Side GetLinesIndex2SideSmart2(const Lines& lines, cv::Mat img, cv::Mat img2);
Index2Side GetLinesIndex2SideSmart(CvPatchs& cps, const Lines& lines, double length);
ints2d  GetLinesIndex(cv::Mat img, const Lines& lines);
Color2Side LinesIndex2Color2(const Lines& lines, Index2Side& idx2s, ColorConstraints& ccms, ColorConstraints& ccms2);
Color2Side LinesIndex2Color(const Lines& lines, Index2Side& idx2s, ColorConstraints& ccms);
Vector3s2d LinesIndex2Color(const Lines& lines, ints2d& idx2s, ColorConstraints& ccms);
Color2Side GetLinesColor2SideSmart(cv::Mat img, cv::Mat color, const Lines& lines,
								   Lines& BLineWidth);
Color2Side GetLinesColor2SideSmart2(cv::Mat img, cv::Mat color, const Color2Side& lines);
Color2Side GetLinesColor2SideSmart3(cv::Mat img, cv::Mat color, const Lines& lines, double length);
void OutputDiffusionCurve(std::string name, int w, int h, Color2Side& c2s, Lines& lines);
cv::Mat MakeIsoSurfaceImg(cv::Mat img, int n);
ColorConstraint_sptr GetColorConstraint(cv::Mat& origin, cv::Mat& tmp);


LineWidthConstraints ConvertToConstraintLW(Lines& lines, Lines& lws);
ColorConstraints ConvertToConstraintC(Lines& lines, Vector3s2d& colors);
Color2SideConstraint ConvertToConstraintC2(Lines& lines, Color2Side& colors);

Lines ConvertFromConstraintLW(Lines& lines, LineWidthConstraints lwcs);
Vector3s2d ConvertFromConstraintC(Lines& lines, ColorConstraints ccs);
Color2Side ConvertFromConstraintC2(Lines& lines, Color2SideConstraint c2c);
