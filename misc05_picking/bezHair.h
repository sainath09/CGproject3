#ifndef BEZHAIR_H
#define BEZHAIR_H

#include <misc05_picking/gridOps.h>
#include <iostream>
#include <vector>
#include <array>
#include <fstream>
#include <string>
#include <cmath>




int genBezierHair(std::vector<Vertex> bezHair, std::vector<unsigned int>  indices, const std::vector<Vertex> controlPoints);

#endif
