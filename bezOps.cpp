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
int compBezCtrlPoints(const vector<Point>& gridPts,
					  const int gridX,
					  const int gridY,
					  const vector<vector<int>>& bezIndices,
					  vector<vector<Point>>& bezCtrlPts)
{
	// Compute delX and delY and use that to generate the rest of Bez ctrl pts
	cout << bezIndices.size() << endl;
	vector<int> idcs = bezIndices[bezIndices.size()-1];
	Point c00, c10, c11;
	c00 = (gridPts[idcs[4]]*16 +
			(gridPts[idcs[1]]+gridPts[idcs[3]] + gridPts[idcs[5]]+gridPts[idcs[7]])*4+
			(gridPts[idcs[0]]+gridPts[idcs[2]] + gridPts[idcs[6]]+gridPts[idcs[8]]))/36;

	c10 = (gridPts[idcs[4]]*8 + (gridPts[idcs[7]]+gridPts[idcs[1]])*2 +
			gridPts[idcs[5]]*4 + gridPts[idcs[2]]+gridPts[idcs[8]]) / 18;

	c11 = (gridPts[idcs[4]]*4 + (gridPts[idcs[1]]+gridPts[idcs[5]])*2 +
			gridPts[2]) / 9; 

	cout << c00.x << " " << c10.x << " " << c11.x << endl;

	cout << " Idcs size " << idcs.size() << endl;
	for (size_t i=0;  i<gridPts.size(); i++)
	{
		//cout << idcs[i] << endl;
		//cout << gridPts[i].x << " " << gridPts[i].y << endl;
	}

	cout << c00.x << "  " << c00.y << endl;
	cout << c10.x << "  " << c10.y << endl;
	cout << c11.x << "  " << c11.y << endl;
	float delX = c11.x - c10.x;
	float delY = c10.y - c00.y;
	delX = std::fabs(delX); delY = std::fabs(delY);

	cout << "Del vals: " << delX << ", " << delY << endl;

	// Go through each cell of grid to generate ctrl pts for each
	for (size_t i=0; i<gridPts.size(); i++)
	{
		Point start = gridPts[0];
		// Control Points corresponding to 1 grid
		vector<Point> ctrlPts(16);
		for (size_t j=0; j< 4; j++)
		{
			for (size_t j=0; j<4; j++)
			{ 
				ctrlPts[j] = start;
				ctrlPts[j].x = start.x + j*delX;
				cout << ctrlPts[j].x << " " << ctrlPts[j].y << endl;
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
int compBezSurf(const vector<vector<Point>>& ctrlPts, const int tess_no,
					  vector<Point>& surfPts)
{
	size_t patchCount = ctrlPts.size();
	size_t ctPtCount = 4; // Curve param: ctrl pt count = curve deg+1
	float surfRes = 1 / float(tess_no);

	// Go through each patch and compute points recursively
	for (size_t i=0; i<patchCount; i++)
	{
		// There are 16 ctrl points per patch, laid out in a linear fashion
		// 0-3, 4-7...12-15
		vector<Point> curCtrlPts = ctrlPts[i];
		vector<vector<Point>> interpPts;

		// Interpolate points in horizontal direction
		for (size_t j=0; j<4; j++)
		{
			//vector<vector<Point>>::const_iterator first = ctrlPts.begin();
			//std::advance(first, j*ctPtCount);
			//vector<vector<Point>>::const_iterator last = ctrlPts.begin();
			//std::advance(last j*ctPtCount + 4;
			vector<Point> horCtrlPts = ctrlPts[i];
			vector<Point> tmpPts;

			for (int k=0; k<tess_no; k++)
			{
				Point bzpt;
				compBezPt(horCtrlPts, bzpt, surfRes*k);
				tmpPts.push_back(bzpt);
			}
			interpPts.push_back(tmpPts);
		}

		int vertCurvCount = interpPts.size();
		// Interpolate points in vertical direction
		for (size_t j=0; j<vertCurvCount; j++)
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
				compBezPt(verCtrlPts, bzpt, surfRes*k);
				// Currently pushing points into surface array only after
				// interpolation is over in both x,y directions
				// TODO: May have to revisit to check if this is the right idea
				surfPts.push_back(bzpt);
				cout << surfRes*k  << " " << bzpt.x << " " << bzpt.y << "], [";
			}	
			cout << endl;
		}
	}
	return 0;
}


int Pt2Vert(const vector<Point>& inVertices, vector<Vertex>& outVertices, int col)
{
	int vertCount = inVertices.size();
	outVertices.resize(vertCount);
	for (int i = 0; i < vertCount; i++)	{
		array<float, 4> temp;
		temp = inVertices[i].toArray();
		outVertices[i].XYZW = temp;
		if (col == 1) {
			outVertices[i].RGBA = {1.0f, 0.0f, 0.0f, 1.0f};
		} else if (col == 2) {
			outVertices[i].RGBA = {0.0f, 1.0f, 0.0f, 1.0f};
		} else if (col == 3) {
			outVertices[i].RGBA = {0.0f, 0.0f, 1.0f, 1.0f};
		} else if (col == 4){
			outVertices[i].RGBA = {1.0f, 1.0f, 1.0f, 1.0f};
		} else if (col == 5){
			outVertices[i].RGBA = {1.0f, 1.0f, 0.0f, 1.0f};
		}
	}
	return 0;
}


int Lin2Vert(const vector<vector<Point> >& inVertices,
					vector<Vertex>& outVertices, int col)
{
	int vertCount = inVertices.size();
	int internalLen = inVertices[0].size();
	outVertices.resize(vertCount*internalLen);
	for (int i = 0; i < vertCount; i++)	{
		for (int j = 0; j < vertCount; j++)	{
			array<float, 4> temp;
			temp = inVertices[i][j].toArray();
			outVertices[j].XYZW = temp;
			if (col == 1) {
				outVertices[j].RGBA = {1.0f, 0.0f, 0.0f, 1.0f};
			} else if (col == 2) {
				outVertices[j].RGBA = {0.0f, 1.0f, 0.0f, 1.0f};
			} else if (col == 3) {
				outVertices[j].RGBA = {0.0f, 0.0f, 1.0f, 1.0f};
			} else if (col == 4){
				outVertices[j].RGBA = {1.0f, 1.0f, 1.0f, 1.0f};
			} else if (col == 5){
				outVertices[j].RGBA = {1.0f, 1.0f, 0.0f, 1.0f};
			}
			
		}
	}
	return 0;
}
/*
	// Simulate a small grid to generate indices for bezCtrl Pt computation
	int lenX=3, lenY=3;
	vector<Point> gridPtInds;
	packGridIndices(lenX, lenY, gridPtInds);
	compBezCtrlPoints(controlPoints, gridPtInds, bezCtrlPts);
*/
