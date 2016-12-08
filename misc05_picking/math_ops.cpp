/* File containing functions to do simple reusable math operations */
#include <misc05_picking/math_ops.h>
#define M_PI 3.141516
#define root3 1.732
using namespace std;
using namespace glm;

namespace mth
{
int createCircPts(const char axis, const int pos, const int circResln,
				  std::vector<glm::vec3>& camCircPts)
{
	float radius = 20*root3;
	float dtheta = 360 / circResln;
	for (size_t i=1; i<circResln; i++) {
		float coord1 = radius * cos(i*dtheta*M_PI/180);
		float coord2 = radius * sin(i*dtheta*M_PI/180);
		glm::vec3 cpos = {0.0, 0.0, 0.0};
		switch(axis) {
		
		case 'x':	
			cpos = {pos, coord1, coord2};
			break;
		case 'y':
			cpos = {coord1, pos, coord2};
			break;
		case 'z':
			cpos = {coord1, coord2, pos};
			break;
		default:
			cpos = {pos, coord1, coord2};
			break;
		}
		camCircPts.push_back(cpos);
	}
	return 0;
}

// Compute points on a 3D projectile path
int compPrjTrajectory(const glm::vec4& initVel, const glm::vec4& p0,
					  const int resln, vector<glm::vec4>& projPath)
{
	cout << "Path Taken: " << endl;
	float sy=100;
	int t = 0;
	while (sy > 0) {
		float sx, sz;
		if (p0.x > 0) {
			sx = p0.x + 2*initVel.x*t*0.005;
		} else {
			sx = p0.x - 2*initVel.x*t*0.005;
		}
		sy = p0.y + 2*initVel.y*t*0.005 - 4.9*t*t*0.005*0.005;
		if (initVel.z > 0) {
			sz = p0.z - 2*initVel.z*t*0.005;
		} else {
			sz = p0.z + 2*initVel.z*t*0.005;
		}
		glm::vec4 pos = glm::vec4(sx, sy, sz, 1.0f);
		cout << pos.x << " " << pos.y << " " << pos.z << " "  << endl;
		projPath.push_back(pos);
		t++;
	}
	return 0;
}

}
