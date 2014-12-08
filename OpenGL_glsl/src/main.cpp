#include <OpenGL_Shaders.h>
#include <Camera.h>
#include <Ship.h>
#include <main.h>
#include <assimp/Importer.hpp>
#include <assimp/PostProcess.h>
#include <assimp/Scene.h>

#define BUFFER_OFFSET(offset) ((void*)(offset))
#define MEMBER_OFFSET(s,m) ((char*)NULL + (offsetof(s,m)))

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

// Global variables
// Window variables
int g_WindowHandle = 0;
int g_WindowWidth = 800;
int g_WindowHeight = 600;

// Button variables
int g_W, g_A, g_S, g_D, g_Q, g_E;
bool g_Shift = false;

// Camera variables
Camera g_Camera;
glm::vec3 g_InitialCameraPosition = glm::vec3(0, 0, 10);
glm::quat g_InitialCameraRotation;

// Mouse variables
glm::ivec2 g_MousePosition;

glm::quat g_Rotation;

// Time varialbes
std::clock_t g_PreviousTicks;
std::clock_t g_CurrentTicks;
float gl_time = 0.0f;

// Assimp variables
Assimp::Importer importer;
const aiScene* scene;
float modelScaleFactor;
std::vector<glm::mat4> matrixStack;
std::vector<Mesh> meshes;
static const std::string model = "../data/models/bench.obj";

// glsl handles
GLuint g_ShaderProgram = 0;

// Uniform handles
GLint g_UniformMVP = -1;
GLint g_TimeUniformId = -1;
GLuint materialUniformID;
GLuint matricesUniformID;

// Other handles
GLint g_PositionAttributeId;
GLint g_NormalAttributeId;
GLint g_AmbientColorAttributeId;
//GLint g_colorAttributeId;

// Shader handles
GLuint vertexShader;
GLuint fragmentShader;

// Shader paths
std::string vertexShaderFilePath = "../data/shaders/lambertianShader.vert";
std::string fragmentShaderFilePath = "../data/shaders/lambertianShader.frag";

// Game variables
std::vector<GLuint> g_vertexBuffers;
std::vector<GLuint> g_indexBuffers;

std::vector<float> g_Vertices;
std::vector<float> g_Normals;
std::vector<unsigned int> g_Indices;

// Forward declarations for functions
// Initialization functions
void InitGL(int argc, char * argv[]);
void InitGLEW();
GLuint LoadShader(GLenum shaderType, const std::string& shaderFile);
GLuint CreateShaderProgram(std::vector<GLuint> shaders);
void InitializeVertexArrayObjects();
void InitializeVertexBuffer();
void LoadEnemyShips();
void LoadBuffers();

// OpenGL functions
void IdleGL();
void DisplayGL();
void KeyboardGL(unsigned char c, int x, int y);
void KeyboardUpGL(unsigned char c, int x, int y);
void SpecialGL(int key, int x, int y);
void SpecialUpGL(int key, int x, int y);
void MouseGL(int button, int state, int x, int y);
void MouseMotionGL(int x, int y);
void ReshapeGL(int width, int height);

// Helper functions
void DrawElement(GLuint vao, GLuint shaderProgram, glm::mat4 model, std::vector<GLuint> indices, GLuint vertexBuffer);
bool ImportModelFromFile(const std::string& file);
void GetBoundingBox(aiVector3D* min, aiVector3D* max);
void GetBoundingBoxForNode(const aiNode* nd, aiVector3D* min, aiVector3D* max);
void DrawElements(const aiScene* scn, const aiNode* nd);
void GenerateVaosAndBuffers(const aiScene* scn);

void PushMatrix(glm::mat4 matrix);
void PopMatrix();


