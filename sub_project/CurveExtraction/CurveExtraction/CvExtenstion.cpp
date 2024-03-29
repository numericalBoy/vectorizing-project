#include <algorithm>
#include <iterator>
#include "CvExtenstion.h"
#include "math\Quaternion.h"
#include "CmCurveEx.h"

cv::Mat CannyEdge(cv::Mat& image, double threshold1/*=0*/,
                  double threshold2/*=30*/, int apertureSize/*=3*/, bool L2gradient/*=false*/)
{
	cv::Mat edges;
	cvtColor(image, edges, CV_BGR2GRAY);   //convert from RGB color space to GRAY
	Canny(edges, edges,
	      threshold1,
	      threshold2,
	      apertureSize, L2gradient);
	cvtColor(edges, edges, CV_GRAY2BGR);
	bwmorph_clean(image);
	//Skel();
	return edges;
}

void bwmorph_clean(cv::Mat& image)
{
	for (int j = 1; j < image.cols - 1; ++j)
	{
		for (int i = 1; i < image.rows - 1; ++i)
		{
			cv::Vec3b& intensity = image.at<cv::Vec3b>(i, j);

			if (intensity[0] != 0 && intensity[1] != 0 && intensity[2] != 0)
			{
				bool zero = true;
				zero &= IsZero(image, i - 1, j - 1);
				zero &= IsZero(image, i - 1, j);
				zero &= IsZero(image, i - 1, j + 1);
				zero &= IsZero(image, i  , j - 1);
				zero &= IsZero(image, i  , j + 1);
				zero &= IsZero(image, i + 1, j - 1);
				zero &= IsZero(image, i + 1, j);
				zero &= IsZero(image, i + 1, j + 1);

				if (zero)
				{
					intensity[0] = 0;
					intensity[1] = 0;
					intensity[2] = 0;
				}
			}
		}
	}
}

bool IsZero(cv::Mat& image, int i, int j)
{
	cv::Vec3b& intensity = image.at<cv::Vec3b>(i, j);

	if (intensity[0] == 0 && intensity[1] == 0 && intensity[2] == 0)
	{
		return true;
	}

	return false;
}

Lines ComputeEdgeLine(const cv::Mat& image)
{
	cv::Mat tImage = image;
	Lines   res;

	for (int i = 0; i < tImage.rows; ++i)
	{
		for (int j = 0; j < tImage.cols; ++j)
		{
			cv::Vec3b& intensity = tImage.at<cv::Vec3b>(i, j);

			if (intensity[0] != 0 || intensity[1] != 0 || intensity[2] != 0)
			{
				Line line;
				line.push_back(Vector2(j, i));
				intensity[0] = 0;
				intensity[1] = 0;
				intensity[2] = 0;
				EdgeLink(tImage, line);
				res.push_back(line);
			}
		}
	}

	return res;
}
struct WeightData
{
	WeightData(Vector2& p, int w): pos(p), weight(w)    {}
	Vector2 pos;
	int  weight;
	bool operator<(const WeightData& wd)
	{
		return weight < wd.weight;
	}
};
typedef std::vector<WeightData> Weights;

Weights wm_init;
Weights wm_init_cross;
Weights wm_init2;
struct _tmp_function
{
	_tmp_function()
	{
		wm_init.push_back(WeightData(Vector2(-1, -1), 1));
		wm_init.push_back(WeightData(Vector2(0, -1), 1));
		wm_init.push_back(WeightData(Vector2(1, -1), 1));
		wm_init.push_back(WeightData(Vector2(-1, 0), 1));
		wm_init.push_back(WeightData(Vector2(1, 0), 1));
		wm_init.push_back(WeightData(Vector2(-1, 1), 1));
		wm_init.push_back(WeightData(Vector2(0, 1), 1));
		wm_init.push_back(WeightData(Vector2(1, 1), 1));
		wm_init_cross.push_back(WeightData(Vector2(0, -1), 1));
		wm_init_cross.push_back(WeightData(Vector2(0, +1), 1));
		wm_init_cross.push_back(WeightData(Vector2(-1, 0), 1));
		wm_init_cross.push_back(WeightData(Vector2(+1, 0), 1));
		wm_init2.push_back(WeightData(Vector2(-2, -2), 1));
		wm_init2.push_back(WeightData(Vector2(-1, -2), 1));
		wm_init2.push_back(WeightData(Vector2(0, -2), 1));
		wm_init2.push_back(WeightData(Vector2(1, -2), 1));
		wm_init2.push_back(WeightData(Vector2(2, -2), 1));
		wm_init2.push_back(WeightData(Vector2(-2, -1), 1));
		wm_init2.push_back(WeightData(Vector2(2, -1), 1));
		wm_init2.push_back(WeightData(Vector2(-2, 0), 1));
		wm_init2.push_back(WeightData(Vector2(2, 0), 1));
		wm_init2.push_back(WeightData(Vector2(-2, 1), 1));
		wm_init2.push_back(WeightData(Vector2(2, 1), 1));
		wm_init2.push_back(WeightData(Vector2(-2, 2), 1));
		wm_init2.push_back(WeightData(Vector2(-1, 2), 1));
		wm_init2.push_back(WeightData(Vector2(0, 2), 1));
		wm_init2.push_back(WeightData(Vector2(1, 2), 1));
		wm_init2.push_back(WeightData(Vector2(2, 2), 1));
	}
} __tmp_function;

void EdgeLink(cv::Mat& image, Line& now_line)
{
	bool    edgefail = false;

	for (; !edgefail;)
	{
		edgefail = true;
		Weights wm = wm_init;

		if (now_line.size() > 1)
		{
			int x, y;
			Vector2 move = now_line.back() - *(now_line.end() - 2);

			for (int i = 0; i < wm.size(); i++)
			{
				if (move.y != 0 && move.y == wm[i].pos.y)
				{
					wm[i].weight++;
				}

				if (move.x != 0 && move.x == wm[i].pos.x)
				{
					wm[i].weight++;
				}

				if (wm[i].pos == move)
				{
					wm[i].weight++;
				}
			}
		}

		std::sort(wm.begin(), wm.end());
		const Vector2& v = now_line.back();

		for (int i = 0; i < wm.size(); i++)
		{
			int x = v.x + wm[i].pos.x;
			int y = v.y + wm[i].pos.y;

			if (y < 0) { y = 0; }

			if (y >= image.rows) { y = image.rows - 1; }

			if (x < 0) { x = 0; }

			if (x >= image.cols) { x = image.cols - 1; }

			cv::Vec3b& intensity = image.at<cv::Vec3b>(y, x);

			if (intensity[0] != 0 || intensity[1] != 0 || intensity[2] != 0)
			{
				now_line.push_back(Vector2(x, y));
				intensity[0] = 0;
				intensity[1] = 0;
				intensity[2] = 0;
				edgefail = false;
				break;
			}
		}

		if (edgefail)
		{
			for (int i = 0; i < wm_init2.size(); i++)
			{
				int x = v.x + wm_init2[i].pos.x;
				int y = v.y + wm_init2[i].pos.y;

				if (y < 0) { y = 0; }

				if (y >= image.rows) { y = image.rows - 1; }

				if (x < 0) { x = 0; }

				if (x >= image.cols) { x = image.cols - 1; }

				cv::Vec3b& intensity = image.at<cv::Vec3b>(y, x);

				if (intensity[0] != 0 || intensity[1] != 0 || intensity[2] != 0)
				{
					now_line.push_back(Vector2(x, y));
					intensity[0] = 0;
					intensity[1] = 0;
					intensity[2] = 0;
					edgefail = false;
					break;
				}
			}
		}
	}

	std::reverse(now_line.begin(), now_line.end());
	edgefail = false;

	for (; !edgefail;)
	{
		edgefail = true;
		Weights wm = wm_init;

		if (now_line.size() > 1)
		{
			int x, y;
			Vector2 move = now_line.back() - *(now_line.end() - 2);

			for (int i = 0; i < wm.size(); i++)
			{
				if (move.y != 0 && move.y == wm[i].pos.y)
				{
					wm[i].weight++;
				}

				if (move.x != 0 && move.x == wm[i].pos.x)
				{
					wm[i].weight++;
				}

				if (wm[i].pos == move)
				{
					wm[i].weight++;
				}
			}
		}

		std::sort(wm.begin(), wm.end());
		const Vector2& v = now_line.back();

		for (int i = 0; i < wm.size(); i++)
		{
			int x = v.x + wm[i].pos.x;
			int y = v.y + wm[i].pos.y;

			if (x < 0) { x = 0; }

			if (x >= image.cols) { x = image.cols - 1; }

			if (y < 0) { y = 0; }

			if (y >= image.rows) { y = image.rows - 1; }

			cv::Vec3b& intensity = image.at<cv::Vec3b>(y, x);

			if (intensity[0] != 0 || intensity[1] != 0 || intensity[2] != 0)
			{
				now_line.push_back(Vector2(x, y));
				intensity[0] = 0;
				intensity[1] = 0;
				intensity[2] = 0;
				edgefail = false;
				break;
			}
		}

		if (edgefail)
		{
			for (int i = 0; i < wm_init2.size(); i++)
			{
				int x = v.x + wm_init2[i].pos.x;
				int y = v.y + wm_init2[i].pos.y;

				if (x < 0) { x = 0; }

				if (x >= image.cols) { x = image.cols - 1; }

				if (y < 0) { y = 0; }

				if (y >= image.rows) { y = image.rows - 1; }

				cv::Vec3b& intensity = image.at<cv::Vec3b>(y, x);

				if (intensity[0] != 0 || intensity[1] != 0 || intensity[2] != 0)
				{
					now_line.push_back(Vector2(x, y));
					intensity[0] = 0;
					intensity[1] = 0;
					intensity[2] = 0;
					edgefail = false;
					break;
				}
			}
		}
	}
}

