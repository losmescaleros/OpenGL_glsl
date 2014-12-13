#include <OpenGL_Shaders.h>
#include <Camera.h>
#include <main.h>
#include <ModelInstance.h>
#include <Ship.h>
#include <Alien.h>
#include <Shot.h>
#include <Game.h>
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
std::clock_t g_ShipPreviousTicks;
std::clock_t g_CurrentTicks;
float gl_time = 0.0f;

// Assimp variables
Assimp::Importer importer;
const aiScene* scene;
float modelScaleFactor;
std::vector<glm::mat4> matrixStack;
std::vector<Mesh> meshes;

ModelAsset cubeAsset;
static const std::string cubeModel = "../data/models/block.obj";
ModelAsset shipAsset;
static const std::string shipModel = "../data/models/ship.obj";
ModelAsset alienAsset;
static const std::string alienModel = "../data/models/Space_Invader.obj";
ModelAsset laserAsset;
static const std::string laserModel = "../data/models/fighter_laser.3ds";
ModelInstance laserTest;

Shot shot;
Ship ship;
ModelInstance block1;
ModelInstance block2;
ModelInstance block3;
ModelInstance block4;

Alien alien1;
Alien alien2;
Alien alien3;
Alien alien4;
Alien alien5;
Alien alien6;
Alien alien7;
Alien alien8;
Alien alien9;
Alien alien10;
Alien alien11;
Alien alien12;
Alien alien13;
Alien alien14;
Alien alien15;
Alien alien16;
Alien alien17;
Alien alien18;
Alien alien19;
Alien alien20;
std::vector<Alien *> g_Aliens;

std::vector<ModelInstance*> g_Instances;

// glsl handles
GLuint g_ShaderProgram = 0;

// Uniform handles
GLint g_UniformMVP = -1;
GLint g_UniformColor = -1;
GLint g_TimeUniformId = -1;
GLuint materialUniformID;
GLuint matricesUniformID;

// Other handles
GLint g_PositionAttributeId;
GLint g_NormalAttributeId;
GLint g_AmbientColorAttributeId;
GLint g_DiffuseColorAttributeId;
GLint g_SpecularColorAttributeId;
//GLint g_colorAttributeId;

// Shader handles
GLuint vertexShader;
GLuint fragmentShader;

// Shader paths
std::string vertexShaderFilePath = "../data/shaders/lambertianShader.vert";
std::string fragmentShaderFilePath = "../data/shaders/lambertianShader.frag";

// Game variables
Game g_Game;
GLuint vertexBuffer, indexBuffer;

std::vector<GLuint> g_vertexBuffers;
std::vector<GLuint> g_indexBuffers;

std::vector<Vertex> g_Vertices;
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
void ImportAssets();
bool ImportModelFromFile(const std::string& file);
void GetBoundingBox(aiVector3D* min, aiVector3D* max);
void GetBoundingBoxForNode(const aiNode* nd, aiVector3D* min, aiVector3D* max);
void DrawElements(const aiScene* scn, const aiNode* nd);
void GenerateVaosAndBuffers(const aiScene* scn, ModelAsset* asset);
void CreateModelInstances();
void LoadAsset(const aiScene* scn, ModelAsset* asset);
void DrawInstance(ModelInstance* inst);
void UpdateAliens(float deltaTime);
void DoInstanceLogic();
void TryToFire();

glm::mat4 Translate(GLfloat x, GLfloat y, GLfloat z);

void PushMatrix(glm::mat4 matrix);
void PopMatrix();


