#pragma once

#include <vector>
#include "opencv.hpp"

using namespace cv;

typedef unsigned __int8		byte;
typedef unsigned __int16	word;
typedef unsigned __int32	dword;
typedef unsigned __int64	qword;

typedef std::vector<Mat>	vec_mat_t;

#define DllExport __declspec(dllexport)