void EdgeLink_LineFragment(cv::Mat& image, Line& now_line, int now_value)
{
	bool    edgefail = false;

	for (; !edgefail;)
	{
		edgefail = true;
		Weights wm = wm_init_cross;

		if (now_line.size() > 1)
		{
			int x, y;
			Vector2 move = now_line.back() - *(now_line.end() - 2);

			for (int i = 0; i < wm.size(); i++)
			{
				if (move.y != 0 && move.y == wm[i].pos.y)
				{
					wm[i].weight++;
				}

				if (move.x != 0 && move.x == wm[i].pos.x)
				{
					wm[i].weight++;
				}

				if (wm[i].pos == move)
				{
					wm[i].weight++;
				}
			}
		}

		std::sort(wm.begin(), wm.end());
		const Vector2& v = now_line.back();

		for (int i = 0; i < wm.size(); i++)
		{
			int x = v.x + wm[i].pos.x;
			int y = v.y + wm[i].pos.y;

			if (y < 0) { y = 0; }

			if (y >= image.rows) { y = image.rows - 1; }

			if (x < 0) { x = 0; }

			if (x >= image.cols) { x = image.cols - 1; }

			cv::Vec3b& intensity = image.at<cv::Vec3b>(y, x);

			if (Vector2(x, y) != v
			        && intensity[0] == (now_value % 255)
			        && intensity[1] == (now_value / 255) % 255
			        && intensity[2] == (now_value / (255 * 255)))
			{
				now_line.push_back(Vector2(x, y));
				intensity[0] = 0;
				intensity[1] = 0;
				intensity[2] = 0;
				edgefail = false;
				//imshow("tmp_image", image);
				//cv::waitKey();
				break;
			}
		}
	}

	// add white joint point
	{
		const Vector2& v = now_line.back();
		Weights wm = wm_init_cross;
		bool getjoint = false;

		for (int i = 0; i < wm.size(); i++)
		{
			int x = v.x + wm[i].pos.x;
			int y = v.y + wm[i].pos.y;

			if (y < 0) { y = 0; }

			if (y >= image.rows) { y = image.rows - 1; }

			if (x < 0) { x = 0; }

			if (x >= image.cols) { x = image.cols - 1; }

			cv::Vec3b& intensity = image.at<cv::Vec3b>(y, x);

			if (intensity[0] == 255
			        && intensity[1] == 255
			        && intensity[2] == 255)
			{
				now_line.push_back(Vector2(x, y));
				getjoint = true;
				break;
			}
		}

		if (!getjoint)
		{
			for (int i = 0; i < wm.size(); i++)
			{
				int x = v.x + wm[i].pos.x;
				int y = v.y + wm[i].pos.y;

				if (y < 0) { y = 0; }

				if (y >= image.rows) { y = image.rows - 1; }

				if (x < 0) { x = 0; }

				if (x >= image.cols) { x = image.cols - 1; }

				if (Vector2(x, y) == now_line.front())
				{
					now_line.push_back(Vector2(x, y));
					getjoint = true;
					break;
				}
			}

			if (now_line.back() == now_line.front())
			{
				getjoint = true;
			}
		}

		assert(getjoint);
	}
	std::reverse(now_line.begin(), now_line.end());
	edgefail = false;

	for (; !edgefail;)
	{
		edgefail = true;
		Weights wm = wm_init_cross;

		if (now_line.size() > 1)
		{
			int x, y;
			Vector2 move = now_line.back() - *(now_line.end() - 2);

			for (int i = 0; i < wm.size(); i++)
			{
				if (move.y != 0 && move.y == wm[i].pos.y)
				{
					wm[i].weight++;
				}

				if (move.x != 0 && move.x == wm[i].pos.x)
				{
					wm[i].weight++;
				}

				if (wm[i].pos == move)
				{
					wm[i].weight++;
				}
			}
		}

		std::sort(wm.begin(), wm.end());
		const Vector2& v = now_line.back();

		for (int i = 0; i < wm.size(); i++)
		{
			int x = v.x + wm[i].pos.x;
			int y = v.y + wm[i].pos.y;

			if (x < 0) { x = 0; }

			if (x >= image.cols) { x = image.cols - 1; }

			if (y < 0) { y = 0; }

			if (y >= image.rows) { y = image.rows - 1; }

			cv::Vec3b& intensity = image.at<cv::Vec3b>(y, x);

			if (Vector2(x, y) != v
			        && intensity[0] == (now_value % 255)
			        && intensity[1] == (now_value / 255) % 255
			        && intensity[2] == (now_value / (255 * 255)))
			{
				now_line.push_back(Vector2(x, y));
				intensity[0] = 0;
				intensity[1] = 0;
				intensity[2] = 0;
				edgefail = false;
				//imshow("tmp_image", image);
				//cv::waitKey();
				break;
			}
		}
	}

	// add white joint point
	{
		const Vector2& v = now_line.back();
		Weights wm = wm_init_cross;
		bool getjoint = false;

		for (int i = wm.size() - 1; i >= 0; --i)
		{
			int x = v.x + wm[i].pos.x;
			int y = v.y + wm[i].pos.y;

			if (y < 0) { y = 0; }

			if (y >= image.rows) { y = image.rows - 1; }

			if (x < 0) { x = 0; }

			if (x >= image.cols) { x = image.cols - 1; }

			cv::Vec3b& intensity = image.at<cv::Vec3b>(y, x);

			if (intensity[0] == 255
			        && intensity[1] == 255
			        && intensity[2] == 255)
			{
				now_line.push_back(Vector2(x, y));
				getjoint = true;
				break;
			}
		}

		if (!getjoint)
		{
			for (int i = 0; i < wm.size(); i++)
			{
				int x = v.x + wm[i].pos.x;
				int y = v.y + wm[i].pos.y;

				if (y < 0) { y = 0; }

				if (y >= image.rows) { y = image.rows - 1; }

				if (x < 0) { x = 0; }

				if (x >= image.cols) { x = image.cols - 1; }

				if (Vector2(x, y) == now_line.front())
				{
					now_line.push_back(Vector2(x, y));
					getjoint = true;
					break;
				}
			}

			if (now_line.back() == now_line.front())
			{
				getjoint = true;
			}
		}

		assert(getjoint);
	}
}

void Skeleton(cv::Mat& image)
{
	cv::Mat skel(image.size(), image.type(), cv::Scalar(0));
	cv::Mat temp;
	cv::Mat eroded;
	cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
	bool done;

	do
	{
		cv::erode(image, eroded, element);
		cv::dilate(eroded, temp, element);   // temp = open(img)
		cv::subtract(image, temp, temp);
		cv::bitwise_or(skel, temp, skel);
		eroded.copyTo(image);
		done = (cv::norm(image) == 0);
	}
	while (!done);

	image = skel;
}

Lines ComputeTrappedBallEdge(cv::Mat& image, const Lines& old_line,
                             int ball_radius)
{
	cv::Mat timage(image.size(), CV_8U, cv::Scalar(0));

	for (Lines::const_iterator it = old_line.begin(); it != old_line.end(); ++it)
	{
		for (Line::const_iterator it2 = it->begin(); it2 != it->end(); ++it2)
		{
			uchar& intensity = timage.at<uchar>(it2->y, it2->x);
			intensity = 255;
		}
	}

	Lines res = old_line;

	for (Lines::iterator it = res.begin(); it != res.end(); ++it)
	{
		Line& li = *it;

		if (li.size() < 4) { continue; }

		bool findhole = false;
		Vector2 ahead = (li[li.size() - 1] - li[li.size() - 2]) +
		                (li[li.size() - 1] - li[li.size() - 3]);
		ahead.normalise();

		if (LinkTrapBallBack(li, ahead, timage, ball_radius))
		{
			continue;
		}

		for (int i = 1; i < 6; ++i)
		{
			Vector2 up = Quaternion::GetRotation(ahead, i * 10);

			if (LinkTrapBallBack(li, up, timage, ball_radius))
			{
				break;
			}

			Vector2 down = Quaternion::GetRotation(ahead, -i * 10);

			if (LinkTrapBallBack(li, down, timage, ball_radius))
			{
				break;
			}
		}
	}

	for (Lines::iterator it = res.begin(); it != res.end(); ++it)
	{
		std::reverse(it->begin(), it->end());
	}

	for (Lines::iterator it = res.begin(); it != res.end(); ++it)
	{
		Line& li = *it;

		if (li.size() < 4) { continue; }

		bool findhole = false;
		Vector2 ahead = (li[li.size() - 1] - li[li.size() - 2]) +
		                (li[li.size() - 1] - li[li.size() - 3]);
		ahead.normalise();

		if (LinkTrapBallBack(li, ahead, timage, ball_radius))
		{
			continue;
		}

		for (int i = 1; i < 6; ++i)
		{
			Vector2 up = Quaternion::GetRotation(ahead, i * 10);

			if (LinkTrapBallBack(li, up, timage, ball_radius))
			{
				break;
			}

			Vector2 down = Quaternion::GetRotation(ahead, -i * 10);

			if (LinkTrapBallBack(li, down, timage, ball_radius))
			{
				break;
			}
		}
	}

	return res;
}

