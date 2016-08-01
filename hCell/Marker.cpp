#include "Marker.h"
#include "Cell.h"

namespace HexagonCells
{
	void CMarker::markGrid(Mat &img, double R, CvScalar color)
	{
		int x, y;
		double X, Y;
		double X0;

		double r = sqrtf(3) * 0.5 * R;
		double dx = 2 * r;
		double dy = 1.5 * R;
		double S = dx * dy;

		//printf("S = %.2f (pixels);\n", S);

		if (img.channels() == 1) cvtColor(img, img, CV_GRAY2RGB);

		X = 0; Y = 0;
		for (y = 0; Y < img.rows; y++) {
			if (y % 2 == 0)	X0 = r;
			else			X0 = 0;
			X = 0;
			for (x = 0; X < img.cols; x++) {
				X = X0 + x * dx;
				Y = R / 2 + y * dy;
				//circle(img, cvPoint(static_cast<int>(X), static_cast<int>(Y)), 1, color);

				line(img, Point2d(X, Y - R), Point2d(X + r, Y - (R / 2)), color, 1, CV_AA);
				line(img, Point2d(X + r, Y - (R / 2)), Point2d(X + r, Y + (R / 2)), color, 1, CV_AA);
				line(img, Point2d(X, Y + R), Point2d(X + r, Y + (R / 2)), color, 1, CV_AA);
			} // x
		} // y
		rectangle(img, Point(0, 0), Point(img.cols - 1, img.rows - 1), color, 1);
	}

	void CMarker::markHexagon(Mat &img, double R, int idx, CvScalar color)
	{
		// Assertions
		//if (img.channels() != 3) return;

		// cell coordinates in image
		CvPoint2D64f C = CCell::idx2d(idx, R, img.size());
		Point ic[6];
		for (int i = 0; i < 6; i++) {
			CvPoint2D64f c = CCell::getBoundaryPoint(C, i, R);
			ic[i].x = static_cast<int>(0.5 + c.x);
			ic[i].y = static_cast<int>(0.5 + c.y);
		}

		const Point * countours[1] = { &ic[0] };
		int npt[] = { 6 };
		fillPoly(img, countours, npt, 1, color);
	}
}