int main(int argc, char * argv[])
{
	// Initialize some global varialbes
	g_PreviousTicks = std::clock();
	g_A = g_W = g_S = g_D = g_Q = g_E = 0;

	// Initialize the camera position and rotation
	g_Camera.SetPosition(g_InitialCameraPosition);
	g_Camera.SetRotation(g_InitialCameraRotation);

	// Initialize OpenGL and glew
	InitGL(argc, argv);
	InitGLEW();

	// Try to import some models
	if (!ImportModelFromFile(model))
	{
		return 0;
	}

	// Load the shaders
	vertexShader = LoadShader(GL_VERTEX_SHADER, vertexShaderFilePath);
	fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fragmentShaderFilePath);

	std::vector<GLuint> shaders;
	shaders.push_back(vertexShader);
	shaders.push_back(fragmentShader);

	// Create the shader program.
	g_ShaderProgram = CreateShaderProgram(shaders);
	assert(g_ShaderProgram != 0);
	
	// Get handles for glsl vertex locations
	g_PositionAttributeId = glGetAttribLocation(g_ShaderProgram, "in_position");
	//colorAtribID = glGetAttribLocation(g_ShaderProgram, "in_color");
	g_AmbientColorAttributeId = glGetAttribLocation(g_ShaderProgram, "in_ambient");

	// Get handles for glsl uniform variables
	g_UniformMVP = glGetUniformLocation(g_ShaderProgram, "MVP");
	g_TimeUniformId = glGetUniformLocation(g_ShaderProgram, "time");

	GenerateVaosAndBuffers(scene);

	// Make sure we disable and unbind everything to prevent rendering issues later.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(g_PositionAttributeId);
	glDisableVertexAttribArray(g_AmbientColorAttributeId);

	glutMainLoop();
}

