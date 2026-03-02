#include "lab_m1/lab2/lab2.h"
#include "lab_m1/Tema2/Tema2.h"
#include "lab_m1/lab5/lab_camera.h"
#include "components/text_renderer.h"
#include <iostream>
#include <vector>
#include "core/engine.h"
#include "utils/gl_utils.h"
#include <random>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


using namespace std;
using namespace m1;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */
float value1 = 0.0f;       // goes between 0 and 1
float direction1 = 1.0f;   // +1 or -1
float speed1 = 1.0f;  // speed of animation
glm::vec3 masinaPos = glm::vec3(0, 0, 0);
float masinaAngle = 0.0f;

float moveSpeed = 3.0f;
float turnSpeed = 2.5f;
float timer = 0.0f;
float stationTimer = 0.0f;
float stationTimer2 = 0.0f;
float railCheckTimer = 0.0f;
bool trainStopped = false;
bool trainStopped2 = false;
float repairTimer = 0.0f;
std::random_device rd;
std::mt19937 gen(rd());	
std::uniform_int_distribution<int> dist(0, 2);
int random_gara;
int random_gara2;
float playTime = 0.0f;
bool gameOver = false;
float train1WaitTimer = 0.0f;
float train2WaitTimer = 0.0f;
float maxWaitTime = 30.0f;
bool hardMode = false;


struct Station {
    glm::vec3 center;
    float width;
    float depth;
    bool trainInside = false;
    bool shouldStop = true;  // Add this flag
    
    Station(glm::vec3 pos, float w, float d, bool stop = true) 
        : center(pos), width(w), depth(d), shouldStop(stop) {}
    
    bool contains(glm::vec3 point) {
        float halfWidth = width / 2.0f;
        float halfDepth = depth / 2.0f;
        
        return (point.x >= center.x - halfWidth && point.x <= center.x + halfWidth &&
                point.z >= center.z - halfDepth && point.z <= center.z + halfDepth);
    }
};


struct Rail {
    glm::vec3 startPosition;
    glm::vec3 endPosition;
    int rotate;      // Rotation in degrees
    Rail* next = nullptr;  // Linked list pointer
    bool isBroken = false;
    bool carInside = false;

    // Constructor
    Rail(const glm::vec3& start,
         const glm::vec3& end,
         int rotate = 0)  // default rotate = 0
        : startPosition(start), endPosition(end), rotate(rotate), isBroken(false), carInside(false) {}
	
    bool contains(glm::vec3 point) {
        glm::vec3 ab = endPosition - startPosition;
        glm::vec3 ap = point - startPosition;
        float t = glm::dot(ap, ab) / glm::dot(ab, ab);
        t = glm::clamp(t, 0.0f, 1.0f);
        glm::vec3 closest = startPosition + ab * t;
        return glm::distance(point, closest) < 1.5f;
    }
};

// Train moving along the rail list
struct Train {
    Rail* rail = nullptr;   // Current rail segment
    float progress = 0.f;   // 0.0 -> 1.0 on current rail
    float speed = 0.f;      // Units per second
    glm::vec3 position;

};

//struct Rail *rail;
struct Train train;
struct Train train2;
std::vector<Rail> rails;
std::vector<Station> stations;
std::vector<Station> stations2;

Tema2::Tema2() {}

Tema2::~Tema2() {
    delete camera;
}

