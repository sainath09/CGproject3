
#include <misc05_picking/bezHair.h>


using namespace std;

// Function to compute Bezier curve for hair
int genBezierHair(std::vector<Vertex> bezHair, std::vector<unsigned int> indices,const std::vector<Vertex> controlPoints)
{
	array<float,4> color = { 1.0,0.0,0.0,1.0 };
	int index = 0;
	for (int id = 216; id <= 286;) {
		for (int j = 1; j <= 8; j++){
			
			bezHair.push_back(controlPoints[id]);
			indices.push_back(index);
			bezHair[index].Color = color;
			index++;
			id++;
			}
		id += 12;
	}
	return 0;
}
