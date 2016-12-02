// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <array>
#include <stack>   
#include <sstream>
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <glfw3.h>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
using namespace glm;
// Include AntTweakBar
#include <AntTweakBar.h>

#include <common/shader.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

const int window_width = 1024, window_height = 768;

typedef struct Vertex {
	float Position[4];
	float Color[4];
	float Normal[3];
	void SetPosition(float *coords) {
		Position[0] = coords[0];
		Position[1] = coords[1];
		Position[2] = coords[2];
		Position[3] = 1.0;
	}
	void SetColor(float *color) {
		Color[0] = color[0];
		Color[1] = color[1];
		Color[2] = color[2];
		Color[3] = color[3];
	}
	void SetNormal(float *coords) {
		Normal[0] = coords[0];
		Normal[1] = coords[1];
		Normal[2] = coords[2];
	}
};
typedef struct campos {
	float x, y, z;
};

campos camp;
float LightIDUni;
// function prototypes
int initWindow(void);
void initOpenGL(void);
void loadObject(char*, glm::vec4, Vertex * &, GLushort* &, int);
void createVAOs(Vertex[], GLushort[], int);
void createObjects(void);
void pickObject(void);
void renderScene(void);
void cleanup(void);
static void keyCallback(GLFWwindow*, int, int, int, int);
static void mouseCallback(GLFWwindow*, int, int, int);
void drawgrid(void);
void changecamera(int);
void translate(Vertex[], int);
vec3 calcvecf(float, vec4);
// GLOBAL VARIABLES
GLFWwindow* window;
bool flagr = false;
glm::mat4 gProjectionMatrix;
glm::mat4 gViewMatrix;

GLuint gPickedIndex = -1;
std::string gMessage;

GLuint programID;
GLuint pickingProgramID;

