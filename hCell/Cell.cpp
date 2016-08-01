#include "Cell.h"
#include "macroses.h"
#include "core\core.hpp"

namespace HexagonCells
{
	// Default Constructor
	CCell::CCell(void) : m_img(Mat()), m_imgSize(cvSize(0, 0)), m_LUT(Mat()), m_R(-1.0), m_r(-1.0), m_nCells(-1), m_cellIntApp(CELL_AVG), m_cellData(Mat())
	{
	}

	// Constructor
	CCell::CCell(CvSize imgSize, cell_int_app cellIntApp) : m_img(Mat()), m_imgSize(imgSize), m_LUT(Mat()), m_R(-1.0), m_r(-1.0), m_nCells(-1), m_cellIntApp(cellIntApp), m_cellData(Mat())
	{
	}

	// Constructor
	CCell::CCell(Mat &img, cell_int_app cellIntApp) : m_LUT(Mat()), m_R(-1.0), m_r(-1.0), m_nCells(-1), m_cellIntApp(cellIntApp), m_cellData(Mat())
	{
		img.copyTo(m_img);
		m_imgSize = img.size();
	}

	// Constructor
	CCell::CCell(double R) : m_img(Mat()), m_imgSize(cvSize(0, 0)), m_LUT(Mat()), m_R(R), m_r(0.5*sqrt(3.0)*R), m_nCells(-1), m_cellIntApp(CELL_AVG), m_cellData(Mat())
	{
	}

	// Constructor
	CCell::CCell(CvSize imgSize, double R, cell_int_app cellIntApp) : m_img(Mat()), m_imgSize(imgSize), m_LUT(Mat()), m_R(R), m_r(0.5*sqrt(3.0)*R), m_nCells(-1), m_cellIntApp(cellIntApp), m_cellData(Mat())
	{
	}

	// Constructor
	CCell::CCell(Mat &img, double R, cell_int_app cellIntApp) : m_LUT(Mat()), m_R(R), m_r(0.5*sqrt(3.0)*R), m_nCells(-1), m_cellIntApp(cellIntApp), m_cellData(Mat())
	{
		img.copyTo(m_img);
		m_imgSize = img.size();
	}

	// Destructor
	CCell::~CCell(void)
	{
		if (!m_img.empty()) m_img.release();
		if (!m_LUT.empty()) m_LUT.release();
		if (!m_cellData.empty()) m_cellData.release();
	}

	void CCell::clear(void)
	{
		if (!m_img.empty()) m_img.release();
		m_imgSize = cvSize(0, 0);
		if (!m_LUT.empty()) m_LUT.release();
		m_R = -1.0;
		m_r = -1.0;
		m_cellIntApp = CELL_AVG;
		if (!m_cellData.empty()) m_cellData.release();
	}

	void CCell::setImage(Mat &img)
	{
		// Assertions
		HCELL_ASSERT_MSG(!img.empty(), "The image is not set");

		if (!m_img.empty()) m_img.release();
		img.copyTo(m_img);

		if ((m_imgSize.width != img.size().width) || (m_imgSize.height != img.size().height)) {		// if new size
			if (!m_LUT.empty()) m_LUT.release();													// release LUT
			m_nCells = -1;																			// reset nCells;
		}
		m_imgSize = img.size();
		if (!m_cellData.empty()) m_cellData.release();
	}

	void CCell::setRadius(double R)
	{
		// Assertions
		HCELL_ASSERT_MSG(R >= MIN_RADIUS, "The cell radius is not set or has a wrong value");
		if (R == m_R) return;

		if (!m_LUT.empty()) m_LUT.release();		// release LUT
		m_R = R;
		m_r = 0.5 * sqrt(3.0) * R;
		m_nCells = -1;
		if (!m_cellData.empty()) m_cellData.release();
	}

	void CCell::setInterpolationApproach(cell_int_app cellIntApp)
	{
		m_cellIntApp = cellIntApp;
		if (!m_cellData.empty()) m_cellData.release();
	}

	cell_params CCell::getInfo(void)
	{
		cell_params res;
		res.R = m_R;
		res.r = m_r;
		res.S = m_R;
		if (res.S >= 0) res.S *= 3 * m_r;
		if (m_nCells < 0) calculate_nCells();
		res.N = m_nCells;
		return res;
	}

	int	CCell::getIDX(int x, int y)
	{
		return d2idx(cvPoint2D64f(x, y));
	}