void Tema2::Init() {
	random_gara = dist(gen);  // 0, 1, or 2	
	random_gara2 = dist(gen);
    camera = new implemented::Camera();
	// Create text renderer
	textRenderer = new gfxc::TextRenderer(window->props.selfDir, window->GetResolution().x, window->GetResolution().y);
	textRenderer->Load(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.ttf"), 24);


	rails.emplace_back(
    glm::vec3(-24.f, 0.01f,  3.f),
    glm::vec3(-17.f, 0.01f,  3.f), 1
);

rails.emplace_back(
    glm::vec3(-17.f, 0.01f,  2.f),
    glm::vec3(-17.f, 0.01f, -8.f), 0
);

rails.emplace_back(
    glm::vec3(-16.f, 0.01f, -8.f), 
    glm::vec3( -2.f, 0.01f, -8.f), 1
);

rails.emplace_back(
    glm::vec3( -2.f, 0.01f, -9.f),
    glm::vec3( -2.f, 0.01f,-12.f), 0
);

rails.emplace_back(
    glm::vec3( -2.f, 0.01f,-13.f),
    glm::vec3( -2.f, 0.01f,-16.f)
);

rails.emplace_back(
    glm::vec3( -1.f, 0.01f,-16.f),
    glm::vec3( 17.f, 0.01f,-16.f)
);

rails.emplace_back(
    glm::vec3( 17.f, 0.01f,-15.f),
    glm::vec3( 17.f, 0.01f, -2.f)
);

rails.emplace_back(
    glm::vec3(16.99f, 0.02f, -1.f),
    glm::vec3(16.99f, 0.02f,  2.f)
);

rails.emplace_back(
    glm::vec3(17.f, 0.01f,  3.f),
    glm::vec3(17.f, 0.01f, 12.f)
);

rails.emplace_back(
    glm::vec3(16.f, 0.01f, 12.f),
    glm::vec3(-7.f, 0.01f, 12.f)
);

rails.emplace_back(
    glm::vec3(-8.f, 0.02f, 12.f),
    glm::vec3(-26.f,0.01f, 12.f)
);

rails.emplace_back(
    glm::vec3(-24.f,0.01f, 12.f),
    glm::vec3(-24.f,0.01f,  4.f)
);
	for (size_t i = 0; i < rails.size() - 1; i++)
    rails[i].next = &rails[i + 1];

rails.back().next = &rails[0];  // Loop back

	// Define stations matching your "gara" positions
stations.emplace_back(glm::vec3(9, 1, 13.5f), 4.0f, 4.0f, false);      // First station
stations.emplace_back(glm::vec3(6, 1, -17.5f), 4.0f, 4.0f, false);     // Second station
stations.emplace_back(glm::vec3(-18.5f, 1, -2), 4.0f, 4.0f, false);    // Third station
stations[random_gara].shouldStop = true;

stations2.emplace_back(glm::vec3(9, 1, 13.5f), 4.0f, 4.0f, false);      // First station
stations2.emplace_back(glm::vec3(6, 1, -17.5f), 4.0f, 4.0f, false);     // Second station
stations2.emplace_back(glm::vec3(-18.5f, 1, -2), 4.0f, 4.0f, false);    // Third station
stations2[random_gara2].shouldStop = true;

	train.speed = 0.2f;
	train2.speed = 0.2f;
	train2.rail = &rails[6];
	
	const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "textures");

    // Load textures
    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "rusty_metal_05_diff_4k.jpg").c_str(), GL_REPEAT);
        mapTextures["metal"] = texture;
    }

	{
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "snow_02_diff_4k.jpg").c_str(), GL_REPEAT);
        mapTextures["snow"] = texture;
    }

	{
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "wood_shutter_diff_4k.jpg").c_str(), GL_REPEAT);
        mapTextures["wood"] = texture;
    }

	{
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "metal_plate_diff_4k.jpg").c_str(), GL_REPEAT);
        mapTextures["metal_platforma"] = texture;
    }

	{
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "green_metal_rust_diff_4k.jpg").c_str(), GL_REPEAT);
        mapTextures["metal_top"] = texture;
    }

	{
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "scuba_suede_diff_4k.jpg").c_str(), GL_REPEAT);
        mapTextures["apa"] = texture;
    }

	{
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "gray_rocks_diff_4k.jpg").c_str(), GL_REPEAT);
        mapTextures["piatra"] = texture;
    }



    {
        Shader *shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }



	cullFace = GL_BACK;
	polygonMode = GL_FILL;

	// Load a mesh from file into GPU memory
	{
		// Mesh *mesh = new Mesh("box");
		// mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS,
		// 						 "primitives"),
		// 			   "box.obj");
		// meshes[mesh->GetMeshID()] = mesh;
	}

	// Create a mesh box using custom data
	{
		// vector<VertexFormat> vertices{
		// 	VertexFormat(glm::vec3(0, 0, 1), glm::vec3(0, 1, 1),
		// 				 glm::vec3(0.2, 0.8, 0.6)),
		// 	VertexFormat(glm::vec3(1, 0, 1), glm::vec3(0, 0, 1),
		// 				 glm::vec3(0.2, 0.7, 0.1)),
		// 	VertexFormat(glm::vec3(0, 1, 1), glm::vec3(1, 0, 1),
		// 				 glm::vec3(0.2, 0.6, 0.2)),
		// 	VertexFormat(glm::vec3(1, 1, 1), glm::vec3(0, 1, 1),
		// 				 glm::vec3(0.2, 0.5, 0.3)),
		// 	VertexFormat(glm::vec3(0, 0, -1), glm::vec3(0, 0, 1),
		// 				 glm::vec3(0.2, 0.4, 0.4)),
		// 	VertexFormat(glm::vec3(1, 0, -1), glm::vec3(1, 0, 1),
		// 				 glm::vec3(0.2, 0.3, 0.5)),
		// 	VertexFormat(glm::vec3(0, 1, -1), glm::vec3(0, 1, 1),
		// 				 glm::vec3(0.2, 0.2, 0.6)),
		// 	VertexFormat(glm::vec3(1, 1, -1), glm::vec3(0, 0, 1),
		// 				 glm::vec3(0.2, 0.1, 0.7)),

		// };

		// vector<unsigned int> indices = {
		// 	0, 1, 2, // indices for first triangle
		// 	1, 3, 2, // indices for second triangle
		// 			 // TODO(student): Complete indices data for the cube mesh
		// 	2, 3, 7, 2, 7, 6, 1, 7, 3, 1, 5, 7, 6, 7, 4, 7, 5, 4, 0, 4, 1, 1, 4,
		// 	5, 2, 6, 4, 0, 2, 4

		// };

		// meshes["cube_A"] = new Mesh("generated cube 1");
		// meshes["cube_A"]->InitFromData(vertices, indices);

		// // Actually create the mesh from the data
		// CreateMesh("cube_B", vertices, indices);
		//Create_Rectangle_2X1();
		// CreateCylinder("Cylinder", 1, 0.5f, 180, glm::vec3(1, 0, 0));
		// CreateCylinder("BigCylinder", 1, 6, 30, glm::vec3(0, 0, 1));
		// CreateParallelepiped("Parallelepiped", 6, 0.25f, 14, glm::vec3(1, 1, 0));
		// CreateParallelepiped("SmallBox", 6, 6, 6, glm::vec3(0, 1, 0));
		// CreateCylinder("SmallCylinder", 1, 0.25f, 180, glm::vec3(1, 0, 1));
		// CreateParallelepiped("Munte", 2, 2, 2, glm::vec3(0.6f, 0.3f, 0.1f));
		// CreateParallelepiped("Iarba", 6, 6, 6, glm::vec3(0.1f, 0.8f, 0.1f));
		// CreateParallelepiped("Apa", 6, 6, 6, glm::vec3(0.0f, 0.5f, 0.8f));
		// CreateParallelepiped("Sina", 1, 0.2f, 1, glm::vec3(1, 1, 1));
		CreateCylinder("Wheel_Cylinder", 0.75f, 0.25f, 45, glm::vec3(0, 0, 1));
		CreateParallelepiped("Platform", 4, 0.25f, 12, glm::vec3(0.6f, 0.3f, 0.1f));
		CreateParallelepiped("Cabine", 4, 4, 4, glm::vec3(0.6f, 0.3f, 0.1f));
		CreateCylinder("Top_Cylinder", 1.2f, 8, 180, glm::vec3(0, 0, 1));
		CreateParallelepiped("Iarba", 1, 0.1f, 1, glm::vec3(0.1f, 0.8f, 0.1f));
		CreateParallelepiped("Apa", 1, 0.1f, 1, glm::vec3(0.1f, 0.8f, 0.1f));
		CreateParallelepiped("Munte", 1, 0.1f, 1, glm::vec3(0.1f, 0.8f, 0.1f));
		CreateParallelepiped("Gara", 1, 1, 2, glm::vec3(0.1f, 0.8f, 0.1f));
		CreateParallelepiped("Sina_neagra", 1, 0.1f, 1, glm::vec3(0.0f, 0.0f, 0.0f));
		CreateParallelepiped("Sina_rosie", 1, 0.1f, 1, glm::vec3(1.0f, 0.0f, 0.0f));
		CreateParallelepiped("Sina_mov", 1, 0.5f, 1, glm::vec3(0.502f, 0.0f, 0.502f));
		CreateParallelepiped("Masina", 0.75f, 0.25f, 1, glm::vec3(1.0f, 0.0f, 0.0f));
		CreateCylinder("Masina_Cylinder", 1.2f, 16, 180, glm::vec3(0, 0, 1));
		CreateParallelepiped("Vagon", 4, 4, 12, glm::vec3(1.0f, 0.0f, 0.0f));
		CreateParallelepiped("ProgressBar", 1.0f, 1.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
		CreateParallelepiped("ProgressBarFill", 1.0f, 1.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		// // Mesh-uri pentru sine
		// CreateParallelepiped("rail_black", 1, 1, 1, glm::vec3(0, 0, 0));
		// CreateParallelepiped("rail_red", 1, 1, 1, glm::vec3(0.8f, 0.1f, 0.1f));
		// CreateParallelepiped("rail_white", 1, 1, 1, glm::vec3(0.9f, 0.9f, 0.9f));
		// CreateParallelepiped("rail_gray", 1, 1, 1, glm::vec3(0.4f, 0.4f, 0.4f));
	
		
	}

	// TODO(student): Create a tetrahedron mesh. You can create it with
	// only 4 vertices, or you can choose the harder route and create it
	// with 12 vertices. Think about it, why would you want that, and how
	// would you do it? After all, a tetrahedron has only 4 vertices
	// by definition!
	{
		vector<VertexFormat> vertices{
			VertexFormat(glm::vec3(3, 3, 0), glm::vec3(0.2, 0.1, 0.2)),
			VertexFormat(glm::vec3(4, 3, 0), glm::vec3(0.5, 0.2, 0.2)),
			VertexFormat(glm::vec3(3, 4, 0), glm::vec3(0.2, 0.2, 0.7)),
			VertexFormat(glm::vec3(3, 3, 1), glm::vec3(0.1, 0.6, 0.4))};

		vector<unsigned int> indices = {0, 1, 3, 0, 3, 2, 0, 2, 1, 1, 2, 3};

		meshes["pyr"] = new Mesh("pyr");
		meshes["pyr"]->InitFromData(vertices, indices);

		CreateMesh("pyrb", vertices, indices);
	}
	// TODO(student): Create a square using two triangles with
	// opposing vertex orientations.
	{
		vector<VertexFormat> vertices{
			VertexFormat(glm::vec3(1, 1, 1), glm::vec3(0.1, 0.2, 0.3)),
			VertexFormat(glm::vec3(1, 2, 1), glm::vec3(0.3, 0.2, 0.7)),
			VertexFormat(glm::vec3(2, 1, 1), glm::vec3(0.6, 0.1, 0.8)),
			VertexFormat(glm::vec3(2, 2, 1), glm::vec3(0.1, 0.2, 0.3)),
		};

		vector<unsigned int> indices = {0, 1, 2, 1, 2, 3};

		meshes["square"] = new Mesh("square");
		meshes["square"]->InitFromData(vertices, indices);

		CreateMesh("squareb", vertices, indices);
	}
	// {
	// 	vector<VertexFormat> vertices{
	// 		VertexFormat(glm::vec3(0, 0, 0), glm::vec3(0.1, 0.2, 0.3)),
	// 	};
	// 	float i = 1, j = 0;
	// 	bool ok = true;
	// 	do {
	// 		vertices.insert(
	// 			vertices.end(),
	// 			VertexFormat(glm::vec3(i, j, 0),
	// 						 glm::vec3((float)i / 2, (float)j / 2, 0)));
	// 		if (i >= -1) {
	// 			i -= 0.01;
	// 		} else {
	// 			i += 0.01;
	// 		}

	// 	} while (ok);

	// 	vector<unsigned int> indices = {0, 1, 2, 1, 2, 3};

	// 	meshes["square"] = new Mesh("square");
	// 	meshes["square"]->InitFromData(vertices, indices);

	// 	CreateMesh("squareb", vertices, indices);
	// }
}

void Tema2::CreateMesh(const char *name,
					  const std::vector<VertexFormat> &vertices,
					  const std::vector<unsigned int> &indices) {
	unsigned int VAO = 0;
	// TODO(student): Create the VAO and bind it
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO = 0;
	// TODO(student): Create the VBO and bind it
	glGenBuffers(1, &VBO);
	// TODO(student): Send vertices data into the VBO buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(),
				 &vertices[0], GL_STATIC_DRAW);
	unsigned int IBO = 0;
	// TODO(student): Create the IBO and bind it
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(),
				 &indices[0], GL_STATIC_DRAW);
	// TODO(student): Send indices data into the IBO buffer

	// ========================================================================
	// This section demonstrates how the GPU vertex shader program
	// receives data. It will be learned later, when GLSL shaders will be
	// introduced. For the moment, just think that each property value from
	// our vertex format needs to be sent to a certain channel, in order to
	// know how to receive it in the GLSL vertex shader.

	// Set vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

	// Set vertex normal attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat),
						  (void *)(sizeof(glm::vec3)));

	// Set texture coordinate attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat),
						  (void *)(2 * sizeof(glm::vec3)));

	// Set vertex color attribute
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat),
						  (void *)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
	// ========================================================================

	// TODO(student): Unbind the VAO
	glBindVertexArray(0);

	// Check for OpenGL errors
	if (GetOpenGLError() == GL_INVALID_OPERATION) {
		cout << "\t[NOTE] : For students : DON'T PANIC! This error should go "
				"away when completing the tasks."
			 << std::endl;
		cout << "\t[NOTE] : For developers : This happens because OpenGL core "
				"spec >=3.1 forbids null VAOs."
			 << std::endl;
	}

	// Mesh information is saved into a Mesh object
	meshes[name] = new Mesh(name);
	meshes[name]->InitFromBuffer(VAO,
								 static_cast<unsigned int>(indices.size()));
	meshes[name]->indices = indices;
}