int main(int argc, char * argv[])
{
	// Initialize some global varialbes
	g_PreviousTicks = std::clock();
	g_ShipPreviousTicks = std::clock();

	g_A = g_W = g_S = g_D = g_Q = g_E = 0;
	g_Game.m_GameOver = false;
	g_Game.m_AlienDirection = 1.0f;
	g_Game.m_AlienMovementSpeed = 1.0f;
	g_Game.m_AlienHitWall = false;

	// Initialize the camera position and rotation
	g_Camera.SetPosition(g_InitialCameraPosition);
	g_Camera.SetRotation(g_InitialCameraRotation);

	// Initialize OpenGL and glew
	InitGL(argc, argv);
	InitGLEW();	

	// Load the shaders and push them onto the shader list
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
	g_NormalAttributeId = glGetAttribLocation(g_ShaderProgram, "in_normal");

	// Get handles for glsl uniform variables
	g_UniformMVP = glGetUniformLocation(g_ShaderProgram, "MVP");
	g_UniformColor = glGetUniformLocation(g_ShaderProgram, "color");

	// Make sure we disable and unbind everything to prevent rendering issues later.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(g_PositionAttributeId);

	ImportAssets();

	CreateModelInstances();

	glutMainLoop();
}

glm::mat4 Translate(GLfloat x, GLfloat y, GLfloat z)
{
	return glm::translate(glm::mat4(), glm::vec3(x, y, z));
}

