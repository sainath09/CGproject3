/* File containing functions to do simple reusable math operations */
#ifndef MATH_OPS_H
#define MATH_OPS_H

#include <iostream>
#include <math.h>
#include <vector>

#include <glm/glm.hpp>

namespace mth
{
// Function creates points on a circle in 3D, around specified axis
// Points stored in glm::vec3 for easy math operations using glm later
int createCircPts(const char axis, // Axis around which circle will be oriented: (x,y or z)
				  const int pos,  // Position to fix around chosen axis
				  const int circResln, // No of points on circle
				  std::vector<glm::vec3>& circCoords); // vector to hold generated points

// Compute points on a 3D projectile path
int compPrjTrajectory(const glm::vec4& initVel, const glm::vec4& p0,
					  const int resln, std::vector<glm::vec4>& projPath);
}

#endif