void Tema2::FrameStart() {
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();

	// Sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

// Functie helper pentru randarea unei sine (poti sa o adaugi in clasa Tema2 in header sau sa o folosesti local)


void Tema2::Update(float deltaTimeSeconds) {
	glLineWidth(3);
	glPointSize(5);
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
	playTime += deltaTimeSeconds;

	int minutes = (int)playTime / 60;
int seconds = (int)playTime % 60;

std::string timeText =
    "Time: " +
    std::to_string(minutes) + ":" +
    (seconds < 10 ? "0" : "") +
    std::to_string(seconds);

	// TODO(student): Enable face culling
	//glEnable(GL_CULL_FACE);

	// TODO(student): Set face custom culling. Use the `cullFace` variable.
	//glCullFace(cullFace);

	// Render an object using face normals for color
	// RenderMesh(meshes["box"], shaders["VertexNormal"],
	// 		   glm::vec3(0, 0.5f, -1.5f), glm::vec3(0.75f));

	// Render an object using colors from vertex
	// RenderMesh(meshes["Rectangle"], shaders["VertexColor"],
	// 		   glm::vec3(0, 0, 0), glm::vec3(1.0f));

	// TODO(student): Draw the mesh that was created with `CreateMesh()`

	// RenderMesh(meshes["cube_B"], shaders["VertexColor"],
	// 		   glm::vec3(-2.5f, 0.5f, 0), glm::vec3(0.25f));

	// RenderMesh(meshes["Cylinder"], shaders["VertexColor"],
	// 		   glm::vec3(-5.0f, 0.5f, 0), glm::vec3(0.25f));		   

	// TODO(student): Draw the tetrahedron
	// RenderMesh(meshes["pyr"], shaders["VertexColor"], glm::vec3(-1.5f, 0.5f, 0),
	// 		   glm::vec3(0.25f));

	// TODO(student): Draw the square
	// RenderMesh(meshes["square"], shaders["VertexColor"],
	// 		   glm::vec3(-2.5f, 1.5f, 2), glm::vec3(0.25f));

	glm::mat4 modelMatrix = glm::mat4(1);

	// Translate
	//modelMatrix = glm::translate(modelMatrix, glm::vec3(2.0f, 0.5f, 0));
	
	// Rotate (90 degrees around Z)
	modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(0, 1, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.35f));

// (Optional) Translate last if already positioned
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-6.2f, 2.5f, 0));
	// Scale
	//modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));


	//Desenez roti
// 	RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.1f, train.position.y + 0.15f, train.position.z + 0.2f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.1f, train.position.y + 0.15f, train.position.z + 0.4f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.1f, train.position.y + 0.15f, train.position.z + 0.6f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.1f, train.position.y + 0.15f, train.position.z + 0.8f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.1f, train.position.y + 0.15f, train.position.z + 1.0f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.1f, train.position.y + 0.15f, train.position.z + 1.2f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.1f, train.position.y + 0.15f, train.position.z + 1.4f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.1f, train.position.y + 0.15f, train.position.z + 1.6f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.1f, train.position.y + 0.15f, train.position.z + 1.8f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.1f, train.position.y + 0.15f, train.position.z + 2.0f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.1f, train.position.y + 0.15f, train.position.z + 2.2f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.1f, train.position.y + 0.15f, train.position.z + 2.4f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.1f, train.position.y + 0.15f, train.position.z + 2.6f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.1f, train.position.y + 0.15f, train.position.z + 2.8f), glm::vec3(0.20f), mapTextures["metal"]);

// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.9f, train.position.y + 0.15f, train.position.z + 0.2f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.9f, train.position.y + 0.15f, train.position.z + 0.4f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.9f, train.position.y + 0.15f, train.position.z + 0.6f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.9f, train.position.y + 0.15f, train.position.z + 0.8f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.9f, train.position.y + 0.15f, train.position.z + 1.0f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.9f, train.position.y + 0.15f, train.position.z + 1.2f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.9f, train.position.y + 0.15f, train.position.z + 1.4f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.9f, train.position.y + 0.15f, train.position.z + 1.6f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.9f, train.position.y + 0.15f, train.position.z + 1.8f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.9f, train.position.y + 0.15f, train.position.z + 2.0f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.9f, train.position.y + 0.15f, train.position.z + 2.2f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.9f, train.position.y + 0.15f, train.position.z + 2.4f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.9f, train.position.y + 0.15f, train.position.z + 2.6f), glm::vec3(0.20f), mapTextures["metal"]);
// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.9f, train.position.y + 0.15f, train.position.z + 2.8f), glm::vec3(0.20f), mapTextures["metal"]);

// RenderSimpleMesh(meshes["Platform"], shaders["LabShader"], glm::vec3(train.position.x + 0.5f, train.position.y + 0.34f, train.position.z + 1.5f), glm::vec3(0.25f), mapTextures["metal_platforma"]);
// RenderSimpleMesh(meshes["Cabine"], shaders["LabShader"], glm::vec3(train.position.x + 0.5f, train.position.y + 0.86f, train.position.z + 0.5f), glm::vec3(0.25f), mapTextures["metal_top"]);

// Rotation angle example (around Y axis)
float angle = glm::radians(90.0f); // set to your desired rotation



