#include "hCell.h"

using namespace HexagonCells;

void print_help(void)
{
	printf("Usage: \"Demo.exe\" input_image [cell_radius]\n");
}

int main(int argc, char *argv[]) 
{   
	if ((argc < 2) || (argc > 3)) {
		print_help();
		return 0;
	}
	
	std::string	  fileName(argv[1]);
	Mat			  img	 = imread(fileName, 1);						// Input image 
	double		  R		 = (argc == 3) ? atof(argv[2]) : 3.102;		// So the hexagon area will be 25 pixels;
	CCell		  cell(img, R);   
	CMarker		  marker;        
	cell_params	  params = cell.getInfo();        
	
	for (int n = 0; n < params.N; n++)        
		marker.markHexagon(img, R, n, cell.getVal(n));     
	
//	marker->markGrid(img, R, CV_RGB(0, 128, 64));    
	
	fileName.insert(fileName.length() - 4, "_hex");
	imwrite(fileName, img);
//	imshow("Demo", img);
//	cvWaitKey();    
	return 0;
}

