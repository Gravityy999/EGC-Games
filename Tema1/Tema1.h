#pragma once

#include "components/simple_scene.h"


namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
     public:
        Tema1();
        ~Tema1();

        void Init() override;

      struct ViewportSpace
      {
         ViewportSpace() : x(0), y(0), width(1), height(1) {}
         ViewportSpace(int x, int y, int width, int height)
            : x(x), y(y), width(width), height(height) {}
         int x;
         int y;
         int width;
         int height;
      };

      struct LogicSpace
      {
         LogicSpace() : x(0), y(0), width(1), height(1) {}
         LogicSpace(float x, float y, float width, float height)
            : x(x), y(y), width(width), height(height) {}
         float x;
         float y;
         float width;
         float height;
      };

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;
        void DrawScene(glm::mat3 visMatrix);
        void DrawScene_game(glm::mat3 visMatrix, float deltaTimeSeconds);
        bool CheckCircleRectCollision(
    float radius,
    float rx, float ry,
    float width, float height);
    void DrawGameOverScreen(glm::mat3 visMatrix);

    float CheckCircleRectCollision_chicken(  
    float radius,           // circle radius
    float rx, float ry,     // rectangle bottom-left corner
    float width, float height);

        void RenderCannon(glm::mat3 baseMatrix, glm::mat3 visMatrix, glm::vec3 color);

        // logic/view helpers (copied from Lab3_Vis2D)
        glm::mat3 VisualizationTransf2D(const LogicSpace &logicSpace, const ViewportSpace &viewSpace);
        glm::mat3 VisualizationTransf2DUnif(const LogicSpace &logicSpace, const ViewportSpace &viewSpace);
        void SetViewportArea(const ViewportSpace &viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);
        void CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned int> &indices);
        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

     protected:
      // From Lab3_Vis2D
      bool draggingSolid = false;
      bool draggingShield = false;
      glm::vec2 dragOffsetSolid;
      float square_workspace_length;
      float rectangle_parts_length;
      float rectangle_parts_width;
      float rectangle_box_length;
      float rectangle_box_width;
      float square_count_length;
      ViewportSpace viewSpace;
      LogicSpace logicSpace;
      glm::mat3 visMatrix;
        float cx, cy;
        float cx_2, cy_2;
        float color1;
        float cx1, cy1;
        glm::mat3 modelMatrix;
        glm::mat3 modelMatrix_solid;
        float translateX, translateY;
        float scaleX, scaleY;
        float angularStep;
        float posX, posY, posZ;
        float posX_solid, posY_solid;
        float posX_solid_s, posY_solid_s;
        float posX_engine, posY_engine;
        float dragOffsetX = 0.f;
        bool solidOnBoard = false;       // already exists
float boardSolidX = 0.0f;        // X position on the board
float boardSolidY = 0.0f;        // Y position on the board
float boardShieldX = 0.0f;        // X position on the board
float boardShieldY = 0.0f; 
float start_workspace_x;
float start_workspace_y;
float padding = 1;
// Drag system
bool draggingEngine = false;
bool draggingCannon = false;


// Original positions
float startX_solid, startY_solid;
float startX_engine, startY_engine;
float startX_cannon, startY_cannon;
bool engineOnBoard = false;
float boardEngineX, boardEngineY;

bool cannonOnBoard = false;
float boardCannonX, boardCannonY;
glm::vec3 cannon_offset;
glm::vec3 cannon_offset_s;
float posX_shield, posY_shield;
float startX_shield, startY_shield;


float dragOffsetY = 0.f;


        // TODO(student): If you need any other class variables, define them here.

    };
}   // namespace m1