bool LinkTrapBallBack(Line& li, const Vector2& ahead, cv::Mat& image,
                      int ball_radius)
{
	for (int r = 1; r <= ball_radius; ++r)
	{
		Vector2 tmp = li.back() + ahead * r;

		if (li.back() == Vector2((int)tmp.x, (int)tmp.y))
		{
			continue;
		}

		if (CorrectPosition(image, (int)tmp.x, (int)tmp.y))
		{
			uchar& intensity = image.at<uchar>((int)tmp.y, (int)tmp.x);

			if (intensity == 255)
			{
				Vector2 t = li.back();

				for (int go_r = 1; go_r <= r; ++go_r)
				{
					Vector2 tmp2 = t + ahead * go_r;

					if (CorrectPosition(image, (int)tmp2.x, (int)tmp2.y))
					{
						uchar& color = image.at<uchar>((int)tmp2.y, (int)tmp2.x);
						color = 255;
						li.push_back(Vector2((int)tmp2.x, (int)tmp2.y));
					}
					else
					{
						break;
					}
				}

				std::unique(li.begin(), li.end());
				return true;
			}
		}
		else
		{
			Vector2 t = li.back();

			for (int go_r = 1; go_r <= r; ++go_r)
			{
				Vector2 tmp2 = t + ahead * go_r;

				if (CorrectPosition(image, (int)tmp2.x, (int)tmp2.y))
				{
					uchar& color = image.at<uchar>((int)tmp2.y, (int)tmp2.x);
					color = 255;
					li.push_back(Vector2((int)tmp2.x, (int)tmp2.y));
				}
				else
				{
					break;
				}
			}

			std::unique(li.begin(), li.end());
			return true;
		}
	}

	return false;
}

void Dilation(cv::Mat& image, int dilation_elem, int dilation_size)
{
	int dilation_type;
	int dilation_size2 = dilation_size * 2 + 1;

	if (dilation_elem == 0) { dilation_type = cv::MORPH_RECT; }
	else if (dilation_elem == 1) { dilation_type = cv::MORPH_CROSS; }
	else if (dilation_elem == 2) { dilation_type = cv::MORPH_ELLIPSE; }

	cv::Mat element = getStructuringElement(dilation_type,
	                                        cv::Size(dilation_size2, dilation_size2),
	                                        cv::Point(dilation_size, dilation_size));
	cv::dilate(image, image, element);
}

void Erosion(cv::Mat& image, int erosion_elem, int erosion_size)
{
	int erosion_type;
	int erosion_size2 = erosion_size * 2 + 1;

	if (erosion_elem == 0) { erosion_type = cv::MORPH_RECT; }
	else if (erosion_elem == 1) { erosion_type = cv::MORPH_CROSS; }
	else if (erosion_elem == 2) { erosion_type = cv::MORPH_ELLIPSE; }

	cv::Mat element = getStructuringElement(erosion_type,
	                                        cv::Size(erosion_size2, erosion_size2),
	                                        cv::Point(erosion_size, erosion_size));
	cv::erode(image, image, element);
}

Lines ComputeEdgeLine2(const cv::Mat& image)
{
	cv::Mat tImage = image;
	Lines   res;

	for (int i = 0; i < tImage.rows; ++i)
	{
		for (int j = 0; j < tImage.cols; ++j)
		{
			char& intensity = tImage.at<char>(i, j);

			if (intensity != 0)
			{
				Line line;
				line.push_back(Vector2(j, i));
				intensity = 0;
				EdgeLink2(tImage, line);
				res.push_back(line);
			}
		}
	}

	return res;
}

void EdgeLink2(cv::Mat& image, Line& now_line)
{
	bool    edgefail = false;

	for (; !edgefail;)
	{
		edgefail = true;
		Weights wm = wm_init;

		if (now_line.size() > 1)
		{
			int x, y;
			Vector2 move = now_line.back() - *(now_line.end() - 2);

			for (int i = 0; i < wm.size(); i++)
			{
				if (move.y != 0 && move.y == wm[i].pos.y)
				{
					wm[i].weight++;
				}

				if (move.x != 0 && move.x == wm[i].pos.x)
				{
					wm[i].weight++;
				}

				if (wm[i].pos == move)
				{
					wm[i].weight++;
				}
			}
		}

		std::sort(wm.begin(), wm.end());
		const Vector2& v = now_line.back();

		for (int i = 0; i < wm.size(); i++)
		{
			int x = v.x + wm[i].pos.x;
			int y = v.y + wm[i].pos.y;

			if (y < 0) { y = 0; }

			if (y >= image.rows) { y = image.rows - 1; }

			if (x < 0) { x = 0; }

			if (x >= image.cols) { x = image.cols - 1; }

			char& intensity = image.at<char>(y, x);

			if (intensity != 0)
			{
				now_line.push_back(Vector2(x, y));
				intensity = 0;
				edgefail = false;
				break;
			}
		}

		if (edgefail)
		{
			for (int i = 0; i < wm_init2.size(); i++)
			{
				int x = v.x + wm_init2[i].pos.x;
				int y = v.y + wm_init2[i].pos.y;

				if (y < 0) { y = 0; }

				if (y >= image.rows) { y = image.rows - 1; }

				if (x < 0) { x = 0; }

				if (x >= image.cols) { x = image.cols - 1; }

				char& intensity = image.at<char>(y, x);

				if (intensity != 0)
				{
					now_line.push_back(Vector2(x, y));
					intensity = 0;
					edgefail = false;
					break;
				}
			}
		}
	}

	std::reverse(now_line.begin(), now_line.end());
	edgefail = false;

	for (; !edgefail;)
	{
		edgefail = true;
		Weights wm = wm_init;

		if (now_line.size() > 1)
		{
			int x, y;
			Vector2 move = now_line.back() - *(now_line.end() - 2);

			for (int i = 0; i < wm.size(); i++)
			{
				if (move.y != 0 && move.y == wm[i].pos.y)
				{
					wm[i].weight++;
				}

				if (move.x != 0 && move.x == wm[i].pos.x)
				{
					wm[i].weight++;
				}

				if (wm[i].pos == move)
				{
					wm[i].weight++;
				}
			}
		}

		std::sort(wm.begin(), wm.end());
		const Vector2& v = now_line.back();

		for (int i = 0; i < wm.size(); i++)
		{
			int x = v.x + wm[i].pos.x;
			int y = v.y + wm[i].pos.y;

			if (x < 0) { x = 0; }

			if (x >= image.cols) { x = image.cols - 1; }

			if (y < 0) { y = 0; }

			if (y >= image.rows) { y = image.rows - 1; }

			char& intensity = image.at<char>(y, x);

			if (intensity != 0)
			{
				now_line.push_back(Vector2(x, y));
				intensity = 0;
				edgefail = false;
				break;
			}
		}

		if (edgefail)
		{
			for (int i = 0; i < wm_init2.size(); i++)
			{
				int x = v.x + wm_init2[i].pos.x;
				int y = v.y + wm_init2[i].pos.y;

				if (x < 0) { x = 0; }

				if (x >= image.cols) { x = image.cols - 1; }

				if (y < 0) { y = 0; }

				if (y >= image.rows) { y = image.rows - 1; }

				char& intensity = image.at<char>(y, x);

				if (intensity != 0)
				{
					now_line.push_back(Vector2(x, y));
					intensity = 0;
					edgefail = false;
					break;
				}
			}
		}
	}
}

