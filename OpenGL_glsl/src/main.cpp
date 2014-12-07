#include <OpenGL_Shaders.h>
#include <Camera.h>
#include <Ship.h>
#include <main.h>


#define BUFFER_OFFSET(offset) ((void*)(offset))
#define MEMBER_OFFSET(s,m) ((char*)NULL + (offsetof(s,m)))

// Global variables
// Window variables
int g_WindowHandle = 0;
int g_WindowWidth = 800;
int g_WindowHeight = 600;

int g_W, g_A, g_S, g_D, g_Q, g_E;
bool g_Shift = false;

glm::ivec2 g_MousePosition;

glm::quat g_Rotation;

std::clock_t g_PreviousTicks;
std::clock_t g_CurrentTicks;

Camera g_Camera;

glm::vec3 g_InitialCameraPosition;
glm::quat g_InitialCameraRotation;


Vertex cubeVertices[8] = {
		{ glm::vec3(1, 1, 1), glm::vec3(1, 1, 1) }, // 0
		{ glm::vec3(-1, 1, 1), glm::vec3(0, 1, 1) }, // 1
		{ glm::vec3(-1, -1, 1), glm::vec3(0, 0, 1) }, // 2
		{ glm::vec3(1, -1, 1), glm::vec3(1, 0, 1) }, // 3
		{ glm::vec3(1, -1, -1), glm::vec3(1, 0, 0) }, // 4
		{ glm::vec3(-1, -1, -1), glm::vec3(0, 0, 0) }, // 5
		{ glm::vec3(-1, 1, -1), glm::vec3(0, 1, 0) }, // 6
		{ glm::vec3(1, 1, -1), glm::vec3(1, 1, 0) }, // 7
};

GLuint cubeIndices[36] = {
	0, 1, 2, 2, 3, 0,           // Front face
	7, 4, 5, 5, 6, 7,           // Back face
	6, 5, 2, 2, 1, 6,           // Left face
	7, 0, 3, 3, 4, 7,           // Right face
	7, 6, 1, 1, 0, 7,           // Top face
	3, 2, 5, 5, 4, 3            // Bottom face
};

//// Handle for the vertex array object for the cube
//GLuint vaoCube = 0;

GLuint vaoShip = 0;

// Handle for the shader program
GLuint g_ShaderProgram = 0;
GLint g_UniformMVP = -1;
GLint timeUniformId = -1;
GLint positionAtribID;
GLint colorAtribID;

// Game variables
std::vector<Ship> g_enemyShips;

std::vector<GLuint> g_vaoIds;
std::vector<GLuint> g_vertexBuffers;
std::vector<GLuint> g_indexBuffers;

void InitGL(int argc, char * argv[]);
void InitGLEW();
GLuint LoadShader(GLenum shaderType, const std::string& shaderFile);
GLuint CreateShaderProgram(std::vector<GLuint> shaders);

void IdleGL();
void DisplayGL();
void KeyboardGL(unsigned char c, int x, int y);
void KeyboardUpGL(unsigned char c, int x, int y);
void SpecialGL(int key, int x, int y);
void SpecialUpGL(int key, int x, int y);
void MouseGL(int button, int state, int x, int y);
void MouseMotionGL(int x, int y);
void ReshapeGL(int width, int height);


void InitializeVertexArrayObjects();
void InitializeVertexBuffer();
void LoadEnemyShips();
void LoadBuffers();
void DrawElement(GLuint vao, GLuint shaderProgram, glm::mat4 model, std::vector<GLuint> indices, GLuint vertexBuffer);
float gl_time = 0.0f;

