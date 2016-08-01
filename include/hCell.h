#pragma once

#include "../hCell/Cell.h"
#include "../hCell/Marker.h"

/**
@mainpage Introduction
@section sec_main Hexagon Cells (hCell)
is a C++ dynamic link library, which allows representing raster images with hexagonical paches. In contrast to the state-of-the-art quadratic representation,
it does not suffer from washer-shaped objects paradox, described in A. Rosenfeld, "Connectivity in Digital Pictures", Journal of the ACM, Vol 17, pp 146-160, 1970.
An additional disadvantage of the square pixel is that the 8 neighbors around the center pixel are not equidistant, which causes the accuracy for the diagonal 
and off diagonal directions to be reduced in their magnitude.

The hexagonocal representation is definied by the hexagon outer radius \f$ R \f$ and must be larger than 1 pixel. The inner hexagon radius is calculated
as follows: \f$ r = \frac{\sqrt{3}}{2}R \f$. Thus the hexagon area \f$S = 3rR =  \frac{3\sqrt{3}}{2}R^2\f$. For additional information please refere to 
<a href="http://en.wikipedia.org/wiki/Regular_hexagon#Regular_hexagon">Regular hexagon</a>.

The library consists of the following classes:
- Cell generation and neighbourhood definition @ref HexagonCells::CCell
- Visualization @ref HexagonCells::CMarker


@section s3 Installation
@subsection s3_1 Installing OpenCV
This library is based on OpenCV library v.3.1.0. In order to use the DGM library, the OpenCV library should be also installed.
-# Download the OpenCV library from <a href="http://sourceforge.net/projects/opencvlibrary/files/opencv-win/3.1.0/" target="_blank">sourcefourge</a>
-# Install the OpenCV library. You may follow the <a href="http://www.project-10.de/forum/viewtopic.php?f=23&t=198#p237" target="_blank">installation guide</a>

@subsection s3_2 Installing hCell
-# Download the DGM library from <a href="http://research.project-10.de/hcell/">Project X Research</a>
-# Unzip it to your local folder (for example to disk @b C:\\, so the library path will be @b C:\\hCell\\)
-# In case you want to rebuild the library from the "Win32" / "x64" packages or you use the "Source" package follow these instructions, otherwise - skip this step
	-# Configure the paths in the hCell Visual Studio solution to match your installed OpenCV paths
	-# Perform Build -> Batch Build
	-# If you want to run the demo applications, you may need to copy OpenCV dll files to the @b C:\\hCell\\bin\\Release and/or @b C:\\hCell\\bin\\Debug folders
-# Specify the following paths and library
	-# Add to Configuration Properties -> C/C++ -> General -> Additional Include Directories the path @b C:\\hCell\\include\\
	-# Add to Configuration Properties -> Linker -> General -> Additional Library Directories the path @b C:\\hCell\\lib\\Release\\ and @b C:\\hCell\\lib\\Debug\\ for Release and Debug configurations accordingly
	-# Add to Configuration Properties -> Linker -> Input -> Additional Dependencies the libraries @b hCell112.lib and @b hCell112d.lib for Release and Debug configurations accordingly
-# Copy the DGM dll files @b hCell112.dlll from @b C:\\hCell\\bin\\Release and @b hCell112d.dll from @b C:\\hCell\\bin\\Debug to your project's Relese and Debug folders.

@section s4 How to use the code
The documentation for hCell consists of one demo, introducing the basic functionality of the library:
- @ref demo : An introduction to hCell library.

@author Sergey G. Kosov, sergey.kosov@project-10.de

@page demo Demo Code
In this demo, we show a very simple example of using our library: a test image will be pixelized with hexagonical patches. First, an image is opened and class instanses 
@ref HexagonCells::CCell and @ref HexagonCells::CMarker are created and initialized. Then, after the number of cells in image is known, we draw hexagons upon the image and show it. After a keypress, 
the application exits.
@code
#include "hCell.h"

using namespace HexagonCells;

int main () 
{
	// CCell class is resposible for calcilating average color values
	// within each hexagon, mapped on the input image
	CCell	cell;

	// CMarker class is responsible for drawing solid and wireframe
	// hexagons on given images
	CMarker marker;

	// We chose the hexagon area S to be equal to 25 pixels,
	// thus the hexagon radius R is calculated as R = 0.6204 * sqrt(S)
	const double R = 3.102;
	cell.setRadius(R);

	// Load and set the test image
	Mat img = imread("test_image.jpg", 1);
	cell.setImage(img);
	
	// Achieving the number of hexagons, corresponding to the input image
	cell_params params = cell.getInfo();
	
	// Drawing solid hexagons on the same input image
	for (register int n = 0; n < params.N; n++)
		marker.markHexagon(img, R, n, cell.getVal(n)); 

	// Optinally mark the drawn hexagons with a grid of custom color
	marker.markGrid(img, R, CV_RGB(0, 128, 64));

	// Show the result
	imshow("Demo", img);
	cvWaitKey();
	return 0;
}
@endcode

Additionally, if you use the version of the library with pre-build binaries, you can drag-and-drop an image to the Demo.exe application. 
The pixellized with hexagons version of the image will be created in the same directory with suffix "_hex" in the file name.
*/