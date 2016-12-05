/***Module to perform operations required to generate points on 3d surface using
	deCasteljau's algorithm for Bezier surfaces****/
#include <bezOps.h>

using namespace std;

// Function to compute indices of grid points to be used for computing
// Bezier control points
int packGridIndices(const int gridX, int gridY, vector<vector<int>>& bezSrcPts)
{
	// Grid is sub-divided into quadrants in both x&y direction
	// For a grid of size 20x20, 100 quadrants will be produced
	for (size_t i=0; i<gridY-1; i+=2)
	{
		int k = 0;
		for (size_t j=0; j<gridX-1; j+=2)
		{
			vector<int> bezPatchPts(9);
			bezPatchPts[k  ] = i*gridX+j;
			bezPatchPts[k+1] = i*gridX+j+1;
			bezPatchPts[k+2] = i*gridX+j+2;
			bezPatchPts[k+3] = (i+1)*gridX+j;
			bezPatchPts[k+4] = (i+1)*gridX+j+1;
			bezPatchPts[k+5] = (i+1)*gridX+j+2;
			bezPatchPts[k+6] = (i+2)*gridX+j;
			bezPatchPts[k+7] = (i+2)*gridX+j+1;
			bezPatchPts[k+8] = (i+2)*gridX+j+2;
			bezSrcPts.push_back(bezPatchPts);
		}
	}
	return 0;
}


// Compute control points for Bezier surface using grid points as starting points
int compBezCtrlPoints(const vector<Point>& gridPts, const int gridX,
					  const int gridY, const vector<vector<int>>& bezIndices,
					  vector<vector<Point>>& bezCtrlPts)
{
	// Compute delX and delY and use that to generate the rest of Bez ctrl pts
	vector<int> idcs = bezIndices[bezIndices.size()-1];
	Point c00, c10, c11;
	float c00 = (16* gridPts[idcs[4]] +
				 4 *(gridPts[idcs[1]]+gridPts[idcs[3]] + gridPts[idcs[5]]+gridPts[idcs[7]])+
					(gridPts[idcs[0]]+gridPts[idcs[2]] + gridPts[idcs[6]]+gridPts[idcs[8]]))/36;

	float c10 = (8*gridPts[idcs[4]] + 2*(gridPts[idcs[7]]+gridPts[idcs[1]]) +
				 4*gridPts[idcs[5]] +    gridPts[idcs[2]]+gridPts[idcs[0]]) / 18;

	float c11 = (4*gridPts[idcs[4]] + 2*(gridPts[idcs[1]]+gridPts[idcs[5]]) +
				 gridPts[2]) / 9; 

	float delX = c10 - c00;
	float delY = c11 - c10;

	// Go through each cell of grid to generate ctrl pts for each
	for (size_t i=0; i<gridPts.size(); j++)
	{
		Point start = gridPts[0];
		// Control Points corresponding to 1 grid
		vector<Point> ctrlPts(16);
		for (size_t i=0; i< 4; i++)
		{
			for (size_t j=0; j<4; j++)
			{ ctrlPts[j] = start.x + j*delX;
			}
			start.y = start.y + delY;
		}
		bezCtrlPts.push_back(ctrlPts);
	}

	return 0;
}


// Compute individual Bezier Curve point at t using deCasteljau's algorithm
int compBezPt(const vector<Point>& ctrlPts, Point& pt, float t)
{
	size_t ctrlPtCnt = ctrlPts.size();
	vector<Point> interpts;
	interpts.resize(ctrlPtCnt);

	// Copy Initial vertices
	for (int i = 0; i < ctrlPtCnt; i++) {
		interpts[i] = ctrlPts[i];
	}

	// Compute bezier coefficient recursively
	for (int i = 1; i < ctrlPtCnt; i++) {
		for (int k = 0; k < (ctrlPtCnt - i); k++) {
			interpts[k] = interpts[k]*(1-t)+
						  interpts[k+1]*t;
		}
	
	}
	pt = interpts[0];

	#ifdef DEBUG
	cout << "Interp  Points: " << endl;
		cout << interpts[2].x << "  " << interpts[2].y << endl;
		cout << interpts[1].x << "  " << interpts[1].y << endl;
	cout << endl;
	#endif
	return 0;
}


// Compute surface points on bi-cubic Bezier surface
int compBezSurf(const vector<Point>& ctrlPts, const int tess_no,
					  vector<Point>& surfPts)
{
	int tess_no = 10;
	size_t patchCount = ctrlPts.size();
	size_t ctPtCount = 4; // Curve param: ctrl pt count = curve deg+1
	float surfRes = 1 / tess_no;

	// Go through each patch and compute points recursively
	for (size_t i=0; i<patchCount; i++)
	{
		// There are 16 ctrl points per patch, laid out in a linear fashion
		// 0-3, 4-7...12-15
		vector<Point> curCtrlPts = ctrlPts[i];
		vector<vector<Point>> interpPts;

		// Interpolate points in horizontal direction
		for (size_t j=0; j<ctPtCount; j++)
		{
			vector<Point>::const_iterator first = ctrlPts.begin() + j*ctPtCount;
			vector<Point>::const_iterator last = ctrlPts.begin() + j*ctPtCount + 4;
			vector<Point> horCtrlPts(first, last);
			vector<Point> tmpPts;

			for (int k=0; k<tess_no; k++)
			{
				Point bzpt;
				compBezPt(horCtrlPts, bzPt, surfRes*k);
				tmpPts.push_back(bzPt);
			}
			interpPts.push_back(tmpPts);
		}

		int vertCurvCount = interpPts.size();
		// Interpolate points in vertical direction
		for (size_t j=0; j<vertCurvCount; k++)
		{
			//vector<Point>::const_iterator first = ctrlPts.begin() + j*ctPtCount;
			//vector<Point>::const_iterator last = ctrlPts.begin() + j*ctPtCount + 4;
			vector<Point> verCtrlPts;
			for (size_t l=0; l<ctPtCount; l++)
			{
				verCtrlPts.push_back(interpPts[l][j]);
			}

			for (int k=0; k<tess_no; k++)
			{
				Point bzpt;
				compBezPt(verCtrlPts, bzPt, surfRes*k);
				// Currently pushing points into surface array only after
				// interpolation is over in both x,y directions
				// TODO: May have to revisit to check if this is the right idea
				surfPts.push_back(bzPt);
			}	
		}
	return 0;
}
