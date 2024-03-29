// stdafx.cpp : source file that includes just the standard includes
// CurveExtraction.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

cv::Point static const DIRECTION4[4] =
{
	cv::Point(1,  0), //Direction 0
	cv::Point(0,  1), //Direction 1
	cv::Point(-1, 0), //Direction 2
	cv::Point(0, -1), //Direction 3
};  //format: {dx, dy}

cv::Point static const DIRECTION8[8] =
{
	cv::Point(1,  0), //Direction 0
	cv::Point(1,  1), //Direction 1
	cv::Point(0,  1), //Direction 2
	cv::Point(-1, 1), //Direction 3
	cv::Point(-1, 0), //Direction 4
	cv::Point(-1, -1), //Direction 5
	cv::Point(0, -1), //Direction 6
	cv::Point(1, -1)  //Direction 7
};  //format: {dx, dy}

cv::Point static const DIRECTION16[16] =
{
	cv::Point(2,  0), //Direction 0
	cv::Point(2,  1), //Direction 1
	cv::Point(2,  2), //Direction 2
	cv::Point(1,  2), //Direction 3
	cv::Point(0,  2), //Direction 4
	cv::Point(-1, 2), //Direction 5
	cv::Point(-2, 2), //Direction 6
	cv::Point(-2, 1), //Direction 7
	cv::Point(-2, 0), //Direction 8
	cv::Point(-2, -1), //Direction 9
	cv::Point(-2, -2), //Direction 10
	cv::Point(-1, -2), //Direction 11
	cv::Point(0, -2), //Direction 12
	cv::Point(1, -2), //Direction 13
	cv::Point(2, -2), //Direction 14
	cv::Point(2, -1)  //Direction 15
}; //format: {dx, dy}

float static const DRT_ANGLE[8] =
{
	0.000000f,
	0.785398f,
	1.570796f,
	2.356194f,
	3.141593f,
	3.926991f,
	4.712389f,
	5.497787f
};

float static const PI_FLOAT = 3.1415926535897932384626433832795f;
float static const PI2 = PI_FLOAT * 2.0f;
float static const PI_HALF = PI_FLOAT * 0.5f;
