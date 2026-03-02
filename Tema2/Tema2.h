#pragma once

#include <vector>

#include "components/simple_scene.h"
#include "lab_m1/lab5/lab_camera.h"
#include "components/text_renderer.h"


namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
     public:
        Tema2();
        ~Tema2();

        void Init() override;

     private:
        void CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned int> &indices);
        void Create_Rectangle_2X1();
        void Create_Cilinder();
        void CreateCylinder(const char *name, float radius, float height, int segments, glm::vec3 color);
        void CreateParallelepiped(const char *name, float width, float height, float depth, glm::vec3 color);
        void FrameStart() override;
        void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, Texture2D *texture = NULL);
        void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::vec3 &position, const glm::vec3 &scale, Texture2D *texture = NULL);
        void Update(float deltaTimeSeconds) override;
        void DrawScene();
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void RenderMiniMap(float deltaTimeSeconds);
        void RenderTime(float deltaTimeSeconds);

        std::unordered_map<std::string, Texture2D *> mapTextures;
        // Camera settings for third person view
        glm::vec3 cameraOffset = glm::vec3(0, 2.0f, -5.0f);  // Behind and above the car
        bool thirdPersonCamera = true;  // Toggle between free and third person camera
        implemented::Camera *camera;
        implemented::Camera* miniMapCamera;
        bool renderMinimap = false;
        glm::mat4 minimapProjection;

        glm::mat4 miniProjection;
        bool isMiniMapPass = false;
        bool gameOver = false;
      void DrawHUD();

     private:
        // The actual renderer object
        gfxc::TextRenderer *textRenderer;

        // Draw primitives mode


        // Colors
        const glm::vec3 kTextColor = NormalizedRGB(166, 172, 205);
        const glm::vec3 kBackgroundColor = NormalizedRGB(41, 45, 62);
     protected:
        GLenum cullFace;
        GLenum polygonMode;
    };
}   // namespace m1