void CreateModelInstances()
{
	block1.SetAsset(&cubeAsset);
	block1.Translate(glm::vec3(-6, -3.5, 0));
	block1.Scale(glm::vec3(1, 1, 0.5));
	g_Instances.push_back(&block1);

	block2.SetAsset(&cubeAsset);
	block2.Translate(glm::vec3(-2, -3.5, 0));
	block2.Scale(glm::vec3(1, 1, 0.5));
	g_Instances.push_back(&block2);

	block3.SetAsset(&cubeAsset);
	block3.Translate(glm::vec3(2, -3.5, 0));
	block3.Scale(glm::vec3(1, 1, 0.5));
	g_Instances.push_back(&block3);

	block4.SetAsset(&cubeAsset);
	block4.Translate(glm::vec3(6, -3.5, 0));
	block4.Scale(glm::vec3(1, 1, 0.5));
	g_Instances.push_back(&block4);

	alien1.SetAsset(&alienAsset);
	alien1.m_Game = &g_Game;
	alien1.m_DeltaX = 1.0f;
	alien1.m_HitWall = false;
	alien1.Scale(glm::vec3(0.08, 0.08, 0.01));
	alien1.Translate(glm::vec3(-6, 4, 0));
	
	g_Instances.push_back(&alien1);
	g_Aliens.push_back(&alien1);

	alien2.SetAsset(&alienAsset);
	alien2.m_Game = &g_Game;
	alien2.m_DeltaX = 1.0f;
	alien2.m_HitWall = false;
	alien2.Scale(glm::vec3(0.08, 0.08, 0.01));
	alien2.Translate(glm::vec3(-5, 4, 0));

	g_Instances.push_back(&alien2);
	g_Aliens.push_back(&alien2);

	alien3.SetAsset(&alienAsset);
	alien3.m_Game = &g_Game;
	alien3.m_DeltaX = 1.0f;
	alien3.m_HitWall = false;
	alien3.Scale(glm::vec3(0.08, 0.08, 0.01));
	alien3.Translate(glm::vec3(-4, 4, 0));

	g_Instances.push_back(&alien3);
	g_Aliens.push_back(&alien3);

	alien4.SetAsset(&alienAsset);
	alien4.m_Game = &g_Game;
	alien4.m_DeltaX = 1.0f;
	alien4.m_HitWall = false;
	alien4.Scale(glm::vec3(0.08, 0.08, 0.01));
	alien4.Translate(glm::vec3(-3, 4, 0));

	g_Instances.push_back(&alien4);
	g_Aliens.push_back(&alien4);

	alien5.SetAsset(&alienAsset);
	alien5.m_Game = &g_Game;
	alien5.m_DeltaX = 1.0f;
	alien5.m_HitWall = false;
	alien5.Scale(glm::vec3(0.08, 0.08, 0.01));
	alien5.Translate(glm::vec3(-2, 4, 0));

	g_Instances.push_back(&alien5);
	g_Aliens.push_back(&alien5);

	alien6.SetAsset(&alienAsset);
	alien6.m_Game = &g_Game;
	alien6.m_DeltaX = 1.0f;
	alien6.m_HitWall = false;
	alien6.Scale(glm::vec3(0.08, 0.08, 0.01));
	alien6.Translate(glm::vec3(-1, 4, 0));

	g_Instances.push_back(&alien6);
	g_Aliens.push_back(&alien6);

	alien7.SetAsset(&alienAsset);
	alien7.m_Game = &g_Game;
	alien7.m_DeltaX = 1.0f;
	alien7.m_HitWall = false;
	alien7.Scale(glm::vec3(0.08, 0.08, 0.01));
	alien7.Translate(glm::vec3(0, 4, 0));

	g_Instances.push_back(&alien7);
	g_Aliens.push_back(&alien7);

	alien8.SetAsset(&alienAsset);
	alien8.m_Game = &g_Game;
	alien8.m_DeltaX = 1.0f;
	alien8.m_HitWall = false;
	alien8.Scale(glm::vec3(0.08, 0.08, 0.01));
	alien8.Translate(glm::vec3(1, 4, 0));

	g_Instances.push_back(&alien8);
	g_Aliens.push_back(&alien8);

	alien9.SetAsset(&alienAsset);
	alien9.m_Game = &g_Game;
	alien9.m_DeltaX = 1.0f;
	alien9.m_HitWall = false;
	alien9.Scale(glm::vec3(0.08, 0.08, 0.01));
	alien9.Translate(glm::vec3(2, 4, 0));

	g_Instances.push_back(&alien9);
	g_Aliens.push_back(&alien9);

	alien10.SetAsset(&alienAsset);
	alien10.m_Game = &g_Game;
	alien10.m_DeltaX = 1.0f;
	alien10.m_HitWall = false;
	alien10.Scale(glm::vec3(0.08, 0.08, 0.01));
	alien10.Translate(glm::vec3(3, 4, 0));

	g_Instances.push_back(&alien10);
	g_Aliens.push_back(&alien10);

	alien11.SetAsset(&alienAsset);
	alien11.m_Game = &g_Game;
	alien11.m_DeltaX = 1.0f;
	alien11.m_HitWall = false;
	alien11.Scale(glm::vec3(0.08, 0.08, 0.01));
	alien11.Translate(glm::vec3(-6, 3, 0));

	g_Instances.push_back(&alien11);
	g_Aliens.push_back(&alien11);

	alien12.SetAsset(&alienAsset);
	alien12.m_Game = &g_Game;
	alien12.m_DeltaX = 1.0f;
	alien12.m_HitWall = false;
	alien12.Scale(glm::vec3(0.08, 0.08, 0.01));
	alien12.Translate(glm::vec3(-5, 3, 0));

	g_Instances.push_back(&alien12);
	g_Aliens.push_back(&alien12);

	alien13.SetAsset(&alienAsset);
	alien13.m_Game = &g_Game;
	alien13.m_DeltaX = 1.0f;
	alien13.m_HitWall = false;
	alien13.Scale(glm::vec3(0.08, 0.08, 0.01));
	alien13.Translate(glm::vec3(-4, 3, 0));

	g_Instances.push_back(&alien13);
	g_Aliens.push_back(&alien13);

	alien14.SetAsset(&alienAsset);
	alien14.m_Game = &g_Game;
	alien14.m_DeltaX = 1.0f;
	alien14.m_HitWall = false;
	alien14.Scale(glm::vec3(0.08, 0.08, 0.01));
	alien14.Translate(glm::vec3(-3, 3, 0));

	g_Instances.push_back(&alien14);
	g_Aliens.push_back(&alien14);

	alien15.SetAsset(&alienAsset);
	alien15.m_Game = &g_Game;
	alien15.m_DeltaX = 1.0f;
	alien15.m_HitWall = false;
	alien15.Scale(glm::vec3(0.08, 0.08, 0.01));
	alien15.Translate(glm::vec3(-2, 3, 0));

	g_Instances.push_back(&alien15);
	g_Aliens.push_back(&alien15);

	alien16.SetAsset(&alienAsset);
	alien16.m_Game = &g_Game;
	alien16.m_DeltaX = 1.0f;
	alien16.m_HitWall = false;
	alien16.Scale(glm::vec3(0.08, 0.08, 0.01));
	alien16.Translate(glm::vec3(-1, 3, 0));

	g_Instances.push_back(&alien16);
	g_Aliens.push_back(&alien16);

	alien17.SetAsset(&alienAsset);
	alien17.m_Game = &g_Game;
	alien17.m_DeltaX = 1.0f;
	alien17.m_HitWall = false;
	alien17.Scale(glm::vec3(0.08, 0.08, 0.01));
	alien17.Translate(glm::vec3(0, 3, 0));

	g_Instances.push_back(&alien17);
	g_Aliens.push_back(&alien17);

	alien18.SetAsset(&alienAsset);
	alien18.m_Game = &g_Game;
	alien18.m_DeltaX = 1.0f;
	alien18.m_HitWall = false;
	alien18.Scale(glm::vec3(0.08, 0.08, 0.01));
	alien18.Translate(glm::vec3(1, 3, 0));

	g_Instances.push_back(&alien18);
	g_Aliens.push_back(&alien18);

	alien19.SetAsset(&alienAsset);
	alien19.m_Game = &g_Game;
	alien19.m_DeltaX = 1.0f;
	alien19.m_HitWall = false;
	alien19.Scale(glm::vec3(0.08, 0.08, 0.01));
	alien19.Translate(glm::vec3(2, 3, 0));

	g_Instances.push_back(&alien19);
	g_Aliens.push_back(&alien19);

	alien20.SetAsset(&alienAsset);
	alien20.m_Game = &g_Game;
	alien20.m_DeltaX = 1.0f;
	alien20.m_HitWall = false;
	alien20.Scale(glm::vec3(0.08, 0.08, 0.01));
	alien20.Translate(glm::vec3(3, 3, 0));

	g_Instances.push_back(&alien20);
	g_Aliens.push_back(&alien20);

	ship.SetAsset(&shipAsset);
	ship.Translate(glm::vec3(0, -4.5, 0));
	//ship.asset = &shipAsset;
	/*ship.transform = ship.transform * glm::rotate(ship.transform, -90.0f, glm::vec3(1, 0, 0));
	ship.transform = glm::translate(ship.transform, glm::vec3(0, 1, -4.5));*/	
	g_Instances.push_back(&ship);

	/*laserTest.SetAsset(&laserAsset);
	g_Instances.push_back(&laserTest);*/
}