void GenerateVaosAndBuffers(const aiScene* scn)
{
	Material material;
	Mesh mesh;
	GLuint vertexBuffer, indexBuffer;

	for (unsigned int i = 0; i < scn->mNumMeshes; i++)
	{
		
		// Get the offset to this mesh's indices
		mesh.indexOffset = g_Indices.size();
		unsigned int indexCountBefore = g_Indices.size();

		int vertexIndexOffset = g_Vertices.size() / 3;

		const aiMesh* aiMesh = scn->mMeshes[i];

		unsigned int* faceArray;

		// Get vertices
		if (aiMesh->HasPositions())
		{
			for (unsigned int j = 0; j < aiMesh->mNumVertices; j++)
			{
				aiVector3D &vec = aiMesh->mVertices[j];

				g_Vertices.push_back(vec.x);
				g_Vertices.push_back(vec.y);
				g_Vertices.push_back(vec.z);
			}
		}
		// Get Normals
		if (aiMesh->HasNormals())
		{
			for (unsigned int j = 0; j < aiMesh->mNumVertices; j++)
			{
				aiVector3D &vec = aiMesh->mNormals[j];
				g_Normals.push_back(vec.x);
				g_Normals.push_back(vec.y);
				g_Normals.push_back(vec.z);
			}
		}
		// Get mesh indices
		for (unsigned int t = 0; t < aiMesh->mNumFaces; t++)
		{
			aiFace* face = &aiMesh->mFaces[t];
			// Only process faces with 3 indices
			if (face->mNumIndices != 3)
			{
				std::cout << "Found a mesh face with number of indices not equal to 3. Skipping this face." << std::endl;
				continue;
			}

			g_Indices.push_back(face->mIndices[0] + vertexIndexOffset);
			g_Indices.push_back(face->mIndices[1] + vertexIndexOffset);
			g_Indices.push_back(face->mIndices[2] + vertexIndexOffset);
		}

		mesh.indexCount = g_Indices.size() - indexCountBefore;
		mesh.material = &material;

		//faceArray = (unsigned int* )malloc(sizeof(unsigned int) * aiMesh->mNumFaces * 3);
		//unsigned int faceIndex = 0;
		//
		//for (unsigned int t = 0; t < aiMesh->mNumFaces; t++)
		//{
		//	const aiFace* face = &aiMesh->mFaces[t];

		//	memcpy(&faceArray[faceIndex], face->mIndices, 3 * sizeof(unsigned int));
		//	faceIndex += 3;
		//}

		mesh.numFaces = scn->mMeshes[i]->mNumFaces;

		// Generate VAO for mesh
		glGenVertexArrays(1, &(mesh.vao));
		glBindVertexArray(mesh.vao);
		//
		//// Generate buffer for the faces
		//glGenBuffers(1, &vertexBuffer);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffer);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * aiMesh->mNumFaces * 3, faceArray, GL_STATIC_DRAW);

		//// Vertex position buffer
		//if (aiMesh->HasPositions())
		//{
		//	glGenBuffers(1, &vertexBuffer);
		//	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * aiMesh->mNumVertices, aiMesh->mVertices, GL_STATIC_DRAW);
		//	glEnableVertexAttribArray(g_PositionAttributeId);
		//	glVertexAttribPointer(g_PositionAttributeId, 3, GL_FLOAT, 0, 0, 0);
		//}

		//// Vertex normal buffer
		//if (aiMesh->HasNormals())
		//{
		//	glGenBuffers(1, &vertexBuffer);
		//	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * aiMesh->mNumVertices, aiMesh->mNormals, GL_STATIC_DRAW);			
		//	glEnableVertexAttribArray(g_NormalAttributeId);
		//	glVertexAttribPointer(g_NormalAttributeId, 3, GL_FLOAT, 0, 0, 0);
		//}

		//// Unbind
		//glBindVertexArray(0);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//aiMaterial * aiMaterial = scn->mMaterials[aiMesh->mMaterialIndex];
		//aiColor4D diffuse(0.f, 0.f, 0.f, 0.f);
		//if (aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse) == AI_SUCCESS)
		//{
		//	glm::vec4 matDiffuse{ diffuse.r, diffuse.g, diffuse.b, diffuse.a };
		//	material.diffuse = matDiffuse;
		//}

		//aiColor4D ambient(0.f, 0.f, 0.f, 0.f);
		//if (aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_AMBIENT, &ambient) == AI_SUCCESS)
		//{
		//	glm::vec4 matAmbient{ ambient.r, ambient.g, ambient.b, ambient.a };
		//	material.ambient = matAmbient;
		//}

		//aiColor4D specular(0.f, 0.f, 0.f, 0.f);
		//if (aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_SPECULAR, &specular) == AI_SUCCESS)
		//{
		//	glm::vec4 matSpecular{ specular.r, specular.g, specular.b, specular.a };
		//	material.specular = matSpecular;
		//}

		//aiColor4D emissive(0.f, 0.f, 0.f, 0.f);
		//if (aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_EMISSIVE, &emissive) == AI_SUCCESS)
		//{
		//	glm::vec4 matEmmissive{ emissive.r, emissive.g, emissive.b, emissive.a };
		//	material.emissive = matEmmissive;
		//}

		//float shininess = 0.0f;
		//unsigned int max;
		//aiGetMaterialFloatArray(aiMaterial, AI_MATKEY_SHININESS, &shininess, &max);
		//material.shininess = shininess;

		meshes.push_back(mesh);
	}

	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &indexBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, g_Vertices.size() * sizeof(float), &g_Vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, g_Indices.size() * sizeof(unsigned int), &g_Indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(g_PositionAttributeId, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(g_PositionAttributeId);
}

void DisplayGL()
{
	// Clear the canvas
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(g_ShaderProgram);

	DrawElements(scene, scene->mRootNode);

	/*for (Ship s : g_enemyShips)
	{
		DrawElement(s.vaoId, g_ShaderProgram, s.GetModelMatrix(), s.GetIndices(), s.vertexBufferId);
	}*/

	gl_time += 0.01f;
	// Unbind program and vertex array, then swap buffers
	glUseProgram(0);
	glBindVertexArray(0);

	glutSwapBuffers();
}

void DrawElements(const aiScene* scn, const aiNode* nd)
{
	aiMatrix4x4 m = nd->mTransformation;
	m.Transpose();

	glm::mat4 modelMatrix{ m.a1, m.a2, m.a3, m.a4,
		m.b1, m.b2, m.b3, m.b4,
		m.c1, m.c2, m.c3, m.c4,
		m.d1, m.d2, m.d3, m.d4 };

	modelMatrix = modelMatrix * glm::toMat4(g_Rotation);

	for (int i = 0; i < nd->mNumMeshes; i++)
	{
		glBindVertexArray(meshes[nd->mMeshes[i]].vao);
		glm::mat4 mvp = g_Camera.GetProjectionMatrix() * g_Camera.GetViewMatrix() * modelMatrix;
		glUniformMatrix4fv(g_UniformMVP, 1, GL_FALSE, glm::value_ptr(mvp));

		glDrawElements(GL_TRIANGLES, meshes[nd->mMeshes[i]].indexCount, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
	}

	for (int j = 0; j < nd->mNumChildren; j++)
	{
		DrawElements(scn, nd->mChildren[j]);
	}
}

void DrawElement(GLuint vao, GLuint shaderProgram, glm::mat4 model, std::vector<GLuint> indices, GLuint vertexBuffer)
{
	glUseProgram(shaderProgram);
	glBindVertexArray(vao);

	glm::mat4 mvp = g_Camera.GetProjectionMatrix() * g_Camera.GetViewMatrix() * model;
	// Pass information to shaders
	glUniformMatrix4fv(g_UniformMVP, 1, GL_FALSE, glm::value_ptr(mvp));

	glUniform1fARB(g_TimeUniformId, gl_time);
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

bool ImportModelFromFile(const std::string& file)
{
	std::ifstream input(file.c_str());
	// If we can't read the file, print error and return
	if (!input.fail())
	{
		input.close();
	}
	else
	{
		std::cout << "Couldn't open model file: " << file << std::endl;
		std::cout << importer.GetErrorString() << std::endl;
		return false;
	}

	scene = importer.ReadFile(file, aiProcessPreset_TargetRealtime_Quality);
	// If there was an import error, print it and return
	if (!scene)
	{
		std::cout << importer.GetErrorString() << std::endl;
		return false;
	}

	aiVector3D sceneMin, sceneMax, sceneCenter;
	GetBoundingBox(&sceneMin, &sceneMax);

	float tmp;
	tmp = sceneMax.x - sceneMin.x;
	tmp = sceneMax.y - sceneMin.y > tmp ? sceneMax.y - sceneMin.y : tmp;
	tmp = sceneMax.z - sceneMin.z > tmp ? sceneMax.z - sceneMin.z : tmp;

	modelScaleFactor = 1.0f / tmp;

	return true;
}

void GetBoundingBox(aiVector3D* min, aiVector3D* max)
{

	min->x = min->y = min->z = 1e10f;
	max->x = max->y = max->z = -1e10f;
	GetBoundingBoxForNode(scene->mRootNode, min, max);
}

void GetBoundingBoxForNode(const aiNode* nd, aiVector3D* min, aiVector3D* max)
{
	aiMatrix4x4 prev;
	unsigned int n = 0, t;

	for (; n < nd->mNumMeshes; ++n) {
		const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		for (t = 0; t < mesh->mNumVertices; ++t) {

			aiVector3D tmp = mesh->mVertices[t];

			min->x = aisgl_min(min->x, tmp.x);
			min->y = aisgl_min(min->y, tmp.y);
			min->z = aisgl_min(min->z, tmp.z);

			max->x = aisgl_max(max->x, tmp.x);
			max->y = aisgl_max(max->y, tmp.y);
			max->z = aisgl_max(max->z, tmp.z);
		}
	}

	for (n = 0; n < nd->mNumChildren; ++n) {
		GetBoundingBoxForNode(nd->mChildren[n], min, max);
	}
}

void PushMatrix(glm::mat4 matrix)
{
	matrixStack.push_back(matrix);
}

void PopMatrix()
{
	matrixStack.pop_back();
}