int main(int argc, char * argv[])
{
	g_PreviousTicks = std::clock();
	g_A = g_W = g_S = g_D = g_Q = g_E = 0;

	g_InitialCameraPosition = glm::vec3(0, 0, 10);
	g_Camera.SetPosition(g_InitialCameraPosition);
	g_Camera.SetRotation(g_InitialCameraRotation);

	InitGL(argc, argv);
	InitGLEW();

	// Load some shaders.
	GLuint vertexShader = LoadShader(GL_VERTEX_SHADER, "../data/shaders/simpleShader.vert");
	GLuint fragmentShader = LoadShader(GL_FRAGMENT_SHADER, "../data/shaders/simpleShader.frag");

	std::vector<GLuint> shaders;
	shaders.push_back(vertexShader);
	shaders.push_back(fragmentShader);

	// Create the shader program.
	g_ShaderProgram = CreateShaderProgram(shaders);
	assert(g_ShaderProgram != 0);

	positionAtribID = glGetAttribLocation(g_ShaderProgram, "in_position");
	colorAtribID = glGetAttribLocation(g_ShaderProgram, "in_color");
	g_UniformMVP = glGetUniformLocation(g_ShaderProgram, "MVP");

	timeUniformId = glGetUniformLocation(g_ShaderProgram, "time");

	LoadEnemyShips();
	LoadBuffers();

	// Make sure we disable and unbind everything to prevent rendering issues later.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(positionAtribID);
	glDisableVertexAttribArray(colorAtribID);

	glutMainLoop();
}
Vertex coords[] = {
		{ glm::vec3(1, 1, 1), glm::vec3(0, 1, 1) },
		{ glm::vec3(-1, 1, 1), glm::vec3(0, 1, 1) },
		{ glm::vec3(-1, -1, 1), glm::vec3(0, 1, 1) },
		{ glm::vec3(1, -1, 1), glm::vec3(0, 1, 1) },

		{ glm::vec3(-2, 1, 1), glm::vec3(0, 1, 1) },
		{ glm::vec3(-4, 1, 1), glm::vec3(0, 1, 1) },
		{ glm::vec3(-4, -1, 1), glm::vec3(0, 1, 1) },
		{ glm::vec3(-2, -1, 1), glm::vec3(0, 1, 1) },

		{ glm::vec3(4, 1, 1), glm::vec3(0, 1, 1) },
		{ glm::vec3(2, 1, 1), glm::vec3(0, 1, 1) },
		{ glm::vec3(2, -1, 1), glm::vec3(0, 1, 1) },
		{ glm::vec3(4, -1, 1), glm::vec3(0, 1, 1) },

		{ glm::vec3(0, -3, 1), glm::vec3(0, 1, 0) },
		{ glm::vec3(-1, -5, 1), glm::vec3(0, 1, 0) },
		{ glm::vec3(1, -5, 1), glm::vec3(0, 1, 0) },
};

GLuint indexArray[] =
{
	0, 1, 2,
	2, 3, 0,

	4, 5, 6,
	6, 7, 4,

	8, 9, 10,
	10, 11, 8,

	12, 13, 14
};

void LoadEnemyShips()
{	
	std::vector<Vertex> vertices(coords, coords + sizeof(coords) / sizeof(Vertex));
	std::vector<GLuint> indices(indexArray, indexArray + sizeof(indexArray) / sizeof(GLuint));
	Ship enemyShip (vertices, indices);
	g_enemyShips.push_back(enemyShip);
}

GLuint vertexBufferObject;
GLuint indexBufferObject;

//void InitializeVertexBuffer()
//{
//	glGenBuffers(1, &vertexBufferObject);
//	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
//
//	glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STATIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	glGenBuffers(1, &indexBufferObject);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//}

//void InitializeVertexArrayObjects()
//{
//	size_t colorOffset = sizeof(float) * 3 * g_enemyShips[0].GetVertices().size();
//	size_t posDataOffset = 0;
//	for (int i = 0; i < g_enemyShips.size(); i++)
//	{
//		glGenVertexArrays(1, &g_enemyShips[i].vaoId);
//		glBindVertexArray(g_enemyShips[i].vaoId);
//		
//		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
//		glEnableVertexAttribArray(0);
//		glEnableVertexAttribArray(1);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)colorOffset);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
//		
//		glBindVertexArray(0);
//
//		posDataOffset += sizeof(float) * 3 * (g_enemyShips[i].GetVertices().size() / 2);
//		colorOffset += sizeof(float) * 4 * (g_enemyShips[i].GetVertices().size() / 2);
//	}
//}

void LoadBuffers()
{
	for (int i = 0; i < g_enemyShips.size(); i++)
	{
		// Create vao for this ship
		glGenVertexArrays(1, &g_enemyShips[i].vaoId);
		glBindVertexArray(g_enemyShips[i].vaoId);

		glGenBuffers(1, &g_enemyShips[i].vertexBufferId);
		glGenBuffers(1, &g_enemyShips[i].indexBufferId);

		// Create vertex array buffer
		glBindBuffer(GL_ARRAY_BUFFER, g_enemyShips[i].vertexBufferId);
		std::vector<Vertex> vertices = g_enemyShips[i].GetVertices();
		glBufferData(GL_ARRAY_BUFFER, 
			vertices.size() * sizeof(Vertex), 
			&vertices[0], 
			GL_STATIC_DRAW);

		// Create vertex buffer object
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_enemyShips[i].indexBufferId);
		std::vector<GLuint> indices = g_enemyShips[i].GetIndices();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
			indices.size() * sizeof(GLuint), 
			&indices[0], 
			GL_STATIC_DRAW);

		glVertexAttribPointer(positionAtribID, 3, GL_FLOAT, false, sizeof(Vertex), MEMBER_OFFSET(Vertex, Position));
		glEnableVertexAttribArray(positionAtribID);

		glVertexAttribPointer(colorAtribID, 3, GL_FLOAT, false, sizeof(Vertex), MEMBER_OFFSET(Vertex, Color));
		glEnableVertexAttribArray(colorAtribID);
		
		g_enemyShips[i].isRegistered = true;
		
		// Unbind
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}	
}

