#include <misc05_picking/gridOps.h>
#include <misc05_picking/bezOps.h>
#include <misc05_picking/ray_casting.h>
#include <misc05_picking/math_ops.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <array>
#include <stack>   
#include <sstream>

#include <GL/glew.h>
#include <GL/glut.h>
#include <glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace glm;
#include <AntTweakBar.h>
#include <common/shader.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/tga.h>

using namespace std;

const int window_width = 1024, window_height = 768;
typedef struct campos {
	float x, y, z;
};

bool flagc = false;
bool BEZIER=false, BEZIER_CTRL=true, BEZIER_SURF=false;
campos camp;
float LightIDUni;

// function prototypes
int initWindow(void);
void initOpenGL(void);
int drawObject(const int vertID, const vector<Vertex>& objVerts,
			   const vector<unsigned short>& objIdcs, int triang);
void createVAOs(vector<Vertex>&, vector<unsigned short>&, int);
void createObjects(void);
int rayCastingFunc(void);
void pickObject(void);
void renderScene(void);
void cleanup(void);
static void keyCallback(GLFWwindow*, int, int, int, int);
static void mouseCallback(GLFWwindow*, int, int, int);
void drawAxisgrid(void);
void genPoints(void);
void fileRead(void);
void fileWrite(void);

void changecamera(int);


// GLOBAL VARIABLES
GLFWwindow* window;
bool flagr = false;
glm::mat4 gProjectionMatrix;
glm::mat4 gViewMatrix;

GLuint gPickedIndex = -1;
unsigned int id;
std::string gMessage;

GLuint programID;
GLuint textureprogramID;
GLuint pickingProgramID;

