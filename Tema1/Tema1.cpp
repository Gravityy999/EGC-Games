#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"

#include "core/engine.h"
#include "utils/gl_utils.h"



bool game_over = 0;
glm::vec3 color_start;
int min_tun = -1;
int max_motor = 1001;
int ok_solid = 0;
bool hasMouseMoved = false;
int rows = 10;
    int cols = 20;
int number_parts = 10;
int problems = 0;
int start_game = 0;
float ty_eggs[9] = {0, 0, 0, 0, 0, 0, 0 ,0 ,0};
int nr_cannons = 0;
bool fire = 0;
float posX_Bullet = 0;
float posY_Bullet = 0;
int one_time = 0;
float ty_chicken = 0;
int chicken = 0;
int nr_cannons_copy = 0;
bool chicken_alive[9]; // tracks if chickens 1-3 are alive (index 0 unused)
    // Create a 10x20 matrix filled with 0
    //std::vector<glm::mat3> Solids;
    std::map<std::pair<int,int>, glm::mat3> Solids;
    std::map<std::pair<int,int>, glm::mat3> Engines;
    std::map<std::pair<int,int>, glm::mat3> Shields;
    std::map<std::pair<int,int>, glm::mat3> Cannons;
    std::map<int, std::pair<float, float>> Bullets;
    std::map<int, std::pair<float, float>> Bullets_freeze;
    std::map<int, std::pair<float, float>> Eggs;
    std::map<int, std::pair<float, float>> Bullets_moving;



int matrix[101][101];
using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}