void DisplayGL()
{
	// Clear the canvas
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (Ship s : g_enemyShips)
	{
		DrawElement(s.vaoId, g_ShaderProgram, s.GetModelMatrix(), s.GetIndices(), s.vertexBufferId);
	}

	gl_time += 0.01f;
	// Unbind program and vertex array, then swap buffers
	glUseProgram(0);
	glBindVertexArray(0);

	glutSwapBuffers();
}

void DrawElement(GLuint vao, GLuint shaderProgram, glm::mat4 model, std::vector<GLuint> indices, GLuint vertexBuffer)
{
	glUseProgram(shaderProgram);
	glBindVertexArray(vao);

	glm::mat4 mvp = g_Camera.GetProjectionMatrix() * g_Camera.GetViewMatrix() * model;
	// Pass information to shaders
	glUniformMatrix4fv(g_UniformMVP, 1, GL_FALSE, glm::value_ptr(mvp));

	glUniform1fARB(timeUniformId, gl_time);
	// Draw
	glDrawElements(GL_TRIANGLES,
		indices.size(),
		GL_UNSIGNED_INT,
		BUFFER_OFFSET(0));
}

void InitGL(int argc, char * argv[])
{
	std::cout << "Initializing OpenGL..." << std::endl;

	glutInit(&argc, argv);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	int windowWidth = glutGet(GLUT_SCREEN_WIDTH);
	int windowHeight = glutGet(GLUT_SCREEN_HEIGHT);

	// Create color buffer with alpha, depth buffer, and swap buffers
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);

	// use OpenGL 3.3
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	glutInitWindowPosition((windowWidth - g_WindowWidth) / 2, (windowHeight - g_WindowHeight) / 2);
	glutInitWindowSize(g_WindowWidth, g_WindowHeight);

	g_WindowHandle = glutCreateWindow("OpenGL Shaders");

	glutIdleFunc(IdleGL);
	glutDisplayFunc(DisplayGL);
	glutKeyboardFunc(KeyboardGL);
	glutKeyboardUpFunc(KeyboardUpGL);
	glutSpecialFunc(SpecialGL);
	glutSpecialUpFunc(SpecialUpGL);
	glutMouseFunc(MouseGL);
	glutMotionFunc(MouseMotionGL);
	glutReshapeFunc(ReshapeGL);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	// Need depth testing and backface culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	std::cout << "OpenGL initialization complete." << std::endl;
}

void InitGLEW()
{
	std::cout << "Initializing GLEW..." << std::endl;
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Error initializing GLEW" << std::endl;
		exit(-1);
	}
	if (!GLEW_VERSION_3_3)
	{
		std::cerr << "OpenGL v. 3.3 support not found" << std::endl;
		exit(-1);
	}
#ifdef _WIN32
	// Use this to get a higher framerate than the screen's default
	if (WGLEW_EXT_swap_control)
	{
		wglSwapIntervalEXT(0);
	}
#endif

	std::cout << "GLEW initialization complete." << std::endl;
}

GLuint LoadShader(GLenum shaderType, const std::string& shaderFile)
{
	std::ifstream inStream;

	inStream.open(shaderFile);

	if (!inStream)
	{
		std::cerr << "Could not open the shader file: " << shaderFile << std::endl;
		return 0;
	}

	std::string source(std::istreambuf_iterator<char>(inStream), (std::istreambuf_iterator<char>()));
	inStream.close();

	GLuint shader = glCreateShader(shaderType);

	const GLchar* sources[] = { source.c_str() };
	glShaderSource(shader, 1, sources, NULL);
	glCompileShader(shader);

	GLint compileStatus;
	// Get the shader compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	// If there was a compile error, get the error log and print it
	if (compileStatus != GL_TRUE)
	{
		GLint logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		GLchar* infoLog = new GLchar[logLength];
		glGetShaderInfoLog(shader, logLength, NULL, infoLog);

		std::cerr << infoLog << std::endl;

		delete infoLog;
		// There was an error, so return 0
		return 0;
	}
	// There was no error, so return the shader ID
	return shader;
}