const GLuint NumObjects = 12;	// ATTN: THIS NEEDS TO CHANGE AS YOU ADD NEW OBJECTS
GLuint VertexArrayId[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
GLuint VertexBufferId[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
GLuint IndexBufferId[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

size_t NumIndices[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
size_t VertexBufferSize[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
size_t IndexBufferSize[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

GLuint MatrixID, ModelMatrixID, ViewMatrixID, ProjMatrixID, tModelMatrixID,
	   tViewMatrixID, tProjMatrixID;
GLuint PickingMatrixID, pickingColorID, LightID, texture, textureID;
GLint gX = 0.0;
GLint gZ = 0.0;

long width, height;

//for grid variables
std::vector<Vertex> vg, vc, controlPoints, bezCtrlVerts, bezSurfVerts;
// ci for linesgrid, cpi for points on grid 
vector<unsigned short> gridIndices, controlIndices, controlPntInd,
						bezCtrlInds,  bezSurfInds;
std::vector<unsigned short> gridTriangs;
std::vector<unsigned short> gridLineInd;

vector<Vertex> faceVerts;
vector<GLushort> faceIdcs;

//for camera
bool fvertical = false, fsides = false;
bool falgSave = false, flagLoad = false;
GLint flagcountup = 0, flagcountsides = 0;
campos temp = { 20.0, 20.0, 20.0 };
float LightIDUniami; // What is this variable for?


void loadObject(char* file, glm::vec4 color,
				vector<Vertex>& outVertices,
				vector<GLushort>& outIndices, int ObjectId)
{
	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ(file, vertices, normals);

	std::vector<GLushort> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(vertices, normals, indices, indexed_vertices, indexed_normals);

	const size_t vertCount = indexed_vertices.size();
	const size_t idxCount = indices.size();

	// populate output arrays
	outVertices.resize(vertCount);// = new Vertex[vertCount];
	for (int i = 0; i < vertCount; i++) {
		outVertices[i].SetPosition(&indexed_vertices[i].x);
		outVertices[i].SetNormal(&indexed_normals[i].x);
		outVertices[i].SetColor(&color[0]);
	}
	outIndices.resize(idxCount); // = new GLushort[idxCount];
	for (int i = 0; i < idxCount; i++) {
		outIndices[i] = indices[i];
	}

	// set global variables!!
	NumIndices[ObjectId] = idxCount;
	VertexBufferSize[ObjectId] = sizeof(outVertices[0]) * vertCount;
	IndexBufferSize[ObjectId] = sizeof(GLushort) * idxCount;
}


void createObjects(void)
{
	//-- COORDINATE AXES --//
	vector<Vertex> CoordVerts =
	{
		{ { 0.0, 0.0, 0.0, 1.0 },{ 1.0, 0.0, 0.0, 1.0 },{ 0.0, 0.0, 1.0 } },
		{ { 10.0, 0.0, 0.0, 1.0 },{ 1.0, 0.0, 0.0, 1.0 },{ 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 0.0, 1.0 },{ 0.0, 1.0, 0.0, 1.0 },{ 0.0, 0.0, 1.0 } },
		{ { 0.0, 10.0, 0.0, 1.0 },{ 0.0, 1.0, 0.0, 1.0 },{ 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 0.0, 1.0 },{ 0.0, 0.0, 1.0, 1.0 },{ 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 10.0, 1.0 },{ 0.0, 0.0, 1.0, 1.0 },{ 0.0, 0.0, 1.0 } },
	};
	VertexBufferSize[0] = CoordVerts.size() * sizeof(CoordVerts[0]);
	vector<unsigned short> CoordIndices;// = {0, 1, 2, 3, 4, 5};
	createVAOs(CoordVerts, CoordIndices, 0);

	//-- GRID --//
	VertexBufferSize[1] = vg.size() * sizeof(vg[0]);
	createVAOs(vg, gridIndices, 1);

	// ATTN: load your models here
	string baseFile = "modules/aru/aru1.obj";
	//string baseFile = "modules/aru/cube.obj";
	//string baseFile = "modules/sainath/poorna.obj";
	char* fname = (char*)baseFile.c_str();
	loadObject(fname, glm::vec4(1.0, 0.80, 0.60, 1.0), faceVerts, faceIdcs, 2);
	createVAOs(faceVerts, faceIdcs, 2);

	//VertexBufferSize[3] = vc.size() * sizeof(vc[0]);
	//createVAOs(vc, controlIndices, 3);
	VertexBufferSize[3] = controlPoints.size() * sizeof(controlPoints[0]);
	createVAOs(controlPoints, gridLineInd, 3);

	//texture = load_texture_TGA("modules/sainath/poorna.tga", 
//							   &width, &height, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	texture = load_texture_TGA("modules/aru/aaru.tga",&width, &height, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	rayCastingFunc();
	VertexBufferSize[4] = controlPoints.size() * sizeof(controlPoints[0]);
	createVAOs(controlPoints, gridTriangs, 4);

	// Simulate a small grid to generate indices for bezCtrl Pt computation
	int lenX=11, lenY=11;
	vector<vector<int>> gridPtInds;
	vector<vector<Point>> bezCtrlPts;
	vector<Point> controlPointConv, bezSurfPts;
	packGridIndices(lenX, lenY, gridPtInds);

	Vert2Pt(controlPoints, controlPointConv);
	compBezCtrlPoints(controlPointConv, gridPtInds, bezCtrlPts);
	compBezSurf(bezCtrlPts, 5, bezSurfPts);
	cout << " Bezier surface point count " << bezSurfPts.size() << endl;

	string ctfname = "ctrlpts.csv";
	//write2File(ctfname, controlPointConv, 11);
	Lin2VertArr(bezCtrlPts, bezCtrlVerts, 1);
	Pt2Vert(bezSurfPts, bezSurfVerts, 3);
	// Compute Indices for bezier control points
	size_t bezCtrlVertCount = bezCtrlVerts.size();
	size_t bezSurfVertCount = bezSurfVerts.size();
	cout << "BezVertcount " << bezCtrlVertCount << endl;
	for (size_t i=0; i<bezCtrlVertCount ; i++)
	{
		bezCtrlInds.push_back(i);
	}
	for (size_t i=0; i<bezSurfVertCount ; i++)
	{
		bezSurfInds.push_back(i);
	}


	VertexBufferSize[5] = bezCtrlVerts.size()*sizeof(bezCtrlVerts[0]);
	createVAOs(bezCtrlVerts, bezCtrlInds, 5);

	VertexBufferSize[6] = bezSurfVerts.size()*sizeof(bezSurfVerts[0]);
	createVAOs(bezSurfVerts, bezSurfInds, 6);
	
	vector<Point> bezPts;
	Vert2Pt(bezCtrlVerts, bezPts);
	cout << bezPts.size() << endl;
	string btfname = "bzPts.csv";
	//write2File(btfname,  bezPts, 44);

}


int rayCastingFunc()
{
	float a[4], b[4], c[4], d[4];
	vector<float> OV(4); //output verts
	float zDirection[] = { 0.0,0.0,1.0,1.0 };

	for (int i = 0; i<controlPoints.size(); i++)
	{
		toFloat(controlPoints[i].Position, d);

		for (int j = 0; j < faceIdcs.size() - 2; j=j+3)
		{
      		toFloat(faceVerts[faceIdcs[j]].Position, a);
			toFloat(faceVerts[faceIdcs[j + 1]].Position, b);
			toFloat(faceVerts[faceIdcs[j + 2]].Position, c);
			ray_cast(a, b, c,d, zDirection, OV); //OV- OutputVerts
			if(OV[0]>0 && OV[1]>0  && OV[2]>0)
			{			
				// a temporary variable for geting co ordinates from barycentric
				std::array<float, 4> outVerts;  
				//outVerts[0] = OV[0] * a[0] + OV[1] * b[1]+ OV[2] * c[0]; 
				//outVerts[1] = OV[0] * a[1] + OV[1] * b[1] + OV[2] * c[1];
				outVerts[2] = OV[0] * a[2] + OV[1] * b[2] + OV[2] * c[2]+1.0;
				//outVerts[3] = 1.0;
				if (controlPoints[i].Position[2] < outVerts[2])
				{
					controlPoints[i].Position[2] = outVerts[2];
				}
			}
		}

	}


	return 0;
}


int drawObject(const int vertID, const vector<Vertex>& objVerts,
	const vector<unsigned short>& objIdcs, int primitive = 0)
{
	glBindVertexArray(VertexArrayId[vertID]);	// draw object
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[vertID]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, objIdcs.size() * sizeof(Vertex), &objVerts[0]);
	switch (primitive) {
	case 0:
		glDrawElements(GL_POINTS, objIdcs.size(), GL_UNSIGNED_SHORT, (void*)0);
		break;
	case 1:
		glDrawElements(GL_LINES, objIdcs.size(), GL_UNSIGNED_SHORT, (void*)0);
		break;
	case 2:
		glDrawElements(GL_LINE_STRIP, objIdcs.size(), GL_UNSIGNED_SHORT, (void*)0);
		break;
	case 3:
		glDrawElements(GL_TRIANGLES, objIdcs.size(), GL_UNSIGNED_SHORT, (void*)0);
		break;
	case 4:
		glDrawElements(GL_TRIANGLE_STRIP, objIdcs.size(), GL_UNSIGNED_SHORT, (void*)0);
		break;
	}
	return 0;
}


bool flags = true; // what is this flag for? And why is it in the middle of nowhere?

void renderScene(void)
{
	//ATTN: DRAW YOUR SCENE HERE. MODIFY/ADAPT WHERE NECESSARY!

	// Dark blue background
	glClearColor(0.0f, 0.2f, 0.2f, 0.0f);
	// Re-clear the screen for real rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);
	{
		glm::vec3 lightPos = glm::vec3(0, 15, -15);
		//glm::vec3 lightPos1 = glm::vec3(0, 0, 0);

		glm::mat4x4 ModelMatrix = glm::mat4(1.0);
		//glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;

		glUniform1f(LightIDUniami, 0.5);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glUniformMatrix4fv(ProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		
		glBindVertexArray(VertexArrayId[0]);	// draw CoordAxes
		glDrawArrays(GL_LINES, 0, 6);
		
		glBindVertexArray(VertexArrayId[1]);
		glDrawArrays(GL_LINES, 0, vg.size()); //for co-ordinate grid on XZ direction!
		
		if (flags) {
			drawObject(2, faceVerts, faceIdcs, 3); //for drawing face
		}
		if(flagc)
		{
			glBindVertexArray(VertexArrayId[3]);
			// draws grid using points generated earlier
			//cout << " Control Point Size " << controlPoints.size() << endl;
			drawObject(3, controlPoints, gridLineInd, 1);

		 	//for grid lines
			glPointSize(5.0);
			glBindVertexArray(VertexArrayId[4]);
			//for grid points //texture is drwan below in textrue programID
			drawObject(4, controlPoints, gridTriangs, 0); 
			glBindVertexArray(0);
		}
		if (BEZIER)	{
			glBindVertexArray(VertexArrayId[5]);
			// draws Bezier Control Grid using generated control points
			drawObject(5, bezCtrlVerts, bezCtrlInds, 0);

			if (BEZIER_SURF) {
				glBindVertexArray(VertexArrayId[6]);
				// draws Bezier Surface computed
				drawObject(6, bezSurfVerts, bezSurfInds, 0);
			}
		}
		if (falgSave) {
			fileRead();
		}
		if (flagLoad) {
			fileWrite();
		}

		// Update Camera position
		// TODO: Cleaup - use right names, check reason for object vanishing at
		// certain camera positions
		if (fvertical) {
			changecamera(flagcountup);
		}
		if (fsides)	{
			changecamera(flagcountsides);
		}
		if (flagr) {
			temp = { 20.0, 20.0, 20.0 };
			camp = temp;
			gViewMatrix = glm::lookAt(glm::vec3(camp.x, camp.y, camp.z),
										glm::vec3(0, 0, 0), // and looks at the origin
										glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
			);
			flagcountup = 0;
			flagcountsides = 0;
			flagr = !flagr;
		}
		glBindVertexArray(0);
	}
	glUseProgram(textureprogramID);
	{
		glm::mat4x4 ModelMatrix = glm::mat4(1.0);
		glUniformMatrix4fv(tViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glUniformMatrix4fv(tProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
		glUniformMatrix4fv(tModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(textureID, 0);
		if(flagc){
		glBindVertexArray(VertexArrayId[4]);
		drawObject(4, controlPoints, gridTriangs, 4);  //for texture
		}
	}
	glUseProgram(0);
	// Draw GUI
	//TwDraw();

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}


void pickObject(void)
{
	// Clear the screen in white
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(pickingProgramID);
	{
		glm::mat4 ModelMatrix = glm::mat4(1.0); // TranslationMatrix * RotationMatrix;
		glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);

// ATTN: DRAW YOUR PICKING SCENE HERE. REMEMBER TO SEND IN A DIFFERENT PICKING COLOR FOR EACH OBJECT BEFOREHAND
		glBindVertexArray(0);
	}
	glUseProgram(0);
	// Wait until all the pending drawing commands are really done.
	// Ultra-mega-over slow ! 
	// There are usually a long time between glDrawElements() and
	// all the fragments completely rasterized.
	glFlush();
	glFinish();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Read the pixel at the center of the screen.
	// You can also use glfwGetMousePos().
	// Ultra-mega-over slow too, even for 1 pixel, 
	// because the framebuffer is on the GPU.
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glm::mat4 ModelMatrix = glm::mat4(1.0);
	mat4 nModelMatrix = gViewMatrix * ModelMatrix;



	unsigned char data[4];
	GLfloat zpos;

	glReadPixels(xpos, window_height - ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top

	glm::vec3 startMousePos = glm::unProject(glm::vec3(xpos, window_height-ypos, 0.0f),
											 nModelMatrix,
											 gProjectionMatrix,
											 vec4(viewport[0],
											 viewport[1], 
											 viewport[2],
											 viewport[3]));
	glm::vec3 endMousePos = glm::unProject(glm::vec3(xpos, window_height-ypos, 1.0f),
										   nModelMatrix,
										   gProjectionMatrix,
										   vec4(viewport[0],
										   viewport[1],
										   viewport[2],
										   viewport[3]));

	double epsilon = 0.1;
	double proj;
	bool found = rayTestPoints(controlPoints, startMousePos, endMousePos, &id, &proj, epsilon, controlPoints.size());
	//bool found_Hor = rayTestPoints(Mesh_Hor, startMousePos, endMousePos, &id_Mesh_Hor, &proj, epsilon, 41);
	//bool found_Ver = rayTestPoints(Mesh_Vert, startMousePos, endMousePos, &id_Mesh_Ver, &proj, epsilon, 41);
	// Convert the color back to an integer ID
	gPickedIndex = id;

	if (gPickedIndex < 0 && gPickedIndex > 441) { // Full white, must be the background !
		std::ostringstream oss;
		gMessage = "background";
		oss << gMessage;
		gMessage = oss.str();
	}
	else {
		std::ostringstream oss;
		oss << "point: " << gPickedIndex;
		gMessage = oss.str();
	}

	// Uncomment these lines to see the picking shader in effect
	//glfwSwapBuffers(window);
	//continue; // skips the normal rendering
}


void moveVertex()
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glm::vec4 vp = glm::vec4(viewport[0], viewport[1], viewport[2], viewport[3]);
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	mat4 nModelMatrix = gViewMatrix;
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	
	if (state == GLFW_PRESS) {
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			if(id>=0&&id<=440) {
				glm::vec3 p = glm::project(glm::vec3(controlPoints[id].Position[0],
													 controlPoints[id].Position[1], 
													 controlPoints[id].Position[2]),
													 nModelMatrix,
													 gProjectionMatrix, vp);
				glm::vec3 W = glm::unProject(glm::vec3(xpos, window_height-ypos, p.z),
													 nModelMatrix,
													 gProjectionMatrix, vp);
				float coords[3] = { controlPoints[id].Position[0], W.y, W.z };
				controlPoints[id].SetPosition(coords);
			}
		} else {
			if (id >= 0 && id <= 440) {
				glm::vec3 p = glm::project(glm::vec3(controlPoints[id].Position[0],
													 controlPoints[id].Position[1], 
													 controlPoints[id].Position[2]),
													 nModelMatrix,
													 gProjectionMatrix, vp);
				glm::vec3 W = glm::unProject(glm::vec3(xpos, window_height-ypos, p.z), 
													 nModelMatrix,
													 gProjectionMatrix, vp);
				float coords[3] = { W.x, W.y, controlPoints[id].Position[2] };
				controlPoints[id].SetPosition(coords);
			}
		}
	}
	// TODO: Shouldn't have to recreate VAO - Figure out how to just update values
	// of existing VAO
	createVAOs(controlPoints, gridTriangs, 4);
}


int initWindow(void)
{
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(window_width, window_height, "Kanamathareddy,Poorna(3591-5896)", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Initialize the GUI
	//TwInit(TW_OPENGL_CORE, NULL);
	//TwWindowSize(window_width, window_height);
	//TwBar * GUI = TwNewBar("Picking");
	//TwSetParam(GUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	//TwAddVarRW(GUI, "Last picked object", TW_TYPE_STDSTRING, &gMessage, NULL);

	// Set up inputs
	glfwSetCursorPos(window, window_width / 2, window_height / 2);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseCallback);

	return 0;
}


void initOpenGL(void)
{

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	gProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	//gProjectionMatrix = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	gViewMatrix = glm::lookAt(glm::vec3(20.0f, 20.0f, 20.0f),	// eye
		glm::vec3(0.0, 0.0, 0.0),	// center
		glm::vec3(0.0, 1.0, 0.0));	// up

									// Create and compile our GLSL program from the shaders
	programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");
	pickingProgramID = LoadShaders("Picking.vertexshader", "Picking.fragmentshader");
	textureprogramID = LoadShaders("texture.vertexshader", "texture.fragmentshader");
	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ProjMatrixID = glGetUniformLocation(programID, "P");
	tModelMatrixID = glGetUniformLocation(textureprogramID, "tM");
	tViewMatrixID = glGetUniformLocation(textureprogramID, "tV");
	tProjMatrixID = glGetUniformLocation(textureprogramID, "tP");

	PickingMatrixID = glGetUniformLocation(pickingProgramID, "MVP");
	// Get a handle for our "pickingColorID" uniform
	pickingColorID = glGetUniformLocation(pickingProgramID, "PickingColor");
	// Get a handle for our "LightPosition" uniform
	LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	
	LightIDUni = glGetUniformLocation(programID, "LightPower");
	LightIDUniami = glGetUniformLocation(programID, "ambientpower");
	textureID = glGetUniformLocation(textureprogramID, "faceTexture");
	createObjects();
	//rayCastingFunc();
}


void createVAOs(std::vector<Vertex>& Vertices, vector<unsigned short>& Indices, int ObjectId)
{
	GLenum ErrorCheckValue = glGetError();
	const size_t VertexSize = sizeof(Vertices[0]);
	const size_t RgbOffset = sizeof(Vertices[0].Position);
	const size_t Normaloffset = sizeof(Vertices[0].Color) + RgbOffset;
	const size_t UVoffsset = sizeof(Vertices[0].Normal)+Normaloffset;
	
	// Create Vertex Array Object
	glGenVertexArrays(1, &VertexArrayId[ObjectId]);	//
	glBindVertexArray(VertexArrayId[ObjectId]);		//
	// Create Buffer for vertex data
	glGenBuffers(1, &VertexBufferId[ObjectId]);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[ObjectId]);
	glBufferData(GL_ARRAY_BUFFER, VertexBufferSize[ObjectId], &Vertices[0], GL_STATIC_DRAW);

	// Create Buffer for indices
	//if (Indices != NULL) {
	if (!Indices.empty()) {
		IndexBufferSize[ObjectId] = sizeof(unsigned short) * Indices.size();
		glGenBuffers(1, &IndexBufferId[ObjectId]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[ObjectId]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexBufferSize[ObjectId],
											  &Indices[0], GL_STATIC_DRAW);
	}

	// Assign vertex attributes
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)Normaloffset);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)UVoffsset);

	glEnableVertexAttribArray(0);	// position
	glEnableVertexAttribArray(1);	// color
	glEnableVertexAttribArray(2);	// Normal
	glEnableVertexAttribArray(3);	// UV

	// Disable our Vertex Buffer Object 
	glBindVertexArray(0);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create a VBO: %s \n",
			gluErrorString(ErrorCheckValue)
		);
	}
}