void Tema1::Init()
{

    for(int i = 0; i < 9; i++)
        chicken_alive[i] = true;
for(int i = 0; i < 6; i++){
    Eggs[i] = {14 + 15 + 25 * i + 3,17 + 63};
}

for(int i = 6; i < 9; i++){
    Eggs[i] = {14 + 27.5 + 25 * (i - 5) + 3,17 + 58};
}

for(int i = 0; i < 6; i++){
    Bullets_moving[i] = {14 + 15 + 25 * i, 17 + 70};
}

for(int i = 6; i < 9; i++){
    Bullets_moving[i] = {14 + 27.5 + 25 * (i - 5), 17 + 60};
}

color_start = glm::vec3(0.0f, 1.0f, 0.0f);

for(int i = 0; i < 12; i++){
    for(int j = 0; j < 22; j++)
        matrix[i][j] = 0;
}
    startX_solid = posX_solid;
startY_solid = posY_solid;

startX_engine = posX_engine;
startY_engine = posY_engine;

startX_cannon = 1 + rectangle_parts_width / 2 - 5 / 2;
startY_cannon = 1 + 24.75 * 2 + 1;


    //glDisable(GL_DEPTH_TEST);
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    logicSpace.x = 0;
    logicSpace.y = 0;
    logicSpace.width = 100;
    logicSpace.height = 100;

    square_workspace_length = 5;
    rectangle_parts_length = 23.75;
    rectangle_parts_width = 40;
    rectangle_box_length = 63;
    rectangle_box_width = 123;
    square_count_length = 7;
    
    // Center coordinates for scaling boxes
    cx_2 = 1 + square_workspace_length/2;// blue box
    cy_2 = 1 + square_workspace_length/2;

    // Meshes

    glm::vec2 pivot(14, 17); // central point for the chicken

    Mesh *Bullet = object2D::CreateCircle("Bullet", glm::vec3(0, 0, 0), 0, 360, 2, glm::vec3(0, 0, 0), true);
    AddMeshToList(Bullet);

auto ScalePoint = [&](glm::vec3 p) -> glm::vec3 {
    return glm::vec3(
        pivot.x + 0.5f * (p.x - pivot.x),
        pivot.y + 0.5f * (p.y - pivot.y),
        p.z
    );
};
    Mesh *egg = object2D::CreateOval("egg", ScalePoint(glm::vec3(14, 17, 0)), 2, 3, glm::vec3(0, 0, 0), true);
    AddMeshToList(egg);
// Legs
Mesh* triangle_chicken_legs = object2D::CreateTriangleFromPoints(
    "triangle_chicken_legs",
    ScalePoint(glm::vec3(13, 15, 0)),
    ScalePoint(glm::vec3(19, 15, 0)),
    ScalePoint(glm::vec3(16, 11, 0)),
    glm::vec3(1.0f, 0.5f, 0.0f), true);
AddMeshToList(triangle_chicken_legs);

Mesh* rectangle_chicken_legs = object2D::CreateRectangle(
    "rectangle_chicken_legs",
    ScalePoint(glm::vec3(17, 15, 0)),
    3 * 0.5f, 2 * 0.5f,
    glm::vec3(1.0f, 0.5f, 0.0f), true);
AddMeshToList(rectangle_chicken_legs);

// Body
Mesh* square_chicken = object2D::CreateSquare(
    "square_chicken",
    ScalePoint(glm::vec3(14, 17, 0)),
    11 * 0.5f,
    glm::vec3(0, 0, 0), true);
AddMeshToList(square_chicken);

// Arms
Mesh* triangle_chicken_arms = object2D::CreateTriangleFromPoints(
    "triangle_chicken_arms",
    ScalePoint(glm::vec3(23, 25, 0)),
    ScalePoint(glm::vec3(35, 29, 0)),
    ScalePoint(glm::vec3(30, 32, 0)),
    glm::vec3(0, 0, 0), true);
AddMeshToList(triangle_chicken_arms);

Mesh* triangle_chicken_arms1 = object2D::CreateTriangleFromPoints(
    "triangle_chicken_arms1",
    ScalePoint(glm::vec3(15, 25, 0)),
    ScalePoint(glm::vec3(11, 29, 0)),
    ScalePoint(glm::vec3(8, 26, 0)),
    glm::vec3(0, 0, 0), true);
AddMeshToList(triangle_chicken_arms1);

// Head
Mesh* circle_chicken_head = object2D::CreateCircle(
    "circle_chicken_head", 
    ScalePoint(glm::vec3(20 ,31, 0)), 0, 360, 4 * 0.5f, glm::vec3(0, 0, 0), true);
AddMeshToList(circle_chicken_head);

Mesh* triangle_chicken_mouth = object2D::CreateTriangleFromPoints(
    "triangle_chicken_mouth",
    ScalePoint(glm::vec3(20, 32, 0)),
    ScalePoint(glm::vec3(23, 29, 0)),
    ScalePoint(glm::vec3(20, 29, 0)),
    glm::vec3(0, 0, 0), true);
AddMeshToList(triangle_chicken_mouth);

Mesh* circle_chicken_eyes = object2D::CreateCircle(
    "circle_chicken_eyes", 
    ScalePoint(glm::vec3(18.5 ,33, 0)), 0, 360, 0.5 * 0.5f, glm::vec3(1.0f, 0, 0), true);
AddMeshToList(circle_chicken_eyes);

// Head details
Mesh* triangle_chicken_head = object2D::CreateTriangleFromPoints(
    "triangle_chicken_head",
    ScalePoint(glm::vec3(17.95f, 34.0f, 0)),
    ScalePoint(glm::vec3(19.65f, 37.8f, 0)),
    ScalePoint(glm::vec3(19.65f, 34.0f, 0)),
    glm::vec3(0, 0, 0), true);
AddMeshToList(triangle_chicken_head);

Mesh* triangle_chicken_head1 = object2D::CreateTriangleFromPoints(
    "triangle_chicken_head1",
    ScalePoint(glm::vec3(17.95f, 34.0f, 0)),
    ScalePoint(glm::vec3(22.0f, 38.0f, 0)),
    ScalePoint(glm::vec3(22.0f, 34.0f, 0)),
    glm::vec3(0, 0, 0), true);
AddMeshToList(triangle_chicken_head1);

    Mesh* square_workspace1 = object2D::CreateSquare("square_workspace1", glm::vec3(90, 50, 0), square_workspace_length, glm::vec3(0, 0, 0), true);
    AddMeshToList(square_workspace1);

    Mesh* triangle_start1 = object2D::CreateTriangleFromPoints("triangle_start1", glm::vec3(157, 87, 0), glm::vec3(164, 87, 0), glm::vec3(157, 80, 0),  glm::vec3(0, 0, 0), true);
    AddMeshToList(triangle_start1);

    Mesh* triangle_start2 = object2D::CreateTriangleFromPoints("triangle_start2", glm::vec3(157, 87, 0), glm::vec3(157, 80, 0), glm::vec3(164, 80, 0),  glm::vec3(0, 0, 0), true);
    AddMeshToList(triangle_start2);

    start_workspace_x = 45;
    start_workspace_y = 3;
    Mesh* square_workspace = object2D::CreateSquare("square_workspace", glm::vec3(start_workspace_x, start_workspace_y, 0), square_workspace_length, glm::vec3(0, 0, 0), true);
    AddMeshToList(square_workspace);

    Mesh* rectangle_parts = object2D::CreateRectangle("rectangle_parts", glm::vec3(1, 1, 0), rectangle_parts_length, rectangle_parts_width, glm::vec3(0, 0, 0));
    AddMeshToList(rectangle_parts);
    // Mesh* square2 = object2D::CreateSquare("square_blue", glm::vec3(44, 3, 0), 61, glm::vec3(1, 0, 0), false);
    // AddMeshToList(square2);
    Mesh* rectangle_box = object2D::CreateRectangle("rectangle_box", glm::vec3(43, 1, 0), rectangle_box_length, rectangle_box_width, glm::vec3(0, 0, 0));
    AddMeshToList(rectangle_box);
    // Mesh* square3 = object2D::CreateSquare("square_p1", glm::vec3(5, 3, 0), 18, glm::vec3(1, 0, 0), false);
    // AddMeshToList(square3);

    Mesh* square_count = object2D::CreateSquare("square_count", glm::vec3(57, 80, 0), square_count_length, glm::vec3(0, 0, 0), true);
    AddMeshToList(square_count);

    posX_solid = 1 + rectangle_parts_width/2 - 5/2;
    posY_solid = 6;
    posX_solid_s = 1 + rectangle_parts_width/2 - 5/2;
    posY_solid_s = 6;

    Mesh* square_solid = object2D::CreateSquare("square_solid", glm::vec3(0, 0, 0), square_workspace_length, glm::vec3(0, 0, 0), true);
    AddMeshToList(square_solid);

    Mesh* square_solid_on_board = object2D::CreateSquare("square_solid_on_board", glm::vec3(45, 3, 0), square_workspace_length, glm::vec3(0, 0, 0), true);
    AddMeshToList(square_solid_on_board);


    // Mesh* engine = object2D::CreateEngine("engine", glm::vec3(1, 1, 0), square_workspace_length, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.8f, 0.2f), true);
    // AddMeshToList(engine);
    posX_engine = 1 + rectangle_parts_width/2 - 5/2;
    posY_engine = 6 + 24.75;
    startX_engine = 1 + rectangle_parts_width/2 - 5/2;
    startY_engine = 6 + 24.75;


    Mesh* engine_square = object2D::CreateSquare("engine_square", glm::vec3(0, 0, 0), square_workspace_length, glm::vec3(0, 0, 0), true);
    AddMeshToList(engine_square);

    Mesh* engine_triangle1 = object2D::CreateTriangle("engine_triangle1", glm::vec3(0, 0, 0), 3, glm::vec3(0, 0, 0), true);
    AddMeshToList(engine_triangle1);
    
    Mesh* engine_triangle2 = object2D::CreateTriangle("engine_triangle2", glm::vec3(0, 0, 0), 3, glm::vec3(0, 0, 0), true);
    AddMeshToList(engine_triangle2);

    Mesh* engine_triangle3 = object2D::CreateTriangle("engine_triangle3", glm::vec3(0, 0, 0), 3, glm::vec3(0, 0, 0), true);
    AddMeshToList(engine_triangle3);
    
    posX_shield = 1 + rectangle_parts_width/2 - 5/2;
    posY_shield = 6 + 24.75 * 3;
    startX_shield = 1 + rectangle_parts_width/2 - 5/2;
    startY_shield = 6 + 24.75 * 3;


    Mesh* shield_square = object2D::CreateSquare("shield_square", glm::vec3(0, 0, 0), square_workspace_length, glm::vec3(0, 0, 0), true);
    AddMeshToList(shield_square);

    Mesh* shield_semicircle = object2D::CreateHalfOval("shield_semicircle", glm::vec3(0, 0, 0), 7.5f, 4.0f,  glm::vec3(1.0f, 0.0f, 0.0f), true);
    AddMeshToList(shield_semicircle);
    

    // Mesh* cannon = object2D::CreateCannon("cannon", glm::vec3(1, 1, 0), square_workspace_length, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.8f, 0.2f), true);
    // AddMeshToList(cannon);

    cannon_offset = glm::vec3(1 + rectangle_parts_width/2 - 5/2, 1 + 24.75 * 2 + 1, 0);
    cannon_offset_s = glm::vec3(1 + rectangle_parts_width/2 - 5/2, 1 + 24.75 * 2 + 1, 0);

    Mesh* cannon_small_rectangle = object2D::CreateRectangle("cannon_small_rectangle", glm::vec3(0, 0, 0), 2.5f, 5.0f, glm::vec3(0, 0, 0), true);
    AddMeshToList(cannon_small_rectangle);

    Mesh* cannon_semicircle1 = object2D::CreateCircle("cannon_semicircle1", glm::vec3(0, 0, 0), 0.0f, 180.0f, 2.5f,  glm::vec3(1.0f, 0.0f, 0.0f), true);
    AddMeshToList(cannon_semicircle1);

    Mesh* cannon_semicircle2 = object2D::CreateCircle("cannon_semicircle2", glm::vec3(0, 0, 0), -180.0f, 0.0f, 2,  glm::vec3(1.0f, 0.0f, 0.0f), true);
    AddMeshToList(cannon_semicircle2);

    Mesh* cannon_big_rectangle = object2D::CreateRectangle("cannon_big_rectangle", glm::vec3(0, 0, 0), 7.5, 4, glm::vec3(0, 0, 0), true);
    AddMeshToList(cannon_big_rectangle);

    Mesh* cannon_trapeze = object2D::CreateTrapeze("cannon_trapeze", glm::vec3(0, 0, 0), 4,5,  2.5, glm::vec3(0, 0, 0), true);
    AddMeshToList(cannon_trapeze);

}


