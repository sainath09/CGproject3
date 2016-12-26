/***Module to perform operations required to generate points on 3d surface using
	deCasteljau's algorithm for Bezier surfaces****/
#include <misc05_picking/bezOps.h>


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
// TODO: Make gridWidth programmable - currently hardcoded to 21
int compBezCtrlPoints(const vector<Point>& gridPts,
					  const vector<vector<int>>& bezIndices,
					  vector<vector<Point>>& bezCtrlPts)
{
	// Compute delX and delY and use that to generate the rest of Bez ctrl pts
	//cout << bezIndices.size() << endl;
	vector<int> idcs = bezIndices[0];
	Point c00, c10, c11;
	c00 = (gridPts[idcs[4]]*16 +
			(gridPts[idcs[1]]+gridPts[idcs[3]] + gridPts[idcs[5]]+gridPts[idcs[7]])*4+
			(gridPts[idcs[0]]+gridPts[idcs[2]] + gridPts[idcs[6]]+gridPts[idcs[8]]))/36;

	c10 = (gridPts[idcs[4]]*8 + (gridPts[idcs[7]]+gridPts[idcs[1]])*2 +
			gridPts[idcs[5]]*4 + gridPts[idcs[2]]+gridPts[idcs[8]]) / 18;

	c11 = (gridPts[idcs[4]]*4 + (gridPts[idcs[1]]+gridPts[idcs[5]])*2 +
			gridPts[2]) / 9; 

	cout << c00.x << " " << c10.x << " " << c11.x << endl;
	cout << c00.y << " " << c10.y << " " << c11.y << endl;

	#ifndef DEBUG
	for (size_t i=0; i<idcs.size(); i++)
	{
		cout << idcs[i] << ":  ";
		cout << gridPts[idcs[i]].x << " " << gridPts[idcs[i]].y << endl;
	}
	#endif

	float delX = c10.x - c00.x;
	float delY = c11.y - c10.y;
	delX = std::fabs(delX); delY = std::fabs(delY);
	cout << "Del vals: " << delX << ", " << delY << endl;
	int ind = 1;

	// Go through each cell of grid to generate ctrl pts for each
	cout << "GridPts size " << gridPts.size() << endl;
	for (size_t i=0; i<gridPts.size()-21; i++)
	{
		Point start = gridPts[i];
		if ((i != 0) && (i !=  21) && (i != 42 ))	{
			if  ((i == 20) || (i == 41) || ((i+1)%21 == 0)) {
				continue;
			}
		}
		
		for (size_t k=0; k< 4; k++)	{
			vector<Point> ctrlPts(4);
			for (size_t j=0; j<4; j++) { 
				ctrlPts[j] = start;
				ctrlPts[j].x = start.x + j*delX;
			}
			bezCtrlPts.push_back(ctrlPts);
			start.y = start.y - delY;
		}
	}
	return 0;
}

// Compute individual Bezier Curve point at t using deCasteljau's algorithm
int compBezPt(const vector<Point>& ctrlPts, Point& pt, float t) {
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
	return 0;
}


// Compute surface points on bi-cubic Bezier surface
int compBezSurf(const vector<vector<Point>>& ctrlPts, const int tess_no,
					  vector<Point>& surfPts) {
	// 16 control points corresponding to 1 patch are stored in 1 vector
	size_t patchCount = ctrlPts.size();
	size_t ctPtCount = 4; // Curve param: ctrl pt count = curve deg+1
	float surfRes = 1 / float(tess_no);

	// Go through each patch and compute points recursively
	for (size_t i=0; i<patchCount; i+=4) {
		// 0-3, 4-7...12-15
		vector<Point> curCtrlPts = ctrlPts[i];
		vector<vector<Point>> interpPts;

		// Interpolate points in horizontal direction
		for (size_t j=0; j<4; j++) {
			vector<Point> horCtrlPts = ctrlPts[i+j];
			vector<Point> tmpPts;

			for (int k=0; k<tess_no; k++) {
				Point bzpt;
				compBezPt(horCtrlPts, bzpt, surfRes*k);
				tmpPts.push_back(bzpt);
			}
			interpPts.push_back(tmpPts);
		}

		int vertCurvCount = interpPts.size();

		ctPtCount = interpPts[0].size();
		// Interpolate points in vertical direction
		for (size_t j=0; j<ctPtCount; j++) {
			vector<Point> verCtrlPts;
			for (size_t l=0; l<vertCurvCount; l++) {
				verCtrlPts.push_back(interpPts[l][j]);
				cout << "[" << interpPts[l][j].x << ", " << interpPts[l][j].y << "] "  << endl;
			}
			cout << endl;

			for (int k=0; k<=tess_no; k++) {
				Point bzpt;
				compBezPt(verCtrlPts, bzpt, surfRes*k);
				// Currently pushing points into surface array only after
				// interpolation is over in both x,y directions
				// TODO: May have to revisit to check if this is the right idea
				surfPts.push_back(bzpt);
			}	
		}
	}
	return 0;
}