void cleanup(void)
{
	// Cleanup VBO and shader
	for (int i = 0; i < NumObjects; i++) {
		glDeleteBuffers(1, &VertexBufferId[i]);
		glDeleteBuffers(1, &IndexBufferId[i]);
		glDeleteVertexArrays(1, &VertexArrayId[i]);
	}
	glDeleteProgram(programID);
	glDeleteProgram(pickingProgramID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}


static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// ATTN: MODIFY AS APPROPRIATE
	if (action == GLFW_PRESS) {
		switch (key)
		{
		case GLFW_KEY_D:
			BEZIER = !BEZIER;
			
		case GLFW_KEY_S:
			BEZIER_SURF = !BEZIER_SURF;
			break;
		case GLFW_KEY_R:
			flagr = true;
			break;
		case GLFW_KEY_F:
			flags = !flags;
			break;
		case GLFW_KEY_C:
			flagc = !flagc;
			break;
		case GLFW_KEY_K:
			falgSave = !falgSave;
			break;
		case GLFW_KEY_L:
			flagLoad = !flagLoad;
			break;
		case GLFW_KEY_UP:
			fvertical = !fvertical;
			flagcountup++;
			if (flagcountup == 16)
			{
				flagcountup = 0;
			}
			break;
		case  GLFW_KEY_RIGHT:
			fsides = !fsides;
			flagcountsides++;
			if (flagcountsides == 16)
			{
				flagcountsides = 0;
			}
			break;
		case  GLFW_KEY_LEFT:
			fsides = !fsides;
			flagcountsides--;
			if (flagcountsides == 16)
			{
				flagcountsides = 0;
			}
			break;
		case GLFW_KEY_DOWN:
			fvertical = !fvertical;
			flagcountup--;
			if (flagcountup == 16)
			{
				flagcountup = 0;
			}
			break;
		default:
			break;
		}
	}

	if (action == GLFW_RELEASE) {
		switch (key)
		{
		case GLFW_KEY_UP:
			fvertical = !fvertical;
			break;
		case GLFW_KEY_DOWN:
			fvertical = !fvertical;
			break;
		case GLFW_KEY_RIGHT:
			fsides = !fsides;
			break;
		case GLFW_KEY_LEFT:
			fsides = !fsides;
			break;
		}
	}
}


