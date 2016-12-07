#ifndef GRID_OPS_H
#define GRID_OPS_H

#include <iostream>
#include <vector>
#include <array>
#include <glm/glm.hpp>
using namespace glm;

// Function generates grid indices in order for opengl to draw triangle strips
int genGridTriangs(const int gridX, const int gridY, std::vector<unsigned short>& gridTriangs);
int genGridInd(const int gridX, const int gridY, std::vector<unsigned short>&gridLineInds);

typedef struct Vertex {
	std::array<float, 4> Position;
	std::array<float, 4> Color;
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
	void setTexture(float* coords) {
		textureCords[0] = coords[0];
		textureCords[1] = coords[1];
	}
}Vertex;

int toFloat(std::array<float,4> & facePoints,float *vert);

bool rayTestPoints(std::vector<Vertex> vert, glm::vec3 start, glm::vec3 end, unsigned int *id, double *proj, double epsilon, int maxRange);
glm::vec3 findCLosestPoint(glm::vec3 rayStartPos, glm::vec3 rayEndPos, glm::vec3 pointPos, double *proj);
bool rayTest(glm::vec3 pointPos, glm::vec3 startPos, glm::vec3 endPos, glm::vec3 *closestPoint, double *proj, double epsilon);


#endif