{
	glm::mat4 model = glm::mat4(1);

	model = glm::translate(model, glm::vec3(0.5f, 0.67f, 2));
	model = glm::rotate(model, RADIANS(90.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.25f));

	RenderSimpleMesh(meshes["Top_Cylinder"], shaders["LabShader"], model, mapTextures["metal_top"]);
	}

	{
	glm::mat4 model = glm::mat4(1);

	model = glm::translate(model, glm::vec3(0.5f, 0.67f, 3));
	model = glm::rotate(model, RADIANS(90.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.10f));
	RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], model, mapTextures["wood"]);

	}
	{
	glm::mat4 model = glm::mat4(1);

	model = glm::translate(model, glm::vec3(0.5f, 0, 0.5f));
	model = glm::scale(model, glm::vec3(51, 1, 51));
	RenderSimpleMesh(meshes["Iarba"], shaders["LabShader"], model, mapTextures["snow"]);
	}

	{
	glm::mat4 model = glm::mat4(1);

	model = glm::translate(model, glm::vec3(18.5f, 0.01f, 0.5f));
	model = glm::scale(model, glm::vec3(15, 1, 4));
	RenderSimpleMesh(meshes["Apa"], shaders["LabShader"], model, mapTextures["apa"]);
	}

	{
	glm::mat4 model = glm::mat4(1);

	model = glm::translate(model, glm::vec3(9, 0.01f, -5));
	model = glm::rotate(model, RADIANS(90.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(15, 1, 4));
	RenderSimpleMesh(meshes["Apa"], shaders["LabShader"], model, mapTextures["apa"]);
	}

	{
	glm::mat4 model = glm::mat4(1);

	model = glm::translate(model, glm::vec3(-0.5f, 0.01f, -10.5f));
	model = glm::scale(model, glm::vec3(15, 1, 4));
	RenderSimpleMesh(meshes["Apa"], shaders["LabShader"], model, mapTextures["apa"]);
	}

	{
	glm::mat4 model = glm::mat4(1);

	model = glm::translate(model, glm::vec3(-10, 0.01f, -16.75f));
	model = glm::rotate(model, RADIANS(90.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(16.5f, 1, 4));
	RenderSimpleMesh(meshes["Apa"], shaders["LabShader"], model, mapTextures["apa"]);
	}

	{
	glm::mat4 model = glm::mat4(1);

	model = glm::translate(model, glm::vec3(-15, 0.01f, 15));
	model = glm::scale(model, glm::vec3(15, 1, 17));
	RenderSimpleMesh(meshes["Munte"], shaders["LabShader"], model, mapTextures["piatra"]);
	}

	{
	glm::mat4 model = glm::mat4(1);

	model = glm::translate(model, glm::vec3(9, 1, 13.5f));
	model = glm::rotate(model, RADIANS(90.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(2, 2, 2));
	RenderSimpleMesh(meshes["Gara"], shaders["VertexColor"], model);
	}

	{
	glm::mat4 model = glm::mat4(1);

	model = glm::translate(model, glm::vec3(6, 1, -17.5f));
	model = glm::rotate(model, RADIANS(90.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(2, 2, 2));
	RenderSimpleMesh(meshes["Gara"], shaders["VertexColor"], model);
	}

	{
	glm::mat4 model = glm::mat4(1);

	model = glm::translate(model, glm::vec3(-18.5f, 1, -2));
	model = glm::scale(model, glm::vec3(2, 2, 2));
	RenderSimpleMesh(meshes["Gara"], shaders["VertexColor"], model);
	}

	//Desenez sina normala
	{
		for(int i = -24;i < -16; i+=1){
	glm::mat4 model = glm::mat4(1);

	model = glm::translate(model, glm::vec3(i, 0.01f, 3));
	model = glm::scale(model, glm::vec3(1, 1, 1));
	if(rails.at(0).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}
	else
		RenderSimpleMesh(meshes["Sina_neagra"], shaders["VertexColor"], model);
	
		}

		for(int i = 2; i > -9; i--){
			glm::mat4 model = glm::mat4(1);

	model = glm::translate(model, glm::vec3(-17, 0.01f, i));
	model = glm::scale(model, glm::vec3(1, 1, 1));
	if(rails.at(1).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}
	else
		RenderSimpleMesh(meshes["Sina_neagra"], shaders["VertexColor"], model);
		}

		for(int i = -16; i < -1; i++){
			glm::mat4 model = glm::mat4(1);

			model = glm::translate(model, glm::vec3(i, 0.01f, -8));
			model = glm::scale(model, glm::vec3(1, 1, 1));
			if(rails.at(2).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}
	else
		
			RenderSimpleMesh(meshes["Sina_neagra"], shaders["VertexColor"], model);
		}
	}

	//Desenez Pod
	for(int i = -9; i > -13; i--)
	{
	{
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-1.63f, 0.02f, i));
			model = glm::scale(model, glm::vec3(0.25f, 1, 1));
			if(rails.at(3).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}
	else
			RenderSimpleMesh(meshes["Sina_rosie"], shaders["VertexColor"], model);
	}

	{
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-1.88f, 0.02f, i));
			model = glm::scale(model, glm::vec3(0.25f, 1, 1));
			if(rails.at(3).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}
	else
			RenderSimpleMesh(meshes["Sina_neagra"], shaders["VertexColor"], model);
	}

	{
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-2.13f, 0.02f, i));
			model = glm::scale(model, glm::vec3(0.25f, 1, 1));
			if(rails.at(3).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}
	else
			RenderSimpleMesh(meshes["Sina_rosie"], shaders["VertexColor"], model);
	}

	{
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-2.38f, 0.02f, i));
			model = glm::scale(model, glm::vec3(0.25f, 1, 1));
			if(rails.at(3).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}
	else
			RenderSimpleMesh(meshes["Sina_neagra"], shaders["VertexColor"], model);
	}
	}


	//Inapoi la sina normala
	for(int i = -13; i > -17; i--){
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-2, 0.01f, i));
			model = glm::scale(model, glm::vec3(1, 1, 1));
			if(rails.at(4).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}
	else
			RenderSimpleMesh(meshes["Sina_neagra"], shaders["VertexColor"], model);
	}

	for(int i = -1; i < 18; i++){
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(i, 0.01f, -16));
			model = glm::rotate(model, RADIANS(90.0f), glm::vec3(0, 1, 0));
			model = glm::scale(model, glm::vec3(1, 1, 1));
			if(rails.at(5).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}
	else
			RenderSimpleMesh(meshes["Sina_neagra"], shaders["VertexColor"], model);
	}

	for(int i = -15; i < -1; i++){
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(17, 0.01f, i));
			model = glm::scale(model, glm::vec3(1, 1, 1));
			if(rails.at(6).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}
	else
			RenderSimpleMesh(meshes["Sina_neagra"], shaders["VertexColor"], model);
	}
	
	//Pod2
	for(int i = -1; i < 3; i ++)
	{
	{
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(16.62f, 0.02f, i));
			model = glm::scale(model, glm::vec3(0.25f, 1, 1));
			if(rails.at(7).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}
	else
			RenderSimpleMesh(meshes["Sina_rosie"], shaders["VertexColor"], model);
	}

	{
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(16.87f, 0.02f, i));
			model = glm::scale(model, glm::vec3(0.25f, 1, 1));
			if(rails.at(7).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}
	else
			RenderSimpleMesh(meshes["Sina_neagra"], shaders["VertexColor"], model);
	}

	{
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(17.12f, 0.02f, i));
			model = glm::scale(model, glm::vec3(0.25f, 1, 1));
			if(rails.at(7).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}
	else
			RenderSimpleMesh(meshes["Sina_rosie"], shaders["VertexColor"], model);
	}

	{
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(17.37f, 0.02f, i));
			model = glm::scale(model, glm::vec3(0.25f, 1, 1));
			if(rails.at(7).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}	
	else
			RenderSimpleMesh(meshes["Sina_neagra"], shaders["VertexColor"], model);
	}
}
	

	//Inapoi la sina neagra
	
	for(int i = 3; i < 13; i++){
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(17, 0.01f, i));
			model = glm::scale(model, glm::vec3(1, 1, 1));
			if(rails.at(8).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}
	else
			RenderSimpleMesh(meshes["Sina_neagra"], shaders["VertexColor"], model);
	}
	

	for(int i = 16; i > -8; i--){
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(i, 0.01f, 12));
			model = glm::rotate(model, RADIANS(90.0f), glm::vec3(0, 1, 0));
			model = glm::scale(model, glm::vec3(1, 1, 1));
			if(rails.at(9).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}
	else
			RenderSimpleMesh(meshes["Sina_neagra"], shaders["VertexColor"], model);
	}
	

	//Tunel
	for(int i = -8; i > -24; i-=4){
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(i, 0.02f, 12));
			model = glm::rotate(model, RADIANS(90.0f), glm::vec3(0, 1, 0));
			model = glm::scale(model, glm::vec3(1, 1, 1));
			if(rails.at(10).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}
	else
			RenderSimpleMesh(meshes["Sina_rosie"], shaders["VertexColor"], model);
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(i-1, 0.02f, 12));
			model = glm::rotate(model, RADIANS(90.0f), glm::vec3(0, 1, 0));
			model = glm::scale(model, glm::vec3(1, 1, 1));
			if(rails.at(10).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}
	else
			RenderSimpleMesh(meshes["Sina_neagra"], shaders["VertexColor"], model);
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(i-2, 0.02f, 12));
			model = glm::rotate(model, RADIANS(90.0f), glm::vec3(0, 1, 0));
			model = glm::scale(model, glm::vec3(1, 1, 1));
			if(rails.at(10).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}
	else
			RenderSimpleMesh(meshes["Sina_rosie"], shaders["VertexColor"], model);
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(i-3, 0.02f, 12));
			model = glm::rotate(model, RADIANS(90.0f), glm::vec3(0, 1, 0));
			model = glm::scale(model, glm::vec3(1, 1, 1));
			if(rails.at(10).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}
	else
			RenderSimpleMesh(meshes["Sina_neagra"], shaders["VertexColor"], model);
	}


	//Inapoi la sina normala
	for(int i = 12; i > 3; i--){
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-24, 0.01f, i));
			model = glm::scale(model, glm::vec3(1, 1, 1));
			if(rails.at(11).isBroken == true){
		RenderSimpleMesh(meshes["Sina_mov"], shaders["VertexColor"], model);
	}
	else
			RenderSimpleMesh(meshes["Sina_neagra"], shaders["VertexColor"], model);
	}

    if (!gameOver) {
        // Update wait timers
        if (trainStopped || (train.rail && train.rail->isBroken)) {
            train1WaitTimer += deltaTimeSeconds;
        } else {
            train1WaitTimer = 0.0f;
        }

        if (trainStopped2 || (train2.rail && train2.rail->isBroken)) {
            train2WaitTimer += deltaTimeSeconds;
        } else {
            train2WaitTimer = 0.0f;
        }

        if (train1WaitTimer > maxWaitTime || train2WaitTimer > maxWaitTime) {
            gameOver = true;
        }

	// Check if train enters any station
// Check if train enters any station
for (auto& station : stations) {
    bool wasInside = station.trainInside;
    station.trainInside = station.contains(train.position);
    
    // Train just entered the station
    if (station.trainInside && !wasInside && station.shouldStop) {
    std::cout << "Train stopped at station" << std::endl;
    stationTimer = 0.0f;
    trainStopped = true;
}
    
    // Train just left the station
    if (!station.trainInside && wasInside) {
        std::cout << "Train left station" << std::endl;
		station.shouldStop = false;
		random_gara = dist(gen);
		stations.at(random_gara).shouldStop = true;
		break;
    }
}

for (auto& station : stations2) {
    bool wasInside = station.trainInside;
    station.trainInside = station.contains(train2.position);
    
    // Train just entered the station
    if (station.trainInside && !wasInside && station.shouldStop) {
    std::cout << "Train 2 stopped at station" << std::endl;
    stationTimer2 = 0.0f;
    trainStopped2 = true;
}
    
    // Train just left the station
    if (!station.trainInside && wasInside) {
        std::cout << "Train 2 left station" << std::endl;
		station.shouldStop = false;
		random_gara2 = dist(gen);
		stations2.at(random_gara2).shouldStop = true;
		break;
    }
}

// Check if car is on any rail
for (auto& rail : rails) {
    bool wasInside = rail.carInside;
    rail.carInside = rail.contains(masinaPos);
    
    if (rail.carInside && !wasInside) {
        std::cout << "Car entered rail segment" << std::endl;
    } else if (!rail.carInside && wasInside) {
        std::cout << "Car left rail segment" << std::endl;
    }
}

// Repair logic
bool isRepairing = false;
if (window->KeyHold(GLFW_KEY_F)) {
    for (auto& rail : rails) {
        if (rail.carInside && rail.isBroken) {
            isRepairing = true;
            repairTimer += deltaTimeSeconds;
            if (repairTimer >= 5.0f) {
                rail.isBroken = false;
                repairTimer = 0.0f;
                std::cout << "Rail fixed!" << std::endl;
            }
            break;
        }
    }
}
if (!isRepairing) {
    repairTimer = 0.0f;
}

if (isRepairing) {
    float progress = repairTimer / 5.0f;
    
    // Render Background
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, masinaPos + glm::vec3(0, 2.0f, 0));
    model = glm::rotate(model, masinaAngle, glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(2.0f, 0.2f, 0.05f));
    RenderSimpleMesh(meshes["ProgressBar"], shaders["VertexColor"], model);
    
    // Render Fill
    model = glm::mat4(1.0f);
    model = glm::translate(model, masinaPos + glm::vec3(0, 2.0f, 0));
    model = glm::rotate(model, masinaAngle, glm::vec3(0, 1, 0));
    model = glm::translate(model, glm::vec3(-1.0f + progress, 0, 0.0f)); // Offset to align left and slightly forward
    model = glm::scale(model, glm::vec3(2.0f * progress, 0.2f, 0.06f));   // Scale width by progress
    RenderSimpleMesh(meshes["ProgressBarFill"], shaders["VertexColor"], model);
}

// Accumulate time if train is stopped at station
if (trainStopped) {
    stationTimer += deltaTimeSeconds;  // Add frame time to timer
    
    // After 3 seconds, resume movement
    if (stationTimer >= 3.0f) {
        trainStopped = false;
        stationTimer = 0.0f;
        std::cout << "Train departing station..." << std::endl;
    }
}

if (trainStopped2) {
    stationTimer2 += deltaTimeSeconds;  // Add frame time to timer
    
    // After 3 seconds, resume movement
    if (stationTimer2 >= 3.0f) {
        trainStopped2 = false;
        stationTimer2 = 0.0f;
        std::cout << "Train 2 departing station..." << std::endl;
    }
}