int Pt2Vert(const vector<Point>& inVertices, std::vector<Vertex>& outVertices, int col)
{
	int vertCount = inVertices.size();
	outVertices.resize(vertCount);
	for (int i = 0; i < vertCount; i++)	{
		array<float, 4> temp;
		temp = inVertices[i].toArray();
		outVertices[i].Position = temp;
		outVertices[i].textureCords = inVertices[i].getTexCoords();
		if (col == 1) {
			outVertices[i].Color = {1.0f, 0.0f, 0.0f, 1.0f};
		} else if (col == 2) {
			outVertices[i].Color = {0.0f, 1.0f, 0.0f, 1.0f};
		} else if (col == 3) {
			outVertices[i].Color = {0.0f, 0.0f, 1.0f, 1.0f};
		} else if (col == 4){
			outVertices[i].Color = {1.0f, 1.0f, 1.0f, 1.0f};
		} else if (col == 5){
			outVertices[i].Color = {1.0f, 1.0f, 0.0f, 1.0f};
		}
	}
	return 0;
}


int Vert2Pt(const vector<Vertex>& inVertices, vector<Point>& outVertices)
{
	size_t vertCount = inVertices.size();
	for (int i = 0; i < vertCount; i++)	{
		Point temp;
		temp = Point(inVertices[i].Position);
		temp.setTexture(inVertices[i].textureCords);
		outVertices.push_back(temp);
	}
	return 0;
}


int Lin2VertArr(const vector<vector<Point>>& inVertices,
					  vector<Vertex>& outVertices, int col)
{
	int vertCount = inVertices.size();
	int internalLen = inVertices[0].size();
	outVertices.resize(vertCount*internalLen);
	for (size_t i = 0; i < vertCount; i++)	{
		for (size_t j = 0; j < internalLen; j++)	{
			array<float, 4> temp;
			array<float, 2> texCoords = {1, 1};
			temp = inVertices[i][j].toArray();
			size_t outInd = i*internalLen + j;
			outVertices[outInd].Position = temp;
			outVertices[outInd].textureCords = texCoords;
			if (col == 1) {
				outVertices[outInd].Color = {1.0f, 0.0f, 0.0f, 1.0f};
			} else if (col == 2) {
				outVertices[outInd].Color = {0.0f, 1.0f, 0.0f, 1.0f};
			} else if (col == 3) {
				outVertices[outInd].Color = {0.0f, 0.0f, 1.0f, 1.0f};
			} else if (col == 4){
				outVertices[outInd].Color = {1.0f, 1.0f, 1.0f, 1.0f};
			} else if (col == 5){
				outVertices[outInd].Color = {1.0f, 1.0f, 0.0f, 1.0f};
			}
			
		}
	}
	return 0;
}


int write2File(string& fname, vector<Point>& points, int gridX)
{
	ofstream coordFile(fname.c_str());
	size_t  ptCount = points.size(); // gridX;
	for (size_t i=0; i<ptCount; i++)
	{
		if ((i!= 0) && (i%gridX == 0))
			coordFile << endl;
		//coordFile << "Row[ " << i << " ]: ";
		//for (size_t j=0; j<gridX; j++)
		{
			coordFile << "[" << points[i].x << "," << points[i].y << "," << points[i].z
					  << ","  << points[i].s << ","  << points[i].t << "], ";
		}
	}
	coordFile.close();
	return 0;
}

int genBezTriangles(int gridX,int gridY,std::vector<unsigned short>&bezTriangulation){
	for (int i = 0; i < gridY-1; i++){
		for (int j = 0; j < gridX-1; j++) {
			std::vector<unsigned short> tempIndc;
			genGridTriangs(10, 10, tempIndc);
			for (int k = 0; k < tempIndc.size(); k++) {
				tempIndc[k] = ((gridX*i+j) * 100) + tempIndc[k];
			}

			bezTriangulation.insert(bezTriangulation.end(), tempIndc.begin(),tempIndc.end());

		}	
	}
	return 0;
}
