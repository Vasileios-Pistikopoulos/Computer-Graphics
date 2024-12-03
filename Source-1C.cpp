//********************************
//
//ΑΜ: 5336                        Όνομα:Pistikopoulos Vasileios
//ΑΜ: 5281                        Όνομα:Markopoulou Eutuxia

//*********************************

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;

/// <summary>
/// //////////////////////////////////////////////
/// </summary>
glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;


glm::mat4 getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}

////////////////////////////////////////////// <summary>
/// Add camera function here
/// </summary>

void camera_function()
{

}

/////////////////////////////////////////////////

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	
	


	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	//printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
///////////////////////////////////////////////////
//Representation of the labyrinth in a 2D array
int labyrinth[10][10] = { {1,1,1,1,1,1,1,1,1,1},
						  {1,0,0,0,0,0,0,0,0,1},
						  {0,0,1,1,1,1,0,1,0,1},
						  {1,0,1,0,0,0,0,1,0,1},
						  {1,0,1,0,1,1,0,1,0,1},
						  {1,0,0,0,0,1,0,0,0,1},
						  {1,0,1,1,0,1,1,1,0,1},
						  {1,0,0,0,0,0,0,1,0,0},
						  {1,0,1,0,1,1,0,0,0,1},
						  {1,1,1,1,1,1,1,1,1,1} };

// Function to get the position of the center of a cube in the labyrinth, relative to its position in the 2D array (used for translation)
glm::vec3 getCubePosition(int row, int col) {
	float x = static_cast<float>(col) - 4.5f;  // Center the column
	float y = 4.5f - static_cast<float>(row);   // Center the row and invert y-axis
	float z = 0.5f;                            // Fixed z-coordinate
	return glm::vec3(x, y, z);
}


std::pair<int, int>  getRandomPosition(const std::vector<std::pair<int, int>>& validPositions, int rowA, int colA) {
	// Generate random positions until a new one is found
	std::pair<int, int> randomPos;
	do {
		int randomIndex = rand() % validPositions.size();
		randomPos = validPositions[randomIndex];
	} while (randomPos.first == rowA && randomPos.second == colA);

	// Return the calculated position
	return randomPos;
}


void updateCubePosition(double& lastUpdateTime, std::vector<std::pair<int, int>> validPositions,int rowA, int colA) {
	
	double currentTime = glfwGetTime();

	//change position every 3 seconds
	if (currentTime - lastUpdateTime >= 3.0) {
		lastUpdateTime = currentTime;
	}
}

