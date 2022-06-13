#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>


#define CAMERA_UP glm::vec3(0.0f, 1.0f, 0.0f)
#define CAMERA_FOV 60.f
#define CAMERA_ASPECT_RATIO 16.0f / 9.0f
#define CAMERA_MOVE_SPEED 5.f
#define CAMERA_ROTATE_SPEED 10.f


Camera::Camera()
{
    m_position = glm::vec3(0.0f);
    m_yaw = -90.0f;
    m_pitch = 0.0f;

    updateViewUsingRotation();
    m_projection = glm::perspective(glm::radians(CAMERA_FOV), CAMERA_ASPECT_RATIO, 0.1f, 100.0f);

    m_isRotationActive = false;

    m_motionForward = 0.f;
    m_motionRight = 0.f;
}

Camera::~Camera()
{

}

void Camera::updateViewUsingRotation()
{
    const float sinYaw = sin(glm::radians(m_yaw));
    const float cosYaw = cos(glm::radians(m_yaw));
    const float sinPitch = sin(glm::radians(m_pitch));
    const float cosPitch = cos(glm::radians(m_pitch));

    m_forward = glm::normalize( glm::vec3(cosYaw * cosPitch, sinPitch, sinYaw * cosPitch) );

    m_view = glm::lookAt(m_position, m_position + m_forward, CAMERA_UP);
    m_right = glm::vec3(m_view[0][0], m_view[1][0], m_view[2][0]);
}

void Camera::handleEvent(SDL_Event& event, float deltaTime)
{
    if(event.type == SDL_KEYDOWN)
    {
        switch(event.key.keysym.sym)
        {
        case SDLK_w:
            m_motionForward = 1.f;
            break;
        case SDLK_s:
            m_motionForward = -1.f;
            break;
        case SDLK_a:
            m_motionRight = -1.f;
            break;
        case SDLK_d:
            m_motionRight = 1.f;
            break;
        }
    }
    else if(event.type == SDL_KEYUP)
    {
        switch(event.key.keysym.sym)
        {
        case SDLK_w:
        case SDLK_s:
            m_motionForward = 0.f;
            break;
        case SDLK_a:
        case SDLK_d:
            m_motionRight = 0.f;
            break;
        }
    }
    else if(event.type == SDL_MOUSEMOTION && m_isRotationActive)
    {
        m_yaw += (float)event.motion.xrel * deltaTime * CAMERA_ROTATE_SPEED;
        m_pitch -= (float)event.motion.yrel * deltaTime * CAMERA_ROTATE_SPEED;

        m_pitch = glm::clamp(m_pitch, -89.f, 89.f);
    }
    else if(event.type == SDL_MOUSEBUTTONDOWN)
    {
        if(event.button.button == SDL_BUTTON_RIGHT)
        {
            m_isRotationActive = !m_isRotationActive;
            SDL_SetRelativeMouseMode(m_isRotationActive ? SDL_TRUE : SDL_FALSE);
        }
    }
}

void Camera::update(float dt)
{
    m_position += m_motionForward * m_forward * dt * CAMERA_MOVE_SPEED;
    m_position += m_motionRight * m_right * dt * CAMERA_MOVE_SPEED;

    updateViewUsingRotation();
}

void Camera::setPosition(glm::vec3 position)
{
    m_position = position;
}

const glm::vec3& Camera::getPosition() const
{
    return m_position;
}

void Camera::setRotation(float yaw, float pitch)
{
    m_yaw = yaw;
    m_pitch = pitch;
}

float Camera::getYaw() const
{
    return m_yaw;
}

float Camera::getPitch() const
{
    return m_pitch;
}

const glm::mat4& Camera::getView() const
{
    return m_view;
}

const glm::mat4& Camera::getProjection() const
{
    // currently no plans for modifying the projection matrix
    return m_projection;
}
