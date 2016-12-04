#include <misc05_picking\gridOps.h>

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