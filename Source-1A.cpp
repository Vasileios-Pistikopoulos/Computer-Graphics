//********************************
//Αυτό το αρχείο θα το χρησιμοποιήσετε
// για να υλοποιήσετε την άσκηση 1Α της OpenGL
//
//ΑΜ: 5336                        Όνομα: Πιστικόπουλος Βασίλειος
//ΑΜ:                         Όνομα:

//*********************************

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

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

//******************
// Η LoadShaders είναι black box για σας

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
	printf("Linking program\n");
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

void calculateA(int row, int col, float& cx, float& cy) {
	cx = col - 4.5f;  // Συντεταγμένη X για το κέντρο
	cy = 4.5f - row;  // Συντεταγμένη Y για το κέντρο
}

int main(void)
{
	//Απεικόνιση του λαβυρίνθου σε δισδιάστατο πίνακα (1 = τοιχος, 0 = μονοπάτι)
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

	// Initialise GLFW
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

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(750, 750, u8"Άσκηση 1Α - 2024", NULL, NULL);


	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Grey background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	//***********************************************
	// Οι shaders σας είναι οι 
	// ProjectVertexShader.vertexshader
	// ProjectFragmentShader.fragmentshader

	GLuint programID = LoadShaders("ProjectVertexShader.vertexshader", "ProjectFragmentShader.fragmentshader");

	///////////////////////////////////////////////////////////////////////////////////////	
	/**Το παρακάτω το αγνοείτε - είναι τοποθέτηση κάμερας ***/
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	glm::mat4 Projection = glm::perspective(glm::radians(30.0f), 4.0f / 4.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(0, 0, 30), // Camera  in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // 
	);

	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;
	///////////////////////////////////////////////////////////////////////////////////////
	//**************************************************
	/// Για βοήθεια το πρόγραμμα αναπαριστά ενα τυχαίο τρίγωνο - εσείς θα πρέπει να βάλετε κορυφές κατάλληλες 
	//  για το δικό σας τρίγωνο.
	//Στην άσκηση αυτή δουλεύετε στις 2 διαστάσεις x,y οπότε η z συνιστώσα θα ειναι πάντα 0.0f

	// Δημιουργία δεδομένων κορυφών για τον λαβύρινθο
	std::vector<GLfloat> vertex_buffer_data;

	// Προσαρμόζουμε τον λαβύρινθο ώστε το κέντρο του να είναι στο (0,0)
	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 10; ++j) {
			if (labyrinth[i][j] == 1) {
				// Υπολογίζουμε τις συντεταγμένες των τετραγώνων (2 τρίγωνα ανά τετράγωνο)
				float x_left = j - 5.0f;  // Οριζόντια συντεταγμένη αριστερής γωνίας
				float x_right = j - 4.0f; // Οριζόντια συντεταγμένη δεξιάς γωνίας
				float y_bottom = 5.0f - i;  // Κάθετη συντεταγμένη κάτω γωνίας
				float y_top = 4.0f - i;     // Κάθετη συντεταγμένη πάνω γωνίας

				// Πρώτο τρίγωνο για το τετράγωνο ( γωνίες: κάτω-αριστερά, κάτω-δεξιά, πάνω-αριστερά)
				vertex_buffer_data.push_back(x_left);    vertex_buffer_data.push_back(y_bottom);  vertex_buffer_data.push_back(0.0f); // κάτω-αριστερά
				vertex_buffer_data.push_back(x_right);   vertex_buffer_data.push_back(y_bottom);  vertex_buffer_data.push_back(0.0f); // κάτω-δεξιά
				vertex_buffer_data.push_back(x_left);    vertex_buffer_data.push_back(y_top);     vertex_buffer_data.push_back(0.0f); // πάνω-αριστερά

				// Δεύτερο τρίγωνο για το τετράγωνο (γωνίες: πάνω-αριστερά, κάτω-δεξιά, πάνω-δεξιά)
				vertex_buffer_data.push_back(x_left);    vertex_buffer_data.push_back(y_top);     vertex_buffer_data.push_back(0.0f); // πάνω-αριστερά
				vertex_buffer_data.push_back(x_right);   vertex_buffer_data.push_back(y_bottom);  vertex_buffer_data.push_back(0.0f); // κάτω-δεξιά
				vertex_buffer_data.push_back(x_right);   vertex_buffer_data.push_back(y_top);     vertex_buffer_data.push_back(0.0f); // πάνω-δεξιά
			}
		}
	}
	// Συντεταγμένες για το αντικείμενο "A" (κεντραρισμένο στο τετράγωνο (1,1))