static void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		pickObject();
	}
}


void drawAxisgrid(void)
{
	size_t ind = 0;
	// Generate points to draw grid
	for (int i = -10; i <= 10; i++) {
		array<float, 4> color = { 1.0, 1.0, 1.0, 1.0 };
		array<float, 3> normal = { 0.0, 0.0, 1.0 };
		array<float, 4> position = { -10.0, 0.0, float(i), 1.0f };
		Vertex tmp;
		tmp.Position = position;
		tmp.Color = color;
		tmp.Normal = normal;
		vg.push_back(tmp);
		gridIndices.push_back(ind);
		ind++;

		position = { 10.0, 0.0, float(i), 1.0f };
		Vertex tmp1;
		tmp1.Color = color;
		tmp1.Normal = normal;
		tmp1.Position = position;
		vg.push_back(tmp1);
		gridIndices.push_back(ind);
		ind++;
	}

	for (int i = -10; i <= 10; i++) {
		array<float, 4> color = { 1.0, 1.0, 1.0, 1.0 };
		array<float, 3> normal = { 0.0, 0.0, 1.0 };
		array<float, 4> position = { float(i), 0.0, -10.0, 1.0f };
		Vertex tmp;
		tmp.Position = position;
		tmp.Color = color;
		tmp.Normal = normal;
		vg.push_back(tmp);
		gridIndices.push_back(ind);
		ind++;

		position = { float(i), 0.0, 10.0, 1.0f };
		Vertex tmp1;
		tmp1.Color = color;
		tmp1.Normal = normal;
		tmp1.Position = position;
		vg.push_back(tmp1);
		gridIndices.push_back(ind);
		ind++;
	}
	cout << "Grid Size " << vg.size() << endl;
}


