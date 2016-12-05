#ifndef BEZOPS_H
#define BEZOPS_H

//#include <datautils.h>
#include <iostream>
#include <vector>
#include <array>

// ATTN: USE POINT STRUCTS FOR EASIER COMPUTATIONS
typedef struct point {
	float x, y, z;
	point(const float x = 0, const float y = 0, const float z = 0) : x(x), y(y), z(z){};
	point(const std::array<float, 4>& coords) : x(coords[0]), y(coords[1]), z(coords[2]){};
	point operator -(const point& a) const {
		return point(x - a.x, y - a.y, z - a.z);
	}
	point operator +(const point& a) const {
		return point(x + a.x, y + a.y, z + a.z);
	}
	point operator *(const float& a) const {
		return point(x*a, y*a, z*a);
	}
	point operator /(const float& a) const {
		return point(x / a, y / a, z / a);
	}
	std::array<float, 4> toArray() const {
		std::array<float, 4> outArray = { x, y, z, 1.0f };
		return outArray;
	}
} Point;



// Function packs indices of generated grid points (strong assumption about
// ordering of grid points inherent in function
int packGridIndices(const int gridX, int gridY, std::vector<std::vector<int>>& bezSrcPts);
// Compute control points for Bezier surface using grid points as starting points
int compBezCtrlPoints(const std::vector<Point>& gridPts, const int gridX,
					  const int gridY, const std;:vector<std::vector<int>>& bezIndices,
					  std::vector<std::vector<Point>>& bezCtrlPts)
// Compute surface points on bi-cubic Bezier surface
int compBezSurf(const std::vector<Point>& ctrlPts, const int tess_no,
					  std::vector<Point>& surfPts);
#endif