	int CCell::getNeighbourIDX(int idx, int i)
	{
		if (m_nCells < 0) calculate_nCells();

		double	dx = 2.0 * m_r;
		double	imgWidth = static_cast<double>(m_imgSize.width);
		int		width0 = static_cast<int> (0.99 + imgWidth / dx);
		int		width1 = 1 + static_cast<int> (0.99 + (imgWidth - m_r) / dx);

		Point c = idx2h(idx, m_R, m_imgSize);

		int d = 0;
		if ((c.y % 2) == 0) d = 1;

		int res = 0;
		switch (i) {
		case 0: c.x += 1;            break;
		case 1: c.x += d;    c.y += 1; break;
		case 2: c.x += d - 1;  c.y += 1; break;
		case 3: c.x -= 1;		       break;
		case 4: c.x += d - 1;  c.y -= 1; break;
		case 5: c.x += d;    c.y -= 1; break;
		default: res = -1;		   break;
		}
		if ((c.x < 0) || (c.y < 0)) res = -1;
		if (((c.y % 2) == 0) && (c.x >= width0)) res = -1;
		if (((c.y % 2) != 0) && (c.x >= width1)) res = -1;
		if (res == 0) {
			res = h2idx(c);
			if (res >= m_nCells) res = -1;
		}
		return res;
	}

	int * CCell::getNeighbourhood(int idx)
	{
		int *res = new int[6];
		for (int i = 0; i < 6; i++) res[i] = getNeighbourIDX(idx, i);
		return res;
	}

	CvScalar CCell::getVal(int idx)
	{
		if (m_cellData.empty()) calculate_cellData();

		CvScalar res;

		int C = m_img.channels();
		for (int c = 0; c < C; c++) res.val[c] = m_cellData.at<double>(0, C * idx + c);

		return res;
	}

	// =================== Auxilary functions ==================
	bool ifInsideTriangle(CvPoint2D64f x, CvPoint2D64f a, CvPoint2D64f b, CvPoint2D64f c)
	{
		double r1, r2, r3;
		r1 = (a.x - x.x) * (b.y - a.y) - (b.x - a.x) * (a.y - x.y);
		r2 = (b.x - x.x) * (c.y - b.y) - (c.x - b.x) * (b.y - x.y);
		r3 = (c.x - x.x) * (a.y - c.y) - (a.x - c.x) * (c.y - x.y);
		if (SIGN(r1) == SIGN(r2) == SIGN(r3)) return true;
		return false;
	}

	// =================== Private functions ===================
	int CCell::calculate_LUT(void)
	{
		// Assertions
		HCELL_ASSERT_MSG((m_imgSize.height != 0) && (m_imgSize.width != 0), "The image size is not set");

		if (!m_LUT.empty()) m_LUT.release();
		m_LUT.create(m_imgSize, CV_32SC1);

		double	dx = 2.0 * m_r;			// x - distance between cells
		double	dy = 1.5 * m_R;			// y - distance between cells

		for (register int y = 0; y < m_LUT.rows; y++) {
			dword *pLUT = m_LUT.ptr<dword>(y);
			for (register int x = 0; x < m_LUT.cols; x++) {
				CvPoint2D64f	X = cvPoint2D64f(x, y);					// X - point in image cartesian <d> coordinate
				Point			c = d2h(X);								// cell indexes in hexagonical <h> coordinate system 

				// cell center coordinates in image
				CvPoint2D64f C;
				C.y = c.y * dy + 0.5 * m_R;
				C.x = c.x * dx + m_r;

				if ((c.y % 2) == 1) X.x += m_r;
				bool inside = ifInsideCell(X, C);
				if (!inside) {
					if ((c.y % 2) == 0) {
						if (X.x >= C.x) c.x++;
					}
					else {
						if (X.x < C.x) c.x--;
					}
					c.y--;
				}
				pLUT[x] = h2idx(c);
			} // x
		} // y
		return 0;
	}

	int CCell::calculate_nCells(void)
	{
		// Assertions
		HCELL_ASSERT_MSG(((m_R >= 0) && (m_r >= 0)), "The cell radius is not set or has a wrong value");

		int res;
		if (m_LUT.empty())	if ((res = calculate_LUT()) < 0)	return res;

		double maxVal;
		minMaxLoc(m_LUT, NULL, &maxVal, NULL, NULL);
		m_nCells = static_cast<int>(maxVal) + 1;

		return 0;
	}