void drawcontrol(void) // use this function only if required
{
	size_t ind = 0;
	// Generate points to draw grid
	for (int i = 0; i <= 20; i++) {
		array<float, 4> color = { 0.0, 1.0, 0.0, 0.5 };
		array<float, 3> normal = { 0.0, 0.0, 1.0 };
		array<float, 4> position = { -10.0, float(i), -5.0, 1.0f };
		Vertex tmp;
		tmp.Position = position;
		tmp.Color = color;
		tmp.Normal = normal;
		vc.push_back(tmp);
		controlIndices.push_back(ind);
		ind++;

		position = { 10.0, float(i), -5.0, 1.0f };
		Vertex tmp1;
		tmp1.Color = color;
		tmp1.Normal = normal;
		tmp1.Position = position;
		vc.push_back(tmp1);
		controlIndices.push_back(ind);
		ind++;
	}
	for (int i = -10; i <= 10; i++) {
		array<float, 4> color = { 0.0, 1.0, 0.0, 0.5 };
		array<float, 3> normal = { 0.0, 0.0, 1.0 };
		array<float, 4> position = { float(i), 0.0, -5.0, 1.0f };
		Vertex tmp;
		tmp.Position = position;
		tmp.Color = color;
		tmp.Normal = normal;
		vc.push_back(tmp);
		controlIndices.push_back(ind);
		ind++;

		position = { float(i), 20.0, -5.0, 1.0f };
		Vertex tmp1;
		tmp1.Color = color;
		tmp1.Normal = normal;
		tmp1.Position = position;
		vc.push_back(tmp1);
		controlIndices.push_back(ind);
		ind++;
	}

	cout << "control Size " << vc.size() << endl;
}