int main(void)
{

	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(950, 950, u8"Εργασία 1Γ – 2024 – Κυνήγι Θησαυρού", NULL, NULL);


	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// background color
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders

	GLuint programID = LoadShaders("P1BVertexShader.vertexshader", "P1BFragmentShader.fragmentshader");

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 20.0f),
		glm::vec3(0.0f, 0.0f, 0.25f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	GLfloat len = 5.0f, wid = 2.5f, heig = 2.5f;

	//Walls vertices
	static const GLfloat cube[] =
	{
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,

		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,

		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f, -0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,

		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,

		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,

		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f
	};

	//CubeA vertices
	static const GLfloat cubeA[] =
	{
		-0.25f, -0.25f, -0.25f,
		-0.25f, -0.25f,  0.25f,
		-0.25f,  0.25f,  0.25f,

		 0.25f,  0.25f, -0.25f,
		-0.25f, -0.25f, -0.25f,
		-0.25f,  0.25f, -0.25f,

		 0.25f, -0.25f,  0.25f,
		-0.25f, -0.25f, -0.25f,
		 0.25f, -0.25f, -0.25f,

		 0.25f,  0.25f, -0.25f,
		 0.25f, -0.25f, -0.25f,
		-0.25f, -0.25f, -0.25f,

		-0.25f, -0.25f, -0.25f,
		-0.25f,  0.25f,  0.25f,
		-0.25f,  0.25f, -0.25f,

		 0.25f, -0.25f,  0.25f,
		-0.25f, -0.25f,  0.25f,
		-0.25f, -0.25f, -0.25f,

		-0.25f,  0.25f,  0.25f,
		-0.25f, -0.25f,  0.25f,
		 0.25f, -0.25f,  0.25f,

		 0.25f,  0.25f,  0.25f,
		 0.25f, -0.25f, -0.25f,
		 0.25f,  0.25f, -0.25f,

		 0.25f, -0.25f, -0.25f,
		 0.25f,  0.25f,  0.25f,
		 0.25f, -0.25f,  0.25f,

		 0.25f,  0.25f,  0.25f,
		 0.25f,  0.25f, -0.25f,
		-0.25f,  0.25f, -0.25f,

		 0.25f,  0.25f,  0.25f,
		-0.25f,  0.25f, -0.25f,
		-0.25f,  0.25f,  0.25f,

		 0.25f,  0.25f,  0.25f,
		-0.25f,  0.25f,  0.25f,
		 0.25f, -0.25f,  0.25f
	};

	//Treasure cube vertices
	static const GLfloat treasureCube[] = {
		-0.4f, -0.4f, -0.4f, 
		-0.4f, -0.4f,  0.4f, 
		-0.4f,  0.4f,  0.4f, 

		 0.4f,  0.4f, -0.4f, 
		-0.4f, -0.4f, -0.4f, 
		-0.4f,  0.4f, -0.4f, 

		 0.4f, -0.4f,  0.4f, 
		-0.4f, -0.4f, -0.4f, 
		 0.4f, -0.4f, -0.4f, 

		 0.4f,  0.4f, -0.4f, 
		 0.4f, -0.4f, -0.4f, 
		-0.4f, -0.4f, -0.4f, 

		-0.4f, -0.4f, -0.4f, 
		-0.4f,  0.4f,  0.4f, 
		-0.4f,  0.4f, -0.4f, 

		 0.4f, -0.4f,  0.4f, 
		-0.4f, -0.4f,  0.4f,  
		-0.4f, -0.4f, -0.4f, 
	

		-0.4f,  0.4f,  0.4f,  
		-0.4f, -0.4f,  0.4f,  
		 0.4f, -0.4f,  0.4f,  

		 0.4f,  0.4f,  0.4f,  
		 0.4f, -0.4f, -0.4f,  
		 0.4f,  0.4f, -0.4f,  

		 0.4f, -0.4f, -0.4f,  
		 0.4f,  0.4f,  0.4f,  
		 0.4f, -0.4f,  0.4f,  

		 0.4f,  0.4f,  0.4f,  
		 0.4f,  0.4f, -0.4f,  
		-0.4f,  0.4f, -0.4f,  

		 0.4f,  0.4f,  0.4f,  
		-0.4f,  0.4f, -0.4f,  
		-0.4f,  0.4f,  0.4f,  

		 0.4f,  0.4f,  0.4f,  
		-0.4f,  0.4f,  0.4f,  
		 0.4f, -0.4f,  0.4f   
	};

	static const GLfloat treasureCubeHalfSize[] = {
	-0.2f, -0.2f, -0.2f,
	-0.2f, -0.2f,  0.2f,
	-0.2f,  0.2f,  0.2f,

	 0.2f,  0.2f, -0.2f,
	-0.2f, -0.2f, -0.2f,
	-0.2f,  0.2f, -0.2f,

	 0.2f, -0.2f,  0.2f,
	-0.2f, -0.2f, -0.2f,
	 0.2f, -0.2f, -0.2f,

	 0.2f,  0.2f, -0.2f,
	 0.2f, -0.2f, -0.2f,
	-0.2f, -0.2f, -0.2f,

	-0.2f, -0.2f, -0.2f,
	-0.2f,  0.2f,  0.2f,
	-0.2f,  0.2f, -0.2f,

	 0.2f, -0.2f,  0.2f,
	-0.2f, -0.2f,  0.2f,
	-0.2f, -0.2f, -0.2f,

	-0.2f,  0.2f,  0.2f,
	-0.2f, -0.2f,  0.2f,
	 0.2f, -0.2f,  0.2f,

	 0.2f,  0.2f,  0.2f,
	 0.2f, -0.2f, -0.2f,
	 0.2f,  0.2f, -0.2f,

	 0.2f, -0.2f, -0.2f,
	 0.2f,  0.2f,  0.2f,
	 0.2f, -0.2f,  0.2f,

	 0.2f,  0.2f,  0.2f,
	 0.2f,  0.2f, -0.2f,
	-0.2f,  0.2f, -0.2f,

	 0.2f,  0.2f,  0.2f,
	-0.2f,  0.2f, -0.2f,
	-0.2f,  0.2f,  0.2f,

	 0.2f,  0.2f,  0.2f,
	-0.2f,  0.2f,  0.2f,
	 0.2f, -0.2f,  0.2f
	};

	
	static const GLfloat treasureCubeUVs[] = {
			// Face 1 (Front)
			0.0f, 0.0f,  // Bottom-left
			0.0f, 1.0f,  // Top-left
			1.0f, 1.0f,  // Top-right

			1.0f, 0.0f,  // Bottom-right
			0.0f, 0.0f,  // Bottom-left
			0.0f, 1.0f,  // Top-left

			// Face 2 (Bottom)
			1.0f, 1.0f,  // Top-right
			0.0f, 1.0f,  // Top-left
			0.0f, 0.0f,  // Bottom-left

			1.0f, 0.0f,  // Bottom-right
			1.0f, 1.0f,  // Top-right
			0.0f, 1.0f,  // Top-left

			// Face 3 (Left)
			1.0f, 0.0f,  // Bottom-right
			1.0f, 1.0f,  // Top-right
			0.0f, 1.0f,  // Top-left

			0.0f, 0.0f,  // Bottom-left
			0.0f, 1.0f,  // Top-left
			1.0f, 0.0f,  // Bottom-right

			// Face 4 (Right)
			0.0f, 0.0f,  // Bottom-left
			0.0f, 1.0f,  // Top-left
			1.0f, 1.0f,  // Top-right

			1.0f, 0.0f,  // Bottom-right
			0.0f, 0.0f,  // Bottom-left
			1.0f, 1.0f,  // Top-right

			// Face 5 (Top)
			1.0f, 0.0f,  // Bottom-right
			0.0f, 0.0f,  // Bottom-left
			0.0f, 1.0f,  // Top-left

			1.0f, 1.0f,  // Top-right
			1.0f, 0.0f,  // Bottom-right
			0.0f, 0.0f,  // Bottom-left

			// Face 6 (Back)
			1.0f, 1.0f,  // Top-right
			0.0f, 1.0f,  // Top-left
			0.0f, 0.0f,  // Bottom-left

			1.0f, 0.0f,  // Bottom-right
			1.0f, 1.0f,  // Top-right
			0.0f, 1.0f   // Top-left
	};


	unsigned int indices[] = {
		// Front face
		0, 1, 2,   // First triangle
		2, 3, 0,   // Second triangle

		// Back face
		4, 5, 6,   // First triangle
		6, 7, 4,   // Second triangle

		// Left face
		0, 3, 7,   // First triangle
		7, 4, 0,   // Second triangle

		// Right face
		1, 5, 6,   // First triangle
		6, 2, 1,   // Second triangle

		// Top face
		3, 2, 6,   // First triangle
		6, 7, 3,   // Second triangle

		// Bottom face
		0, 4, 5,   // First triangle
		5, 1, 0    // Second triangle
	};





	GLfloat a = 0.4f;
	//Color for the walls
	static const GLfloat color[] = {
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
		0.0f,  0.0f,  1.0 ,a,
	};

	//Color for the cube A
	static const GLfloat colorA[] = {
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a,
	1.0f,  1.0f,  0.0f,  a
	};
	
	//Calculate valid positions for the treasure cube
	std::vector<std::pair<int, int>> validPositions;

	for (int row = 0; row < 10; ++row) {
		for (int col = 0; col < 10; ++col) {
			if (labyrinth[row][col] == 0) {
				validPositions.emplace_back(row, col);
			}
		}
	}

	
	
	//Buffers for the Labyrinth walls
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

	//Buffers for the cube A
	GLuint vertexbuffer_cubeA;
	glGenBuffers(1, &vertexbuffer_cubeA);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_cubeA);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeA), cubeA, GL_STATIC_DRAW);


	GLuint colorbuffer_cubeA;
	glGenBuffers(1, &colorbuffer_cubeA);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer_cubeA);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorA), colorA, GL_STATIC_DRAW);



	//Ιmage loading for object B
	// Texture Loading for Treasure Cube
	GLuint treasureTextureID;
	int width, height, nrChannels;
	unsigned char* data = stbi_load("coins.jpg", &width, &height, &nrChannels, 0);
	if (!data) {
		std::cerr << "Failed to load texture: coins.jpg\n";
	}
	else {
		std::cout << "Texture loaded successfully: " << width << "x" << height << "\n";

		glGenTextures(1, &treasureTextureID);
		glBindTexture(GL_TEXTURE_2D, treasureTextureID);

		GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint treasureVertexShader = LoadShaders("treasureVertexShader.vertexshader", "treasureFragmentShader.fragmentshader");
	GLuint treasureProgramID = glCreateProgram();
	glAttachShader(treasureProgramID, treasureVertexShader);
	glLinkProgram(treasureProgramID);



	//Initialization of the cubeA position
	int rowA = 2;
	int colA = 0;
	int rowAbegin;
	int colAbegin;
	
	//Used for the edge cases of the labyrinth
	int startrow = 2;
	int startcol = 0;
	int endrow = 7;
	int endcol = 9;

	//Used for the key presses checks (cubeA movement
	bool keys[4] = { false, false, false, false };
	bool end = false;
	bool start = false;

	//Panning variables
	float cameraPanX = 0.0f; // Horizontal pan offset
	float cameraPanY = 0.0f; // Vertical pan offset
	float panSpeed = 0.005f;   // Speed of panning

	//Camera zooming variables
	float cameraSpeed = 0.01f; // Camera movement speed
	float zoomSpeed = 0.01f; // Zoom speed 
		

	//Initialization of the cube B position
	srand(static_cast<unsigned int>(time(0)));
	std::pair<int, int> posB = getRandomPosition(validPositions, rowA, colA);


	int rowB = posB.first;
	int colB = posB.second;

	// Variables to track time 
	double lastUpdateTime = glfwGetTime();
	double timeInterval = 1.5; // Interval in seconds

	double treasureFoundTime = -1.0; // Time when the treasure was found
	bool treasureSizeChanged = false;
	bool treasureVisible = true;

	// Generate and bind buffer for the treasure cube vertices
	GLuint vertexbuffer_treasureCube;
	glGenBuffers(1, &vertexbuffer_treasureCube);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_treasureCube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(treasureCube), treasureCube, GL_STATIC_DRAW);

	GLuint currentVertexbuffer = vertexbuffer_treasureCube;

	GLuint vertexbuffer_treasureCube_Half;
	glGenBuffers(1, &vertexbuffer_treasureCube_Half);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_treasureCube_Half);
	glBufferData(GL_ARRAY_BUFFER, sizeof(treasureCubeHalfSize), treasureCubeHalfSize, GL_STATIC_DRAW);
	
	GLuint UVbuffer_treasureCube;
	glGenBuffers(1, &UVbuffer_treasureCube);
	glBindBuffer(GL_ARRAY_BUFFER, UVbuffer_treasureCube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(treasureCubeUVs), treasureCubeUVs, GL_STATIC_DRAW);
	glm::vec3 treasurePosition = getCubePosition(rowB, colB);



	// Element Buffer (Optional)
	GLuint elementBuffer;
	glGenBuffers(1, &elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	bool found = false;
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 20.0f);
	do {
		
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { // Move camera left
			cameraPos.x -= cameraSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) { // Move camera right
			cameraPos.x += cameraSpeed;
		}

		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { // Move camera up
			cameraPos.y += cameraSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) { // Move camera down
			cameraPos.y -= cameraSpeed;
		}

		if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) { // Zoom in
			cameraPos.z -= zoomSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) { // Zoom out
			cameraPos.z += zoomSpeed;
		}

		// Ensure the camera doesn't go inside the labyrinth
		cameraPos.z = glm::max(cameraPos.z, 5.0f);


		//Panning keys check
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
			cameraPanX -= panSpeed; // Pan left
		}

		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
			cameraPanX += panSpeed; // Pan right
		}

		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
			cameraPanY += panSpeed; // Pan up
		}

		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
			cameraPanY -= panSpeed; // Pan down
		}



		// View and Projection Matrices
		glm::mat4 View = glm::lookAt(
			glm::vec3(cameraPos.x + cameraPanX, cameraPos.y + cameraPanY, cameraPos.z),
			glm::vec3(cameraPanX, cameraPanY, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		
		glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)950 / (float)950, 0.1f, 100.0f);

		glm::mat4 MVP = Projection * View;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		//Draw the walls of the labyrinth
		for (int row = 0; row < 10; ++row) {
			for (int col = 0; col < 10; ++col) {
				if (labyrinth[row][col] == 1) {
					glm::vec3 position = getCubePosition(row, col);
					glm::mat4 Model = glm::mat4(1.0f);
					Model = glm::translate(Model, position);
					glm::mat4 MVP = Projection * View * Model;

					glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

					// 1rst attribute buffer : vertices
					glEnableVertexAttribArray(0);
					glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
					glVertexAttribPointer(
						0,
						3,
						GL_FLOAT,
						GL_FALSE,
						0,
						(void*)0
					);

					// 2nd attribute buffer : colors
					glEnableVertexAttribArray(1);
					glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
					glVertexAttribPointer(
						1,
						4,                                // size
						GL_FLOAT,
						GL_FALSE,
						0,
						(void*)0
					);

					// Draw triangles 
					glDrawArrays(GL_TRIANGLES, 0, 36);

					glDisableVertexAttribArray(0);
				}
			}
		}

		rowAbegin = rowA;
		colAbegin = colA;
		double currentTime = glfwGetTime();
		if (!found || !treasureVisible) {  // Only allow movement if the treasure has not been found
			treasureVisible = true;
			// Handle the movement of cube A (Player's Cube)
			if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !keys[0]) { // Move right
				keys[0] = true;
				// Prevent moving into Cube B's position

				if (!(colA + 1 == colB && rowA == rowB) && colA + 1 < 10 && labyrinth[rowA][colA + 1] == 0) {
					colA++;
				}
				if (colA + 1 == colB && rowA == rowB) {
					found = true;
				}
			}
			if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && !keys[1]) { // Move left
				keys[1] = true;
				// Prevent moving into Cube B's position
				if (!(colA - 1 == colB && rowA == rowB) && colA - 1 >= 0 && labyrinth[rowA][colA - 1] == 0) {
					colA--;
				}
				if (colA - 1 == colB && rowA == rowB) {
					found = true;
				}
			}
			if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && !keys[2]) { // Move up
				keys[2] = true;
				// Prevent moving into Cube B's position
				if (!(colA == colB && rowA - 1 == rowB) && rowA - 1 >= 0 && labyrinth[rowA - 1][colA] == 0) {
					rowA--;
				}
				if (colA == colB && rowA - 1 == rowB) {
					found = true;
				}
			}
			if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && !keys[3]) { // Move down
				keys[3] = true;
				// Prevent moving into Cube B's position
				if (!(colA == colB && rowA + 1 == rowB) && rowA + 1 < 10 && labyrinth[rowA + 1][colA] == 0) {
					rowA++;
				}
				if (colA == colB && rowA + 1 == rowB) {
					found = true;
				}
			}
			

			// Reset movement keys when released
			if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) keys[0] = false;
			if (glfwGetKey(window, GLFW_KEY_J) == GLFW_RELEASE) keys[1] = false;
			if (glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE) keys[2] = false;
			if (glfwGetKey(window, GLFW_KEY_K) == GLFW_RELEASE) keys[3] = false;
		}
		else {
			// Record the time when the treasure is found
			if (treasureFoundTime == -1.0) {
				treasureFoundTime = currentTime; // Store the time when the treasure is found
			}

			// If the treasure was found, change its size after 0.75 seconds
			if (currentTime - treasureFoundTime >= 1 && !treasureSizeChanged) {
				// Change the vertex buffer to the smaller version of the treasure
				treasureSizeChanged = true;
				// Use the smaller treasure vertex buffer
				currentVertexbuffer = vertexbuffer_treasureCube_Half;
			}
			if (currentTime - treasureFoundTime >= 1.2) {
				treasureVisible = false; // Set the flag to make the treasure disappear
				rowB = -1;
				colB = -1;
				
			}
			if (currentTime - treasureFoundTime >= 1.5) {
				found = false; // Reset the found flag
				treasureFoundTime = -1.0; // Reset the time when the treasure was found
				treasureSizeChanged = false; // Reset the treasure size change flag
				currentVertexbuffer = vertexbuffer_treasureCube; // Reset the vertex buffer to the original treasure

			}

			// If 1.5 seconds have passed since the treasure was found, close the game
			
		}

		// When the treasure is found, stop Cube B from moving
		if (!found) {
			// Keep updating Cube B's position every 1.5 seconds if treasure hasn't been found
			if (currentTime - lastUpdateTime >= timeInterval) {
				// Update the last time
				lastUpdateTime = currentTime;

				// Get a new random position for the cube B
				std::pair<int, int> posB = getRandomPosition(validPositions, rowA, colA);
				treasurePosition = getCubePosition(posB.first, posB.second);
				rowB = posB.first;
				colB = posB.second;
			}

		}
		// Draw cube A
		
		glm::vec3 positionA = getCubePosition(rowA, colA);
		glm::mat4 ModelA = glm::mat4(1.0f);
		ModelA = glm::translate(ModelA, positionA);
		glm::mat4 MVPA = Projection * View * ModelA;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPA[0][0]);

		// 1rst attribute buffer : vertices
			
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_cubeA);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer_cubeA);

		glVertexAttribPointer(
			1,
			4,                                // size
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		// Draw triangles
		glDrawArrays(GL_TRIANGLES, 0, 36);

		
		//Draw cube B
		//glUseProgram(treasureProgramID);

		// Bind the MVP matrix for this shader
		//GLuint MatrixID = glGetUniformLocation(treasureProgramID, "MVP");
		//glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPTreasure[0][0]);

		// Bind the texture
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D,treasureTextureID);
		//glUniform1i(glGetUniformLocation(treasureProgramID, "textureSampler"), 0);

		// Model transformation for the treasure cube
		if (treasureVisible) {
			glUseProgram(treasureProgramID);
			//Draw the treasure cube if needed
			//Create and apply the model transformation
			glm::mat4 ModelTreasure = glm::mat4(1.0f);
			ModelTreasure = glm::translate(ModelTreasure, treasurePosition);

			//Calculate the MVP matrix
			glm::mat4 MVPTreasure = Projection * View * ModelTreasure;

			//Send the MVP matrix to the vertex shader
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPTreasure[0][0]);

			//Set the `useTexture` uniform to enable texture rendering
			//GLint useTextureLocation = glGetUniformLocation(programID, "textureSampler");
			//glUniform1i(useTextureLocation, GL_TRUE);

			// Bind the buffer for the vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, currentVertexbuffer);
			glVertexAttribPointer(
				0,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);

			// Bind the buffer for the UVs
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, UVbuffer_treasureCube);
			glVertexAttribPointer(
				1,
				2,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);

			// Bind the texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, treasureProgramID);

			// Send the texture sampler uniform to the fragment shader
			GLint textureSamplerLocation = glGetUniformLocation(treasureProgramID, "textureSampler");
			glUniform1i(textureSamplerLocation, 0); // Texture unit 0

			// Draw the treasure cube
			// If using indices
			
			glDrawArrays(GL_TRIANGLES, 0, 36);

			// Disable vertex attributes after drawing
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
		}


		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the SPACE key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

