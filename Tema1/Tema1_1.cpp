// #include "lab_m1/Tema1/Tema1.h"

// #include <vector>
// #include <iostream>

// #include "lab_m1/lab3/transform2D.h"
// #include "lab_m1/lab3/object2D.h"

// #include "core/engine.h"
// #include "utils/gl_utils.h"

// Tema1_1::Tema1_1()
// {
// }


// Tema1_1::~Tema1()
// {
// }


// void Tema1::Init()
// {

//     //camera
//     glm::ivec2 resolution = window->GetResolution();
//     auto camera = GetSceneCamera();
//     camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
//     camera->SetPosition(glm::vec3(0, 0, 50));
//     camera->SetRotation(glm::vec3(0, 0, 0));
//     camera->Update();
//     GetCameraInput()->SetActive(false);

//     //coordonate logice
//     logicSpace.x = 0;
//     logicSpace.y = 0;
//     logicSpace.width = 100;
//     logicSpace.height = 100;

//     //patrate tabla
//     start_workspace_x = 45;
//     start_workspace_y = 3;
//     square_workspace_length = 5;
//     Mesh* square_workspace = object2D::CreateSquare("square_workspace", glm::vec3(start_workspace_x, start_workspace_y, 0), square_workspace_length, glm::vec3(0, 0, 0), true);
//     AddMeshToList(square_workspace);

//     //dreptunghiuri stanga
//     start_rectangle_parts_x = 1;
//     start_rectangle_parts_y = 1;
//     rectangle_parts_length = 23.75;
//     rectangle_parts_width = 40;
//     Mesh* rectangle_parts = object2D::CreateRectangle("rectangle_parts", glm::vec3(start_rectangle_parts_x, start_rectangle_parts_y, 0), rectangle_parts_length, rectangle_parts_width, glm::vec3(0, 0, 0));
//     AddMeshToList(rectangle_parts);

//     //dreptunghi tabla
//     start_rectangle_box_x = 43;
//     start_rectangle_box_y = 1;
//     rectangle_box_length = 63;
//     rectangle_box_width = 123;
//     Mesh* rectangle_box = object2D::CreateRectangle("rectangle_box", glm::vec3(start_rectangle_box_x, start_rectangle_box_y, 0), rectangle_box_length, rectangle_box_width, glm::vec3(0, 0, 0));
//     AddMeshToList(rectangle_box);

//     //patrate numarat
//     start_count_x = 57;
//     start_count_y = 80;
//     count_length = 7;
//     Mesh* square_count = object2D::CreateSquare("square_count", glm::vec3(57, 80, 0), count_length, glm::vec3(0, 0, 0), true);
//     AddMeshToList(square_count);
//     padding_cannon = 1;
//     //cannon
//     glm::vec3 cannon_offset = glm::vec3(start_rectangle_parts_x + rectangle_parts_width/2 - square_workspace_length/2
//         , start_rectangle_parts_y + rectangle_parts_length * 2 + padding_cannon);

        
// }    