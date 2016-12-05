#ifndef GRID_OPS_H
#define GRID_OPS_H

#include <iostream>
#include <vector>
#include<array>

// Function generates grid indices in order for opengl to draw triangle strips
int genGridTriangs(const int gridX, const int gridY, std::vector<unsigned short>& gridTriangs);

typedef struct Vertex {
	std::array<float, 4> Position;
	std:: array<float, 4> Color;
	std::array<float, 3> Normal;
	std::array<float, 2> textureCords;
	void SetPosition(float* coords) {
		Position[0] = coords[0];
		Position[1] = coords[1];
		Position[2] = coords[2];
		Position[3] = 1.0;
	}
	void SetColor(float* color) {
		Color[0] = color[0];
		Color[1] = color[1];
		Color[2] = color[2];
		Color[3] = color[3];
	}
	void SetNormal(float* coords) {
		Normal[0] = coords[0];
		Normal[1] = coords[1];
		Normal[2] = coords[2];
	}
	void setTexture(float* coords)
	{
		textureCords[0] = coords[0];
		textureCords[1] = coords[1];

	}
}Vertex;

int toFloat(std::array<float,4> & facePoints,float *vert);

#endif