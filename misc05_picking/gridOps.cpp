#include <misc05_picking/gridOps.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace glm;


using namespace std;

int genGridTriangs(const int gridX, const int gridY, std::vector<unsigned short>& gridTriangs)
{
	int a = 0;
	
		for (int i = 0; i < gridX - 1; i++)
	{
		for (int j = 0; j < gridY - 1; j++)
		{
			if (j == 0)
			{
				a = i*gridX + j;
				gridTriangs.push_back(a);
				a = (i + 1)*gridX + (j);
				gridTriangs.push_back(a);
			}
			
			a = i*gridX + (j + 1);
			gridTriangs.push_back(a);
			a = (i + 1) * gridX + (j + 1);
			gridTriangs.push_back(a);
		}
		a = ((i + 1)*gridX) + (gridX - 1);
		//gridTriangs.push_back(a);
		a = (i + 1) * gridX;
		//gridTriangs.push_back(a);
	}
	return 0;
}

int genGridInd(const int gridX, const int gridY, std::vector<unsigned short>&gridLineInds)
{
	for (int i = 0; i <= gridX-1; i++)
	{
		for (int j = 0; j < gridY-1; j++)
		{
			
				gridLineInds.push_back(i*gridX+j);
				gridLineInds.push_back(i*gridX+j + gridX);
				
			
			gridLineInds.push_back(i*gridX + j + gridX + 1);
			gridLineInds.push_back(i*gridX + j + 1);
			gridLineInds.push_back(i*gridX + j);
			

		}
	}
	return 0;
}
int toFloat(array<float, 4> & facePoints, float *vert)
{
	vert[0] = facePoints[0];
	vert[1] = facePoints[1];
	vert[2] = facePoints[2];
	vert[3] = facePoints[3];
	return 0;

}

//test code for picking 
glm::vec3 findCLosestPoint(vec3 rayStartPos, vec3 rayEndPos, vec3 pointPos, double *proj)
{
	glm::vec3 rayVector = rayEndPos - rayStartPos;
	double raySquared = glm::dot(rayVector, rayVector);
	glm::vec3 projection = pointPos - rayStartPos;
	double projectionVal = glm::dot(projection, rayVector);
	*proj = projectionVal / raySquared;
	glm::vec3 closestPoint = rayStartPos + glm::vec3(rayVector.x * (*proj), rayVector.y * (*proj), rayVector.z * (*proj));
	return closestPoint;
}

bool rayTest(glm::vec3 pointPos, glm::vec3 startPos, glm::vec3 endPos, glm::vec3 *closestPoint, double *proj, double epsilon)

{
	*closestPoint = findCLosestPoint(startPos, endPos, pointPos, proj);
	double len = glm::distance2(*closestPoint, pointPos);
	return len < epsilon;
}

bool rayTestPoints(std::vector<Vertex> vert, vec3 start, vec3 end, unsigned int *id, double *proj, double epsilon, int maxRange)
{
	unsigned int pointID = maxRange + 1;
	bool foundCollision = false;
	double minDistToStart = 10000000.0;
	double distance;
	glm::vec3 point;
	for (unsigned int i = 0; i<maxRange; ++i)
	{
		vec3 pointPos = glm::vec3(vert[i].Position[0], vert[i].Position[1], vert[i].Position[2]);
		if (rayTest(pointPos, start, end, &point, proj, epsilon))
		{
			distance = glm::distance2(start, point);
			if (distance<minDistToStart)
			{
				minDistToStart = distance;
				pointID = i;
				foundCollision = true;
			}
		}
	}
	*id = pointID;
	return foundCollision;
}