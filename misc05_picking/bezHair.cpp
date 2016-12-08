
#include <misc05_picking/bezHair.h>


using namespace std;

// Function to compute Bezier curve for hair
int genBezierHair(std::vector<Vertex> &bezHair, std::vector<unsigned int> &indices,const std::vector<Vertex>& controlPoints)
{
	array<float,4> color = { 0.5,0.5,0.5,1.0 };
	int index = 0;
	for (int id = 216; id <= 360;) {
		for (int j = 1; j <= 8; j++){
			
			bezHair.push_back(controlPoints[id]);
			indices.push_back(index);
			bezHair[index].Color = color;
			bezHair[index].Position[2] = 2*rand()/float(RAND_MAX);
			index++;
			id++;
			}
		id += 12;
	}
	return 0;
}