void genPoints()
{
	size_t ind = 0;
	int widthGrid = 21;
	int heightGrid = 21;
	genGridTriangs(widthGrid, heightGrid, gridTriangs);
	genGridInd(widthGrid, heightGrid, gridLineInd);
	float s = 0.1, t = 0.9;
	// Generate points to draw grid
	for (float j = heightGrid-1; j >= 0; j=j-1) {
		//cout << "index " << ind << endl;
		size_t indBegX = ind;
		for (float i = -widthGrid/2; i <= widthGrid/2; i=i+1) {
			array<float, 4> color = { 0.0, 1.0, 0.0, 1.0 };
			array<float, 3> normal = { 0.0, 0.0, 1.0 };
			array<float, 4> position = { float(i), float(j), 0.0, 1.0f };
			array<float, 2> texture = { t,s };
			s = s + (1/(float(widthGrid-1)));
			
			Vertex tmp;
			tmp.Position = position;
			tmp.Color = color;
			tmp.Normal = normal;
			tmp.textureCords = texture;
			controlPoints.push_back(tmp);
			controlPntInd.push_back(ind);
			
			//cout << ":"<<ind << ":" << ind + 1 << ":" << ind + widthGrid + 1 << ":" << ind + widthGrid<<endl;
			ind++;
		}
		controlPntInd.push_back(indBegX);
		s = 0.1;
		t = t - (1 / (float(heightGrid-1)));

	}
}


