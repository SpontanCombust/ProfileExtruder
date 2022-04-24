#pragma once

#include <glm/glm.hpp>
#include <SDL2/SDL_events.h>

class Camera
{
private:
    glm::vec3 m_position;
    float m_yaw;
    float m_pitch;

    glm::vec3 m_forward;
    glm::vec3 m_right;

    glm::mat4 m_view;
    glm::mat4 m_projection;    


public:
    Camera();
    ~Camera();

    void handleEvent(SDL_Event& event, float deltaTime);
    void update();

    void setPosition(glm::vec3 position);
    const glm::vec3& getPosition() const;

    const glm::mat4& getView() const;
    const glm::mat4& getProjection() const;

private:
    void updateViewUsingRotation();
};