void ImportAssets()
{
	ImportModelFromFile(cubeModel);
	LoadAsset(scene, &cubeAsset);

	ImportModelFromFile(shipModel);
	LoadAsset(scene, &shipAsset);

	ImportModelFromFile(alienModel);
	LoadAsset(scene, &alienAsset);

	ImportModelFromFile(laserModel);
	LoadAsset(scene, &laserAsset);
}

//bool ImportAssets(std::string modelFile, ModelAsset* asset)
//{
//	ImportModelFromFile(modelFile);
//
//	GenerateVaosAndBuffers(scene, asset);
//
//	glGenBuffers(1, &vertexBuffer);
//	glGenBuffers(1, &indexBuffer);
//
//	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
//	glBufferData(GL_ARRAY_BUFFER, g_Vertices.size() * sizeof(Vertex), &g_Vertices[0], GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, g_Indices.size() * sizeof(unsigned int), &g_Indices[0], GL_STATIC_DRAW);
//	// Position and normal attributes
//	glVertexAttribPointer(g_PositionAttributeId, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), MEMBER_OFFSET(Vertex, Position));
//	glEnableVertexAttribArray(g_PositionAttributeId);
//
//	//glVertexAttribPointer(g_NormalAttributeId, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), MEMBER_OFFSET(Vertex, Normal));
//	//glEnableVertexAttribArray(g_NormalAttributeId);
//	//// Color attributes
//	//glVertexAttribPointer(g_AmbientColorAttributeId, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), MEMBER_OFFSET(Vertex, Ambient));
//	//glEnableVertexAttribArray(g_AmbientColorAttributeId);
//
//	//glVertexAttribPointer(g_DiffuseColorAttributeId, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), MEMBER_OFFSET(Vertex, Diffuse));
//	//glEnableVertexAttribArray(g_DiffuseColorAttributeId);
//
//	//glVertexAttribPointer(g_SpecularColorAttributeId, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), MEMBER_OFFSET(Vertex, Specular));
//	//glEnableVertexAttribArray(g_SpecularColorAttributeId);
//
//	return true;
//}