// Uniform 2D visualization matrix (same scale factor on x and y axes)
glm::mat3 Tema1::VisualizationTransf2DUnif(const LogicSpace & logicSpace, const ViewportSpace & viewSpace)
{
    float sx, sy, tx, ty, smin;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    if (sx < sy)
        smin = sx;
    else
        smin = sy;
    tx = viewSpace.x - smin * logicSpace.x;
    ty = viewSpace.y - smin * logicSpace.y;


    return glm::transpose(glm::mat3(
        smin, 0.0f, tx,
        0.0f, smin, ty,
        0.0f, 0.0f, 1.0f));
}

void Tema1::SetViewportArea(const ViewportSpace & viewSpace, glm::vec3 colorColor, bool clear)
{
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
    GetSceneCamera()->Update();
}

int check_if_ship_ok(int x, int y, std::string type){
    if(type == "Solid"){
        if(matrix[x - 1][y] == 0 && matrix[x + 1][y] == 0 && matrix[x][y + 1] == 0 && matrix[x][y - 1] == 0){
            for(int i = 1; i < 11; i++){
                for(int j = 1; j < 21; j++){
                    if(matrix[i][j] != 0 && (i != x || j != y)){
                        color_start = glm::vec3(1.0f, 0.0f, 0.0f);
                        return 0;
                    }
                }
            }
        }
        if(x > max_motor){
            color_start = glm::vec3(1.0f, 0.0f, 0.0f);
            return 0;
        }

        if(x < min_tun){
            color_start = glm::vec3(1.0f, 0.0f, 0.0f);
            return 0;
        }

        for(int i = x + 1; i < 21; i++) 
        {
            if(matrix[i][y] == 4){
                color_start = glm::vec3(1.0f, 0.0f, 0.0f);
                return 0;
            }
        }
        
    }
    else if(type == "Engine"){
        if(x < max_motor)
            max_motor = x;
        if(matrix[x - 1][y] == 0 && matrix[x][y - 1] == 0 && matrix[x][y + 1] == 0){
            for(int i = 1; i < 11; i++){
                for(int j = 1; j < 21; j++){
                    if(matrix[i][j] != 0 && (i != x || j != y) && (i != x + 1 || j != y)){
                        color_start = glm::vec3(1.0f, 0.0f, 0.0f);
                        return 0;
                    }
                }
            }
        } 
        if(x < max_motor){
            color_start = glm::vec3(1.0f, 0.0f, 0.0f);
            return 0;
        }

        if(x > min_tun){
            color_start = glm::vec3(1.0f, 0.0f, 0.0f);
            return 0;
        }

    }

    else if(type == "Cannon"){
        if(x > min_tun)
            min_tun = x - 2;
        if(matrix[x + 1][y] == 0 && matrix[x][y - 1] == 0 && matrix[x][y + 1] == 0){
            for(int i = 1; i < 11; i++){
                for(int j = 1; j < 21; j++){
                    if(matrix[i][j] != 0 && (i != x || j != y) && (i != x - 1 || j != y) && (i != x - 2 || j != y)){
                        color_start = glm::vec3(1.0f, 0.0f, 0.0f);
                        return 0;
                    }
                }
            }
        }
        if(x > max_motor){
            color_start = glm::vec3(1.0f, 0.0f, 0.0f);
            return 0;
        }

        if(x < min_tun){
            color_start = glm::vec3(1.0f, 0.0f, 0.0f);
            return 0;
        }

        if(matrix[x][y + 1] == 4 || matrix[x - 1][y + 1] == 4 || matrix[x - 2][y + 1] == 4 || matrix[x][y - 1] == 4 
            || matrix[x - 1][y - 1] == 4 || matrix[x - 2][y - 1] == 4){
                color_start = glm::vec3(1.0f, 0.0f, 0.0f);
                return 0;
            }

        for(int i = x + 1; i < 11; i++) 
        {
            if(matrix[i][y] == 4){
                color_start = glm::vec3(1.0f, 0.0f, 0.0f);
                return 0;
            }
        }

    }
    else if(type == "Shield"){
            if(matrix[x][y - 1] == 4 || matrix[x][y + 1] == 4){
                color_start = glm::vec3(1.0f, 0.0f, 0.0f);
                return 0;
            }

            if(matrix[x][y - 1] == 0 && matrix[x][y + 1] == 0 && matrix[x + 1][y] == 0){
                for(int i = 1; i < 11; i++){
                    for(int j = 1; j < 21; j++){
                        if(matrix[i][j] != 0 && (i != x || j != y) && (i != x - 1 || j != y) 
                        && (i != x - 1 || j != y + 1) && (i != x - 1 || j != y - 1)){
                            color_start = glm::vec3(1.0f, 0.0f, 0.0f);
                            return 0;
                        }
                    }
                }
            }
            if(x > max_motor){
                color_start = glm::vec3(1.0f, 0.0f, 0.0f);
                return 0;
            }

            if(x < min_tun){
                color_start = glm::vec3(1.0f, 0.0f, 0.0f);
                return 0;

            if(matrix[x][y - 1] == 3 ||  matrix[x][y + 1] == 3){
                color_start = glm::vec3(1.0f, 0.0f, 0.0f);
                return 0;

            }
            }
    }
    return 1;
}


void Tema1::FrameStart()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Tema1::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->GetResolution();

    viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
    SetViewportArea(viewSpace, glm::vec3(0.9f), true);
    
    // Compute uniform 2D visualization matrix
    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);
    if(game_over == 1){
        DrawGameOverScreen(visMatrix);
    }
    else
    if(start_game == 0)
        DrawScene(visMatrix);
    else
    {
    
        DrawScene_game(visMatrix, deltaTimeSeconds);    
    }
}