	int CCell::calculate_cellData(void)
	{
		// Assertions
		HCELL_ASSERT_MSG(!m_img.empty(), "The image is not set");

		int				C = m_img.channels();

		if (m_nCells < 0) calculate_nCells();
		if (m_cellData.empty()) {
			m_cellData.create(1, m_nCells, CV_MAKE_TYPE(CV_64F, C));
			m_cellData.setTo(0);
		}

		double	* pData = m_cellData.ptr<double>(0);
		int		* pNumVal = NULL;
		int		* pMaxVal = NULL;
		int		  len = m_nCells;

		if (m_cellIntApp == CELL_MV) len *= 256;
		pNumVal = new int[len];
		if (m_cellIntApp == CELL_MV)  pMaxVal = new int[m_nCells];

		for (int c = 0; c < C; c++) {
			memset(pNumVal, 0, len * sizeof(int));
			if (pMaxVal != NULL) memset(pMaxVal, 0, m_nCells * sizeof(int));
			for (register int y = 0; y < m_img.rows; y++) {
				byte *pImg = m_img.ptr<byte>(y);
				for (register int x = 0; x < m_img.cols; x++) {
					int id = d2idx(cvPoint2D64f(x, y));
					byte val = pImg[C*x + c];

					if (pMaxVal != NULL) {
						pNumVal[256 * id + val]++;
						if (pNumVal[256 * id + val] > pMaxVal[id]) {
							pMaxVal[id]++;
							pData[C*id + c] = val;
						}
					}
					else {
						pData[C*id + c] = (pNumVal[id] * pData[C*id + c] + val) / (pNumVal[id] + 1);
						pNumVal[id]++;
					}

				} // x
			} // y
		} // c	


		delete[] pNumVal;
		if (pMaxVal != NULL) delete[] pMaxVal;
		return 0;

	}

	bool CCell::ifInsideCell(CvPoint2D64f x, CvPoint2D64f C) const
	{
		bool res;
		for (int i = 0; i < 6; i++) {
			int j = (i + 1) % 6;
			res = ifInsideTriangle(x, C, getBoundaryPoint(C, i, m_R), getBoundaryPoint(C, j, m_R));
			if (res) break;
		}
		return res;
	}

	CvPoint2D64f CCell::getBoundaryPoint(CvPoint2D64f C, int i, double R)
	{
		double r = 0.5 * sqrt(3.0) * R;
		switch (i) {
		case 0:		return cvPoint2D64f(C.x, C.y - R);
		case 1:		return cvPoint2D64f(C.x + r, C.y - 0.5*R);
		case 2:		return cvPoint2D64f(C.x + r, C.y + 0.5*R);
		case 3:		return cvPoint2D64f(C.x, C.y + R);
		case 4:		return cvPoint2D64f(C.x - r, C.y + 0.5*R);
		case 5:		return cvPoint2D64f(C.x - r, C.y - 0.5*R);
		default:	return cvPoint2D64f(0, 0);
		}
	}

	Point CCell::idx2h(int idx, double R, CvSize imgSize)
	{
		double	r = 0.5 * sqrt(3.0) * R;
		double	dx = 2.0 * r;
		double	imgWidth = static_cast<double>(imgSize.width);
		int		width0 = static_cast<int> (0.99 + imgWidth / dx);
		int		width1 = 1 + static_cast<int> (0.99 + (imgWidth - r) / dx);
		int		widthD = width0 + width1;

		// cell indexes
		Point res;
		res.y = idx / widthD;
		res.x = idx - res.y * widthD;
		res.y *= 2;
		if (res.x >= width0) {
			res.y++;
			res.x -= width0;
		}
		return res;
	}

	int CCell::h2idx(Point c) const
	{
		double	dx = 2.0 * m_r;
		double	imgWidth = static_cast<double>(m_imgSize.width);
		int		width0 = static_cast<int> (0.99 + imgWidth / dx);
		int		width1 = 1 + static_cast<int> (0.99 + (imgWidth - m_r) / dx);
		int		widthD = width0 + width1;

		// index
		int res;
		int a = c.y / 2;
		int b = c.y - 2 * a;
		res = a * widthD;
		if (b == 1) res += width0;
		res += c.x;

		return res;
	}

	CvPoint2D64f CCell::idx2d(int idx, double R, CvSize imgSize)
	{
		double	r = 0.5 * sqrt(3.0) * R;
		double	dx = 2.0 * r;			// x - distance between cells
		double	dy = 1.5 * R;			// y - distance between cells

		// cell indexes
		Point C = idx2h(idx, R, imgSize);

		// cell coordinates in image
		CvPoint2D64f res;
		res.y = C.y * dy + 0.5 * R;
		res.x = C.x * dx + r;
		if ((C.y % 2) != 0) res.x -= r;

		return res;
	}

	int	CCell::d2idx(CvPoint2D64f C) {
		if (m_LUT.empty()) { if (calculate_LUT() != 0) return -1; }
		return m_LUT.at<int>(static_cast<int>(C.y), static_cast<int>(C.x));
	}

	Point	CCell::d2h(CvPoint2D64f C) const
	{
		double	dx = 2.0 * m_r;			// x - distance between cells
		double	dy = 1.5 * m_R;			// y - distance between cells

		Point c;
		c.y = static_cast<int>((C.y + 0.5 * m_R) / dy);
		c.x = static_cast<int>(C.x / dx);

		return c;
	}

}