//void GenerateVaosAndBuffers(const aiScene* scn, ModelAsset* asset)
//{
//	Material material;
//	Mesh mesh;	
//	
//	for (unsigned int i = 0; i < scn->mNumMeshes; i++)
//	{
//		// Get the offset to this mesh's indices
//		asset->indexOffset = g_Indices.size();
//		// mesh.indexOffset = g_Indices.size();
//
//		unsigned int indexCountBefore = g_Indices.size();
//		int vertexIndexOffset = g_Vertices.size() / 3;
//
//		const aiMesh* aiMesh = scn->mMeshes[i];
//		unsigned int* faceArray;
//
//		for (unsigned j = 0; j < aiMesh->mNumVertices; j++)
//		{
//			Vertex v;
//
//			// Get vertex positions
//			if (aiMesh->HasPositions())
//			{
//				aiVector3D &vec = aiMesh->mVertices[j];
//				v.Position = glm::vec3(vec.x, vec.y, vec.z);
//			}
//			// Get vertex normals
//			/*if (aiMesh->HasNormals())
//			{
//			aiVector3D &vec = aiMesh->mNormals[j];
//			v.Normal = glm::vec3(vec.x, vec.y, vec.z);
//			}*/
//
//			//aiMaterial * aiMaterial = scn->mMaterials[aiMesh->mMaterialIndex];
//			//aiColor4D ambient(0.f, 0.f, 0.f, 0.f);
//			//
//			//if (aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_AMBIENT, &ambient) == AI_SUCCESS)
//			//{
//			//	// material.ambient = matAmbient;
//			//}
//			//glm::vec4 matAmbient{ ambient.r, ambient.g, ambient.b, ambient.a };
//			//v.Ambient = matAmbient;
//
//			//aiColor4D diffuse(0.f, 0.f, 0.f, 0.f);
//
//			//if (aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_AMBIENT, &diffuse) == AI_SUCCESS)
//			//{
//			//	// material.ambient = matAmbient;
//			//}
//			//glm::vec4 matDiffuse{ diffuse.r, diffuse.g, diffuse.b, diffuse.a };
//			//v.Diffuse = matDiffuse;
//
//			//aiColor4D specular(0.f, 0.f, 0.f, 0.f);
//
//			//if (aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_AMBIENT, &specular) == AI_SUCCESS)
//			//{
//			//	// material.ambient = matAmbient;
//			//}
//			//glm::vec4 matSpecular{ specular.r, specular.g, specular.b, specular.a };
//			//v.Ambient = matSpecular;
//
//			g_Vertices.push_back(v);
//		}
//		// Get mesh indices
//		for (unsigned int t = 0; t < aiMesh->mNumFaces; t++)
//		{
//			aiFace* face = &aiMesh->mFaces[t];
//			// Only process faces with 3 indices
//			if (face->mNumIndices != 3)
//			{
//				std::cout << "Found a mesh face with number of indices not equal to 3. Skipping this face." << std::endl;
//				continue;
//			}
//
//			g_Indices.push_back(face->mIndices[0] + vertexIndexOffset);
//			g_Indices.push_back(face->mIndices[1] + vertexIndexOffset);
//			g_Indices.push_back(face->mIndices[2] + vertexIndexOffset);
//		}
//
//		asset->indexCount = g_Indices.size() - indexCountBefore;
//		asset->material = &material;
//
//		/*mesh.indexCount = g_Indices.size() - indexCountBefore;
//		mesh.material = &material;*/
//		//mesh.numFaces = scn->mMeshes[i]->mNumFaces;
//
//		// Generate VAO for mesh
//		/*glGenVertexArrays(1, &(mesh.vao));
//		glBindVertexArray(mesh.vao);*/
//
//		glGenVertexArrays(1, &asset->vao);
//		glBindVertexArray(asset->vao);
//
//		// meshes.push_back(mesh);
//	}
//}