void changecamera(int i)
{
	float r = 20 * 1.732;
	if (fvertical)
	{
		//r = temp.y;
		camp.x = temp.x;
		camp.z = (sqrt(pow(r, 2) - pow(camp.x, 2))*cosf(2 * 3.1415*(i + 2) / 16));
		camp.y = (sqrt(pow(r, 2) - pow(camp.x, 2))*sinf(2 * 3.1415*(i + 2) / 16));
		gViewMatrix = glm::lookAt(
			glm::vec3(camp.x, camp.y, camp.z),
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
		temp.z = camp.z;
		temp.y = camp.y;

	}
	else if (fsides)
	{
		//r = temp.z;
		camp.y = temp.y;
		camp.x = (sqrt(pow(r, 2) - pow(camp.y, 2))*cosf(2 * 3.1415*(i + 2) / 16));
		camp.z = (sqrt(pow(r, 2) - pow(camp.y, 2))*sinf(2 * 3.1415*(i + 2) / 16));
		gViewMatrix = glm::lookAt(
			glm::vec3(camp.x, camp.y, camp.z),
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
		temp.x = camp.x;
		temp.z = camp.z;
	}
}


void fileWrite()
{
	FILE *fp;
	fp = fopen("cm.p3", "w+");
	printf("Writing in Progress!");
	for (int i = 0; i < controlPoints.size(); i++)
	{
		for (int j = 0; j < 4; j++) {
			fprintf(fp, "%0.2f", controlPoints[i].Position[j]);
			fprintf(fp, "%s", ",");
		}
		for (int j = 0; j < 4; j++) {
			fprintf(fp, "%0.2f", controlPoints[i].Color[j]);
			fprintf(fp, "%s", ",");
		}
		for (int j = 0; j < 3; j++) {
			fprintf(fp, "%0.2f", controlPoints[i].Normal[j]);
			fprintf(fp, "%s", ",");
		}
		if (i != (440))fprintf(fp, "%s", "\n");
	}
	fprintf(fp, "%s", "p");
	fclose(fp);
	printf("\nWriting Completed\n");
}


void fileRead()
{
		FILE *fp;
		fp = fopen("cm.p3", "r");
		if (!fp)
			printf("no such file");
		char cRead = NULL;
		char*  vertexValue = (char*)malloc(6 * sizeof(char));
		int pos = 0, cnt = 0, j = 0;
		float floatValofVertex = 0;
		int i = 0;
		while (cRead != 'p') {
			cRead = (char)fgetc(fp);
			if (cRead == ',')
			{
				j = 0;
				floatValofVertex = (float)atof(vertexValue);
				if (cnt == 0)
				{
					controlPoints[pos].Position[0] = floatValofVertex;
				}
				else if (cnt == 1)
				{
					controlPoints[pos].Position[1] = floatValofVertex;
				}
				else if (cnt == 2)
				{
					controlPoints[pos].Position[2] = floatValofVertex;
				}
				else if (cnt == 3)
				{
					controlPoints[pos].Position[3] = floatValofVertex;
				}
				else if (cnt == 4)
				{
					controlPoints[pos].Color[0] = floatValofVertex;
				}
				else if (cnt == 5)
				{
					controlPoints[pos].Color[1] = floatValofVertex;
				}
				else if (cnt == 6)
				{
					controlPoints[pos].Color[2] = floatValofVertex;
				}
				else if (cnt == 7)
				{
					controlPoints[pos].Color[3] = floatValofVertex;
				}
				else if (cnt == 8)
				{
					controlPoints[pos].Normal[0] = floatValofVertex;
				}
				else if (cnt == 9)
				{
					controlPoints[pos].Normal[1] = floatValofVertex;
				}
				else if (cnt == 10)
				{
					controlPoints[pos].Normal[2] = floatValofVertex;
				}
				cnt++;
				if (cnt == 11)
					pos++;
				continue;
			}
			else if (cRead != '\n' && cRead != 'p') {
				vertexValue[j] = cRead;
				j++;
			}
			if (cRead == '\n') {
				cnt = 0;
			}
		}
		fclose(fp);
		
		printf("\nReading Completed: Control[0] value : %0.2f\n", controlPoints[0].Position[0]);
		
}


int main(void)
{
	// initialize window
	int errorCode = initWindow();
	if (errorCode != 0)
		return errorCode;
	drawAxisgrid();
	// not using this one. might require in future //for drawing lines on control points
	//drawcontrol(); 
	genPoints();
	
	// initialize OpenGL pipeline
	initOpenGL();
	do
	{
		// DRAWING POINTS
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
			moveVertex();
		renderScene();
	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
			glfwWindowShouldClose(window) == 0);
	cleanup();
	return 0;
}
