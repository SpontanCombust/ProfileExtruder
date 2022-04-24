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

    m_isRotationActive = true;
    SDL_SetRelativeMouseMode(SDL_TRUE);
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
            m_position += m_forward * deltaTime * CAMERA_MOVE_SPEED;
            break;
        case SDLK_s:
            m_position -= m_forward * deltaTime * CAMERA_MOVE_SPEED;
            break;
        case SDLK_a:
            m_position -= m_right * deltaTime * CAMERA_MOVE_SPEED;
            break;
        case SDLK_d:
            m_position += m_right * deltaTime * CAMERA_MOVE_SPEED;
            break;
        }
    }
    else if(event.type == SDL_MOUSEMOTION && m_isRotationActive)
    {
        m_yaw += (float)event.motion.xrel * deltaTime * CAMERA_ROTATE_SPEED;
        m_pitch -= (float)event.motion.yrel * deltaTime * CAMERA_ROTATE_SPEED;
    }
    else if(event.type == SDL_MOUSEBUTTONDOWN)
    {
        if(event.button.button == SDL_BUTTON_LEFT)
        {
            SDL_SetRelativeMouseMode(SDL_TRUE);
            m_isRotationActive = true;
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
            SDL_SetRelativeMouseMode(SDL_FALSE);
            m_isRotationActive = false;
        }
    }
}

void Camera::update()
{
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

const glm::mat4& Camera::getView() const
{
    return m_view;
}

const glm::mat4& Camera::getProjection() const
{
    // currently no plans for modifying the projection matrix
    return m_projection;
}
