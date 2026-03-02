#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class RailType {
    NORMAL,
    BRIDGE,
    TUNNEL
};

class Rail {
public:
    glm::vec3 startPosition;
    glm::vec3 endPosition;
    Rail* next;
    RailType type;

    Rail(glm::vec3 start, glm::vec3 end)
        : startPosition(start), endPosition(end), next(nullptr) {}
    Rail(glm::vec3 start, glm::vec3 end, RailType type = RailType::NORMAL)
        : startPosition(start), endPosition(end), next(nullptr), type(type) {}
};

class Train {
public:
    Rail* currentRail;
    float progress; // 0.0f to 1.0f
    float speed;
    glm::vec3 position;

    Train(Rail* rail, float speed)
        : currentRail(rail), progress(0.0f), speed(speed), position(glm::vec3(0))
    {
        if (currentRail) {
            position = currentRail->startPosition;
        }
    }

    void Update(float deltaTime) {
        if (!currentRail) return;

        float railLength = glm::distance(currentRail->startPosition, currentRail->endPosition);

        if (railLength > 0.001f) {
            progress += (speed * deltaTime) / railLength;
        }

        if (progress >= 1.0f) {
            if (currentRail->next) {
                // Calculate how much we overshot in distance
                float excessDistance = (progress - 1.0f) * railLength;
                
                currentRail = currentRail->next;
                
                // Calculate new progress on the next rail
                float nextRailLength = glm::distance(currentRail->startPosition, currentRail->endPosition);
                if (nextRailLength > 0.001f) {
                    progress = excessDistance / nextRailLength;
                } else {
                    progress = 0.0f;
                }
            } else {
                progress = 1.0f; // Stop at the end
            }
        }

        // Update position based on linear interpolation
        position = glm::mix(currentRail->startPosition, currentRail->endPosition, progress);
    }
};