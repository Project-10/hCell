// Marker Class 
// Written by Sergey Kosov in 2013 for Project X
#pragma once

#include "types.h"

namespace HexagonCells
{

	// ================================ Marker Class ================================
	/**
	@brief Marker class
	@details This class allows to visualize the hexagonical cells
	@author Sergey G. Kosov, sergey.kosov@project-10.de
	*/
	class CMarker
	{
	public:
		DllExport CMarker(void) {}
		DllExport ~CMarker(void) {}

		/**
		@brief Draws the hexagonical grid on the image.
		@param[in,out] img The image
		@param[in] R Hexagon outer radius
		@param[in] color Grid color
		*/
		DllExport void markGrid(Mat &img, double R, CvScalar color);

		/**
		@brief Draws a single filled hexagon
		@param[in,out] img The 3-channel RGB color image
		@param[in] R Hexagon outer radius
		@param[in] idx The hexagon index
		@param[in] color Cell color
		*/
		DllExport void markHexagon(Mat &img, double R, int idx, CvScalar color);
	};
}