const GLuint NumObjects = 11;	// ATTN: THIS NEEDS TO CHANGE AS YOU ADD NEW OBJECTS
GLuint VertexArrayId[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
GLuint VertexBufferId[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
GLuint IndexBufferId[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

size_t NumIndices[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
size_t VertexBufferSize[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
size_t IndexBufferSize[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

GLuint MatrixID;
GLuint ModelMatrixID;
GLuint ViewMatrixID;
GLuint ProjMatrixID;
GLuint PickingMatrixID;
GLuint pickingColorID;
GLuint LightID, LightID1, LightID2;

GLint gX = 0.0;
GLint gZ = 0.0;

// animation control
bool animation = false;
GLfloat phi = 0.0;

//for grid variables
Vertex vx[44], vy[44];
//Vertex vcx[44], vcy[44];
float whitecolor[4] = { 1.0, 1.0, 1.0, 1.0 };

//for camera

bool fvertical = false, fsides = false, fverticalpen = true, fsidespen = true;
GLint flagcountup = 0, flagcountsides = 0;
campos temp = { 20.0, 20.0, 20.0 };


float LightIDUniami;
void loadObject(char* file, glm::vec4 color, Vertex * &out_Vertices, GLushort* &out_Indices, int ObjectId)
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
	out_Vertices = new Vertex[vertCount];

	for (int i = 0; i < vertCount; i++) {

		out_Vertices[i].SetPosition(&indexed_vertices[i].x);
		out_Vertices[i].SetNormal(&indexed_normals[i].x);
		out_Vertices[i].SetColor(&color[0]);

	}
	out_Indices = new GLushort[idxCount];
	for (int i = 0; i < idxCount; i++) {
		out_Indices[i] = indices[i];
	}

	// set global variables!!
	NumIndices[ObjectId] = idxCount;
	VertexBufferSize[ObjectId] = sizeof(out_Vertices[0]) * vertCount;
	IndexBufferSize[ObjectId] = sizeof(GLushort) * idxCount;
}



void createObjects(void)
{
	//-- COORDINATE AXES --//
	Vertex CoordVerts[] =
	{
		{ { 0.0, 0.0, 0.0, 1.0 },{ 1.0, 0.0, 0.0, 1.0 },{ 0.0, 0.0, 1.0 } },
		{ { 10.0, 0.0, 0.0, 1.0 },{ 1.0, 0.0, 0.0, 1.0 },{ 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 0.0, 1.0 },{ 0.0, 1.0, 0.0, 1.0 },{ 0.0, 0.0, 1.0 } },
		{ { 0.0, 10.0, 0.0, 1.0 },{ 0.0, 1.0, 0.0, 1.0 },{ 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 0.0, 1.0 },{ 0.0, 0.0, 1.0, 1.0 },{ 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 10.0, 1.0 },{ 0.0, 0.0, 1.0, 1.0 },{ 0.0, 0.0, 1.0 } },
	};

	VertexBufferSize[0] = sizeof(CoordVerts);	// ATTN: this needs to be done for each hand-made object with the ObjectID (subscript)
	createVAOs(CoordVerts, NULL, 0);

	//-- GRID --//
	VertexBufferSize[1] = sizeof(vx);
	createVAOs(vx, NULL, 1);
	VertexBufferSize[2] = sizeof(vy);
	createVAOs(vy, NULL, 2);

	//createVAOs(vcx, NULL, 4);
	//createVAOs(vcy, NULL, 4);
	// ATTN: create your grid vertices here!

	//-- .OBJs --//

	// ATTN: load your models here
	Vertex* Verts_torus;
	Vertex* Verts_cylinder1;
	Vertex* Verts_rectarm;
	Vertex* Verts_cylinder2;
	Vertex* Verts_arm2;
	Vertex* Verts_pen;
	Vertex* Verts_penb;

	GLushort* Idcs_torus;
	loadObject("modules/aru/AaruKalaikannan_meshlab.obj", glm::vec4(1.0, 0.80, 0.60, 1.0), Verts_torus, Idcs_torus, 3);
	//loadObject("modules/sainath/poorna1.obj", glm::vec4(1.0, 0.80, 0.60, 1.0), Verts_torus, Idcs_torus, 3);

	createVAOs(Verts_torus, Idcs_torus, 3);

}

float x = 0, y = 0, z = 0;
glm::vec4 coordinates = glm::vec4(0, 0, 0, 1);
bool flags = true;
void renderScene(void)
{
	//ATTN: DRAW YOUR SCENE HERE. MODIFY/ADAPT WHERE NECESSARY!


	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
	// Re-clear the screen for real rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);
	{
		glm::vec3 lightPos = glm::vec3(0, 15, -15);
		//glm::vec3 lightPos1 = glm::vec3(0, 0, 0);

		glm::mat4x4 ModelMatrix = glm::mat4(1.0);
		//glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;


		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		//glUniform3f(LightID1, lightPos1.x, lightPos1.y, lightPos1.z);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glUniformMatrix4fv(ProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		glBindVertexArray(VertexArrayId[0]);	// draw CoordAxes
		glDrawArrays(GL_LINES, 0, 6);

		glBindVertexArray(VertexArrayId[1]);
		glDrawArrays(GL_LINES, 0, 44);			//grid1

		glBindVertexArray(VertexArrayId[2]);
		glDrawArrays(GL_LINES, 0, 44);		//grid2
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		{


			if (fvertical)
			{
				changecamera(flagcountup);
			}
			if (fsides)
			{
				changecamera(flagcountsides);
			}
		}
		if (flagr)
		{
			temp = { 20.0, 20.0, 20.0 };
			camp = temp;
			gViewMatrix = glm::lookAt(
				glm::vec3(camp.x, camp.y, camp.z),
				glm::vec3(0, 0, 0), // and looks at the origin
				glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
			);
			flagcountup = 0;
			flagcountsides = 0;
			flagr = !flagr;
		}

		glm::mat4 trans;
		trans = glm::scale(trans, glm::vec3(40.0, 40.0, 40.0));
		ModelMatrix = trans;

		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		if (flags) {
			glBindVertexArray(VertexArrayId[3]);	//face
			glDrawElements(GL_TRIANGLES, NumIndices[3], GL_UNSIGNED_SHORT, (void*)0);
		}

		glBindVertexArray(0);

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
	unsigned char data[4];
	glReadPixels(xpos, window_height - ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top

																					 // Convert the color back to an integer ID
	gPickedIndex = int(data[0]);

	if (gPickedIndex == 255) { // Full white, must be the background !
		gMessage = "background";
	}
	else {
		std::ostringstream oss;

	}

	// Uncomment these lines to see the picking shader in effect
	//glfwSwapBuffers(window);
	//continue; // skips the normal rendering
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
	window = glfwCreateWindow(window_width, window_height, "Kanamathareddy,Poorna(35915896)", NULL, NULL);
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

	//// Initialize the GUI
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

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ProjMatrixID = glGetUniformLocation(programID, "P");

	PickingMatrixID = glGetUniformLocation(pickingProgramID, "MVP");
	// Get a handle for our "pickingColorID" uniform
	pickingColorID = glGetUniformLocation(pickingProgramID, "PickingColor");
	// Get a handle for our "LightPosition" uniform
	LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	LightID1 = glGetUniformLocation(programID, "LightPosition_worldspace1");

	LightIDUni = glGetUniformLocation(programID, "LightPower");
	LightIDUniami = glGetUniformLocation(programID, "ambientpower");

	createObjects();
}

void createVAOs(Vertex Vertices[], unsigned short Indices[], int ObjectId) {
	
	GLenum ErrorCheckValue = glGetError();

	const size_t VertexSize = sizeof(Vertices[0]);
	const size_t RgbOffset = sizeof(Vertices[0].Position);
	const size_t Normaloffset = sizeof(Vertices[0].Color) + RgbOffset;

	// Create Vertex Array Object
	glGenVertexArrays(1, &VertexArrayId[ObjectId]);	//
	glBindVertexArray(VertexArrayId[ObjectId]);		//

													// Create Buffer for vertex data
	glGenBuffers(1, &VertexBufferId[ObjectId]);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[ObjectId]);
	glBufferData(GL_ARRAY_BUFFER, VertexBufferSize[ObjectId], Vertices, GL_STATIC_DRAW);

	// Create Buffer for indices
	if (Indices != NULL) {
		glGenBuffers(1, &IndexBufferId[ObjectId]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[ObjectId]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexBufferSize[ObjectId], Indices, GL_STATIC_DRAW);
	}

	// Assign vertex attributes
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)Normaloffset);

	glEnableVertexAttribArray(0);	// position
	glEnableVertexAttribArray(1);	// color
	glEnableVertexAttribArray(2);	// normal

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
		case GLFW_KEY_R:
			flagr = true;
			break;

		case GLFW_KEY_F:
			flags = !flags;
			break;


		case GLFW_KEY_UP:

			fvertical = !fvertical;
			fsidespen = false;
			fverticalpen = true;

			if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
			{
				flagcountup++;
				if (flagcountup == 16)
				{
					flagcountup = 0;
				}
			}


			break;
		case  GLFW_KEY_RIGHT:

			fsides = !fsides;


			if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
			{
				flagcountsides++;
				if (flagcountsides == 16)
				{
					flagcountsides = 0;
				}
			}


			break;
		case  GLFW_KEY_LEFT:

			fsides = !fsides;


			if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
			{
				flagcountsides--;
				if (flagcountsides == 16)
				{
					flagcountsides = 0;
				}
			}


			break;
		case GLFW_KEY_DOWN:


			fvertical = !fvertical;

			if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
			{
				flagcountup--;
				if (flagcountup == 16)
				{
					flagcountup = 0;
				}
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
void drawgrid(void)
{
	GLint i = 10, j = 5, k = 0;
	bool up = false;
	bool down = true;
	int count = 0;
	float a[3], b[3];
	for (i = 20; i >= 0;)
	{
		if (down)
		{
			for (j = 20; j >= 0; j = j - 20)
			{
				a[0] = i - 10;

				a[2] = j - 10;
				a[1] = 0.0;
				b[1] = 0.0;
				b[2] = i - 10;


				b[0] = j - 10;

				vx[k].SetPosition(a);
				vy[k].SetPosition(b);
				vx[k].SetColor(whitecolor);
				vy[k].SetColor(whitecolor);
				k++;

				if (j == 0)
				{
					up = !up;
					down = !down;
				}
			}
			i--;
		}

		else if (up)
		{
			for (j = 0; j <= 20; j = j + 20)
			{
				a[0] = i - 10;

				a[2] = j - 10;
				a[1] = 0.0;
				b[1] = 0.0;
				b[2] = i - 10;

				b[0] = j - 10;

				vx[k].SetPosition(a);
				vy[k].SetPosition(b);
				vx[k].SetColor(whitecolor);
				vy[k].SetColor(whitecolor);
				k++;
				//printf("X:%f::Y:%f\n", a[0], a[2]);
				//printf("X:%f::Y:%f\n", b[0], b[2]);
				if (j == 20)
				{
					up = !up;
					down = !down;
				}
			}
			i--;
		}
	}
}

/*void drawcontrol(void)
{
GLint i = 10, j = 5, k = 0;
bool up = false;
bool down = true;
int count = 0;
float a[3], b[3];
for (i = 20; i >= 0;)
{
if (down)
{
for (j = 20; j >= 0; j = j - 20)
{
a[0] = i - 10;

a[1] = j - 10;
a[2] = 0.0;
b[2] = 0.0;
b[1] = i - 10;


b[0] = j - 10;

vcx[k].SetPosition(a);
vcy[k].SetPosition(b);
vcx[k].SetColor(whitecolor);
vcy[k].SetColor(whitecolor);
k++;

if (j == 0)
{
up = !up;
down = !down;
}
}
i--;
}

else if (up)
{
for (j = 0; j <= 20; j = j + 20)
{
a[0] = i - 10;

a[1] = j - 10;
a[2] = 0.0;
b[2] = 0.0;
b[1] = i - 10;

b[0] = j - 10;

vcx[k].SetPosition(a);
vcy[k].SetPosition(b);
vcx[k].SetColor(whitecolor);
vcy[k].SetColor(whitecolor);
k++;
//printf("X:%f::Y:%f\n", a[0], a[2]);
//printf("X:%f::Y:%f\n", b[0], b[2]);
if (j == 20)
{
up = !up;
down = !down;
}
}
i--;
}
}
}*/
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



int main(void)
{
	// initialize window
	int errorCode = initWindow();
	if (errorCode != 0)
		return errorCode;
	drawgrid();
	printf("before initiazing opengl\n");
	// initialize OpenGL pipeline
	initOpenGL();
	printf("after initiazing opengl\n");

	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;
	do {
		//// Measure speed
		//double currentTime = glfwGetTime();
		//nbFrames++;
		//if (currentTime - lastTime >= 1.0){ // If last prinf() was more than 1sec ago
		//	// printf and reset
		//	printf("%f ms/frame\n", 1000.0 / double(nbFrames));
		//	nbFrames = 0;c
		//	lastTime += 1.0;
		//}

		if (animation) {
			phi += 0.01;
			if (phi > 360)
				phi -= 360;
		}

		// DRAWING POINTS
		renderScene();


	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	cleanup();

	return 0;
}