// Check for broken rails every 60 seconds
railCheckTimer += deltaTimeSeconds;
if (railCheckTimer >= 60.0f) {
    railCheckTimer = 0.0f;
    std::uniform_real_distribution<float> chance(0.0f, 1.0f);
    for (auto& r : rails) {
        // 30% chance to break if not already broken
        if (!r.isBroken && chance(gen) < 0.3f) {
            r.isBroken = true;
            std::cout << "A rail segment has broken!" << std::endl;
        }
    }
}

// Move train only if not stopped
if (!rails.empty()) {
    if (train.rail == nullptr)
        train.rail = &rails[0];  // Start on first rail
    
    // Only move if train is not stopped at station AND rail is not broken
    if (!trainStopped && !train.rail->isBroken) {
        train.progress += deltaTimeSeconds * train.speed;
        
        // Move along the rail
        while (train.progress > 1.0f) {
            train.progress -= 1.0f;
            if (train.rail->next)
                train.rail = train.rail->next;  // Go to next rail
            else
                train.rail = &rails[0];        // Loop back to first rail
        }
    }
    
    train.position = glm::mix(train.rail->startPosition, train.rail->endPosition, train.progress);
    // Move along the rail
    while (train.progress > 1.0f) {
        train.progress -= 1.0f;
        if (train.rail->next)
            train.rail = train.rail->next;  // Go to next rail
        else
            train.rail = &rails[0];        // Loop back to first rail
    }

    // Train 2 movement
    if (train2.rail == nullptr)
        train2.rail = &rails[0];  // Start on first rail
    
    // Only move if train is not stopped at station AND rail is not broken
    if (!trainStopped2 && !train2.rail->isBroken) {
        train2.progress += deltaTimeSeconds * train2.speed;
        
        // Move along the rail
        while (train2.progress > 1.0f) {
            train2.progress -= 1.0f;
            if (train2.rail->next)
                train2.rail = train2.rail->next;  // Go to next rail
            else
                train2.rail = &rails[0];        // Loop back to first rail
        }
    }
    
    train2.position = glm::mix(train2.rail->startPosition, train2.rail->endPosition, train2.progress);

    train.position = glm::mix(train.rail->startPosition, train.rail->endPosition, train.progress);
    }
	
	// Calculate train orientation
glm::vec3 direction = glm::normalize(train.rail->endPosition - train.rail->startPosition);
float angleY = atan2(direction.x, direction.z);
train.position = glm::mix(train.rail->startPosition, train.rail->endPosition, train.progress);

//Vagon
	{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, train.position + glm::vec3(-3 * sin(angleY), 0.75f, -3 * cos(angleY)));
	//model = glm::translate(model, glm::vec3(0, 1, 0));
    model = glm::rotate(model, angleY, glm::vec3(0.0f, 1, 0.0f));
    model = glm::scale(model, glm::vec3(0.25f));
    RenderSimpleMesh(meshes["Vagon"], shaders["LabShader"], model, mapTextures["metal_platforma"]);
	}
	glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, train.position); // World position first
    model = glm::rotate(model, angleY, glm::vec3(0, 1, 0)); // Then rotation
    model = glm::translate(model, glm::vec3(-0.3f, 0.15f, -2)); // Then local offset
    model = glm::scale(model, glm::vec3(0.20f));
    RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], model, mapTextures["metal"]);
	{
	glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, train.position); // World position first
    model = glm::rotate(model, angleY, glm::vec3(0, 1, 0)); // Then rotation
    model = glm::translate(model, glm::vec3(-0.3f, 0.15f, -4)); // Then local offset
    model = glm::scale(model, glm::vec3(0.20f));
    RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], model, mapTextures["metal"]);
	}
	{
	glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, train.position); // World position first
    model = glm::rotate(model, angleY, glm::vec3(0, 1, 0)); // Then rotation
    model = glm::translate(model, glm::vec3(0.3f, 0.15f, -2)); // Then local offset
    model = glm::scale(model, glm::vec3(0.20f));
    RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], model, mapTextures["metal"]);	
	}
	{
		glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, train.position); // World position first
    model = glm::rotate(model, angleY, glm::vec3(0, 1, 0)); // Then rotation
    model = glm::translate(model, glm::vec3(0.3f, 0.15f, -4)); // Then local offset
    model = glm::scale(model, glm::vec3(0.20f));
    RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], model, mapTextures["metal"]);
	}

// Left wheels
for (float z = -1.4f; z <= 1.4f; z += 0.2f) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, train.position); // World position first
    model = glm::rotate(model, angleY, glm::vec3(0, 1, 0)); // Then rotation
    model = glm::translate(model, glm::vec3(-0.3f, 0.15f, z)); // Then local offset
    model = glm::scale(model, glm::vec3(0.20f));
    RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], model, mapTextures["metal"]);
}

// Right wheels
for (float z = -1.4f; z <= 1.4f; z += 0.2f) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, train.position); // World position first
    model = glm::rotate(model, angleY, glm::vec3(0, 1, 0)); // Then rotation
    model = glm::translate(model, glm::vec3(0.3f, 0.15f, z)); // Then local offset
    model = glm::scale(model, glm::vec3(0.20f));
    RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], model, mapTextures["metal"]);
}
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, train.position);
    model = glm::rotate(model, RADIANS(90.0f) * cos(angleY), glm::vec3(0, 1, 0));
    model = glm::translate(model, glm::vec3(0, 0.67f, 0));
    model = glm::scale(model, glm::vec3(0.25f));
    RenderSimpleMesh(meshes["Top_Cylinder"], shaders["LabShader"], model, mapTextures["metal_top"]);
}

{
	glm::mat4 model = glm::mat4(1.0f);
model = glm::translate(model, train.position);
model = glm::rotate(model, RADIANS(90.0F) + angleY, glm::vec3(0, 1, 0)); // Use actual direction angle
model = glm::translate(model, glm::vec3(-1, 0.67f, 0)); // Increase Z to move forward
model = glm::scale(model, glm::vec3(0.10f));
RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], model, mapTextures["wood"]);

	}
// for (float z = 0.2f; z <= 2.8f; z += 0.2f) {
//     glm::mat4 model = trainModel;
//     model = glm::translate(model, glm::vec3(0.9f, 0.15f, z));
//     model = glm::scale(model, glm::vec3(0.20f));

//     RenderSimpleMesh(meshes["Wheel_Cylinder"],
//                      shaders["LabShader"],
//                      model,
//                      mapTextures["metal"]);
// }



// Right wheels
// for (float z = 0.2f; z <= 2.8f; z += 0.2f) {
//     glm::mat4 model = trainModel;
//     model = glm::translate(model, glm::vec3(0.9f, 0.15f, z));
//     //model = glm::rotate(model, angleY, glm::vec3(0.0f, 1, 0.0f));
//     model = glm::scale(model, glm::vec3(0.20f));
//     RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], model, mapTextures["metal"]);
// }

// Platform
{
    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::translate(model, train.position + glm::vec3(0.5f, 0.34f, 1.5f));
	model = glm::translate(model, train.position + glm::vec3(0, 0.34f, 0));
    model = glm::rotate(model, angleY, glm::vec3(0.0f, 1, 0.0f));
    model = glm::scale(model, glm::vec3(0.25f));
    RenderSimpleMesh(meshes["Platform"], shaders["LabShader"], model, mapTextures["metal_platforma"]);
}

// Cabine
{
    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::translate(model, train.position + glm::vec3(0.5f, 0.86f, 0.5f));
	model = glm::translate(model, train.position + glm::vec3(-1 * sin(angleY), 0.86f, -1 * cos(angleY)));
    //model = glm::rotate(model, angleY, glm::vec3(0.0f, 1, 0.0f));
    model = glm::scale(model, glm::vec3(0.25f));
    RenderSimpleMesh(meshes["Cabine"], shaders["LabShader"], model, mapTextures["metal_top"]);
}

// Train 2 Rendering
glm::vec3 direction2 = glm::normalize(train2.rail->endPosition - train2.rail->startPosition);
float angleY2 = atan2(direction2.x, direction2.z);

//Vagon
	{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, train2.position + glm::vec3(-3 * sin(angleY2), 0.75f, -3 * cos(angleY2)));
	//model = glm::translate(model, glm::vec3(0, 1, 0));
    model = glm::rotate(model, angleY2, glm::vec3(0.0f, 1, 0.0f));
    model = glm::scale(model, glm::vec3(0.25f));
    RenderSimpleMesh(meshes["Vagon"], shaders["LabShader"], model, mapTextures["metal_platforma"]);
	}
	{
	glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, train2.position); // World position first
    model = glm::rotate(model, angleY2, glm::vec3(0, 1, 0)); // Then rotation
    model = glm::translate(model, glm::vec3(-0.3f, 0.15f, -4)); // Then local offset
    model = glm::scale(model, glm::vec3(0.20f));
    RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], model, mapTextures["metal"]);
	}
	{
	glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, train2.position); // World position first
    model = glm::rotate(model, angleY2, glm::vec3(0, 1, 0)); // Then rotation
    model = glm::translate(model, glm::vec3(0.3f, 0.15f, -2)); // Then local offset
    model = glm::scale(model, glm::vec3(0.20f));
    RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], model, mapTextures["metal"]);	
	}
	{
		glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, train2.position); // World position first
    model = glm::rotate(model, angleY2, glm::vec3(0, 1, 0)); // Then rotation
    model = glm::translate(model, glm::vec3(0.3f, 0.15f, -4)); // Then local offset
    model = glm::scale(model, glm::vec3(0.20f));
    RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], model, mapTextures["metal"]);
	}


