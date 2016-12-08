#ifndef BEZOPS_H
#define BEZOPS_H

#include <misc05_picking/gridOps.h>
#include <iostream>
#include <vector>
#include <array>
#include <fstream>
#include <string>
#include <cmath>

// ATTN: USE POINT STRUCTS FOR EASIER COMPUTATIONS
typedef struct point {
	float x, y, z, s, t;
	point(const float x = 0, const float y = 0, const float z = 0) : x(x), y(y), z(z){};
	point(const std::array<float, 4>& coords) : x(coords[0]), y(coords[1]), z(coords[2]){};
	void setTexture(const std::array<float, 2>& coords) {
		s = coords[0];
		t = coords[1];
	}
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
	std::array<float, 2> getTexCoords() const {
		std::array<float, 2> outArray = {s, t};
		return outArray;
	}
} Point;



// Function packs indices of generated grid points (strong assumption about
// ordering of grid points inherent in function
int packGridIndices(const int gridX, int gridY, std::vector<std::vector<int>>& bezSrcPts);
// Compute control points for Bezier surface using grid points as starting points
int compBezCtrlPoints(const std::vector<Point>& gridPts,
					  const std::vector<std::vector<int>>& bezIndices,
					  std::vector<std::vector<Point>>& bezCtrlPts);
// Compute surface points on bi-cubic Bezier surface
int compBezSurf(const std::vector<std::vector<Point>>& ctrlPts, const int tess_no,
					  std::vector<Point>& surfPts);
int Pt2Vert(const std::vector<Point>& inVertices, std::vector<Vertex>& outVertices, int col);
int Vert2Pt(const std::vector<Vertex>& inVertices, std::vector<Point>& outVertices);
int Lin2VertArr(const std::vector<std::vector<Point> >& inVertices,
					  std::vector<Vertex>& outVertices, int col);
int write2File(std::string& fname, std::vector<Point>& points, int gridX);
int genBezTriangles(int noOfPatches, std::vector<unsigned short>bezTriangulation);
#endif