void S1FloodFill(cv::Mat& image, cv::Mat& mask, int x, int y,
                 CvPoints2d& out_array)
{
	cv::Vec3b& c = image.at<cv::Vec3b>(y, x);

	if ((c[2] != 0 && c[1] != 0 && c[0] != 0) || c[0] == 255) { return; }

	cv::Point seed(x, y);
	cv::Rect ccomp;
	cv::Scalar newVal(128, 1, 1);
	int area;
	threshold(mask, mask, 1, 128, CV_THRESH_BINARY);
	area = floodFill(image, mask, seed, newVal, &ccomp);
	// get Contour line
	cv::Mat mask2 = mask.clone();

	for (int i = 0; i < mask2.rows ; i++)
	{
		char& c = mask2.at<char>(i, 0);
		c = 0;
		char& c2 = mask2.at<char>(i, mask2.cols - 1);
		c2 = 0;
	}

	for (int j = 0; j < mask2.cols ; j++)
	{
		char& c = mask2.at<char>(0, j);
		c = 0;
		char& c2 = mask2.at<char>(mask2.rows - 1, j);
		c2 = 0;
	}

//  Dilation( mask2, 0, 1 );
//  Erosion( mask2, 0, 1 );

	for (int i = 1; i < mask2.rows - 1; i++)
	{
		for (int j = 1; j < mask2.cols - 1; j++)
		{
			char& c = mask2.at<char>(i, j);

			if (c)
			{
				c = 255;
			}
		}
	}

	CvPoints2d points;
	cv::findContours(mask2, points, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	double tarea = cv::contourArea(points.front());

	if (tarea < 50) { return ; }

	CV_Assert(points.size() == 1);
	out_array.push_back(points.front());
}

void S1FloodFill(cv::Mat& image, cv::Mat& mask, int x, int y,
                 CvPatchs& out_array, int dilation, int erosion)
{
	cv::Vec3b& c = image.at<cv::Vec3b>(y, x);

	if ((c[2] != 0 && c[1] != 0 && c[0] != 0) || c[0] == 255) { return; }

	cv::Point seed(x, y);
	cv::Rect ccomp;
	cv::Scalar newVal(128, 1, 1);
	int area;
	threshold(mask, mask, 1, 128, CV_THRESH_BINARY);
	area = floodFill(image, mask, seed, newVal, &ccomp);
	// get Contour line
	cv::Mat mask2 = mask.clone();
	ClearEdge(mask2);

	for (int i = 1; i < mask2.rows - 1; i++)
	{
		for (int j = 1; j < mask2.cols - 1; j++)
		{
			uchar& intensity = mask2.at<uchar>(i, j);

			if (intensity > 0)
			{
				intensity = 255;
			}
		}
	}

	cv::imshow("image", image);
	cv::waitKey();
	CvPoints2d points;
	cv::findContours(mask2, points, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
	double tarea = cv::contourArea(points.front());

	if (points.empty() || tarea < 6) { return; }

	printf("num: %d\t area: %f\n", points.size(), tarea);

	if (dilation > 0)
	{
		Dilation(mask2, 1, dilation);
	}

	cv::Mat mask22 = mask2.clone();

	if (erosion > 0)
	{
		Erosion(mask2, 2, erosion);
	}

	CvPoints2d points2;
	cv::findContours(mask2, points2, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);

	for (int i = erosion - 1; i >= 0 && points2.empty(); --i)
	{
		mask2 = mask22.clone();

		if (i > 0)
		{
			Erosion(mask2, 2, i);
		}

		cv::findContours(mask2, points2, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
	}

	//cv::waitKey();
	CvPatch cvp;
	cvp.Outer() = points.front();

	if (points.size() > 1)
	{
		std::copy(points.begin() + 1, points.end(), std::back_inserter(cvp.Inter()));
	}

	cvp.Outer2() = points2.front();

	if (points2.size() > 1)
	{
		std::copy(points2.begin() + 1, points2.end(), std::back_inserter(cvp.Inter2()));
	}

	out_array.push_back(cvp);
}

CvPoints2d GetSidelines(const cv::Mat& image0)
{
	cv::Mat mask, image;
	image0.copyTo(image);
	mask.create(image.rows + 2, image.cols + 2, CV_8UC1);
	mask = cv::Scalar::all(0);
	CvPoints2d cvp;

	for (int i = 1; i < image.rows - 1; i++)
	{
		for (int j = 1; j < image.cols - 1; j++)
		{
			S1FloodFill(image, mask, j, i, cvp);
		}
	}

	return cvp;
}

Lines GetLines(const CvPoints2d& cvp)
{
	Lines lines;

	for (auto it1 = cvp.begin(); it1 != cvp.end(); ++it1)
	{
		lines.push_back(Line());

		for (auto it2 = it1->begin(); it2 != it1->end(); ++it2)
		{
			lines.back().push_back(Vector2(it2->x - 1, it2->y - 1));
		}
	}

	return lines;
}

cv::Mat Laplace(const cv::Mat& image, int aperture_size)
{
	cv::Mat edges;
	cvtColor(image, edges, CV_BGR2GRAY);   //convert from RGB color space to GRAY
	Laplacian(edges, edges, CV_8U, aperture_size, 1);
	cvtColor(edges, edges, CV_GRAY2BGR);
	return edges;
}

cv::Mat MakeLineImage(const cv::Mat& image0, const Lines& lines)
{
	cv::Mat image(image0.size(), CV_8UC3, cv::Scalar(0));

	for (Lines::const_iterator it = lines.begin(); it != lines.end(); ++it)
	{
		for (Line::const_iterator it2 = it->begin(); it2 != it->end(); ++it2)
		{
			cv::Vec3b& intensity = image.at<cv::Vec3b>(it2->y, it2->x);
			intensity[0] = 255;
			intensity[1] = 255;
			intensity[2] = 255;
		}
	}

	return image;
}

cv::Mat MakeLineImage(const cv::Mat& image0, Patch& patch)
{
	cv::Mat image(image0.size(), CV_8UC3, cv::Scalar(0));

	for (auto it = patch.Outer().begin(); it != patch.Outer().end(); ++it)
	{
		if (it->y >= image.rows) { it->y = image.rows - 1; }

		if (it->x >= image.cols) { it->x = image.cols - 1; }

		cv::Vec3b& intensity = image.at<cv::Vec3b>(it->y, it->x);
		intensity[0] = 255;
		intensity[1] = 255;
		intensity[2] = 255;
	}

	return image;
}

CvPatchs S1GetPatchs(const cv::Mat& image0)
{
	cv::Mat mask, image;
	image0.copyTo(image);
	mask.create(image.rows + 2, image.cols + 2, CV_8UC1);
	mask = cv::Scalar::all(0);
	CvPatchs cvps;

	for (int i = 1; i < image.rows - 1; i++)
	{
		for (int j = 1; j < image.cols - 1; j++)
		{
			S1FloodFill(image, mask, j, i, cvps);
		}
	}

	return cvps;
}

CvPatchs S1GetPatchs(const cv::Mat& image0, int dilation, int erosion)
{
	cv::Mat mask, image;
	image0.copyTo(image);
	mask.create(image.rows + 2, image.cols + 2, CV_8UC1);
	mask = cv::Scalar::all(0);
	CvPatchs cvps;

	for (int i = 1; i < image.rows - 1; i++)
	{
		for (int j = 1; j < image.cols - 1; j++)
		{
			S1FloodFill(image, mask, j, i, cvps, dilation, erosion);
		}
	}

	return cvps;
}

CvPatchs S2GetPatchs(const cv::Mat& image0, int dilation, int erosion)
{
	assert(image0.type() == CV_8UC3);
	cv::Mat img1u, cImg2;
	cImg2 = image0.clone();
	cvtColor(image0, img1u, CV_BGR2GRAY);
	cv::Mat srcImg1f, show3u = cv::Mat::zeros(img1u.size(), CV_8UC3);
	img1u.convertTo(srcImg1f, CV_32FC1, 1.0 / 255);
	CmCurveEx dEdge(srcImg1f);
	dEdge.CalSecDer(3, 0.01f, 0.2f);
	cv::Mat Der2 = dEdge.GetDer2();
	Dilation(Der2, 1, 1);
	cv::Mat mask, image;
	image0.copyTo(image);
	mask.create(image.rows + 2, image.cols + 2, CV_8UC1);
	mask = cv::Scalar::all(0);
	CvPatchs cvps;

	for (int i = 1; i < image.rows - 1; i++)
	{
		for (int j = 1; j < image.cols - 1; j++)
		{
			S2FloodFill(image, mask, Der2, 20, j, i, dilation, erosion);
		}
	}

	Erosion(mask, 1, 2);

	for (int i = 1; i < mask.rows - 1; i++)
	{
		for (int j = 1; j < mask.cols - 1; j++)
		{
			uchar& v = mask.at<uchar>(i, j);
			uchar& v2 = Der2.at<uchar>(i - 1, j - 1);

			if (v > 0)
			{
				v2 = 0;
			}
			else
			{
				v2 = 255;
			}
		}
	}

	image0.copyTo(image);
	mask = cv::Scalar::all(0);

	for (int i = 1; i < image.rows - 1; i++)
	{
		for (int j = 1; j < image.cols - 1; j++)
		{
			S2FloodFill(image, mask, Der2, 20, j, i, dilation, erosion);
		}
	}

	for (int i = 0; i < image.rows ; i++)
	{
		for (int j = 0; j < image.cols ; j++)
		{
			if (mask.at<uchar>(i + 1 , j + 1) == 0)
			{
				cv::Vec3b& v = image.at<cv::Vec3b>(i, j);
				v[0] = 0;
				v[1] = 0;
				v[2] = 0;
			}
		}
	}

	//imshow("Image2", image);
	//FixHole(image);
	mask = cv::Scalar::all(0);
	int cc = 0;

	for (int i = 1; i < image.rows - 1; i++)
	{
		for (int j = 1; j < image.cols - 1; j++)
		{
			S2FloodFill(cc, image, mask, Der2, 0, j, i, cvps, dilation, erosion);
		}
	}

	//imshow("mask", mask);
	return cvps;
}

void visualize_patch_lines(ImageSpline imageSpline, int idx, cv::Mat& image)
{
	PatchSpline& ps = imageSpline.m_PatchSplines[idx];
	LineIndex start_idx = ps.m_LineIndexs.front();
	Vector2 start, end;
	int b = rand() % 255;
	int g = rand() % 255;
	int r = rand() % 255;

	if (start_idx.m_Forward)
	{
		Vector2 v = imageSpline.m_LineFragments[start_idx.m_id].m_Points.front();
		start = v;
		cv::Vec3b& intensity = image.at<cv::Vec3b>(v.y, v.x);
		intensity[0] = b;
		intensity[1] = g;
		intensity[2] = r;
	}
	else
	{
		Vector2 v = imageSpline.m_LineFragments[start_idx.m_id].m_Points.back();
		start = v;
		cv::Vec3b& intensity = image.at<cv::Vec3b>(v.y, v.x);
		intensity[0] = b;
		intensity[1] = g;
		intensity[2] = r;
	}

	for (auto it = ps.m_LineIndexs.begin(); it != ps.m_LineIndexs.end(); ++it)
	{
		Line pts = imageSpline.m_LineFragments[it->m_id].m_Points;

		if (it->m_Forward)
		{
			for (auto it2 = pts.begin(); it2 != pts.end(); ++it2)
			{
				cv::Vec3b& intensity = image.at<cv::Vec3b>(it2->y, it2->x);
				intensity[0] = b;
				intensity[1] = g;
				intensity[2] = r;
			}

			end = pts.back();
		}
		else
		{
			for (auto it2 = pts.rbegin(); it2 != pts.rend(); ++it2)
			{
				cv::Vec3b& intensity = image.at<cv::Vec3b>(it2->y, it2->x);
				intensity[0] = b;
				intensity[1] = g;
				intensity[2] = r;
			}

			end = pts.front();
		}
	}

	cv::namedWindow("joint_mask2", 0);
	imshow("joint_mask2", image);
	cv::waitKey();
	assert(start == end);
}

bool CheckCathetus(cv::Point& p1, cv::Point& p2)
{
	int len1 = p1.x - p2.x;

	if (len1 < 0) { len1 = - len1; }

	int len2 = p1.y - p2.y;

	if (len2 < 0) { len2 = - len2; }

	if (len1 + len2 > 1)
	{
		return false;
	}
	else
	{
		return true;
	}
}

cv::Point GetCathetusPoint(cv::Point& p1, cv::Point& p2)
{
	cv::Point p3 = p2 - p1 + p2;
	return p3;
}

void DrawCvPoints(CvPoints& tmp_cvps, cv::Mat tmp_image2)
{
	tmp_image2 = cv::Scalar(0);

	for (auto it2 = tmp_cvps.begin(); it2 != tmp_cvps.end(); ++it2)
	{
		cv::Vec3b& intensity = tmp_image2.at<cv::Vec3b>(it2->y - 1, it2->x - 1);
		intensity[0] = 255;
		intensity[1] = 255;
		intensity[2] = 255;
	}

	cv::namedWindow("joint_mask2", 0);
	imshow("joint_mask2", tmp_image2);
	cv::waitKey();
}

void AddCathetus(CvPatchs& cvps, cv::Mat timage)
{
	for (auto it = cvps.begin(); it != cvps.end(); ++it)
	{
		CvPoints& cps = it->Outer2();

		if (!CheckCathetus(cps.front(), *(cps.begin() + 1)))
		{
			cv::Point p3 = cps.front();
			p3.x -= 1;
			cps.insert(cps.begin() + 1, p3);
		}

		for (int i = 1; i < cps.size() - 1; ++i)
		{
			if (!CheckCathetus(cps[i], cps[i + 1]))
			{
				if (CheckCathetus(cps[i - 1], cps[i]))
				{
					cv::Point p3 = GetCathetusPoint(cps[i - 1], cps[i]);
					cps.insert(cps.begin() + i + 1, p3);
				}
			}
		}

		// double check
		if (!CheckCathetus(cps.back(), cps.front()))
		{
			if (CheckCathetus(*(cps.end() - 2), cps.back()))
			{
				cv::Point p3 = GetCathetusPoint(*(cps.end() - 2), cps.back());
				cps.insert(cps.begin(), p3);
			}
		}

		if (!CheckCathetus(cps.front(), *(cps.begin() + 1)))
		{
			if (CheckCathetus(cps.back(), cps.front()))
			{
				cv::Point p3 = GetCathetusPoint(cps.back(), cps.front());
				cps.insert(cps.begin() + 1, p3);
			}
		}

		for (int i = 1; i < 4 && i < cps.size() - 1; ++i)
		{
			if (!CheckCathetus(cps[i], cps[i + 1]))
			{
				if (CheckCathetus(cps[i - 1], cps[i]))
				{
					cv::Point p3 = GetCathetusPoint(cps[i - 1], cps[i]);
					cps.insert(cps.begin() + i + 1, p3);
				}
			}
		}

		//DrawCvPoints(cps, timage);
	}
}

void DrawCvPatchs(CvPatchs& tmp_cvps, cv::Mat tmp_image2)
{
	tmp_image2 = cv::Scalar(0);

	for (auto it = tmp_cvps.begin(); it != tmp_cvps.end(); ++it)
	{
		for (auto it2 = it->Outer2().begin(); it2 != it->Outer2().end(); ++it2)
		{
			cv::Vec3b& intensity = tmp_image2.at<cv::Vec3b>(it2->y, it2->x);
			intensity[0] = 255;
			intensity[1] = 255;
			intensity[2] = 255;
		}
	}

	cv::namedWindow("DrawCvPatchs", 0);
	imshow("DrawCvPatchs", tmp_image2);
	cv::waitKey();
}


ImageSpline S3GetPatchs(const cv::Mat& image0, int dilation, int erosion)
{
	assert(image0.type() == CV_8UC3);
	cv::Mat img1u, cImg2;
	cImg2 = image0.clone();
	cvtColor(image0, img1u, CV_BGR2GRAY);
	cv::Mat srcImg1f, show3u = cv::Mat::zeros(img1u.size(), CV_8UC3);
	img1u.convertTo(srcImg1f, CV_32FC1, 1.0 / 255);
	CmCurveEx dEdge(srcImg1f);
	dEdge.CalSecDer(3, 0.01f, 0.2f);
	cv::Mat Der2 = dEdge.GetDer2();
	Dilation(Der2, 1, 1);
	cv::Mat mask, image, joint_mask, tmp_image, joint_image;
	image0.copyTo(image);
	joint_mask.create(image.rows * 2 + 1, image.cols * 2 + 1, CV_8UC1);
	tmp_image.create(image.rows + 2, image.cols + 2, CV_8UC3);
	joint_mask = cv::Scalar::all(0);
	mask.create(image.rows + 2, image.cols + 2, CV_8UC1);
	mask = cv::Scalar::all(0);
//  cv::namedWindow("gap", 0);
//  imshow("gap", Der2);
//  cv::waitKey();
//  normalize(Der2, Der2, 0, 1, cv::NORM_MINMAX);
//  cvThin(Der2.clone(), Der2, 10);
//  normalize(Der2, Der2, 0, 255, cv::NORM_MINMAX);
//  imshow("gap", Der2);
//  cv::waitKey();
	CvPatchs cvps;

	for (int i = 1; i < image.rows - 1; i++)
	{
		for (int j = 1; j < image.cols - 1; j++)
		{
			S2FloodFill(image, mask, Der2, 20, j, i, dilation, erosion);
		}
	}

	Erosion(mask, 1, 2);

	for (int i = 1; i < mask.rows - 1; i++)
	{
		for (int j = 1; j < mask.cols - 1; j++)
		{
			uchar& v = mask.at<uchar>(i, j);
			uchar& v2 = Der2.at<uchar>(i - 1, j - 1);

			if (v > 0)
			{
				v2 = 0;
			}
			else
			{
				v2 = 255;
			}
		}
	}

	image0.copyTo(image);
	mask = cv::Scalar::all(0);

	for (int i = 1; i < image.rows - 1; i++)
	{
		for (int j = 1; j < image.cols - 1; j++)
		{
			S2FloodFill(image, mask, Der2, 20, j, i, dilation, erosion);
		}
	}

	for (int i = 0; i < image.rows ; i++)
	{
		for (int j = 0; j < image.cols ; j++)
		{
			if (mask.at<uchar>(i + 1 , j + 1) == 0)
			{
				cv::Vec3b& v = image.at<cv::Vec3b>(i, j);
				v[0] = 0;
				v[1] = 0;
				v[2] = 0;
			}
		}
	}

	//imshow("Image2", image);
	//FixHole(image);
	//imshow("Image", image);
	mask = cv::Scalar::all(0);
	int cc = 1;

	for (int i = 1; i < image.rows - 1; i++)
	{
		for (int j = 1; j < image.cols - 1; j++)
		{
			S2FloodFill(cc, image, mask, Der2, 0, j, i, cvps, dilation, erosion);
		}
	}

	// create bigger image to fix border problem
	tmp_image = cv::Scalar::all(0);

	for (int i = 0; i < image.rows ; i++)
	{
		for (int j = 0; j < image.cols ; j++)
		{
			tmp_image.at<cv::Vec3b>(i + 1, j + 1) = image.at<cv::Vec3b>(i , j);
		}
	}

	cv::Mat gap_image;
	gap_image.create(joint_mask.rows + 2, joint_mask.cols + 2, CV_8UC1);
	gap_image = cv::Scalar(0);
	image = image0.clone();

	// Find Boundary
	for (int i = 0; i < joint_mask.rows ; i++)
	{
		for (int j = 0; j < joint_mask.cols ; j++)
		{
			int id2 = i % 2;
			int jd2 = j % 2;
			cv::Vec3b& v = tmp_image.at<cv::Vec3b>(i / 2, j / 2);

			if ((v[0] + v[1] + v[2]) == 0)
			{
				if (i > 0 && j > 0)
				{
					gap_image.at<uchar>(i - 1 , j - 1) = 255;
				}
			}

			if (id2 == 1 && jd2 == 1)
			{
				continue;
			}

			if (id2 == 0 && jd2 == 0)
			{
				cv::Vec3b& v1 = tmp_image.at<cv::Vec3b>(i / 2, j / 2);
				cv::Vec3b& v2 = tmp_image.at<cv::Vec3b>(i / 2, j / 2 + 1);
				cv::Vec3b& v3 = tmp_image.at<cv::Vec3b>(i / 2 + 1, j / 2);
				cv::Vec3b& v4 = tmp_image.at<cv::Vec3b>(i / 2 + 1, j / 2 + 1);

				if (v1 != v2)
				{
					joint_mask.at<uchar>(i , j) += 1;
				}

				if (v1 != v3)
				{
					joint_mask.at<uchar>(i , j) += 1;
				}

				if (v4 != v2)
				{
					joint_mask.at<uchar>(i , j) += 1;
				}

				if (v4 != v3)
				{
					joint_mask.at<uchar>(i , j) += 1;
				}
			}
			else if (id2 == 1)
			{
				cv::Vec3b& v1 = tmp_image.at<cv::Vec3b>(i / 2 + 1, j / 2);
				cv::Vec3b& v2 = tmp_image.at<cv::Vec3b>(i / 2 + 1, j / 2 + 1);

				if (v1 != v2)
				{
					joint_mask.at<uchar>(i , j) += 1;
				}
			}
			else if (jd2 == 1)
			{
				cv::Vec3b& v1 = tmp_image.at<cv::Vec3b>(i / 2 , j / 2 + 1);
				cv::Vec3b& v2 = tmp_image.at<cv::Vec3b>(i / 2 + 1, j / 2 + 1);

				if (v1 != v2)
				{
					joint_mask.at<uchar>(i , j) += 1;
				}
			}
		}
	}

	mask.create(joint_mask.rows + 2, joint_mask.cols + 2, CV_8UC1);
	mask = cv::Scalar::all(0);
	// create smaller image to fix border problem
	joint_image.create(joint_mask.rows , joint_mask.cols , CV_8UC3);
	joint_image = cv::Scalar::all(0);
	Der2 = mask.clone();

	// show joint
	for (int i = 0; i < joint_mask.rows ; i++)
	{
		for (int j = 0; j < joint_mask.cols ; j++)
		{
			if (joint_mask.at<uchar>(i, j) >= 3) // joint
			{
				joint_mask.at<uchar>(i, j) = 255;
				//printf("%d, %d\n", j, i);
			}
			else if (joint_mask.at<uchar>(i, j) > 0)
			{
				joint_mask.at<uchar>(i, j) = 60;
			}
		}
	}

//  // delete gap
//  gap_image = cv::Scalar::all(0);
	tmp_image = joint_image.clone();

	for (int i = 0; i < joint_image.rows ; i++)
	{
		for (int j = 0; j < joint_image.cols ; j++)
		{
			if (joint_mask.at<uchar>(i , j) == 255) // joint
			{
				cv::Vec3b& v1 = joint_image.at<cv::Vec3b>(i, j);
				v1[0] = 255;
				v1[1] = 255;
				v1[2] = 255;
				cv::Vec3b& v2 = tmp_image.at<cv::Vec3b>(i, j);
				v2[0] = 255;
				v2[1] = 255;
				v2[2] = 255;
				gap_image.at<uchar>(i, j) = 128;
			}
			else if (joint_mask.at<uchar>(i , j) == 60)
			{
				cv::Vec3b& v1 = joint_image.at<cv::Vec3b>(i, j);
				v1[0] = 255;
				v1[1] = 255;
				v1[2] = 255;
				cv::Vec3b& v2 = tmp_image.at<cv::Vec3b>(i, j);
				v2[0] = 60;
				v2[1] = 60;
				v2[2] = 60;
				gap_image.at<uchar>(i, j) = 128;
			}
		}
	}

	mask = cv::Scalar::all(0);
	cc = 1;

	for (int i = 0; i < tmp_image.rows; i++)
	{
		for (int j = 0; j < tmp_image.cols; j++)
		{
			LineFloodFill(tmp_image, mask, cc, j, i);
		}
	}

	Lines lines = GetAllLineFromLineImage(tmp_image);
	cc = 1;
	CvPatchs tmp_cvps;
	cv::namedWindow("gap_image", 0);
	imshow("gap_image", gap_image);
	cv::waitKey();
	// don't floodfill gap
	//gap_image = cv::Scalar::all(0);
	mask = cv::Scalar::all(0);

	for (int i = 0; i < joint_image.rows - 1; i++)
	{
		for (int j = 0; j < joint_image.cols - 1; j++)
		{
			S2FloodFill(cc, joint_image, mask, gap_image, 0, j, i, tmp_cvps, 1);
		}
	}

	cv::namedWindow("joint_image", 0);
	imshow("joint_image", joint_image);
	cv::waitKey();
	cv::Mat tmp_image2 = gap_image.clone();
	tmp_image2.create(gap_image.size(), CV_8UC3);
	tmp_image2 = cv::Scalar::all(0);
	AddCathetus(tmp_cvps, tmp_image2);

	for (auto it = tmp_cvps.begin(); it != tmp_cvps.end(); ++it)
	{
		for (auto it2 = it->Outer2().begin(); it2 != it->Outer2().end(); ++it2)
		{
			it2->x -= 1;
			it2->y -= 1;
		}
	}

	printf("\n");
	DrawCvPatchs(tmp_cvps, tmp_image2);
	tmp_image = cv::Scalar::all(0);
	int count = 1;
	gap_image = tmp_image.clone();

	for (auto it = lines.begin(); it != lines.end(); ++it)
	{
		int b = rand() % 255;
		int g = rand() % 255;
		int r = rand() % 255;

		for (auto it2 = it->begin(); it2 != it->end(); ++it2)
		{
			cv::Vec3b& intensity2 = tmp_image2.at<cv::Vec3b>(it2->y, it2->x);
			intensity2[0] = b;
			intensity2[1] = g;
			intensity2[2] = r;
			cv::Vec3b& intensity = tmp_image.at<cv::Vec3b>(it2->y, it2->x);
			intensity[0] = count % 255;
			intensity[1] = (count / 255) % 255;
			intensity[2] = count / (255 * 255);
		}

//      cv::namedWindow("joint_image", 0);
//      imshow("joint_image", tmp_image2);
//      cv::waitKey();
		count++;
	}

//  for (int i = 0; i < joint_mask.rows ; i++)
//  {
//      for (int j = 0; j < joint_mask.cols ; j++)
//      {
//          if (joint_mask.at<uchar>(i , j) == 255) // joint
//          {
//              cv::Vec3b& v1 = tmp_image.at<cv::Vec3b>(i, j);
//              v1[0] = 255;
//              v1[1] = 255;
//              v1[2] = 255;
//          }
//      }
//  }
//	PatchLines res = GetPatchSplines(cvps, image);
	ImageSpline  is = GetImageSpline(tmp_cvps, lines, tmp_image);
//  for (int i = 1; i < is.m_PatchSplines.size(); ++i)
//  {
//      visualize_patch_lines(is, i, gap_image);
//  }
//  for (int i = 0; i < res.size(); ++i)
//  {
//      is.AddPatchSpline(res[i]);
//  }
	//is.ComputeToLineFragments();
	is.ComputeToSplineFragments();
	return is;
}

ImageSpline GetImageSpline(CvPatchs& patchs, Lines& lines, cv::Mat lineImage)
{
	ImageSpline is;
	LineFragments lfs;
	PatchSplines pss;
	int last_lineidx;
	int patchid = 0;
	printf("patchssize: %d\n", patchs.size());
	cv::Mat vis = lineImage.clone();

	for (auto it = patchs.begin(); it != patchs.end(); ++it, ++patchid)
	{
		last_lineidx = -1;
		PatchSpline ps;
		vis = cv::Scalar(0);

		for (auto it2 = ++(it->Outer2().begin()); it2 != it->Outer2().end(); ++it2)
		{
			cv::Vec3b& intensity = lineImage.at<cv::Vec3b>(it2->y, it2->x);
			int hash = intensity[0] + intensity[1] * 255 + intensity[2] * 255 * 255;
			hash -= 1;

			if (hash != last_lineidx && hash >= 0)
			{
				Line& now_line = lines[hash];

				if (*(now_line.begin() + 1) == Vector2(it2->x, it2->y))
				{
					last_lineidx = hash;
					LineIndex li;
					li.m_id = hash;

					if (now_line.front() == Vector2((it2 - 1)->x, (it2 - 1)->y))
					{
						li.m_Forward = true;
					}
					else
					{
						li.m_Forward = false;
					}

					bool has_insert = false;

					for (int i = 0; i < ps.m_LineIndexs.size(); ++i)
					{
						if (li.m_id == ps.m_LineIndexs[i].m_id)
						{
							has_insert = true;
						}
					}

					if (!has_insert)
					{
						ps.m_LineIndexs.push_back(li);
					}
				}
				else if (* (now_line.end() - 2) == Vector2(it2->x, it2->y))
				{
					last_lineidx = hash;
					LineIndex li;
					li.m_id = hash;

					if (now_line.back() == Vector2((it2 - 1)->x, (it2 - 1)->y))
					{
						li.m_Forward = false;
					}
					else
					{
						li.m_Forward = true;
					}

					bool has_insert = false;

					for (int i = 0; i < ps.m_LineIndexs.size(); ++i)
					{
						if (li.m_id == ps.m_LineIndexs[i].m_id)
						{
							has_insert = true;
						}
					}

					if (!has_insert)
					{
						ps.m_LineIndexs.push_back(li);
					}
				}
			}
		}

		LineIndex li2 = ps.m_LineIndexs.back();
		LineIndex li1 = ps.m_LineIndexs.front();
		Vector2 start, end;

		if (li1.m_Forward)
		{
			start = lines[li1.m_id].front();
		}
		else
		{
			start = lines[li1.m_id].back();
		}

		if (li2.m_Forward)
		{
			end = lines[li2.m_id].back();
		}
		else
		{
			end = lines[li2.m_id].front();
		}

		assert(start == end);
		pss.push_back(ps);
	}

	for (auto it = lines.begin(); it != lines.end(); ++it)
	{
		LineFragment lf;
		lf.m_id = -1;
		lf.m_Points = *it;
		lfs.push_back(lf);
	}

	is.m_LineFragments = lfs;
	is.m_PatchSplines = pss;
	return is;
}

cv::Vec3b& GetColor(cv::Mat& image, int x, int y)
{
	if (y < 0) { y = 0; }

	if (y >= image.rows) { y = image.rows - 1; }

	if (x < 0) { x = 0; }

	if (x >= image.cols) { x = image.cols - 1; }

	return image.at<cv::Vec3b>(y, x);
}

void ClearEdge(cv::Mat& mask2)
{
	for (int i = 0; i < mask2.rows ; i++)
	{
		char& c = mask2.at<char>(i, 0);
		c = 0;
		char& c2 = mask2.at<char>(i, mask2.cols - 1);
		c2 = 0;
	}

	for (int j = 0; j < mask2.cols ; j++)
	{
		char& c = mask2.at<char>(0, j);
		c = 0;
		char& c2 = mask2.at<char>(mask2.rows - 1, j);
		c2 = 0;
	}
}

void S2FloodFill(int& cc, cv::Mat& image, cv::Mat& mask01, cv::Mat mask02,
                 int range, int x, int y, CvPatchs& out_array, int dilation, int erosion)
{
	if (mask01.at<uchar>(y + 1, x + 1) > 0
	        || mask02.at<uchar>(y , x) > 0
	   )
	{
		return;
	}

//  if (image.at<cv::Vec3b>(y, x)[0] == 255
//      || image.at<cv::Vec3b>(y, x)[1] == 255
//      || image.at<cv::Vec3b>(y, x)[2] == 255
//      )
//  {
//      return;
//  }
	cv::Point seed(x, y);
	cv::Rect ccomp;
	printf("cc %d\n", cc);
	int b = cc % 255;
	int g = cc / 255 ;
	int r = cc / (255 * 255);
	cc++;
	cv::Scalar newVal(b, g, r);
	int area;
	int lo = range;
	int up = range;
	threshold(mask01, mask01, 1, 128, CV_THRESH_BINARY);
	int flags = 4 + (255 << 8) + CV_FLOODFILL_FIXED_RANGE;
	area = floodFill(image, mask01, seed, newVal, &ccomp, cv::Scalar(lo, lo, lo),
	                 cv::Scalar(up, up, up), flags);
	// get Contour line
	cv::Mat mask2 = mask01.clone();
	ClearEdge(mask2);

	for (int i = 1; i < mask2.rows - 1; i++)
	{
		for (int j = 1; j < mask2.cols - 1; j++)
		{
			uchar& v = mask2.at<uchar>(i, j);

			if (v > 128)
			{
				v = 255;
			}
			else
			{
				v = 0;
			}
		}
	}

	CvPoints2d points;
	cv::findContours(mask2, points, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
	double tarea = cv::contourArea(points.front());
	//printf("num: %d\t area: %f\n", points.size(), tarea);

	if (dilation > 0)
	{
		Dilation(mask2, 1, dilation);
	}

	cv::Mat mask22 = mask2.clone();

	if (erosion > 0)
	{
		Erosion(mask2, 1, erosion);
	}

	CvPoints2d points2;
	cv::findContours(mask2, points2, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);

	for (int i = erosion - 1; i >= 0 && points2.empty(); --i)
	{
		mask2 = mask22.clone();

		if (i > 0)
		{
			Erosion(mask2, 2, i);
		}

		cv::findContours(mask2, points2, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
	}

	CvPatch cvp;
	cvp.Outer() = points.front();

	if (points.size() > 1)
	{
		std::copy(points.begin() + 1, points.end(), std::back_inserter(cvp.Inter()));
	}

	cvp.Outer2() = points2.front();

	if (points2.size() > 1)
	{
		std::copy(points2.begin() + 1, points2.end(), std::back_inserter(cvp.Inter2()));
	}

	out_array.push_back(cvp);
}

void S2FloodFill(cv::Mat& image, cv::Mat& mask01, cv::Mat mask02, int range,
                 int x, int y, int dilation/*=0*/, int erosion/*=0*/)
{
	if (mask01.at<uchar>(y + 1, x + 1) > 0
	        || mask02.at<uchar>(y , x) > 0
	   )
	{
		return;
	}

	cv::Point seed(x, y);
	cv::Rect ccomp;
	int b = rand() % 255;
	int g = rand() % 255;
	int r = rand() % 255;
	cv::Scalar newVal(b, g, r);
	int area;
	int lo = range;
	int up = range;
	threshold(mask01, mask01, 1, 128, CV_THRESH_BINARY);
	int flags = 4 + (255 << 8) + CV_FLOODFILL_FIXED_RANGE;
	area = floodFill(image, mask01, seed, newVal, &ccomp, cv::Scalar(lo, lo, lo),
	                 cv::Scalar(up, up, up), flags);
	cv::Mat mask2 = mask01.clone();
	ClearEdge(mask2);

	for (int i = 1; i < mask2.rows - 1; i++)
	{
		for (int j = 1; j < mask2.cols - 1; j++)
		{
			uchar& v = mask2.at<uchar>(i, j);

			if (v > 128)
			{
				v = 255;
			}
			else
			{
				v = 0;
			}
		}
	}

	Dilation(mask2, 1, 1);
	CvPoints2d points;
	cv::findContours(mask2, points, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
	double tarea = cv::contourArea(points.front());

	if (points.empty() || tarea < 5) { return; }

	if (dilation > 0)
	{
		Dilation(mask2, 1, dilation);
	}

	cv::Mat mask22 = mask2.clone();

	if (erosion > 0)
	{
		Erosion(mask2, 1, erosion);
	}

	CvPoints2d points2;
	cv::findContours(mask2, points2, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);

	for (int i = erosion - 1; i >= 0 && points2.empty(); --i)
	{
		mask2 = mask22.clone();

		if (i > 0)
		{
			Erosion(mask2, 2, i);
		}

		cv::findContours(mask2, points2, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
	}
}


void LineFloodFill(cv::Mat& image, cv::Mat& mask01, int& cc, int x, int y)
{
	if (image.at<cv::Vec3b>(y, x)[0] != 60
	        || image.at<cv::Vec3b>(y, x)[1] != 60
	        || image.at<cv::Vec3b>(y, x)[2] != 60
	   )
	{
		return;
	}

//  int b = cc % 255;
//  int g = cc / 255 ;
//  int r = cc / (255 * 255);
	cc++;
	int b = rand() % 255;
	int g = rand() % 255;
	int r = rand() % 255;
	cv::Point seed(x, y);
	cv::Rect ccomp;
	cv::Scalar newVal(b, g, r);
	int area;
	int lo = 0;
	int up = 0;
	threshold(mask01, mask01, 1, 128, CV_THRESH_BINARY);
	int flags = 4 + (255 << 8) + CV_FLOODFILL_FIXED_RANGE;
	area = floodFill(image, mask01, seed, newVal, &ccomp, cv::Scalar(lo, lo, lo),
	                 cv::Scalar(up, up, up), flags);
}

struct ColorPtr
{
	ColorPtr() {}
	ColorPtr(cv::Vec3b* _c, int _x, int _y)
		: c(_c), x(_x), y(_y)
	{}
	cv::Vec3b* c;
	int x, y;
};
typedef std::vector<ColorPtr> Vec3bptrs;
typedef std::vector<Vec3bptrs> Vec3bptrs2d;

void GetMatrix(int w, int h, Vec3bptrs& ary, int x, int y, cv::Mat& img)
{
	const int xend = x + w / 2;
	const int yend = y + h / 2;

	for (int i = x - w / 2, i2 = 0; i <= xend; ++i, ++i2)
	{
		for (int j = y - h / 2, j2 = 0; j <= yend; ++j, ++j2)
		{
			int ix = abs(i);
			int jy = abs(j);

			if (ix >= img.cols - 1)
			{
				ix -= ix - (img.cols - 1) + 1;
			}

			if (jy >= img.rows - 1)
			{
				jy -= jy - (img.rows - 1) + 1;
			}

			cv::Vec3b& v = img.at<cv::Vec3b>(jy, ix);
			ary[j2 * w + i2].c = &v;
			ary[j2 * w + i2].x = ix;
			ary[j2 * w + i2].y = jy;
		}
	}
}

PatchLines GetPatchSplines(CvPatchs& patchs, cv::Mat& patchImage)
{
	PatchLines res;
	Vec3bptrs ary(9);

	for (int i = 0; i < patchs.size(); ++i)
	{
		CvPatch& patch = patchs[i];
		CvPoints& cps = patch.Outer();
		cv::Vec3b& color = patchImage.at<cv::Vec3b>(cps[0].y - 1, cps[0].x - 1);
		int last_near = color[0] + color[1] * 255 + color[2] * 255 * 255;
		PatchLine ps;
		ps.m_id = i;
		LineFragment lf;
		lf.m_id = i;
		lf.m_id_near = last_near;

		for (int j = 0; j < cps.size(); ++j)
		{
			GetMatrix(3, 3, ary, cps[j].x - 1, cps[j].y - 1, patchImage);
			int chash;
			bool has_old_near = false;

			// check old Neighbor
			for (int q = 1; q < 9; q++)
			{
				cv::Vec3b& v = *ary[q].c;
				chash = v[0] + v[1] * 255 + v[2] * 255 * 255;

				if (chash == last_near && last_near != i)
				{
					has_old_near = true;
				}
			}

			// find new Neighbor
			if (!has_old_near)
			{
				if (cps[j].x - 1 == 0 || cps[j].y - 1 == 0 || cps[j].x == patchImage.cols
				        || cps[j].y == patchImage.rows)
				{
					if (last_near != i)
					{
						lf.AddCvPoint(cps[j]);
						ps.m_LineFragments.push_back(lf);
						lf.m_Points.clear();
						last_near = chash;
						lf.m_id_near = chash;
						lf.AddCvPoint(cps[j]);
					}
					else
					{
						lf.AddCvPoint(cps[j]);
					}
				}
				else
				{
					for (int q = 1; q < 9; q++)
					{
						cv::Vec3b& v = *ary[q].c;
						chash = v[0] + v[1] * 255 + v[2] * 255 * 255;

						if (chash == i) { chash = last_near; }

						if (chash != last_near)
						{
							if (!lf.m_Points.empty())
							{
								lf.AddCvPoint(cps[j]);
								ps.m_LineFragments.push_back(lf);
							}

							lf.m_Points.clear();
							last_near = chash;
							lf.m_id_near = chash;
							lf.AddCvPoint(cps[j]);
							break;
						}
					}
				}
			}
			else
			{
				lf.AddCvPoint(cps[j]);
			}
		}

		if (!ps.m_LineFragments.empty()
		        && lf.m_id_near == ps.m_LineFragments.front().m_id_near)
		{
			std::copy(ps.m_LineFragments.front().m_Points.begin(),
			          ps.m_LineFragments.front().m_Points.end(), std::back_inserter(lf.m_Points));
			ps.m_LineFragments.front().m_Points = lf.m_Points;
		}
		else
		{
			ps.m_LineFragments.push_back(lf);
		}

		res.push_back(ps);
		printf("%2d %3d\n", i, res.back().m_LineFragments.size());
	}

	return res;
}

Lines GetAllLineFromLineImage(cv::Mat& image)
{
	Lines res;
	cv::Mat tImage = image;
	//cv::namedWindow("tmp_image", 0);

	for (int i = 0; i < tImage.rows; ++i)
	{
		for (int j = 0; j < tImage.cols; ++j)
		{
			cv::Vec3b& intensity = tImage.at<cv::Vec3b>(i, j);

			if ((intensity[0] != 0 || intensity[1] != 0 || intensity[2] != 0)
			        && (intensity[0] != 255 && intensity[1] != 255 && intensity[2] != 255))
			{
				int cc = intensity[0] + intensity[1] * 255 + intensity[2] * 255 * 255;
				Line line;
				line.push_back(Vector2(j, i));
				intensity[0] = 0;
				intensity[1] = 0;
				intensity[2] = 0;
				EdgeLink_LineFragment(tImage, line, cc);
				res.push_back(line);
			}
		}
	}

	return res;
}


void FixHole(cv::Mat& patchImage)
{
	Vec3bptrs ary(9);

	for (int count = 0; count < 2; ++count)
	{
		int y = 0;
		y = count % 2;

		for (; y < patchImage.rows; y += 2)
		{
			for (int x = 0; x < patchImage.cols; x += 2)
			{
				cv::Vec3b& color = patchImage.at<cv::Vec3b>(y, x);

				if (color[0] == 0 && color[1] == 0 && color[2] == 0)
				{
					GetMatrix(3, 3, ary, x, y, patchImage);

					for (int i = 0; i < 9; i += 1)
					{
						cv::Vec3b& v = *ary[i].c;
						int sum = v[0] + v[1] + v[2];

						if (sum > 0)
						{
							color = v;
							break;
						}
					}
				}
			}

			for (int x = 1; x < patchImage.cols; x += 2)
			{
				cv::Vec3b& color = patchImage.at<cv::Vec3b>(y, x);

				if (color[0] == 0 && color[1] == 0 && color[2] == 0)
				{
					GetMatrix(3, 3, ary, x, y, patchImage);

					for (int i = 1; i < 9; i += 2)
					{
						cv::Vec3b& v = *ary[i].c;
						int sum = v[0] + v[1] + v[2];

						if (sum > 0)
						{
							color = v;
							break;
						}
					}
				}
			}
		}
	}

//  for (int y =0; y < patchImage.rows; y++)
//  {
//          for (int x = 0; x < patchImage.cols; x++)
//          {
//              cv::Vec3b& color = patchImage.at<cv::Vec3b>(y, x);
//
//              if (color[0] == 0 && color[1] == 0 && color[2] == 0)
//              {
//                  GetMatrix(3, 3, ary, x, y, patchImage);
//
//                  for (int i = 0; i < 9; i += 1)
//                  {
//                      cv::Vec3b& v = *ary[i].c;
//                      int sum = v[0] + v[1] + v[2];
//
//                      if (sum > 0)
//                      {
//                          color = v;
//                          break;
//                      }
//                  }
//              }
//          }
//  }
}

void cvThin(cv::Mat& src, cv::Mat& dst, int iterations /*= 1*/)
{
	cv::Size size(src.cols, src.rows);
	dst = src.clone();
	int n = 0, i = 0, j = 0;

	for (n = 0; n < iterations; n++)
	{
		cv::Mat t_image = dst.clone();

		for (i = 0; i < size.height;  i++)
		{
			for (j = 0; j < size.width; j++)
			{
				if (t_image.at<uchar>(i, j) == 1)
				{
					int ap = 0;
					int p2 = (i == 0) ? 0 : t_image.at<uchar>(i - 1, j);
					int p3 = (i == 0 || j == size.width - 1) ? 0 : t_image.at<uchar>(i - 1, j + 1);

					if (p2 == 0 && p3 == 1)
					{
						ap++;
					}

					int p4 = (j == size.width - 1) ? 0 : t_image.at<uchar>(i, j + 1);

					if (p3 == 0 && p4 == 1)
					{
						ap++;
					}

					int p5 = (i == size.height - 1
					          || j == size.width - 1) ? 0 : t_image.at<uchar>(i + 1, j + 1);

					if (p4 == 0 && p5 == 1)
					{
						ap++;
					}

					int p6 = (i == size.height - 1) ? 0 : t_image.at<uchar>(i + 1, j);

					if (p5 == 0 && p6 == 1)
					{
						ap++;
					}

					int p7 = (i == size.height - 1 || j == 0) ? 0 : t_image.at<uchar>(i + 1, j - 1);

					if (p6 == 0 && p7 == 1)
					{
						ap++;
					}

					int p8 = (j == 0) ? 0 : t_image.at<uchar>(i, j - 1);

					if (p7 == 0 && p8 == 1)
					{
						ap++;
					}

					int p9 = (i == 0 || j == 0) ? 0 : t_image.at<uchar>(i - 1, j - 1);

					if (p8 == 0 && p9 == 1)
					{
						ap++;
					}

					if (p9 == 0 && p2 == 1)
					{
						ap++;
					}

					if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) > 1
					        && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) < 7)
					{
						if (ap == 1)
						{
							if (!(p2 && p4 && p6))
							{
								if (!(p4 && p6 && p8))
								{
									dst.at<uchar>(i, j) = 0;
								}
							}
						}
					}
				}
			}
		}

		t_image = dst.clone();

		for (i = 0; i < size.height;  i++)
		{
			for (int j = 0; j < size.width; j++)
			{
				if (t_image.at<uchar>(i, j) == 1)
				{
					int ap = 0;
					int p2 = (i == 0) ? 0 : t_image.at<uchar>(i - 1, j);
					int p3 = (i == 0 || j == size.width - 1) ? 0 : t_image.at<uchar>(i - 1, j + 1);

					if (p2 == 0 && p3 == 1)
					{
						ap++;
					}

					int p4 = (j == size.width - 1) ? 0 : t_image.at<uchar>(i, j + 1);

					if (p3 == 0 && p4 == 1)
					{
						ap++;
					}

					int p5 = (i == size.height - 1
					          || j == size.width - 1) ? 0 : t_image.at<uchar>(i + 1, j + 1);

					if (p4 == 0 && p5 == 1)
					{
						ap++;
					}

					int p6 = (i == size.height - 1) ? 0 : t_image.at<uchar>(i + 1, j);

					if (p5 == 0 && p6 == 1)
					{
						ap++;
					}

					int p7 = (i == size.height - 1 || j == 0) ? 0 : t_image.at<uchar>(i + 1, j - 1);

					if (p6 == 0 && p7 == 1)
					{
						ap++;
					}

					int p8 = (j == 0) ? 0 : t_image.at<uchar>(i, j - 1);

					if (p7 == 0 && p8 == 1)
					{
						ap++;
					}

					int p9 = (i == 0 || j == 0) ? 0 : t_image.at<uchar>(i - 1, j - 1);

					if (p8 == 0 && p9 == 1)
					{
						ap++;
					}

					if (p9 == 0 && p2 == 1)
					{
						ap++;
					}

					if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) > 1
					        && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) < 7)
					{
						if (ap == 1)
						{
							if (p2 * p4 * p8 == 0)
							{
								if (p2 * p6 * p8 == 0)
								{
									dst.at<uchar>(i, j) = 0;
								}
							}
						}
					}
				}
			}
		}
	}
}