// Left wheels
for (float z = -1.4f; z <= 1.4f; z += 0.2f) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, train2.position); // World position first
    model = glm::rotate(model, angleY2, glm::vec3(0, 1, 0)); // Then rotation
    model = glm::translate(model, glm::vec3(-0.3f, 0.15f, z)); // Then local offset
    model = glm::scale(model, glm::vec3(0.20f));
    RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], model, mapTextures["metal"]);
}

// Right wheels
for (float z = -1.4f; z <= 1.4f; z += 0.2f) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, train2.position); // World position first
    model = glm::rotate(model, angleY2, glm::vec3(0, 1, 0)); // Then rotation
    model = glm::translate(model, glm::vec3(0.3f, 0.15f, z)); // Then local offset
    model = glm::scale(model, glm::vec3(0.20f));
    RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], model, mapTextures["metal"]);
}
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, train2.position);
    model = glm::rotate(model, RADIANS(90.0f) * cos(angleY2), glm::vec3(0, 1, 0));
    model = glm::translate(model, glm::vec3(0, 0.67f, 0));
    model = glm::scale(model, glm::vec3(0.25f));
    RenderSimpleMesh(meshes["Top_Cylinder"], shaders["LabShader"], model, mapTextures["metal_top"]);
}

{
	glm::mat4 model = glm::mat4(1.0f);
model = glm::translate(model, train2.position);
model = glm::rotate(model, RADIANS(90.0F) + angleY2, glm::vec3(0, 1, 0)); // Use actual direction angle
model = glm::translate(model, glm::vec3(-1, 0.67f, 0)); // Increase Z to move forward
model = glm::scale(model, glm::vec3(0.10f));
RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], model, mapTextures["wood"]);

	}

// Platform
{
    glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, train2.position + glm::vec3(0, 0.34f, 0));
    model = glm::rotate(model, angleY2, glm::vec3(0.0f, 1, 0.0f));
    model = glm::scale(model, glm::vec3(0.25f));
    RenderSimpleMesh(meshes["Platform"], shaders["LabShader"], model, mapTextures["metal_platforma"]);
}

// Cabine
{
    glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, train2.position + glm::vec3(-1 * sin(angleY2), 0.86f, -1 * cos(angleY2)));
    model = glm::scale(model, glm::vec3(0.25f));
    RenderSimpleMesh(meshes["Cabine"], shaders["LabShader"], model, mapTextures["metal_top"]);
}

	glm::mat4 modelMatrix = glm::mat4(1);

	// Translate
	//modelMatrix = glm::translate(modelMatrix, glm::vec3(2.0f, 0.5f, 0));

	// Rotate (90 degrees around Z)
	modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(0, 1, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.35f));

// (Optional) Translate last if already positioned
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-6.2f, 2.5f, 0));
	// Scale
	//modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));


	//Desenez roti
	//RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(train.position.x + 0.1f, train.position.y + 0.15f, train.position.z + 0.2f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.1f, 0.15f, 0.4f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.1f, 0.15f, 0.6f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.1f, 0.15f, 0.8f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.1f, 0.15f, 1), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.1f, 0.15f, 1.2f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.1f, 0.15f, 1.4f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.1f, 0.15f, 1.6f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.1f, 0.15f, 1.8f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.1f, 0.15f, 2), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.1f, 0.15f, 2.2f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.1f, 0.15f, 2.4f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.1f, 0.15f, 2.6f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.1f, 0.15f, 2.8f), glm::vec3(0.20f), mapTextures["metal"]);

	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.9f, 0.15f, 0.2f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.9f, 0.15f, 0.4f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.9f, 0.15f, 0.6f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.9f, 0.15f, 0.8f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.9f, 0.15f, 1), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.9f, 0.15f, 1.2f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.9f, 0.15f, 1.4f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.9f, 0.15f, 1.6f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.9f, 0.15f, 1.8f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.9f, 0.15f, 2), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.9f, 0.15f, 2.2f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.9f, 0.15f, 2.4f), glm::vec3(0.20f), mapTextures["metal"]);
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.9f, 0.15f, 2.6f), glm::vec3(0.20f), mapTextures["metal"]);
    // RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], glm::vec3(0.9f, 0.15f, 2.8f), glm::vec3(0.20f), mapTextures["metal"]);


	// RenderSimpleMesh(meshes["Platform"], shaders["LabShader"], glm::vec3(0.5f, 0.34f, 1.5f), glm::vec3(0.25f), mapTextures["metal_platforma"]);
	// RenderSimpleMesh(meshes["Cabine"], shaders["LabShader"], glm::vec3(0.5f, 0.86f, 0.5f), glm::vec3(0.25f), mapTextures["metal_top"]);
	// {
	// glm::mat4 model = glm::mat4(1);

	// model = glm::translate(model, glm::vec3(0.5f, 0.67f, 2));
	// model = glm::rotate(model, RADIANS(90.0f), glm::vec3(0, 1, 0));
	// model = glm::scale(model, glm::vec3(0.25f));

	// RenderSimpleMesh(meshes["Top_Cylinder"], shaders["LabShader"], model, mapTextures["metal_top"]);
	// }

	// {
	// glm::mat4 model = glm::mat4(1);

	// model = glm::translate(model, glm::vec3(0.5f, 0.67f, 3));
	// model = glm::rotate(model, RADIANS(90.0f), glm::vec3(0, 1, 0));
	// model = glm::scale(model, glm::vec3(0.10f));
	// RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], model, mapTextures["wood"]);

	// }
	
	//Masinuta
	glm::vec3 forward(
    sin(masinaAngle),
    0.0f,
    cos(masinaAngle)
);

if (window->KeyHold(GLFW_KEY_W))
    masinaPos += forward * moveSpeed * deltaTimeSeconds;

if (window->KeyHold(GLFW_KEY_S))
    masinaPos -= forward * moveSpeed * deltaTimeSeconds;

if (window->KeyHold(GLFW_KEY_A))
    masinaAngle += turnSpeed * deltaTimeSeconds;

if (window->KeyHold(GLFW_KEY_D))
    masinaAngle -= turnSpeed * deltaTimeSeconds;
glm::mat4 masinaParent = glm::mat4(1.0f);
masinaParent = glm::translate(masinaParent, masinaPos);
masinaParent = glm::rotate(masinaParent, masinaAngle, glm::vec3(0, 1, 0));

	{
	glm::mat4 model = masinaParent;
	model = glm::translate(model, glm::vec3(0, 0.30f, 0));
    //model = glm::rotate(model, angleY, glm::vec3(0.0f, 1, 0.0f));
    //model = glm::scale(model, glm::vec3(0.25f));
    RenderSimpleMesh(meshes["Masina"], shaders["LabShader"], model, mapTextures["metal_top"]);	
	}
	{
	glm::mat4 model = masinaParent;
	model = glm::translate(model, glm::vec3(0.40f, 0.20, 0.30f));
    //model = glm::rotate(model, angleY, glm::vec3(0.0f, 1, 0.0f));
    model = glm::scale(model, glm::vec3(0.20f));
    RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], model, mapTextures["metal"]);		
	}
	{
	glm::mat4 model = masinaParent;
	model = glm::translate(model, glm::vec3(0.40f, 0.20, -0.30f));
    //model = glm::rotate(model, angleY, glm::vec3(0.0f, 1, 0.0f));
    model = glm::scale(model, glm::vec3(0.20f));
    RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], model, mapTextures["metal"]);		
	}
	{
	glm::mat4 model = masinaParent;
	model = glm::translate(model, glm::vec3(-0.40f, 0.20, -0.30f));
    //model = glm::rotate(model, angleY, glm::vec3(0.0f, 1, 0.0f));
    model = glm::scale(model, glm::vec3(0.20f));
    RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], model, mapTextures["metal"]);		
	}
	{
	glm::mat4 model = masinaParent;
	model = glm::translate(model, glm::vec3(-0.40f, 0.20, 0.30f));
    //model = glm::rotate(model, angleY, glm::vec3(0.0f, 1, 0.0f));
    model = glm::scale(model, glm::vec3(0.20f));
    RenderSimpleMesh(meshes["Wheel_Cylinder"], shaders["LabShader"], model, mapTextures["metal"]);		
	}
	{
	glm::mat4 model = masinaParent;
	model = glm::translate(model, glm::vec3(0, 0.5f, 0));
    model = glm::rotate(model, RADIANS(90.0f), glm::vec3(1, 0, 0.0f));
    model = glm::scale(model, glm::vec3(0.25f, 0.75f, 0.20f));
    RenderSimpleMesh(meshes["Masina"], shaders["LabShader"], model, mapTextures["metal_top"]);	
	}

	{
    glm::mat4 model = masinaParent;
	model = glm::translate(model, glm::vec3(0, 0.75f, 0));
    model = glm::rotate(model, RADIANS(90.0f), glm::vec3(0, 0, 1));
    model = glm::scale(model, glm::vec3(0.05f));
    RenderSimpleMesh(meshes["Top_Cylinder"], shaders["LabShader"], model, mapTextures["metal_platforma"]);
	}
	
	
	value1 = value1 + direction1 * speed1 * deltaTimeSeconds;

if (value1 >= 1.0f) {
    value1 = 1.0f;
    direction1 = -1.0f;
}
else if (value1 <= -1.0f) {
    value1 = -1.0f;
    direction1 = 1.0f;
}
	float angle = glm::mix(0.0f, RADIANS(45.0f), value1);

glm::mat4 parent = masinaParent;

// pivot of the whole assembly
parent = glm::translate(parent, glm::vec3(0.0f, 0.95f, 0.0f));

// base orientation
//parent = glm::rotate(parent, RADIANS(90.0f), glm::vec3(0, 1, 0));