GLuint CreateShaderProgram(std::vector<GLuint> shaders)
{
	GLuint program = glCreateProgram();

	// Attach all the shaders to the program then link them
	for (GLuint shader : shaders)
	{
		glAttachShader(program, shader);
	}

	glLinkProgram(program);

	// Check the status of the program
	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	// If there was an error linking, get the log and print it
	if (linkStatus != GL_TRUE)
	{
		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		GLchar * infoLog = new GLchar[logLength];

		glGetProgramInfoLog(program, logLength, NULL, infoLog);

		std::cerr << infoLog << std::endl;

		delete infoLog;
		return 0;
	}

	// Linking went okay, return the program ID
	return program;
}

void ReshapeGL(int width, int height)
{
	if (height <= 0)
	{
		height = 1;
	}

	if (width <= 0)
	{
		width = 1;
	}

	g_WindowWidth = width;
	g_WindowHeight = height;
	// Set viewport to fill the entire render window
	g_Camera.SetViewport(0, 0, width, height);
	g_Camera.SetProjectionRH(60.0f, width / (float)height, 0.1f, 100.0f);
	// Redraw the scene
	glutPostRedisplay();
}

void IdleGL()
{
	g_CurrentTicks = std::clock();
	float deltaTicks = (float)(g_CurrentTicks - g_PreviousTicks);
	g_PreviousTicks = g_CurrentTicks;

	float deltaTime = deltaTicks / (float)CLOCKS_PER_SEC;

	float cameraSpeed = 1.0f;
	
	g_Camera.Translate(glm::vec3(g_D - g_A, g_Q - g_E, g_S - g_W) * cameraSpeed * deltaTime);

	glutPostRedisplay();
}

void KeyboardGL(unsigned char c, int x, int y)
{
	switch (c)
	{
	case 'w':
	case 'W':
		g_W = 1;
		break;
	case 'a':
	case 'A':
		g_A = 1;
		break;
	case 's':
	case 'S':
		g_S = 1;
		break;
	case 'd':
	case 'D':
		g_D = 1;
		break;
	case 'q':
	case 'Q':
		g_Q = 1;
		break;
	case 'e':
	case 'E':
		g_E = 1;
		break;
	case 'r':
	case 'R':
		g_Camera.SetPosition(g_InitialCameraPosition);
		g_Rotation = glm::quat();
		break;
	case 27:
		// Escape key
		glutLeaveMainLoop();
		break;
	}
}

void KeyboardUpGL(unsigned char c, int x, int y)
{
	switch (c)
	{
	case 'w':
	case 'W':
		g_W = 0;
		break;
	case 'a':
	case 'A':
		g_A = 0;
		break;
	case 's':
	case 'S':
		g_S = 0;
		break;
	case 'd':
	case 'D':
		g_D = 0;
		break;
	case 'q':
	case 'Q':
		g_Q = 0;
		break;
	case 'e':
	case 'E':
		g_E = 0;
		break;

	default:
		break;
	}
}

void SpecialGL(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_SHIFT_L:
	case GLUT_KEY_SHIFT_R:
	{
		g_Shift = true;
	}
		break;
	}
}

void SpecialUpGL(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_SHIFT_L:
	case GLUT_KEY_SHIFT_R:
	{
		g_Shift = false;
	}
		break;
	}
}

void MouseGL(int button, int state, int x, int y)
{
	g_MousePosition = glm::ivec2(x, y);
}

void MouseMotionGL(int x, int y)
{
	glm::ivec2 mousePos = glm::ivec2(x, y);
	glm::vec2 delta = glm::vec2(mousePos - g_MousePosition);
	g_MousePosition = mousePos;

	std::cout << "dX: " << delta.x << " dY: " << delta.y << std::endl;

	glm::quat rotX = glm::angleAxis<float>(glm::radians(delta.y) * 0.5f, glm::vec3(1, 0, 0));
	glm::quat rotY = glm::angleAxis<float>(glm::radians(delta.x) * 0.5f, glm::vec3(0, 1, 0));

	//g_Camera.Rotate( rotX * rotY );
	g_Rotation = (rotX * rotY) * g_Rotation;
}