void Tema1::FrameEnd()
{
}

void Tema1::DrawGameOverScreen(glm::mat3 visMatrix) {
    // just fill screen with red rectangle
    modelMatrix = visMatrix;
    RenderMesh2D(meshes["square_workspace1"], modelMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
    // maybe render some text: "GAME OVER"
}


void Tema1::DrawScene_game(glm::mat3 visMatrix, float deltaTimeSeconds){
        
    for(int i = 0; i < 6; i++){
        if(CheckCircleRectCollision_chicken(2, 14 + 15 + 25 * i, 17 + 70 - ty_chicken, 11 * 0.5f, 11 * 0.5f) == true){
            chicken_alive[i] = false;
        }
        if(chicken_alive[i] == true){
        ty_chicken = ty_chicken + deltaTimeSeconds * 0.1f;
        modelMatrix = visMatrix * transform2D::Translate(15 + 25 * i, 70 - ty_chicken);
        RenderMesh2D(meshes["triangle_chicken_legs"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));
        modelMatrix = visMatrix * transform2D::Translate(4, 0.2f) * transform2D::Translate(15 + 25 * i, 70 - ty_chicken);
        RenderMesh2D(meshes["triangle_chicken_legs"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));
        modelMatrix = visMatrix * transform2D::Translate(15 + 25 * i, 70 - ty_chicken);
        RenderMesh2D(meshes["rectangle_chicken_legs"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));
        modelMatrix = visMatrix * transform2D::Translate(2.1f, 0) * transform2D::Translate(15 + 25 * i, 70 - ty_chicken);
        RenderMesh2D(meshes["rectangle_chicken_legs"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));
        modelMatrix = visMatrix * transform2D::Translate(15 + 25 * i, 70 - ty_chicken);
        RenderMesh2D(meshes["square_chicken"], modelMatrix, glm::vec3(0.3f, 0.7f, 1.0f));
        RenderMesh2D(meshes["triangle_chicken_arms"], modelMatrix, glm::vec3(0.7f, 0.7f, 0.7f));
        RenderMesh2D(meshes["triangle_chicken_arms1"], modelMatrix, glm::vec3(0.7f, 0.7f, 0.7f));
        RenderMesh2D(meshes["circle_chicken_head"], modelMatrix, glm::vec3(0.7f, 0.7f, 0.7f));
        RenderMesh2D(meshes["triangle_chicken_head"], modelMatrix, glm::vec3(1.0f, 0, 0));
        RenderMesh2D(meshes["triangle_chicken_head1"], modelMatrix, glm::vec3(1.0f, 0, 0));
        RenderMesh2D(meshes["circle_chicken_eyes"], modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
        modelMatrix = visMatrix * transform2D::Translate(1.5, 0) * transform2D::Translate(15 + 25 * i, 70 - ty_chicken);
        RenderMesh2D(meshes["circle_chicken_eyes"], modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
        modelMatrix = visMatrix * transform2D::Translate(15 + 25 * i, 70 - ty_chicken);
        RenderMesh2D(meshes["triangle_chicken_mouth"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));
        if(ty_chicken > 70)
            game_over = 1;
        if(ty_eggs[i] > 63)
            ty_eggs[i] = 0;
        ty_eggs[i] = ty_eggs[i] + ((2 + i) % 3 + 2) * deltaTimeSeconds;
        modelMatrix = visMatrix * transform2D::Translate(15 + 25 * i + 3, 63 - ty_eggs[i] - ty_chicken);
        RenderMesh2D(meshes["egg"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));
        }
        // Eggs.push_back({14 + 15 + 25 * i + 3,17 + 63 - ty_eggs[i]});

    }

    for(int i = 1; i < 4; i++){
        ty_chicken = ty_chicken + deltaTimeSeconds * 0.1f;
        if(CheckCircleRectCollision_chicken(2, 14 + 27.5 + 25 * i, 17 + 60 - ty_chicken, 11 * 0.5f, 11 * 0.5f) == true){
            chicken_alive[i + 5] = false;
        }
        if(chicken_alive[i + 5] ==true){
        modelMatrix = visMatrix * transform2D::Translate(27.5 + 25 * i, 60 - ty_chicken);
        RenderMesh2D(meshes["triangle_chicken_legs"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));
        modelMatrix = visMatrix * transform2D::Translate(4, 0.2f) * transform2D::Translate(27.5 + 25 * i, 60 - ty_chicken);
        RenderMesh2D(meshes["triangle_chicken_legs"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));
        modelMatrix = visMatrix * transform2D::Translate(27.5 + 25 * i, 60 - ty_chicken);
        RenderMesh2D(meshes["rectangle_chicken_legs"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));
        modelMatrix = visMatrix * transform2D::Translate(2.1f, 0) * transform2D::Translate(27.5 + 25 * i, 60 - ty_chicken);
        RenderMesh2D(meshes["rectangle_chicken_legs"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));
        modelMatrix = visMatrix * transform2D::Translate(27.5 + 25 * i, 60 - ty_chicken);
        RenderMesh2D(meshes["square_chicken"], modelMatrix, glm::vec3(0.3f, 0.7f, 1.0f));
        RenderMesh2D(meshes["triangle_chicken_arms"], modelMatrix, glm::vec3(0.7f, 0.7f, 0.7f));
        RenderMesh2D(meshes["triangle_chicken_arms1"], modelMatrix, glm::vec3(0.7f, 0.7f, 0.7f));
        RenderMesh2D(meshes["circle_chicken_head"], modelMatrix, glm::vec3(0.7f, 0.7f, 0.7f));
        RenderMesh2D(meshes["triangle_chicken_head"], modelMatrix, glm::vec3(1.0f, 0, 0));
        RenderMesh2D(meshes["triangle_chicken_head1"], modelMatrix, glm::vec3(1.0f, 0, 0));
        RenderMesh2D(meshes["circle_chicken_eyes"], modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
        modelMatrix = visMatrix * transform2D::Translate(1.5, 0) * transform2D::Translate(27.5 + 25 * i, 60 - ty_chicken);
        RenderMesh2D(meshes["circle_chicken_eyes"], modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
        modelMatrix = visMatrix * transform2D::Translate(27.5 + 25 * i, 60 - ty_chicken);
        RenderMesh2D(meshes["triangle_chicken_mouth"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));
        
        if(ty_chicken > 60)
            game_over = 1;
        if(ty_eggs[i + 5] > 63)
            ty_eggs[i + 5] = 0;
        ty_eggs[i + 5] = ty_eggs[i + 5] + ((2 + i) % 3 + 2) * deltaTimeSeconds;
        modelMatrix = visMatrix * transform2D::Translate(27.5 + 25 * i + 3, 58 - ty_eggs[i + 5] - ty_chicken);
        RenderMesh2D(meshes["egg"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));
        }
        // Eggs.push_back({14 + 27.5 + 25 * i + 3,17 + 58 - ty_eggs[i + 5]});

    }
    for(int j = 1; j < 21; j++){
        for(int i = 1; i < 11; i++){
            int offset_x = 20 + j * 5;
            int offset_y = (11 - i) * 5;
            if(matrix[i][j] == 1){
    modelMatrix = visMatrix * transform2D::Translate(offset_x + posX, offset_y + posY);
    RenderMesh2D(meshes["square_solid"], modelMatrix, glm::vec3(0, 0, 0));
    if(CheckCircleRectCollision(1.5f, offset_x + posX, offset_y + posY, 5, 5) == 1)
                game_over = 1;
}

else if(matrix[i][j] == 2){
    modelMatrix = visMatrix * transform2D::Translate(offset_x + posX, offset_y + posY);
    RenderMesh2D(meshes["engine_square"], modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
    if(CheckCircleRectCollision(1.5f, offset_x + posX, offset_y + posY, 5, 5) == 1)
                game_over = 1;

    modelMatrix = visMatrix * transform2D::Translate(offset_x + posX, offset_y - 3 + 0.5f + posY);
    RenderMesh2D(meshes["engine_triangle1"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));

    modelMatrix = visMatrix * transform2D::Translate(offset_x + 1 + posX, offset_y - 3 + 0.5f + posY);
    RenderMesh2D(meshes["engine_triangle2"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));

    modelMatrix = visMatrix * transform2D::Translate(offset_x + 2 + posX, offset_y - 3 + 0.5f + posY);
    RenderMesh2D(meshes["engine_triangle3"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));

    i++;
}

else if(matrix[i][j] == 3){
    int cannon_y = offset_y - 10;

        Bullets[nr_cannons].first = offset_x + 0.5f + posX + 2;
        Bullets[nr_cannons ++].second = cannon_y + 12.5f + posY + 2;
    

    modelMatrix = visMatrix * transform2D::Translate(offset_x + posX, cannon_y + posY);
    RenderMesh2D(meshes["cannon_small_rectangle"], modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

    modelMatrix = visMatrix * transform2D::Translate(offset_x + 2.5f + posX, cannon_y + 2.5f + posY);
    RenderMesh2D(meshes["cannon_semicircle1"], modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

    modelMatrix = visMatrix * transform2D::Translate(offset_x + 2.5f + posX, cannon_y + 5.0f + posY);
    RenderMesh2D(meshes["cannon_semicircle2"], modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));

    modelMatrix = visMatrix * transform2D::Translate(offset_x + 0.5f + posX, cannon_y + 5.0f + posY);
    RenderMesh2D(meshes["cannon_big_rectangle"], modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    if(CheckCircleRectCollision(1.5f, offset_x + 0.5f + posX, cannon_y + 5.0f + posY, 5, 5) == 1)
                game_over = 1;

    modelMatrix = visMatrix * transform2D::Translate(offset_x + 0.5f + posX, cannon_y + 12.5f + posY);
    RenderMesh2D(meshes["cannon_trapeze"], modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    if(CheckCircleRectCollision(1.5f, offset_x + 0.5f + posX, cannon_y + 12.5f + posY, 5, 5) == 1)
                game_over = 1;

    i += 2;
}

else if(matrix[i][j] == 4){
    int shield_x = offset_x + 5;
    int shield_y = offset_y - 5;

    modelMatrix = visMatrix * transform2D::Translate(shield_x + posX, shield_y + posY);
    RenderMesh2D(meshes["shield_square"], modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
    if(CheckCircleRectCollision(1.5f, shield_x + posX, shield_y + posY, 5, 5) == 1)
                game_over = 1;


    modelMatrix = visMatrix * transform2D::Translate(shield_x + 2.5f + posX, shield_y + 5 + posY);
    RenderMesh2D(meshes["shield_semicircle"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));
    if(CheckCircleRectCollision(1.5f, shield_x - 5 + posX, shield_y + 5 + posY, 10, 5) == 1)
                game_over = 1;


    matrix[i][j + 1] = 0;
    matrix[i][j + 2] = 0;
    matrix[i + 1][j + 1] = 0;
}

        }
    }
    if(fire == 1){
        if(one_time == 0){
            for(int i = 0; i < nr_cannons; i ++){
                Bullets_freeze[i] = Bullets[i];
            }
            one_time = 1;
        }
        for(int i = 0; i < nr_cannons; i ++){
            posY_Bullet = posY_Bullet + deltaTimeSeconds * 50;
            modelMatrix = visMatrix * transform2D::Translate(Bullets_freeze[i].first,Bullets_freeze[i].second + posY_Bullet);
            RenderMesh2D(meshes["Bullet"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));
        }
        if(posY_Bullet > 100.0f)
        {
            fire = 0;
            posY_Bullet = 0;
        }
    }
    nr_cannons_copy = nr_cannons;
    nr_cannons = 0;
}

bool Tema1:: CheckCircleRectCollision(    
    float radius,           // circle radius
    float rx, float ry,     // rectangle bottom-left corner
    float width, float height)
{
    for (int i = 0; i < 6; i++) {
        float Eggx = Eggs[i].first;
        float Eggy = Eggs[i].second - ty_eggs[i];
        // Find closest point on rectangle to the circle
        float closestX = glm::clamp(Eggx, rx, rx + width);
        float closestY = glm::clamp(Eggy, ry, ry + height);

        // Compute distance from circle center → closest point
        float dx = Eggx - closestX;
        float dy = Eggy - closestY;

        float distanceSquared = dx * dx + dy * dy;

        // Collision if distance < radius
        if (distanceSquared < radius * radius)
            return true;
    }

    

    for (int i = 1; i < 4; i++) {
        float Eggx = Eggs[i + 5].first;
        float Eggy = Eggs[i + 5].second - ty_eggs[i + 5];
        // Find closest point on rectangle to the circle
        float closestX = glm::clamp(Eggx, rx, rx + width);
        float closestY = glm::clamp(Eggy, ry, ry + height);

        // Compute distance from circle center → closest point
        float dx = Eggx - closestX;
        float dy = Eggy - closestY;

        float distanceSquared = dx * dx + dy * dy;

        // Collision if distance < radius
        if (distanceSquared < radius * radius)
            return true;
    }
    return false;
}

// And in CheckCircleRectCollision_chicken, fix the bullet Y position:
float Tema1::CheckCircleRectCollision_chicken(   
      // Add this parameter
    float radius,
    float rx, float ry,
    float width, float height)
{
    for (int i = 0; i < nr_cannons_copy; i++) {
        float Eggx = Bullets_freeze[i].first;
        float Eggy = Bullets_freeze[i].second + posY_Bullet;
        
        float closestX = glm::clamp(Eggx, rx, rx + width);
        float closestY = glm::clamp(Eggy, ry, ry + height);

        float dx = Eggx - closestX;
        float dy = Eggy - closestY;
        float distanceSquared = dx * dx + dy * dy;

        if (distanceSquared < radius * radius){
            std::cout << "Hit chicken "  << "!" << std::endl;
            return true; // Return which bullet hit
        }
    }
    return false; // No collision
}



void Tema1::DrawScene(glm::mat3 visMatrix)
{
    // Draw blue rectangle with scaling

    modelMatrix = visMatrix;
    if(problems <= 0){
        color_start = glm::vec3(0.0f, 1.0f, 0.0f);
    }
    RenderMesh2D(meshes["triangle_start1"], modelMatrix, color_start);
    RenderMesh2D(meshes["triangle_start2"], modelMatrix, color_start);
        // draw red parts
        RenderMesh2D(meshes["rectangle_box"], modelMatrix, glm::vec3(1.0f, 0.0f, 0.0f));    

    // Draw red boxes (square_p1)
    for(int i = 0; i < 4; i++)
    {
        modelMatrix = visMatrix *
                      transform2D::Translate(0, i * 24.75);
        // draw red parts
        RenderMesh2D(meshes["rectangle_parts"], modelMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
    }
    for(int i = 0; i < number_parts; i++){
    modelMatrix = visMatrix * transform2D::Translate(i * 10, 0);
        // draw red parts
        RenderMesh2D(meshes["square_count"], modelMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
    } 
    //if(draggingSolid == true && hasMouseMoved == 1){   
//     modelMatrix = visMatrix * transform2D::Translate(posX_solid, posY_solid);
// RenderMesh2D(meshes["square_solid"], modelMatrix, glm::vec3(0, 0, 0));
    // }
    //  else {
    //     modelMatrix = visMatrix;
    //      RenderMesh2D(meshes["square_solid"], modelMatrix, glm::vec3(1, 0, 0));
    //  }
        
        // Get cursor position in logic space
    glm::ivec2 res = window->GetResolution();
glm::ivec2 cursorPos = window->GetCursorPosition();

// convert from window coords to OpenGL coords
glm::vec3 mouseScreen(cursorPos.x, res.y - cursorPos.y, 1);

// convert to logic space
glm::vec3 mouseLogic = glm::inverse(visMatrix) * mouseScreen;

// use a local vector for the cursor position in logic space
glm::vec2 cursorLogic(mouseLogic.x, mouseLogic.y);

    // cursorLogic.x = (float(cursorPos.x) / window->GetResolution().x) * logicSpace.width + logicSpace.x;
    // cursorLogic.y = (1 - float(cursorPos.y) / window->GetResolution().y) * logicSpace.height;

    //cout<<cursorLogic.x<<" ";

    for(int i = 0; i < 20; i++){
    for(int j = 0; j < 10; j++){
        float x =start_workspace_x + i * (square_workspace_length + padding);
        float y =start_workspace_y + j * (square_workspace_length + padding);


        modelMatrix = visMatrix * transform2D::Translate(i * (square_workspace_length + padding), j * (square_workspace_length + padding));

        if(hasMouseMoved && cursorLogic.x > x && cursorLogic.x < x + square_workspace_length &&
           cursorLogic.y > y && cursorLogic.y < y + square_workspace_length)
        {
            RenderMesh2D(meshes["square_workspace"], modelMatrix, glm::vec3(0.529f, 0.808f, 0.980f));
            
        }
        else
        {
            RenderMesh2D(meshes["square_workspace"], modelMatrix, glm::vec3(0.529f, 0.808f, 0.980f));
        }
    }
}

modelMatrix = visMatrix * transform2D::Translate(posX_shield, posY_shield);

RenderMesh2D(meshes["shield_square"], modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));

modelMatrix = visMatrix * transform2D::Translate(posX_shield + 2.5f, posY_shield + 5);
RenderMesh2D(meshes["shield_semicircle"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));






 modelMatrix = visMatrix * transform2D::Translate(posX_engine, posY_engine);
    RenderMesh2D(meshes["engine_square"], modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));

    modelMatrix = visMatrix * transform2D::Translate(posX_engine, posY_engine - 3 + 0.5);
    RenderMesh2D(meshes["engine_triangle1"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));
    modelMatrix = visMatrix * transform2D::Translate(posX_engine + 1, posY_engine - 3 + 0.5);
    RenderMesh2D(meshes["engine_triangle2"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));
    modelMatrix = visMatrix * transform2D::Translate(posX_engine + 2, posY_engine - 3 + 0.5);
    RenderMesh2D(meshes["engine_triangle3"], modelMatrix, glm::vec3(1.0f, 0.5f, 0.0f));


modelMatrix = visMatrix * transform2D::Translate(cannon_offset.x, cannon_offset.y);
                      
        // draw red parts
        //glDisable(GL_DEPTH_TEST);
        RenderMesh2D(meshes["cannon_small_rectangle"],  modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
    
        modelMatrix = visMatrix * transform2D::Translate(cannon_offset.x + 5.0f / 2, cannon_offset.y + 2.5);
        
        RenderMesh2D(meshes["cannon_semicircle1"],  modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

        modelMatrix = visMatrix * transform2D::Translate(cannon_offset.x + 5.0f / 2, cannon_offset.y + 2.5 + 2.5);

        RenderMesh2D(meshes["cannon_semicircle2"],  modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));

        modelMatrix = visMatrix * transform2D::Translate(cannon_offset.x + 5.0f / 2 - 2, cannon_offset.y + 2.5 + 2.5);
    
        RenderMesh2D(meshes["cannon_big_rectangle"],  modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));

        modelMatrix = visMatrix * transform2D::Translate(cannon_offset.x + 5.0f / 2 - 2, cannon_offset.y + 2.5 + 2.5 + 7.5);
        // modelMatrix = visMatrix * transform2D::Translate(1 + rectangle_parts_width/2 - square_workspace_length/2 + square_workspace_length/2 - square_workspace_length/2, 1 + rectangle_parts_length/2 - square_workspace_length + square_workspace_length + square_workspace_length/2 + 15);
        RenderMesh2D(meshes["cannon_trapeze"],  modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
      

glDisable(GL_DEPTH_TEST);

    modelMatrix = visMatrix * transform2D::Translate(posX_solid, posY_solid);

RenderMesh2D(meshes["square_solid"], modelMatrix, glm::vec3(0, 0, 0));

for(auto & pair : Solids){
    modelMatrix = visMatrix * pair.second;
    RenderMesh2D(meshes["square_solid"], modelMatrix, glm::vec3(0, 0, 0));
}

for (auto& pair : Cannons) {
    RenderCannon(pair.second, visMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
}

for (auto& pair : Engines) {
    modelMatrix = visMatrix * pair.second;
    RenderMesh2D(meshes["engine_square"], modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f)); // gray square
    RenderMesh2D(meshes["engine_triangle1"], modelMatrix * transform2D::Translate(0, -2.5f), glm::vec3(1.0f, 0.5f, 0.0f)); // orange triangles
    RenderMesh2D(meshes["engine_triangle2"], modelMatrix * transform2D::Translate(1, -2.5f), glm::vec3(1.0f, 0.5f, 0.0f));
    RenderMesh2D(meshes["engine_triangle3"], modelMatrix * transform2D::Translate(2, -2.5f), glm::vec3(1.0f, 0.5f, 0.0f));
}

for (auto& pair : Shields) {
    modelMatrix = visMatrix * pair.second;
    RenderMesh2D(meshes["shield_square"], modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
    RenderMesh2D(meshes["shield_semicircle"], modelMatrix * transform2D::Translate(2.5f, 5), glm::vec3(1.0f, 0.5f, 0.0f));
}


}
/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */
void Tema1::RenderCannon(glm::mat3 baseMatrix, glm::mat3 visMatrix, glm::vec3 color) {
    glm::mat3 M = visMatrix * baseMatrix;

    RenderMesh2D(meshes["cannon_small_rectangle"], M, color);
    RenderMesh2D(meshes["cannon_semicircle1"], M * transform2D::Translate(5.0f / 2, 2.5), color);
    RenderMesh2D(meshes["cannon_semicircle2"], M * transform2D::Translate(5.0f / 2, 2.5 + 2.5), glm::vec3(0.0f, 0.0f, 0.0f));
    RenderMesh2D(meshes["cannon_big_rectangle"], M * transform2D::Translate(5.0f / 2 - 2, 2.5 + 2.5), glm::vec3(0.0f, 0.0f, 0.0f));
    RenderMesh2D(meshes["cannon_trapeze"], M * transform2D::Translate(5.0f / 2 - 2, 2.5 + 2.5 + 7.5), glm::vec3(0.0f, 0.0f, 0.0f));
}


void Tema1::OnInputUpdate(float deltaTime, int mods)
{

    float speed =20.0f; // movement speed

    if (window->KeyHold(GLFW_KEY_W)) {
        posY += speed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        posY -= speed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        posX -= speed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        posX += speed * deltaTime;
    }
}




void Tema1::OnKeyPress(int key, int mods)
{
    if(key == GLFW_KEY_SPACE){
        one_time = 0;
        fire = 1;
    }
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    glm::ivec2 res = window->GetResolution();
    glm::vec3 mouseScreen(mouseX, res.y - mouseY, 1);
    glm::vec3 m = glm::inverse(visMatrix) * mouseScreen;

    if (draggingSolid)
    {
        posX_solid = m.x - dragOffsetX;
        posY_solid = m.y - dragOffsetY;
    }

    if (draggingEngine)
    {
        posX_engine = m.x - dragOffsetX;
        posY_engine = m.y - dragOffsetY;
    }

    if (draggingCannon)
    {
        cannon_offset.x = m.x - dragOffsetX;
        cannon_offset.y = m.y - dragOffsetY;
    }

    if (draggingShield)
    {
        posX_shield = m.x - dragOffsetX;
        posY_shield = m.y - dragOffsetY;
    }
}




void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    glm::ivec2 res = window->GetResolution();
    glm::vec3 mouseScreen(mouseX, res.y - mouseY, 1);
    glm::vec3 m = glm::inverse(visMatrix) * mouseScreen;

    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
       
    float S = square_workspace_length;

    // SOLID — place it on board permanently

        // Check if released inside board
        if (m.x >= start_workspace_x && m.x <= start_workspace_x + 20 * (S + padding) &&
            m.y >= start_workspace_y && m.y <= start_workspace_y + 10 * (S + padding))
        {
            solidOnBoard = true;

            int col = (m.x - start_workspace_x) / (S + padding);
            int row = (m.y - start_workspace_y) / (S + padding);
            if(Solids.find({col, row}) != Solids.end()){
                Solids.erase({col, row});
                number_parts++;
                matrix[row + 1][col + 1] = 0;
                if(check_if_ship_ok(row + 1, col + 1, "Solid") == 0)
                problems--;
            }

            if(Engines.find({col, row}) != Engines.end()){
                Engines.erase({col, row});
                number_parts++;
                matrix[row + 1][col + 1] = 0;
                matrix[row + 2][col + 1] = 0;
                if(check_if_ship_ok(row + 1, col + 1, "Engine") == 0)
                problems--;

            }

            if(Shields.find({col, row}) != Shields.end()){
                Shields.erase({col, row});
                number_parts++;
                matrix[row + 1][col + 1] = 0;
                matrix[row][col + 1] = 0;
                matrix[row][col] = 0;
                matrix[row][col + 2] = 0;
                if(check_if_ship_ok(row + 1, col + 1, "Shield") == 0)
                problems--;
            }

            if(Cannons.find({col, row}) != Cannons.end()){
                Cannons.erase({col, row});
                number_parts++;
                matrix[row + 1][col + 1] = 0;
                matrix[row][col + 1] = 0;
                matrix[row - 1][col + 1] = 0;
                if(check_if_ship_ok(row + 1, col + 1, "Cannon") == 0)
                problems--;
            }
    }
}
    // Add key press event

    float S = square_workspace_length;

    // click Solid
    if (m.x >= posX_solid_s && m.x <= posX_solid_s + S &&
        m.y >= posY_solid_s && m.y <= posY_solid_s + S)
    {
        draggingSolid = true;
        dragOffsetX = m.x - posX_solid;
        dragOffsetY = m.y - posY_solid;
        return;
    }

    // click Engine
    if (m.x >= posX_engine && m.x <= posX_engine + S &&
        m.y >= posY_engine && m.y <= posY_engine + S)
    {
        draggingEngine = true;
        dragOffsetX = m.x - posX_engine;
        dragOffsetY = m.y - posY_engine;
        return;
    }

    // click Cannon (use same bounding square)
    if (m.x >= cannon_offset.x && m.x <= cannon_offset.x + square_workspace_length &&
        m.y >= cannon_offset.y && m.y <= startY_cannon + 3 * square_workspace_length)
    {
        draggingCannon = true;
        dragOffsetX = m.x - cannon_offset.x;
        dragOffsetY = m.y - cannon_offset.y;
        return;
    }

    if (m.x >= posX_shield - S && m.x <= posX_shield + 2 * S &&
        m.y >= posY_shield && m.y <= posY_shield + 2 * S)
    {
        draggingShield = true;
        dragOffsetX = m.x - posX_shield;
        dragOffsetY = m.y - posY_shield;
        return;
    }

    if (m.x >= 157 && m.x <= 167 && m.y >= 80 && m.y <= 87){
        start_game = 1;
        for (int row = 1; row <11; ++row) {
        std::cout << "[ ";
        for (int col = 1; col < 21; ++col) {
            std::cout << matrix[row][col] << " "; // column-major access
        }
        std::cout << "]\n";
    }
    }
}





void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    glm::ivec2 res = window->GetResolution();
    glm::vec3 mouseScreen(mouseX, res.y - mouseY, 1);
    glm::vec3 m = glm::inverse(visMatrix) * mouseScreen;

    float S = square_workspace_length;

    // SOLID — place it on board permanently
    if (draggingSolid)
    {
        draggingSolid = false;

        // Check if released inside board
        if (m.x >= start_workspace_x && m.x <= start_workspace_x + 20 * (S + padding) &&
            m.y >= start_workspace_y && m.y <= start_workspace_y + 10 * (S + padding))
        {
            solidOnBoard = true;

            int col = (m.x - start_workspace_x) / (S + padding);
            int row = (m.y - start_workspace_y) / (S + padding);

            boardSolidX =  start_workspace_x  + col * (S + padding);
            boardSolidY =start_workspace_y + row * (S + padding);
            modelMatrix = transform2D::Translate(boardSolidX, boardSolidY);
            Solids[{col, row}] = modelMatrix;
            posX_solid = posX_solid_s;
            posY_solid = posY_solid_s;
            number_parts--;
            matrix[11-(row + 1)][col + 1] = 1;
            if(check_if_ship_ok(row + 1, col + 1, "Solid") == 0)
                problems++;

        }
        else
        {
            // If not dropped on board, return to spawn
            posX_solid = posX_solid_s;
            posY_solid = posY_solid_s;
        }
    }

    // ENGINE — same logic
    // ENGINE — same logic
// ENGINE — same logic as Solid and Cannon
if (draggingEngine)
{
    draggingEngine = false;

    if (m.x >= start_workspace_x && m.x <= start_workspace_x + 20 * (S + padding) &&
        m.y >= start_workspace_y && m.y <= start_workspace_y + 10 * (S + padding))
    {
        engineOnBoard = true;

        int col = (m.x - start_workspace_x) / (S + padding);
        int row = (m.y - start_workspace_y) / (S + padding);

        // Compute snapped position
        boardEngineX = start_workspace_x + col * (S + padding);
        boardEngineY = start_workspace_y + row * (S + padding);

        // Save transformation matrix for persistent rendering
        modelMatrix = transform2D::Translate(boardEngineX, boardEngineY);
        Engines[{col, row}] = modelMatrix;

        // Return engine to spawn position
        posX_engine = startX_engine;
        posY_engine = startY_engine;
        number_parts--;
        matrix[11 - (row + 1)][col + 1] = 2;
        matrix[11 - row][col + 1] = 2;
        if(check_if_ship_ok(row + 1, col + 1, "Engine") == 0)
                problems++;
    }
    else
    {
        // If not dropped on board, return to spawn
        posX_engine = startX_engine;
        posY_engine = startY_engine;
    }
}



    // CANNON — same logic
if (draggingCannon)
    {
        draggingCannon = false;

        if (m.x >= start_workspace_x && m.x <= start_workspace_x + 20 * (S + padding) &&
            m.y >= start_workspace_y && m.y <= start_workspace_y + 10 * (S + padding))
        {
            cannonOnBoard = true;
            int col = (m.x - start_workspace_x) / (S + padding);
            int row = (m.y - start_workspace_y) / (S + padding);

            boardCannonX =  start_workspace_x  + col * (S + padding);
            boardCannonY =start_workspace_y + row * (S + padding);
            modelMatrix = transform2D::Translate(boardCannonX, boardCannonY);
            Cannons[{col, row}] = modelMatrix;
            cannon_offset.x = cannon_offset_s.x;
            cannon_offset.y = cannon_offset_s.y;
            number_parts--;
            matrix[11 - (row + 1)][col + 1] = 3;
            matrix[11 - (row + 2)][col + 1] = 3;
            matrix[11 - (row + 3)][col + 1] = 3;
            if(check_if_ship_ok(row + 1, col + 1, "Cannon") == 0)
                problems++;
        }
        else
        {
            // If not dropped on board, return to spawn
            cannon_offset.x = cannon_offset_s.x;
            cannon_offset.y = cannon_offset_s.y;
        }
}

if (draggingShield)
    {
        draggingShield = false;

        if (m.x >= start_workspace_x && m.x <= start_workspace_x + 20 * (S + padding) &&
            m.y >= start_workspace_y && m.y <= start_workspace_y + 10 * (S + padding))
        {
            cannonOnBoard = true;
            int col = (m.x - start_workspace_x) / (S + padding);
            int row = (m.y - start_workspace_y) / (S + padding);

            boardShieldX =  start_workspace_x  + col * (S + padding);
            boardShieldY =start_workspace_y + row * (S + padding);
            modelMatrix = transform2D::Translate(boardShieldX, boardShieldY);
            Shields[{col, row}] = modelMatrix;
            posX_shield = startX_shield;
            posY_shield = startY_shield;
            number_parts--;
            matrix[11 - (row + 1)][col + 1] = 4;
            matrix[11 - (row + 2)][col + 1] = 4;
            matrix[11 - (row + 2)][col] = 4;
            matrix[11 - (row + 2)][col + 2] = 4;
            if(check_if_ship_ok(row + 1, col + 1, "Shield") == 0)
                problems++;
        }
        else
        {
            // If not dropped on board, return to spawn
            posX_shield = startX_shield;
            posY_shield = startY_shield;
        }
}
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{

}


void Tema1::OnWindowResize(int width, int height)
{
    // Update the viewSpace with the new window size
    viewSpace = ViewportSpace(0, 0, width, height);

    // Keep the logic space fixed (so the game scale doesn't change)
    logicSpace.x = 0;
    logicSpace.y = 0;
    logicSpace.width = 100;
    logicSpace.height = 100;

    // Recompute visualization matrix (so it stays centered)
    visMatrix = VisualizationTransf2DUnif(logicSpace, viewSpace);
}