// animated rotation
parent = glm::rotate(parent, angle, glm::vec3(0, 0, 1));

	{
    glm::mat4 model = parent;
	parent = glm::rotate(parent, RADIANS(90.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(0.05f));

    RenderSimpleMesh(
        meshes["Masina_Cylinder"],
        shaders["LabShader"],
        model,
        mapTextures["metal_platforma"]
    );
}


	{
    glm::mat4 model = parent;
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.4f));
    model = glm::scale(model, glm::vec3(0.03f));

    RenderSimpleMesh(
        meshes["Masina_Cylinder"],
        shaders["VertexColor"],
        model
    );
}

	{
    glm::mat4 model = parent;
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.4f));

    model = glm::scale(model, glm::vec3(0.03f));

    RenderSimpleMesh(
        meshes["Masina_Cylinder"],
        shaders["VertexColor"],
        model
    );
}



}
// Update third person camera
if (thirdPersonCamera) {
    // Calculate camera position relative to car
    glm::mat4 carTransform = glm::mat4(1.0f);
    carTransform = glm::translate(carTransform, masinaPos);
    carTransform = glm::rotate(carTransform, masinaAngle, glm::vec3(0, 1, 0));
    
    // Apply offset in car's local space
    glm::vec4 worldOffset = carTransform * glm::vec4(cameraOffset, 1.0f);
    glm::vec3 cameraPosition = glm::vec3(worldOffset);
    
    // Camera looks at the car
    glm::vec3 cameraTarget = masinaPos + glm::vec3(0, 1.0f, 0);  // Look slightly above car center
    glm::vec3 cameraUp = glm::vec3(0, 1, 0);
    
    // Set camera view
    camera->Set(cameraPosition, cameraTarget, cameraUp);
}


// Top-left corner text
    {
        glm::ivec2 resolution = window->GetResolution();
        textRenderer->m_textShader->Use();
        glUniformMatrix4fv(glGetUniformLocation(textRenderer->m_textShader->program, "projection"), 1, GL_FALSE, glm::value_ptr(glm::ortho(0.0f, (float)resolution.x, (float)resolution.y, 0.0f)));
    }
textRenderer->RenderText(
    timeText,
    10,                              // X (left)
    window->GetResolution().y - 30,  // Y (top)
    1.0f,
    glm::vec3(1.0f, 1.0f, 1.0f)      // WHITE
);

    if (hardMode) {
        textRenderer->RenderText("HARD", window->GetResolution().x - 100, 30, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    }

    if (gameOver) {
        textRenderer->RenderText("GAME OVER", window->GetResolution().x / 2 - 100, window->GetResolution().y / 2, 3.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    }


// Example: red text just below
//textRenderer->RenderText("Lives: 3", x, y - 30.0f, scale, glm::vec3(1.0f, 0.0f, 0.0f));



// 	RenderMesh(meshes["Parallelepiped"], shaders["VertexColor"], glm::vec3(0, 0.5f, 1.5f), glm::vec3(0.25f));
// 	RenderMesh(meshes["SmallBox"], shaders["VertexColor"], glm::vec3(0, 1.275f, 0.5f), glm::vec3(0.25f));
// 	RenderMesh(meshes["BigCylinder"], shaders["VertexColor"], modelMatrix);
// 	modelMatrix = glm::mat4(1);
// 	modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(0, 1, 0));
// 	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.20f));

// 	// --- 1. IARBA (Fundalul Verde) ---
//     {
//         glm::mat4 modelMatrix = glm::mat4(1);
//         modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -0.01f, 0)); // Puțin sub drum
//         modelMatrix = glm::scale(modelMatrix, glm::vec3(7.5f, 0.01f, 7.5f));   // O suprafață mare plană
//         RenderMesh(meshes["Iarba"], shaders["VertexColor"], modelMatrix); 
//         // Notă: Ar fi bine să ai un mesh verde. Dacă SmallBox e verde (0,1,0), e perfect.
//     }

//     // --- 2. ZONA MARO (Clădirea) ---
//     {
//         glm::mat4 modelMatrix = glm::mat4(1);
//         modelMatrix = glm::translate(modelMatrix, glm::vec3(-10, 0.05f, -11));
//         modelMatrix = glm::scale(modelMatrix, glm::vec3(10, 0.1f, 5));
//         // Poți folosi un mesh cu culoarea maro sau refolosi unul existent
//         RenderMesh(meshes["Munte"], shaders["VertexColor"], modelMatrix);
//     }

//     // --- 3. DRUMUL ALBASTRU (Segmentele în zig-zag) ---
//     // Folosim meshes["BigCylinder"] sau un cub scalat. 
//     // Dacă vrei să fie exact ca în poză, folosim cuburi scalate (albastre).

//     // Segment 1 (Vertical jos în imagine - de fapt pe axa Z)
//     {
//         glm::mat4 modelMatrix = glm::mat4(1);
//         modelMatrix = glm::translate(modelMatrix, glm::vec3(-8, 0.01f, 13.5f));
//         modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 0.02f, 3));
//         RenderMesh(meshes["Apa"], shaders["VertexColor"], modelMatrix); // BigCylinder e albastru în codul tău
//     }

//     // Segment 2 (Orizontal spre dreapta)
//     {
//         glm::mat4 modelMatrix = glm::mat4(1);
//         modelMatrix = glm::translate(modelMatrix, glm::vec3(2.5f, 0.01f, 7.5f));
//         modelMatrix = glm::scale(modelMatrix, glm::vec3(3, 0.02f, 1));
//         RenderMesh(meshes["Apa"], shaders["VertexColor"], modelMatrix);
//     }

//     // Segment 3 (Vertical în sus)
//     {
//         glm::mat4 modelMatrix = glm::mat4(1);
//         modelMatrix = glm::translate(modelMatrix, glm::vec3(9, 0.01f, 1.5f));
//         modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 0.02f, 3));
//         RenderMesh(meshes["Apa"], shaders["VertexColor"], modelMatrix);
//     }

//     // Segment 4 (Orizontal dreapta sus)
//     {
//         glm::mat4 modelMatrix = glm::mat4(1);
//         modelMatrix = glm::translate(modelMatrix, glm::vec3(14.4f, 0.01f, -5));
//         modelMatrix = glm::scale(modelMatrix, glm::vec3(2.8f, 0.02f, 1));
//         RenderMesh(meshes["Apa"], shaders["VertexColor"], modelMatrix);
//     }
// 	{
// 		glm::mat4 modelMatrix = glm::mat4(1);
// 		modelMatrix = glm::translate(modelMatrix, glm::vec3(22, 0, 0));
// 		modelMatrix = glm::scale(modelMatrix, glm::vec3(6, 0.3f, 1));
// 		RenderMesh(meshes["Sina"], shaders["VertexColor"], modelMatrix);
// 	}
// // // (Optional) Translate last if already positioned
// 	modelMatrix = glm::translate(modelMatrix, glm::vec3(-16.2f, 4.3, 0));
// 	RenderMesh(meshes["SmallCylinder"], shaders["VertexColor"], modelMatrix);

// 	RenderMesh(meshes["Cylinder"], shaders["VertexColor"], glm::vec3(0, 1, 0), glm::vec3(0.25f));
	// TODO(student): Disable face culling
	//glDisable(GL_CULL_FACE);

    // Exemplu de randare a sinelor (trebuie sa ai o lista de sine definita undeva)
    // static std::vector<Rail> myRails = {
    //     Rail(glm::vec3(0,0,0), glm::vec3(0,0,10), RailType::NORMAL),
    //     Rail(glm::vec3(0,0,10), glm::vec3(10,0,10), RailType::BRIDGE),
    //     Rail(glm::vec3(10,0,10), glm::vec3(10,0,0), RailType::TUNNEL)
    // };
    // for(auto& rail : myRails) RenderRail(this, &rail);
}

void Tema2::FrameEnd() {
    glm::mat4 viewMatrix;
    if (thirdPersonCamera) {
        viewMatrix = camera->GetViewMatrix();
    } else {
        viewMatrix = GetSceneCamera()->GetViewMatrix();
    }
    DrawCoordinateSystem(viewMatrix, GetSceneCamera()->GetProjectionMatrix());
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

void Tema2::OnInputUpdate(float deltaTime, int mods) {}

void Tema2::OnKeyPress(int key, int mods) {

	// TODO(student): Switch between GL_FRONT and GL_BACK culling.
	if (key == GLFW_KEY_F2) {
		switch (cullFace) {
		case GL_FRONT:
			cullFace = GL_BACK;
			break;
		case GL_BACK:
			cullFace = GL_FRONT;
			break;
		}
	}
	// Save the state in `cullFace` variable and apply it in the
	// `Update()` method, NOT here!

	if (key == GLFW_KEY_SPACE) {
		switch (polygonMode) {
		case GL_POINT:
			polygonMode = GL_FILL;
			break;
		case GL_LINE:
			polygonMode = GL_POINT;
			break;
		default:
			polygonMode = GL_LINE;
			break;
		}
	}

    if (key == GLFW_KEY_K) {
        hardMode = true;
        maxWaitTime = 15.0f;
    }

    if (key == GLFW_KEY_N) {
        hardMode = false;
        maxWaitTime = 30.0f;
    }
}

void Tema2::OnKeyRelease(int key, int mods) {
	// Add key release event
}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
	// Add mouse move event
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
	// Add mouse button press event
}

void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
	// Add mouse button release event
}

void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {}

void Tema2::OnWindowResize(int width, int height) {}