void DisplayGL()
{
	// Clear the canvas
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(g_ShaderProgram);

	for (int i = 0; i < g_Instances.size(); i++)
	{
		if (!g_Instances[i]->m_Hidden)
		{
			DrawInstance(g_Instances[i]);
		}				
	}

	// Unbind program and vertex array, then swap buffers
	glUseProgram(0);
	glBindVertexArray(0);

	glutSwapBuffers();
}
void DrawInstance(ModelInstance* inst)
{
	ModelAsset* asset = inst->GetModelAsset();
	glBindVertexArray(asset->vao);
	glm::mat4 modelMatrix = inst->GetModelMatrix();
	glm::mat4 mvp = g_Camera.GetProjectionMatrix() * g_Camera.GetViewMatrix() * modelMatrix;
	glUniformMatrix4fv(g_UniformMVP, 1, GL_FALSE, glm::value_ptr(mvp));

	/*glm::vec4 color = inst.asset->material->diffuse;
	float c[4] = { color.r, color.g, color.b, color.a };
	glUniform4fv(g_UniformColor, 1, c);*/

	glDrawElements(asset->drawType, asset->indexCount, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	glBindVertexArray(0);
}

//void DrawElements(const aiScene* scn, const aiNode* nd)
//{
//	aiMatrix4x4 m = nd->mTransformation;
//	m.Transpose();
//
//	glm::mat4 modelMatrix = glm::toMat4(g_Rotation);
//
//	for (int i = 0; i < meshes.size(); i++)
//	{
//		glBindVertexArray(meshes[i].vao);
//		glm::mat4 mvp = g_Camera.GetProjectionMatrix() * g_Camera.GetViewMatrix() * modelMatrix;
//		glUniformMatrix4fv(g_UniformMVP, 1, GL_FALSE, glm::value_ptr(mvp));
//		
//
//		glDrawElements(GL_TRIANGLES, meshes[i].indexCount, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
//	}
//
//	/*for (int j = 0; j < nd->mNumChildren; j++)
//	{
//		DrawElements(scn, nd->mChildren[j]);
//	}*/
//}

//void DrawElement(GLuint vao, GLuint shaderProgram, glm::mat4 model, std::vector<GLuint> indices, GLuint vertexBuffer)
//{
//	glUseProgram(shaderProgram);
//	glBindVertexArray(vao);
//
//	glm::mat4 mvp = g_Camera.GetProjectionMatrix() * g_Camera.GetViewMatrix() * model;
//	// Pass information to shaders
//	glUniformMatrix4fv(g_UniformMVP, 1, GL_FALSE, glm::value_ptr(mvp));
//
//	glUniform1fARB(g_TimeUniformId, gl_time);
//	// Draw
//	glDrawElements(GL_TRIANGLES,
//		indices.size(),
//		GL_UNSIGNED_INT,
//		BUFFER_OFFSET(0));
//}

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

void DoInstanceLogic()
{
	if (g_Game.m_GameLogicRequired)
	{
		for (int i = 0; i < g_Instances.size(); i++)
		{
			g_Instances[i]->DoLogic();
		}
		g_Game.m_GameLogicRequired = false;
	}
}

void IdleGL()
{
	if (!g_Game.m_GameOver)
	{
		g_CurrentTicks = std::clock();
		float deltaTicks = (float)(g_CurrentTicks - g_PreviousTicks);
		float deltaTime = deltaTicks / (float)CLOCKS_PER_SEC;
		g_PreviousTicks = g_CurrentTicks;
	
		// Update alien positions
		UpdateAliens(deltaTime);
		
		shot.MoveStep(deltaTime);

		// Update ship position
		ship.Translate(glm::vec3(g_D - g_A, 0, 0) * Ship::MOVEMENT_SPEED * deltaTime);
		// Update camera position
		float cameraSpeed = 1.0f;
		g_Camera.Translate(glm::vec3(0, g_Q - g_E, g_S - g_W) * cameraSpeed * deltaTime);
		// Redraw
		glutPostRedisplay();
		// Perform any changes to instance logic
		DoInstanceLogic();
	}
	else
	{
		exit(0);
	}
}

void UpdateAliens(float deltaTime)
{
	for (int i = 0; i < g_Aliens.size(); i++)
	{
		g_Aliens[i]->MoveStep(deltaTime);
	}

}

void KeyboardGL(unsigned char c, int x, int y)
{
	switch (c)
	{
	case ' ':
		TryToFire();
		break;
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

void LoadAsset(const aiScene* scn, ModelAsset* asset)
{
	asset->drawType = GL_TRIANGLES;
	asset->indexOffset = 0;
	asset->indexCount = 0;
	
	glGenVertexArrays(1, &asset->vao);
	glBindVertexArray(asset->vao);

	glGenBuffers(1, &asset->vbo);	

	glGenBuffers(1, &asset->indexBuffer);


	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < scn->mNumMeshes; i++)
	{
		unsigned int indexCountBefore = indices.size();
		int vertexIndexOffset = vertices.size() / 3;

		const aiMesh* aiMesh = scn->mMeshes[i];
		
		// get the vertices 
		for (unsigned j = 0; j < aiMesh->mNumVertices; j++)
		{
			Vertex v;

			aiVector3D &vec = aiMesh->mVertices[j];
			v.Position = glm::vec3(vec.x, vec.y, vec.z);
			v.Normal = glm::vec3(0, 0, 0);

			if (aiMesh->HasNormals())
			{
				aiVector3D &norm = aiMesh->mNormals[j];
				v.Normal = glm::vec3(norm.x, norm.y, norm.z);
			}

			vertices.push_back(v);
		}
		for (unsigned int t = 0; t < aiMesh->mNumFaces; t++)
		{
			aiFace* face = &aiMesh->mFaces[t];
			if (face->mNumIndices != 3)
			{
				std::cout << "Found a mesh face with number of indices not equal to 3. Skipping this face." << std::endl;
				continue;
			}
			indices.push_back(face->mIndices[0] + vertexIndexOffset);
			indices.push_back(face->mIndices[1] + vertexIndexOffset);
			indices.push_back(face->mIndices[2] + vertexIndexOffset);
		}
		aiMaterial* aiMaterial = scn->mMaterials[aiMesh->mMaterialIndex];
		// TODO: get material parameters

		aiColor4D diffuse(0.f, 0.f, 0.f, 0.f);

		Material* material = new Material();
		
		// asset->material->diffuse = glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
		if (aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse) == AI_SUCCESS)
		{
			material->diffuse = glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
		}
		asset->material = material;
		// asset->material = material;
		asset->indexCount = indices.size() - indexCountBefore;
	}

	glBindBuffer(GL_ARRAY_BUFFER, asset->vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, asset->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(g_PositionAttributeId, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), MEMBER_OFFSET(Vertex, Position));
	glEnableVertexAttribArray(g_PositionAttributeId);

	glVertexAttribPointer(g_NormalAttributeId, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), MEMBER_OFFSET(Vertex, Normal));
	glEnableVertexAttribArray(g_NormalAttributeId);
	// Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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

void TryToFire()
{	
	shot.SetAsset(&laserAsset);
	shot.Reinitialize(ship.GetPosition());
	g_Instances.push_back(&shot);
}