// Cell class
// Written by Sergey G. Kosov in 2013 for Project X
#pragma once

#include "types.h"

const double MIN_RADIUS = 1.0;		///< Minimal allowed hexagon outer radius

namespace HexagonCells
{
	///@brief Cell parameters structure
	typedef struct {
		double	R;		///< Hexagon outer radius
		double	r;		///< Hexagon inner radius
		double	S;		///< Hexagon area in pixels
		int		N;		///< Number of hexagons in the image
	} cell_params;

	/**
	@brief Cell interpolation approach
	@details The CELL_AVG approach returns the average value of all the pixels in the cell; the CELL_MV
	approach returns the most frequent value of the pixels in the cell.
	@warning The CELL_MV approach may return unexpected results on images with losely compression as JPEG
	*/
	enum cell_int_app {
		CELL_AVG,		///< Average value approach
		CELL_MV			///< Majority voting approach
	};


	// ================================ Cell Class ================================
	/**
	@brief Cell class
	@todo Optimize the \b calculate_LUT() function
	@todo Add sub-pixel accuracy support
	@bug Images with resolution 640 x 480 pixels and cell radius R = 3.102 have an uncovered by cells regions at the right image edge.
	@author Sergey G. Kosov, sergey.kosov@project-10.de
	*/
	class CCell
	{
		friend class CMarker;

	public:
		/**
		@brief Default constuctor
		*/
		DllExport CCell(void);
		/**
		@brief Constuctor
		@param imgSize The image size
		@param cellIntApp Cell interpolation approach (Ref. @ref cell_int_app)
		*/
		DllExport CCell(CvSize imgSize, cell_int_app cellIntApp = CELL_AVG);
		/**
		@brief Constuctor
		@param img The image
		@param cellIntApp Cell interpolation approach (Ref. @ref cell_int_app)
		*/
		DllExport CCell(Mat &img, cell_int_app cellIntApp = CELL_AVG);
		/**
		@brief Constuctor
		@param R Hexagon outer radius
		*/
		DllExport CCell(double R);
		/**
		@brief Constuctor
		@param imgSize The image size
		@param R Hexagon outer radius
		@param cellIntApp Cell interpolation approach (Ref. @ref cell_int_app)
		*/
		DllExport CCell(CvSize imgSize, double R, cell_int_app cellIntApp = CELL_AVG);
		/**
		@brief Constuctor
		@param img The image
		@param R Hexagon outer radius
		@param cellIntApp Cell interpolation approach (Ref. @ref cell_int_app)
		*/
		DllExport CCell(Mat &img, double R, cell_int_app cellIntApp = CELL_AVG);
		DllExport ~CCell(void);

		/**
		@brief Resets the class by releasing memory and setting the class variable by default
		*/
		DllExport void			  clear(void);
		/**
		@brief (Re-) sets the image
		@param img The image
		*/
		DllExport void			  setImage(Mat &img);
		/**
		@brief (Re-) sets the hexagon outer radius
		@param R Hexagon outer radius
		*/
		DllExport void			  setRadius(double R);
		/**
		@brief (Re-) sets the interpolation approach for cell color generation
		@param cellIntApp Cell interpolation approach (Ref. @ref cell_int_app)
		*/
		DllExport void			  setInterpolationApproach(cell_int_app cellIntApp);

		/**
		@brief Returns the cell parameters
		@return %cell_params structure (Ref. @ref cell_params)
		*/
		DllExport cell_params	  getInfo(void);
		/**
		@brief Returns the cell index
		@param x x-coordinate of a pixel in the image
		@param y y-coordinate of a pixel in the image
		@return Index of the cell, to which the pixel belongs
		@todo Maybe switch to the Point structure as an argument
		*/
		DllExport int			  getIDX(int x, int y);
		/**
		@brief Returns the neighbouring cell index
		@param idx Cell index
		@param i Cell neighbour index in range from 0 till 5, which corresponds to the neighbours depicted at \b Fig. \b 1.
		@image html cell.jpg "Fig. 1"
		@retval Index of the neighbouring cell
		@retval -1 If the neighbour is beyond the image borders
		*/
		DllExport int			  getNeighbourIDX(int idx, int i);
		/**
		@brief Returns all 6 neighbouring cell indexs
		@param idx Cell index
		@return Array of the neighbouring cell indexes. The length of the array is 6 and each elemet corresponds
		to neighbour, indexed according to the \b Fig. \b 1. from @ref getNeighbourIDX
		. Neighbouring cell index
		may be equal to -1 if the neighbour is beyond the image borders.
		*/
		DllExport int			* getNeighbourhood(int idx);
		/**
		@brief Returns the color of the specified cell
		@param idx Cell index
		@return Cell color
		*/
		DllExport CvScalar		  getVal(int idx);

		// Brute - force functions
		DllExport Mat			  getLUT(void) { return m_LUT; }
		DllExport void			  setLUT(Mat &LUT) { LUT.copyTo(m_LUT); }


	private:
		int calculate_LUT(void);		// 0 on success, error_code otherwise
		int calculate_nCells(void);		// 0 on success, error_code otherwise
		int calculate_cellData(void);	// 0 on success, error_code otherwise

		bool				  ifInsideCell(CvPoint2D64f x, CvPoint2D64f C) const;
		static CvPoint2D64f   getBoundaryPoint(CvPoint2D64f C, int i, double R);


		// Coordinate translation functions (idx <-> h <-> d)
		static Point		  idx2h(int idx, double R, CvSize imgSize);			// index to hexagonal
		inline int			  h2idx(Point c) const;			// hexagonal to index
		static CvPoint2D64f	  idx2d(int idx, double R, CvSize imgSize);			// index to cartesian
		inline int			  d2idx(CvPoint2D64f C);							// cartesian to index
	//	static CvPoint2D64f	  h2d(Point c);
		inline Point		  d2h(CvPoint2D64f C) const;						// cartesian to hexagonal


	private:
		Mat				m_img;			// Mat();			// The image
		CvSize			m_imgSize;		// cvSize(0, 0);	// 
		Mat				m_LUT;			// Mat();			// Look-up table Mat(m_imgSize, CV_32SC1)
		double			m_R;			// -1;				// Hexagon outer radius
		double			m_r;			// -1;				// Hexagon inner radius
		int				m_nCells;		// -1;				// Number of of hexagons in the image
		cell_int_app	m_cellIntApp;	// CELL_AVG;		// Cell interpolation approach
		Mat				m_cellData;		// Mat();			// Direct cell datas



		// Copy semantics are disabled
		CCell(const CCell &rhs) {}
		const CCell & operator= (const CCell & rhs) { return *this; }
	};

}