void Tema2::Create_Rectangle_2X1(){
    vector<VertexFormat> vertices{
			VertexFormat(glm::vec3(0, 0, 1), glm::vec3(0, 1, 1),
						 glm::vec3(0.2, 0.8, 0.6)),
			VertexFormat(glm::vec3(1, 0, 1), glm::vec3(0, 0, 1),
						 glm::vec3(0.2, 0.7, 0.1)),
			VertexFormat(glm::vec3(0, 1, 1), glm::vec3(1, 0, 1),
						 glm::vec3(0.2, 0.6, 0.2)),
			VertexFormat(glm::vec3(1, 1, 1), glm::vec3(0, 1, 1),
						 glm::vec3(0.2, 0.5, 0.3)),
			VertexFormat(glm::vec3(0, 0, -1), glm::vec3(0, 0, 1),
						 glm::vec3(0.2, 0.4, 0.4)),
			VertexFormat(glm::vec3(1, 0, -1), glm::vec3(1, 0, 1),
						 glm::vec3(0.2, 0.3, 0.5)),
			VertexFormat(glm::vec3(0, 1, -1), glm::vec3(0, 1, 1),
						 glm::vec3(0.2, 0.2, 0.6)),
			VertexFormat(glm::vec3(1, 1, -1), glm::vec3(0, 0, 1),
						 glm::vec3(0.2, 0.1, 0.7)),

		};

        vector<unsigned int> indices = {
			0, 1, 2, // indices for first triangle
			1, 3, 2, // indices for second triangle
					 // TODO(student): Complete indices data for the cube mesh
			2, 3, 7, 2, 7, 6, 1, 7, 3, 1, 5, 7, 6, 7, 4, 7, 5, 4, 0, 4, 1, 1, 4,
			5, 2, 6, 4, 0, 2, 4

		};
		

		meshes["Rectangle"] = new Mesh("generated cube 1");
		meshes["Rectangle"]->InitFromData(vertices, indices);
        CreateMesh("Rectangle", vertices, indices);
}


void Tema2::CreateCylinder(const char *name, float radius, float height, int segments, glm::vec3 color) {
    vector<VertexFormat> vertices;
    vector<unsigned int> indices;

    // Side vertices
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float y = radius * cos(angle);
        float z = radius * sin(angle);
        float u = (float)i / segments;
    
        // Normal for side points outwards
        glm::vec3 normal = glm::normalize(glm::vec3(0, y, z));

        vertices.push_back(VertexFormat(glm::vec3(-height / 2, y, z), color, normal, glm::vec2(u, 0.0f)));
        vertices.push_back(VertexFormat(glm::vec3(height / 2, y, z), color, normal, glm::vec2(u, 1.0f)));
    }

    // Side indices
    for (int i = 0; i < segments; i++) {
        int bottom1 = 2 * i;
        int top1 = 2 * i + 1;
        int bottom2 = 2 * (i + 1);
        int top2 = 2 * (i + 1) + 1;

        indices.push_back(bottom1);
        indices.push_back(bottom2);
        indices.push_back(top1);

        indices.push_back(bottom2);
        indices.push_back(top2);
        indices.push_back(top1);
    }

    // Top cap
    int topCenterIndex = vertices.size();
    vertices.push_back(VertexFormat(glm::vec3(height / 2, 0, 0), color, glm::vec3(1, 0, 0), glm::vec2(0.5f, 0.5f)));

    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float y = radius * cos(angle);
        float z = radius * sin(angle);
        float u = (cos(angle) + 1) * 0.5f;
        float v = (sin(angle) + 1) * 0.5f;
        // Normal points up
        vertices.push_back(VertexFormat(glm::vec3(height / 2, y, z), color, glm::vec3(1, 0, 0), glm::vec2(u, v)));
    }

    for (int i = 0; i < segments; i++) {
        int v1 = topCenterIndex + 1 + i;
        int v2 = topCenterIndex + 1 + (i + 1);
        indices.push_back(topCenterIndex);
        indices.push_back(v2);
        indices.push_back(v1);
    }

    // Bottom cap
    int bottomCenterIndex = vertices.size();
    vertices.push_back(VertexFormat(glm::vec3(-height / 2, 0, 0), color, glm::vec3(-1, 0, 0), glm::vec2(0.5f, 0.5f)));

    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float y = radius * cos(angle);
        float z = radius * sin(angle);
        float u = (cos(angle) + 1) * 0.5f;
        float v = (sin(angle) + 1) * 0.5f;
        // Normal points down
        vertices.push_back(VertexFormat(glm::vec3(-height / 2, y, z), color, glm::vec3(-1, 0, 0), glm::vec2(u, v)));
    }

    for (int i = 0; i < segments; i++) {
        int v1 = bottomCenterIndex + 1 + i;
        int v2 = bottomCenterIndex + 1 + (i + 1);
        indices.push_back(bottomCenterIndex);
        indices.push_back(v1);
        indices.push_back(v2);
    }

    CreateMesh(name, vertices, indices);
}

void Tema2::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);

    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glm::mat4 viewMatrix;
    if (thirdPersonCamera) {
        viewMatrix = camera->GetViewMatrix();
    } else {
        viewMatrix = GetSceneCamera()->GetViewMatrix();
    }
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    if (texture)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);
    }

    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema2::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::vec3 &position, const glm::vec3 &scale, Texture2D *texture)
{
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);
    RenderSimpleMesh(mesh, shader, modelMatrix, texture);
}

void Tema2::CreateParallelepiped(const char *name, float width, float height, float depth, glm::vec3 color) {
    glm::vec3 corner = glm::vec3(width / 2, height / 2, depth / 2);

    vector<VertexFormat> vertices = {
        // Front face
        VertexFormat(glm::vec3(-corner.x, -corner.y,  corner.z), color, glm::vec3(0, 0, 1), glm::vec2(0, 0)),
        VertexFormat(glm::vec3( corner.x, -corner.y,  corner.z), color, glm::vec3(0, 0, 1), glm::vec2(1, 0)),
        VertexFormat(glm::vec3( corner.x,  corner.y,  corner.z), color, glm::vec3(0, 0, 1), glm::vec2(1, 1)),
        VertexFormat(glm::vec3(-corner.x,  corner.y,  corner.z), color, glm::vec3(0, 0, 1), glm::vec2(0, 1)),

        // Back face
        VertexFormat(glm::vec3( corner.x, -corner.y, -corner.z), color, glm::vec3(0, 0, -1), glm::vec2(0, 0)),
        VertexFormat(glm::vec3(-corner.x, -corner.y, -corner.z), color, glm::vec3(0, 0, -1), glm::vec2(1, 0)),
        VertexFormat(glm::vec3(-corner.x,  corner.y, -corner.z), color, glm::vec3(0, 0, -1), glm::vec2(1, 1)),
        VertexFormat(glm::vec3( corner.x,  corner.y, -corner.z), color, glm::vec3(0, 0, -1), glm::vec2(0, 1)),

        // Top face
        VertexFormat(glm::vec3(-corner.x,  corner.y,  corner.z), color, glm::vec3(0, 1, 0), glm::vec2(0, 0)),
        VertexFormat(glm::vec3( corner.x,  corner.y,  corner.z), color, glm::vec3(0, 1, 0), glm::vec2(1, 0)),
        VertexFormat(glm::vec3( corner.x,  corner.y, -corner.z), color, glm::vec3(0, 1, 0), glm::vec2(1, 1)),
        VertexFormat(glm::vec3(-corner.x,  corner.y, -corner.z), color, glm::vec3(0, 1, 0), glm::vec2(0, 1)),

        // Bottom face
        VertexFormat(glm::vec3(-corner.x, -corner.y, -corner.z), color, glm::vec3(0, -1, 0), glm::vec2(0, 0)),
        VertexFormat(glm::vec3( corner.x, -corner.y, -corner.z), color, glm::vec3(0, -1, 0), glm::vec2(1, 0)),
        VertexFormat(glm::vec3( corner.x, -corner.y,  corner.z), color, glm::vec3(0, -1, 0), glm::vec2(1, 1)),
        VertexFormat(glm::vec3(-corner.x, -corner.y,  corner.z), color, glm::vec3(0, -1, 0), glm::vec2(0, 1)),

        // Right face
        VertexFormat(glm::vec3( corner.x, -corner.y,  corner.z), color, glm::vec3(1, 0, 0), glm::vec2(0, 0)),
        VertexFormat(glm::vec3( corner.x, -corner.y, -corner.z), color, glm::vec3(1, 0, 0), glm::vec2(1, 0)),
        VertexFormat(glm::vec3( corner.x,  corner.y, -corner.z), color, glm::vec3(1, 0, 0), glm::vec2(1, 1)),
        VertexFormat(glm::vec3( corner.x,  corner.y,  corner.z), color, glm::vec3(1, 0, 0), glm::vec2(0, 1)),

        // Left face
        VertexFormat(glm::vec3(-corner.x, -corner.y, -corner.z), color, glm::vec3(-1, 0, 0), glm::vec2(0, 0)),
        VertexFormat(glm::vec3(-corner.x, -corner.y,  corner.z), color, glm::vec3(-1, 0, 0), glm::vec2(1, 0)),
        VertexFormat(glm::vec3(-corner.x,  corner.y,  corner.z), color, glm::vec3(-1, 0, 0), glm::vec2(1, 1)),
        VertexFormat(glm::vec3(-corner.x,  corner.y, -corner.z), color, glm::vec3(-1, 0, 0), glm::vec2(0, 1)),
    };

    vector<unsigned int> indices = {
        0, 1, 2, 0, 2, 3,       // Front
        4, 5, 6, 4, 6, 7,       // Back
        8, 9, 10, 8, 10, 11,    // Top
        12, 13, 14, 12, 14, 15, // Bottom
        16, 17, 18, 16, 18, 19, // Right
        20, 21, 22, 20, 22, 23  // Left
    };

    CreateMesh(name, vertices, indices);
}