//float cx = -3.5f; // X συντεταγμένη του κέντρου του τετραγώνου A
//float cy = 3.5f;  // Y συντεταγμένη του κέντρου του τετραγώνου A
float cx, cy;
float colA = 0;
float rowA = 2;
calculateA(rowA, colA, cx, cy);
// Ορισμός κορυφών για το τετράγωνο A (2 τρίγωνα)
std::vector<GLfloat> objectA_vertices = {
    // Πρώτο τρίγωνο για το τετράγωνο A
    cx - 0.25f, cy - 0.25f, 0.0f,  // Κάτω-αριστερά
    cx + 0.25f, cy - 0.25f, 0.0f,  // Κάτω-δεξιά
    cx - 0.25f, cy + 0.25f, 0.0f,  // Πάνω-αριστερά

    // Δεύτερο τρίγωνο για το τετράγωνο A
    cx - 0.25f, cy + 0.25f, 0.0f,  // Πάνω-αριστερά
    cx + 0.25f, cy - 0.25f, 0.0f,  // Κάτω-δεξιά
    cx + 0.25f, cy + 0.25f, 0.0f   // Πάνω-δεξιά
};

// Προσθήκη των κορυφών αυτών στα δεδομένα του vertex buffer
vertex_buffer_data.insert(vertex_buffer_data.end(), objectA_vertices.begin(), objectA_vertices.end());
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(GLfloat), vertex_buffer_data.data(), GL_STATIC_DRAW);

	bool key_pressed[4] = { false, false, false, false };  // J, L, I, K
	do {
		bool moved = false;
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);  /// Αυτό αφορά την κάμερα  - το αγνοείτε

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

		//// Draw the triangle !
		//glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle
		glDrawArrays(GL_TRIANGLES, 0, vertex_buffer_data.size() / 3);

		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !key_pressed[0]) {
			if (colA < 9 && labyrinth[(int)rowA][(int)(colA + 1)] == 0) {
				colA += 1;
				moved = true;
			}
			key_pressed[0] = true;
		}
		else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) {
			key_pressed[0] = false;
		}

		// Έλεγχος για κίνηση αριστερά (J)
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && !key_pressed[1]) {
			if (colA > 0 && labyrinth[(int)rowA][(int)(colA - 1)] == 0) {
				colA -= 1;
				moved = true;
			}
			key_pressed[1] = true;
		}
		else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_RELEASE) {
			key_pressed[1] = false;
		}

		// Έλεγχος για κίνηση πάνω (I)
		if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && !key_pressed[2]) {
			if (rowA > 0 && labyrinth[(int)(rowA - 1)][(int)colA] == 0) {
				rowA -= 1;
				moved = true;
			}
			key_pressed[2] = true;
		}
		else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE) {
			key_pressed[2] = false;
		}

		// Έλεγχος για κίνηση κάτω (K)
		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && !key_pressed[3]) {
			if (rowA < 9 && labyrinth[(int)(rowA + 1)][(int)colA] == 0) {
				rowA += 1;
				moved = true;
			}
			key_pressed[3] = true;
		}
		else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_RELEASE) {
			key_pressed[3] = false;
		}

		// Ενημέρωση των κορυφών του αντικειμένου A κατά την κίνηση
		if (moved) {
			// Υπολογισμός των νέων συντεταγμένων του χαρακτήρα A
			calculateA(rowA, colA, cx, cy);

			// Ενημέρωση των κορυφών για το νέο τετράγωνο του A
			objectA_vertices = {
				// Πρώτο τρίγωνο
				cx - 0.25f, cy - 0.25f, 0.0f,  // Κάτω-αριστερά
				cx + 0.25f, cy - 0.25f, 0.0f,  // Κάτω-δεξιά
				cx - 0.25f, cy + 0.25f, 0.0f,  // Πάνω-αριστερά

				// Δεύτερο τρίγωνο
				cx - 0.25f, cy + 0.25f, 0.0f,  // Πάνω-αριστερά
				cx + 0.25f, cy - 0.25f, 0.0f,  // Κάτω-δεξιά
				cx + 0.25f, cy + 0.25f, 0.0f   // Πάνω-δεξιά
			};

			// Η ανανέωση του buffer για το αντικείμενο A
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glBufferSubData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(GLfloat) - objectA_vertices.size() * sizeof(GLfloat),
				objectA_vertices.size() * sizeof(GLfloat), objectA_vertices.data());
			if (colA == 9 && rowA == 7) {
				std::cout << "Κερδισες!" << std::endl;
				glfwSetWindowShouldClose(window, GL_TRUE); 
			}
		}

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_Q) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

