#include "stdcv.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <iostream>
#include "vavImage.h"
#include "CvExtenstion2.h"
#include "math/Quaternion.h"
#include "Line.h"
#include "IFExtenstion.h"

ColorConstraint_sptrs MakeColors(int regions, const cv::Mat& mask,
                                 const cv::Mat& img)
{
    ColorConstraint_sptrs ans;
    for(int i = 0; i < regions; ++i)
    {
        ans.push_back(ColorConstraint_sptr(new ColorConstraint));
    }
    for(int i = 0; i < img.rows; ++i)
    {
        for(int j = 0; j < img.cols; ++j)
        {
            const cv::Vec3b& id_color = mask.at<cv::Vec3b>(i, j);
            int idx = id_color[0] + id_color[1] * 255 + id_color[2] * 255 * 255;
            const cv::Vec3b& color = img.at<cv::Vec3b>(i, j);
            ans[idx % ans.size()]->AddPoint(j, i, Vector3(color[0], color[1], color[2]));
        }
    }
    return ans;
}

cv::Vec3b& GetColor(cv::Mat& image, int x, int y)
{
    if(y < 0)
    {
        y = 0;
    }
    if(y >= image.rows)
    {
        y = image.rows - 1;
    }
    if(x < 0)
    {
        x = 0;
    }
    if(x >= image.cols)
    {
        x = image.cols - 1;
    }
    return image.at<cv::Vec3b>(y, x);
}

Vector3s2d GetLinesColor(cv::Mat img, const Lines& lines)
{
    vavImage vimg(img);
    Vector3s2d ans;
    for(auto it = lines.cbegin(); it != lines.cend(); ++it)
    {
        ans.push_back(Vector3s());
        Vector3s& li = ans.back();
        for(auto it2 = it->cbegin(); it2 != it->cend(); ++it2)
        {
            double r = vimg.GetBilinearR_if0(it2->x, it2->y);
            double g = vimg.GetBilinearG_if0(it2->x, it2->y);
            double b = vimg.GetBilinearB_if0(it2->x, it2->y);
            li.push_back(Vector3(r, g, b));
        }
    }
    return ans;
}

Vector3s GetLinesColor(cv::Mat img, const Line& lines)
{
    vavImage vimg(img);
    Vector3s ans;
    for(auto it2 = lines.cbegin(); it2 != lines.cend(); ++it2)
    {
        double r = vimg.GetBilinearR_if0(it2->x, it2->y);
        double g = vimg.GetBilinearG_if0(it2->x, it2->y);
        double b = vimg.GetBilinearB_if0(it2->x, it2->y);
        ans.push_back(Vector3(r, g, b));
    }
    return ans;
}

Color2Side GetLinesColor(const Lines& lines, ColorConstraints& ccms)
{
    Color2Side res;
    res.left.resize(lines.size());
    res.right.resize(lines.size());
    Vector3s2d& cleft = res.left;
    Vector3s2d& cright = res.right;
    for(int i = 0; i < cleft.size(); ++i)
    {
        cleft[i].resize(lines[i].size());
        for(int j = 0; j < cleft[i].size(); ++j)
        {
            cleft[i][j] = ccms.at(i).GetColorVector3(lines[i][j].x, lines[i][j].y);
        }
    }
    for(int i = 0; i < cright.size(); ++i)
    {
        cright[i].resize(lines[i].size());
        for(int j = 0; j < cright[i].size(); ++j)
        {
            cright[i][j] = Vector3(255, 255, 255);
        }
    }
    return res;
}

Color2Side GetLinesColor2Side(cv::Mat img, const Lines& lines, double normal_len)
{
    vavImage vimg(img);
    Lines normals = GetNormalsLen2(lines);
    Color2Side ans;
    Vector3s2d& ans_left = ans.left;
    Vector3s2d& ans_right = ans.right;
    ans_left.resize(lines.size());
    ans_right.resize(lines.size());
    for(int i = 0; i < lines.size(); ++i)
    {
        Lines::const_iterator it = lines.cbegin() + i;
        Vector3s& li = ans_left[i];
        int j = 0;
        li.resize(it->size());
        for(auto it2 = it->cbegin(); it2 != it->cend(); ++it2, ++j)
        {
            Vector2 pos = *it2 - normals[i][j] * normal_len;
            double r = vimg.GetBilinearR_if0(pos.x, pos.y);
            double g = vimg.GetBilinearG_if0(pos.x, pos.y);
            double b = vimg.GetBilinearB_if0(pos.x, pos.y);
            li[j] = Vector3(r, g, b);
        }
        Vector3s& ri = ans_right[i];
        ri.resize(it->size());
        j = 0;
        for(auto it2 = it->cbegin(); it2 != it->cend(); ++it2, ++j)
        {
            Vector2 pos = *it2 + normals[i][j] * normal_len;
            double r = vimg.GetBilinearR_if0(pos.x, pos.y);
            double g = vimg.GetBilinearG_if0(pos.x, pos.y);
            double b = vimg.GetBilinearB_if0(pos.x, pos.y);
            ri[j] = Vector3(r, g, b);
        }
    }
    return ans;
}

