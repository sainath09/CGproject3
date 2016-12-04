#ifndef GRID_OPS_H
#define GRID_OPS_H

#include <iostream>
#include <vector>

// Function generates grid indices in order for opengl to draw triangle strips
int genGridTriangs(const int gridX, const int gridY, std::vector<unsigned short>& gridTriangs);

#endif