Color2Side GetLinesColor2SideSmart(cv::Mat img, cv::Mat color, const Lines& lines,
                                   Lines& BLineWidth)
{
    vavImage vimg(img);
    vavImage vcolor(color);
    Lines showLines;
    BLineWidth.clear();
    BLineWidth.resize(lines.size());
    Lines normals = GetNormalsLen2(lines);
    normals = SmoothingLen5(normals, 0, 3);
    const double blackRadio = 1;
    for(int idx1 = 0; idx1 < lines.size(); ++idx1)
    {
        const Line& nowLine = lines[idx1];
        const Line& nowNormals = normals[idx1];
        Line& lineWidths = BLineWidth[idx1];
        lineWidths.clear();
        for(int idx2 = 0; idx2 < nowLine.size(); ++idx2)
        {
            const double LINE_WIDTH = 4;
            Vector2 start(nowLine[idx2] - nowNormals[idx2] * LINE_WIDTH);
            Vector2 end(nowLine[idx2] + nowNormals[idx2] * LINE_WIDTH);
            double_vector line1 = vimg.GetLineLight(start.x, start.y, end.x, end.y, 360);
            double_vector width1 = GetColorWidth(ConvertToSquareWave(ConvertToAngle(line1),
                                                 15, 50), LINE_WIDTH * 2);
            if(width1.size() == 2)
            {
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
    Color2Side ans;
    Vector3s2d& ans_left = ans.left;
    Vector3s2d& ans_right = ans.right;
    ans_left.resize(lines.size());
    ans_right.resize(lines.size());
    for(int i = 0; i < lines.size(); ++i)
    {
        Lines::const_iterator it = lines.cbegin() + i;
        Vector3s& li = ans_left[i];
        Line& lineWidths = BLineWidth[i];
        int j = 0;
        li.resize(it->size());
        for(auto it2 = it->cbegin(); it2 != it->cend(); ++it2, ++j)
        {
            if(lineWidths[j][0] > 0)
            {
                Vector2 pos = *it2 - normals[i][j] * (lineWidths[j][0]);
                double r = vcolor.GetBilinearR_if0(pos.x, pos.y);
                double g = vcolor.GetBilinearG_if0(pos.x, pos.y);
                double b = vcolor.GetBilinearB_if0(pos.x, pos.y);
                li[j] = Vector3(r, g, b);
            }
        }
        Vector3s& ri = ans_right[i];
        ri.resize(it->size());
        j = 0;
        for(auto it2 = it->cbegin(); it2 != it->cend(); ++it2, ++j)
        {
            if(lineWidths[j][1] > 0)
            {
                Vector2 pos = *it2 + normals[i][j] * (lineWidths[j][1]);
                double r = vcolor.GetBilinearR_if0(pos.x, pos.y);
                double g = vcolor.GetBilinearG_if0(pos.x, pos.y);
                double b = vcolor.GetBilinearB_if0(pos.x, pos.y);
                ri[j] = Vector3(r, g, b);
            }
        }
    }
    return ans;
}

Color2Side GetLinesColor2SideSmart2(cv::Mat img, cv::Mat color, const Lines& lines)
{
    vavImage vimg(img);
    vavImage vcolor(color);
    Lines showLines;
    Lines normals = GetNormalsLen2(lines);
    normals = SmoothingLen5(normals, 0, 3);
    const double blackRadio = 1;
    Color2Side ans;
    Vector3s2d& ans_left = ans.left;
    Vector3s2d& ans_right = ans.right;
    ans_left.resize(lines.size());
    ans_right.resize(lines.size());
    for(int idx1 = 0; idx1 < lines.size(); ++idx1)
    {
        const Line& nowLine = lines[idx1];
        const Line& nowNormals = normals[idx1];
        Vector3s& nowLineL = ans_left[idx1];
        Vector3s& nowLineR = ans_right[idx1];
        for(int idx2 = 0; idx2 < nowLine.size(); ++idx2)
        {
            const double LINE_WIDTH = 4;
            Vector2 start(nowLine[idx2] - nowNormals[idx2] * LINE_WIDTH);
            Vector2 end(nowLine[idx2] + nowNormals[idx2] * LINE_WIDTH);
            Vector3s line1 = vcolor.GetLineColor(nowLine[idx2].x, nowLine[idx2].y, start.x, start.y, 120);
            Vector3s line2 = vcolor.GetLineColor(nowLine[idx2].x, nowLine[idx2].y, end.x, end.y, 120);
            line1.erase(line1.begin(), line1.begin() + 30);
            line2.erase(line2.begin(), line2.begin() + 30);
            std::sort(line1.begin(), line1.end(), LightCompareVector3);
            std::sort(line2.begin(), line2.end(), LightCompareVector3);
            nowLineL.push_back(line1[line1.size() / 2]);
            nowLineR.push_back(line2[line2.size() / 2]);
        }
    }
    return ans;
}


Color2Side GetLinesColor2SideSmart3(cv::Mat img, cv::Mat color, const Lines& lines, double length)
{
    length *= length;
    vavImage vimg(img);
    vavImage vcolor(color);
    Lines showLines;
    Lines normals = GetNormalsLen2(lines);
    normals = SmoothingLen5(normals, 0, 3);
    const double blackRadio = 1;
    Color2Side ans;
    Vector3s2d& ans_left = ans.left;
    Vector3s2d& ans_right = ans.right;
    ans_left.resize(lines.size());
    ans_right.resize(lines.size());
    for(int idx1 = 0; idx1 < lines.size(); ++idx1)
    {
        const Line& nowLine = lines[idx1];
        const Line& nowNormals = normals[idx1];
        Vector3s& nowLineL = ans_left[idx1];
        Vector3s& nowLineR = ans_right[idx1];
        for(int idx2 = 0; idx2 < nowLine.size(); ++idx2)
        {
            const double LINE_WIDTH = 4;
            Vector2 start(nowLine[idx2] - nowNormals[idx2] * LINE_WIDTH);
            Vector2 end(nowLine[idx2] + nowNormals[idx2] * LINE_WIDTH);
            Vector3s line1 = vcolor.GetLineColor(nowLine[idx2].x, nowLine[idx2].y, start.x, start.y, 120);
            Vector3s line2 = vcolor.GetLineColor(nowLine[idx2].x, nowLine[idx2].y, end.x, end.y, 120);
            Vector3s lineOK1, lineOK2;
            Vector3 src = vcolor.GetBilinearColor(nowLine[idx2].x, nowLine[idx2].y);
            for(int lid = 30; lid < line1.size(); ++lid)
            {
                if(src.squaredDistance(line1[lid]) > length)
                {
                    lineOK1.push_back(line1[lid]);
                }
            }
            if(lineOK1.empty())
            {
                //nowLineL.push_back(Vector3());
                std::sort(line1.begin(), line1.end(), LightCompareVector3);
                nowLineL.push_back(line1[line1.size() / 2]);
            }
            else
            {
                std::sort(lineOK1.begin(), lineOK1.end(), LightCompareVector3);
                nowLineL.push_back(lineOK1[lineOK1.size() / 2]);
            }
            for(int lid = 30; lid < line2.size(); ++lid)
            {
                if(src.squaredDistance(line2[lid]) > length)
                {
                    lineOK2.push_back(line2[lid]);
                }
            }
            if(lineOK2.empty())
            {
                //nowLineR.push_back(Vector3());
                std::sort(line2.begin(), line2.end(), LightCompareVector3);
                nowLineR.push_back(line2[line2.size() / 2]);
            }
            else
            {
                std::sort(lineOK2.begin(), lineOK2.end(), LightCompareVector3);
                nowLineR.push_back(lineOK2[lineOK2.size() / 2]);
            }
        }
    }
    return ans;
}

void OutputDiffusionCurve(std::string name, int w, int h, Color2Side& c2s,
                          Lines& lines)
{
    std::ofstream out(name);
    out << "<!DOCTYPE CurveSetXML>" << std::endl;
    out << " <curve_set image_width=\"" << w << "\" image_height=\"" << h <<
        "\" nb_curves=\"" << lines.size() << "\" >" << std::endl;
    for(int i = 0; i < lines.size(); ++i)
    {
        Line& now_line = lines[i];
        Vector3s& now_left = c2s.left[i];
        Vector3s& now_right = c2s.right[i];
        for(int j = 0; j < now_line.size(); j ++)
        {
            now_line[j] = Quaternion::GetRotation(now_line[j], 90);
            now_line[j].y = -now_line[j].y;
        }
//      if (now_left.size() > 5000)
//      {
//          Vector3s tmp;
//
//          for (int j = 0; j < now_left.size(); j += 2)
//          {
//              tmp.push_back(now_left[j]);
//          }
//
//          now_left = tmp;
//          tmp.clear();
//
//          for (int j = 0; j < now_right.size(); j += 2)
//          {
//              tmp.push_back(now_right[j]);
//          }
//
//          now_right = tmp;
//      }
        out << " <curve nb_control_points=\"" << now_line.size() <<
            "\" nb_left_colors=\"" << now_left.size() << "\" nb_right_colors=\"" <<
            now_right.size() << "\" nb_blur_points=\"2\" lifetime=\"32\" >" <<
            std::endl;
        out << "  <control_points_set>" << std::endl;
        for(int j = 0; j < now_line.size(); j++)
        {
            out << "   <control_point x=\"" << now_line[j].x << "\" y=\"" << now_line[j].y
                << "\" />" << std::endl;
        }
        out << "  </control_points_set>" << std::endl;
        out << "  <left_colors_set>" << std::endl;
        double globalID_step = 10000.0 / (now_left.size() - 1);
        for(int j = 0; j < now_left.size(); j++)
        {
            out << "   <left_color G=\"" << (int)now_left[j].y << "\" R=\"" <<
                (int)now_left[j].z << "\" globalID=\"" << (int)(j * globalID_step) <<
                "\" B=\"" << (int)now_left[j].x << "\" />" << std::endl;
        }
        out << "  </left_colors_set>" << std::endl;
        out << "  <right_colors_set>" << std::endl;
        for(int j = 0; j < now_right.size() ; j++)
        {
            out << "   <right_color G=\"" << (int)now_right[j].y << "\" R=\"" <<
                (int)now_right[j].z << "\" globalID=\"" << (int)(j * globalID_step) <<
                "\" B=\"" << (int)now_right[j].x << "\" />" << std::endl;
        }
        out << "  </right_colors_set>" << std::endl;
        out << "  <blur_points_set>" << std::endl;
        out << "   <best_scale value=\"0\" globalID=\"0\" />" << std::endl;
        out << "   <best_scale value=\"0\" globalID=\"10000\" />" << std::endl;
        out << "  </blur_points_set>" << std::endl;
        out << " </curve>" << std::endl;
    }
    out << "</curve_set>" << std::endl;
}

cv::Mat MakeIsoSurfaceImg(cv::Mat img, int n)
{
    //cv::GaussianBlur(img, img, cv::Size(5, 5), 2);
    cv::Mat ans = img.clone();
    for(int i = 0; i < img.rows; i++)
    {
        for(int j = 0; j < img.cols; j++)
        {
            cv::Vec3b& v = img.at<cv::Vec3b>(i, j);
//          float vv = 0.299 * v[2] + 0.587 * v[1] + 0.114 * v[0];
//          vv = int(vv / n) * n;
            cv::Vec3b& a = ans.at<cv::Vec3b>(i, j);
            //a[0] = a[1] = a[2] = vv;
            a[0] = int(v[0] / n) * n;
            a[1] = int(v[1] / n) * n;
            a[2] = int(v[2] / n) * n;
        }
    }
    return ans;
}

ColorConstraint_sptr GetColorConstraint(cv::Mat& origin, cv::Mat& tmp)
{
    ColorConstraint_sptr res = ColorConstraint_sptr(new ColorConstraint);
    for(int i = 1; i < origin.cols; i++)
    {
        for(int j = 1; j < origin.rows - 1; j++)
        {
            if(tmp.at<uchar>(j + 1, i + 1))
            {
                res->AddPoint(i, j, origin.at<cv::Vec3b>(j, i));
            }
        }
    }
    return res;
}

Index2Side GetLinesIndex2Side(cv::Mat img, const Lines& lines, double normal_len)
{
    vavImage vimg(img);
    Lines normals = GetNormalsLen2(lines);
    Index2Side ans;
    ints2d& ans_left = ans.left;
    ints2d& ans_right = ans.right;
    ans_left.resize(lines.size());
    ans_right.resize(lines.size());
    const int NO_COLOR = 0;
    for(int i = 0; i < lines.size(); ++i)
    {
        Lines::const_iterator it = lines.cbegin() + i;
        ints& li = ans_left[i];
        int j = 0;
        li.resize(it->size());
        for(auto it2 = it->cbegin(); it2 != it->cend(); ++it2, ++j)
        {
            Vector2 pos = *it2 * 2 - normals[i][j] * normal_len;
            if(j >= NO_COLOR && j < it->size() - NO_COLOR)
            {
                li[j] = vimg.GetIndex_if255(pos.x, pos.y);
            }
        }
        ints& ri = ans_right[i];
        ri.resize(it->size());
        j = 0;
        for(auto it2 = it->cbegin(); it2 != it->cend(); ++it2, ++j)
        {
            Vector2 pos = *it2 * 2 + normals[i][j] * normal_len;
            if(j >= NO_COLOR && j < it->size() - NO_COLOR)
            {
                ri[j] = vimg.GetIndex_if255(pos.x, pos.y);
            }
        }
    }
    return ans;
}

Index2Side GetLinesIndex2SideCV(const Lines& lines, cv::Mat img)
{
    vavImage vimg(img);
    Lines normals = GetNormalsLen2(lines);
    Index2Side ans;
    ints2d& ans_left = ans.left;
    ints2d& ans_right = ans.right;
    ans_left.resize(lines.size());
    ans_right.resize(lines.size());
    for(int i = 0; i < lines.size(); ++i)
    {
        const Line& nowline = lines[i];
        ints& li = ans_left[i];
        int j = 0;
        li.resize(nowline.size());
        for(int j = 0; j < nowline.size(); ++j)
        {
            Vector2 pos;
            if(j == 0)
            {
                pos = (nowline[0] + nowline[1]) * 0.5;
            }
            else
            {
                pos = (nowline[j - 1] + nowline[j]) * 0.5;
            }
            pos.x = floor(pos.x);
			pos.y = floor(pos.y);
            int vx = -normals[i][j].x;
            if(vx < 0)
            {
                pos.x -= 1;
            }
            int vy = -normals[i][j].y;
            if(vy < 0)
            {
                pos.y -= 1;
            }
            li[j] = vimg.GetIndex_int(pos.x, pos.y);
        }
        ints& ri = ans_right[i];
        ri.resize(nowline.size());
        for(int j = 0; j < nowline.size(); ++j)
        {
            Vector2 pos;
            if(j == 0)
            {
                pos = (nowline[0] + nowline[1]) * 0.5;
            }
            else
            {
                pos = (nowline[j - 1] + nowline[j]) * 0.5;
            }
			pos.x = floor(pos.x);
			pos.y = floor(pos.y);
            int vx = normals[i][j].x;
            if(vx < 0)
            {
                pos.x -= 1;
            }
            int vy = normals[i][j].y;
            if(vy < 0)
            {
                pos.y -= 1;
            }
            ri[j] = vimg.GetIndex_int(pos.x, pos.y);
        }
    }
    return ans;
}

Index2Side GetLinesIndex2SideSmart(CvPatchs& cps, const Lines& lines, double normal_len)
{
    Lines normals = GetNormalsLen2(lines);
    Index2Side ans;
    ints2d& ans_left = ans.left;
    ints2d& ans_right = ans.right;
    ans_left.resize(lines.size());
    ans_right.resize(lines.size());
    for(int i = 0; i < lines.size(); ++i)
    {
        Lines::const_iterator it = lines.cbegin() + i;
        ints& li = ans_left[i];
        int j = 0;
        li.resize(it->size());
        for(auto it2 = it->cbegin(); it2 != it->cend(); ++it2, ++j)
        {
            li[j] = -1;
            Vector2 pos = *it2 - normals[i][j] * normal_len;
            for(int k = 0; k < cps.size(); ++k)
            {
                if(cps[k].Inside(pos.x, pos.y))
                {
                    li[j] = k + 1;
                    break;
                }
            }
        }
        ints& ri = ans_right[i];
        ri.resize(it->size());
        j = 0;
        for(auto it2 = it->cbegin(); it2 != it->cend(); ++it2, ++j)
        {
            ri[j] = -1;
            Vector2 pos = *it2 + normals[i][j] * normal_len;
            for(int k = 0; k < cps.size(); ++k)
            {
                if(cps[k].Inside(pos.x, pos.y))
                {
                    ri[j] = k + 1;
                    break;
                }
            }
        }
    }
    return ans;
}

Index2Side GetLinesIndex2SideSmart(const Lines& lines, cv::Mat img)
{
    vavImage vimg(img);
    Lines normals = GetNormalsLen2(lines);
    Index2Side ans;
    ints2d& ans_left = ans.left;
    ints2d& ans_right = ans.right;
    ans_left.resize(lines.size());
    ans_right.resize(lines.size());
    const int NO_COLOR = 0;
    for(int i = 0; i < lines.size(); ++i)
    {
        Lines::const_iterator it = lines.cbegin() + i;
        ints& li = ans_left[i];
        int j = 0;
        li.resize(it->size());
        for(auto it2 = it->cbegin(); it2 != it->cend(); ++it2, ++j)
        {
            li[j] = -1;
            if(j >= NO_COLOR && j < it->size() - NO_COLOR)
            {
                double normal_len = 1;
                do
                {
                    Vector2 pos = *it2 - normals[i][j] * normal_len;
                    li[j] = vimg.GetIndex_no255(pos.x, pos.y);
                    normal_len += 0.5;
                }
                while(li[j] == -1 && normal_len < 5);
            }
        }
        ints& ri = ans_right[i];
        ri.resize(it->size());
        j = 0;
        for(auto it2 = it->cbegin(); it2 != it->cend(); ++it2, ++j)
        {
            ri[j] = -1;
            if(j >= NO_COLOR && j < it->size() - NO_COLOR)
            {
                double normal_len = 1;
                do
                {
                    Vector2 pos = *it2 + normals[i][j] * normal_len;
                    ri[j] = vimg.GetIndex_no255(pos.x, pos.y);
                    normal_len += 0.5;
                }
                while(ri[j] == -1 && normal_len < 5);
            }
        }
    }
    return ans;
}

Index2Side GetLinesIndex2SideSmart2(const Lines& lines, cv::Mat img, cv::Mat img2)
{
    vavImage vimg(img);
    vavImage vimg2(img2);
    Lines normals = GetNormalsLen2(lines);
    Index2Side ans;
    ints2d& ans_left = ans.left;
    ints2d& ans_right = ans.right;
    ans_left.resize(lines.size());
    ans_right.resize(lines.size());
    const int NO_COLOR = 0;
    for(int i = 0; i < lines.size(); ++i)
    {
        Lines::const_iterator it = lines.cbegin() + i;
        ints& li = ans_left[i];
        int j = 0;
        li.resize(it->size());
        for(auto it2 = it->cbegin(); it2 != it->cend(); ++it2, ++j)
        {
            li[j] = -1;
            if(j >= NO_COLOR && j < it->size() - NO_COLOR)
            {
                double normal_len = 1.5;
                do
                {
                    Vector2 pos = *it2 - normals[i][j] * normal_len;
                    li[j] = vimg.GetIndex_no255(pos.x, pos.y);
                    normal_len += 0.5;
                }
                while(li[j] == -1 && normal_len < 4);
                if(li[j] < 2)
                {
                    normal_len = 1.5;
                    li[j] = -1;
                    do
                    {
                        Vector2 pos = *it2 - normals[i][j] * normal_len;
                        li[j] = -vimg2.GetIndex_no255(pos.x, pos.y);
                        normal_len += 0.5;
                    }
                    while(li[j] == -1 && normal_len < 6);
                }
            }
        }
        ints& ri = ans_right[i];
        ri.resize(it->size());
        j = 0;
        for(auto it2 = it->cbegin(); it2 != it->cend(); ++it2, ++j)
        {
            ri[j] = -1;
            if(j >= NO_COLOR && j < it->size() - NO_COLOR)
            {
                double normal_len = 1.5;
                do
                {
                    Vector2 pos = *it2 + normals[i][j] * normal_len;
                    ri[j] = vimg.GetIndex_no255(pos.x, pos.y);
                    normal_len += 0.5;
                }
                while(ri[j] == -1 && normal_len < 6);
                if(ri[j] < 2)
                {
                    normal_len = 1.5;
                    ri[j] = -1;
                    do
                    {
                        Vector2 pos = *it2 + normals[i][j] * normal_len;
                        ri[j] = -vimg2.GetIndex_no255(pos.x, pos.y);
                        normal_len += 0.5;
                    }
                    while(ri[j] == -1 && normal_len < 6);
                }
            }
        }
    }
    return ans;
}


Color2Side LinesIndex2Color2(const Lines& lines, Index2Side& idx2s, ColorConstraints& ccms, ColorConstraints& ccms2)
{
    printf("ccms.size(%d)\n", ccms.size());
    Color2Side res;
    res.left.resize(idx2s.left.size());
    res.right.resize(idx2s.right.size());
    Vector3s2d& cleft = res.left;
    Vector3s2d& cright = res.right;
    for(int i = 0; i < cleft.size(); ++i)
    {
        cleft[i].resize(idx2s.left[i].size());
        cright[i].resize(idx2s.right[i].size());
        for(int j = 0; j < cleft[i].size(); ++j)
        {
            if(idx2s.left[i][j] > 0 && idx2s.left[i][j] <= ccms.size())
            {
                cleft[i][j] = ccms.at(idx2s.left[i][j] - 1).GetColorVector3Reverse(lines[i][j].x, lines[i][j].y);
            }
            else if(idx2s.left[i][j] < -1 && abs(idx2s.left[i][j]) < ccms2.size())
            {
                cleft[i][j] = ccms2.at(abs(idx2s.left[i][j]) - 1).GetColorVector3Reverse(lines[i][j].x, lines[i][j].y);
                cright[i][j] = cleft[i][j];
                continue;
            }
            else
            {
                //printf("bad idx l %d\n", idx2s.left[i][j] - 1);
                cleft[i][j] = Vector3(0, 255, 0);
            }
            if(idx2s.right[i][j] > 0 && idx2s.right[i][j] <= ccms.size())
            {
                cright[i][j] = ccms.at(idx2s.right[i][j] - 1).GetColorVector3Reverse(lines[i][j].x, lines[i][j].y);
            }
            else if(idx2s.right[i][j] < -1 && abs(idx2s.right[i][j]) <= ccms2.size())
            {
                cright[i][j] = ccms2.at(abs(idx2s.right[i][j]) - 1).GetColorVector3Reverse(lines[i][j].x, lines[i][j].y);
                cleft[i][j] = cright[i][j];
                continue;
            }
            else
            {
                //printf("bad idx r %d\n", idx2s.right[i][j]);
                cright[i][j] = Vector3(0, 255, 0);
            }
        }
    }
    return res;
}


Color2Side LinesIndex2Color(const Lines& lines, Index2Side& idx2s, ColorConstraints& ccms)
{
    printf("ccms.size(%d)\n", ccms.size());
    Color2Side res;
    res.left.resize(idx2s.left.size());
    res.right.resize(idx2s.right.size());
    Vector3s2d& cleft = res.left;
    Vector3s2d& cright = res.right;
    Lines normals = GetNormalsLen2(lines);
    for(int i = 0; i < cleft.size(); ++i)
    {
        cleft[i].resize(idx2s.left[i].size());
        for(int j = 0; j < cleft[i].size(); ++j)
        {
            if(idx2s.left[i][j] > 0 && idx2s.left[i][j] <= ccms.size())
            {
                Vector2 pos = lines[i][j] - normals[i][j] * 2;
                cleft[i][j] = ccms.at(idx2s.left[i][j] - 1).GetColorVector3Reverse(pos.x, pos.y);
            }
            else
            {
                //printf("bad idx l %d\n", idx2s.left[i][j] - 1);
                cleft[i][j] = Vector3(255, 255, 255);
            }
        }
    }
    for(int i = 0; i < cright.size(); ++i)
    {
        cright[i].resize(idx2s.right[i].size());
        for(int j = 0; j < cright[i].size(); ++j)
        {
            if(idx2s.right[i][j] > 0 && idx2s.right[i][j] <= ccms.size())
            {
                Vector2 pos = lines[i][j] + normals[i][j] * 2;
                cright[i][j] = ccms.at(idx2s.right[i][j] - 1).GetColorVector3Reverse(pos.x, pos.y);
            }
            else
            {
                //printf("bad idx r %d\n", idx2s.right[i][j] - 1);
                cright[i][j] = Vector3(255, 255, 255);
            }
        }
    }
    return res;
}

Vector3s2d LinesIndex2Color(const Lines& lines, ints2d& idx2s, ColorConstraints& ccms)
{
    printf("ccms.size(%d)\n", ccms.size());
    Vector3s2d res;
    res.resize(idx2s.size());
    for(int i = 0; i < res.size(); ++i)
    {
        res[i].resize(idx2s[i].size());
        for(int j = 0; j < res[i].size(); ++j)
        {
            if(idx2s[i][j] > 0 && idx2s[i][j] <= ccms.size())
            {
                res[i][j] = ccms.at(idx2s[i][j] - 1).GetColorVector3Reverse(lines[i][j].x, lines[i][j].y);
            }
            else
            {
                printf("bad idx l %d\n", idx2s[i][j] - 1);
                res[i][j] = Vector3(255, 255, 255);
            }
        }
    }
    return res;
}

ints2d GetLinesIndex(cv::Mat img, const Lines& lines)
{
    vavImage vimg(img);
    ints2d ans;
    ans.resize(lines.size());
    for(int i = 0; i < lines.size(); ++i)
    {
        Lines::const_iterator it = lines.cbegin() + i;
        ints& li = ans[i];
        int j = 0;
        li.resize(it->size());
        for(auto it2 = it->cbegin(); it2 != it->cend(); ++it2, ++j)
        {
            li[j] = vimg.GetIndex_no255(it2->x, it2->y);
        }
    }
    return ans;
}
Lines GetLinesFromCvPatch(CvPatchs& cps)
{
    Lines res;
    for(int i = 0; i < cps.size(); ++i)
    {
        res.push_back(GetLine(cps[i].Outer2()));
    }
    return res;
}

cv::Mat ShowColorToLine(const Line& cps, const cv::Mat img)
{
    cv::Mat sc;
    int w = cps.size();
    if(w < 200)
    {
        w = 200;
    }
    sc.create(200, w, CV_8UC3);
    sc = cv::Scalar(0);
    for(int i = 0; i < cps.size(); ++i)
    {
        cv::Vec3b tc = img.at<cv::Vec3b>(cps[i].y * 2, cps[i].x * 2);
        for(int j = 0; j < 200; ++j)
        {
            sc.at<cv::Vec3b>(j, i) = tc;
        }
    }
    return sc;
}

Color2SideConstraint ConvertToConstraintC2(Lines& lines, Color2Side& colors)
{
    Color2SideConstraint res;
    for(int i = 0; i < lines.size(); ++i)
    {
        res.left.push_back(ColorConstraint());
        res.right.push_back(ColorConstraint());
        ColorConstraint& left = res.left.back();
        ColorConstraint& right = res.right.back();
        Line& line = lines[i];
        Vector3s& cl = colors.left[i];
        Vector3s& cr = colors.right[i];
        for(int j = 0; j < line.size(); ++j)
        {
            left.AddPoint(line[j].x, line[j].y, cl[j]);
            right.AddPoint(line[j].x, line[j].y, cr[j]);
        }
        left.BuildModel();
        right.BuildModel();
    }
    return res;
}

ColorConstraints ConvertToConstraintC(Lines& lines, Vector3s2d& colors)
{
    ColorConstraints res;
    for(int i = 0; i < lines.size(); ++i)
    {
        res.push_back(ColorConstraint());
        ColorConstraint& now = res.back();
        Line& line = lines[i];    
        Vector3s& cl = colors[i];
        for(int j = 0; j < line.size(); ++j)
        {
            now.AddPoint(line[j].x, line[j].y, cl[j]);
        }
        now.BuildModel();
    }
    return res;
}

LineWidthConstraints ConvertToConstraintLW(Lines& lines, Lines& lws)
{
    LineWidthConstraints res;
    for(int i = 0; i < lines.size(); ++i)
    {
        res.push_back(LineWidthConstraint());
        LineWidthConstraint& now = res.back();
        Line& line = lines[i];
        Line& ll = lws[i];
        for(int j = 0; j < line.size(); ++j)
        {
            now.AddPoint(line[j].x, line[j].y, ll[j].x, ll[j].y);
        }
        now.BuildModel();
    }
    return res;
}

Lines ConvertFromConstraintLW(Lines& lines, LineWidthConstraints lwcs)
{
    Lines res;
    for(int i = 0; i < lines.size(); ++i)
    {
        res.push_back(Line());
        Line& now = res.back();
        Line& line = lines[i];
        LineWidthConstraint& ll = lwcs[i];
        now.push_back(ll.GetColorVector2(0));
        double sum = 0;
        for(int j = 1; j < line.size(); ++j)
        {
            sum += line[j - 1].distance(line[j]);
            now.push_back(ll.GetColorVector2(sum));
        }
    }
    return res;
}

Vector3s2d ConvertFromConstraintC(Lines& lines, ColorConstraints ccs)
{
    Vector3s2d res;
    for(int i = 0; i < lines.size(); ++i)
    {
        res.push_back(Vector3s());
        Vector3s& now = res.back();
        Line& line = lines[i];
        ColorConstraint& ll = ccs[i];
        for(int j = 0; j < line.size(); ++j)
        {
            now.push_back(ll.GetColorVector3(line[j].x, line[j].y));
        }
    }
    return res;
}

Color2Side ConvertFromConstraintC2(Lines& lines, Color2SideConstraint c2c)
{
    Color2Side res;
    for(int i = 0; i < lines.size(); ++i)
    {
        res.left.push_back(Vector3s());
        res.right.push_back(Vector3s());
        Vector3s& resll = res.left.back();
        Vector3s& resrr = res.right.back();
        Line& line = lines[i];
        ColorConstraint& ll = c2c.left[i];
        ColorConstraint& rr = c2c.right[i];
        for(int j = 0; j < line.size(); ++j)
        {
            resll.push_back(ll.GetColorVector3(line[j].x, line[j].y));
            resrr.push_back(rr.GetColorVector3(line[j].x, line[j].y));
        }
    }
    return res;
}
