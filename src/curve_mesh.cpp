#include "curve_mesh.hpp"

#include <glm/gtx/rotate_vector.hpp>

#include <algorithm> // std::for_each
#include <cmath> // std::acos
#include <cstdio>


const glm::vec3 INIT_PROFILE_FORWARD_VECTOR = glm::vec3(0.f, 1.f, 0.f);

CurveMesh extrudeProfile(std::vector<glm::vec2> profile, const std::vector<ExtrusionPoint>& extrusionPoints)
{
    CurveMesh mesh{};

    if(extrusionPoints.size() < 2)
    {
        printf("[ERROR][%s(%d)] Not enough points to construct a mesh", __FILE__, __LINE__);
        return mesh;
    }

    for(const auto& ep : extrusionPoints)
    {
        glm::vec3 direction = glm::normalize(ep.direction);
        // the angle of rotation of the direction vector
        float directionAngleDelta = std::acos(glm::dot(INIT_PROFILE_FORWARD_VECTOR, direction));
        // the vector around which said direction vector is rotated
        glm::vec3 directionRotationVec;
        // avoid calculating with NaN value
        if(direction == INIT_PROFILE_FORWARD_VECTOR || direction == -INIT_PROFILE_FORWARD_VECTOR)
        {
            directionRotationVec = {0.f, 0.f, 1.f};
        }
        else
        {
            directionRotationVec = glm::cross(INIT_PROFILE_FORWARD_VECTOR, direction);
        }
        
        for(const auto& p : profile)
        {
            // initialize the vertex with profile coords
            glm::vec3 vert = {p.x, 0.f, p.y};
            
            // rotate the vertex by the angle the direction vector makes with base vector of the profile
            vert = glm::rotate(vert, directionAngleDelta, directionRotationVec);
            // now rotate the vertex by the roll angle
            vert = glm::rotate(vert, ep.roll, direction);
            // move vertex to the position
            vert += ep.position;
            
            mesh.vertices.push_back(vert);
        }
    }

    for (size_t i = 0; i < extrusionPoints.size() - 1; i++)
    {
        for (size_t j = 0; j < profile.size() - 1; j++)
        {
            mesh.indices.push_back(i * profile.size() + j);
            mesh.indices.push_back(i * profile.size() + (j + 1));
            mesh.indices.push_back((i + 1) * profile.size() + (j + 1));

            mesh.indices.push_back(i * profile.size() + j);
            mesh.indices.push_back((i + 1) * profile.size() + (j + 1));
            mesh.indices.push_back((i + 1) * profile.size() + j);
        }

        mesh.indices.push_back(i * profile.size() + profile.size() - 1);
        mesh.indices.push_back(i * profile.size() + 0);
        mesh.indices.push_back((i + 1) * profile.size() + 0);

        mesh.indices.push_back(i * profile.size() + profile.size() - 1);
        mesh.indices.push_back((i + 1) * profile.size() + 0);
        mesh.indices.push_back((i + 1) * profile.size() + profile.size() - 1);
    }

    for (size_t i = 0; i < extrusionPoints.size(); i++)
    {
        for (size_t j = 0; j < profile.size(); j++)
        {
            mesh.uvs.push_back(glm::vec2(0.f, 0.f));
            mesh.uvs.push_back(glm::vec2(1.f, 0.f));
            mesh.uvs.push_back(glm::vec2(1.f, 1.f));
            
            mesh.uvs.push_back(glm::vec2(0.f, 0.f));
            mesh.uvs.push_back(glm::vec2(1.f, 1.f));
            mesh.uvs.push_back(glm::vec2(0.f, 1.f));
        }
    }

    
    return mesh;
}

// All elements besides the first and last in curvePoints are treated as control points
CurveMesh extrudeProfileWithCurve(std::vector<glm::vec2> profile, const std::vector<BezierCurvePoint>& curvePoints, unsigned int segmentCount)
{
    CurveMesh mesh{};

    auto curve = plotBezierCurve(curvePoints, segmentCount);

    if(curve.size() < 2)
    {
        printf("[ERROR][%s(%d)] Not enough points to plot a curve", __FILE__, __LINE__);
        return mesh;
    }

    std::vector<ExtrusionPoint> extrusionPoints(curve.size());

    extrusionPoints.push_back({curve[0], curve[1] - curve[0], 0.f});
    for (size_t i = 1; i < curve.size() - 1; i++)
    {
        extrusionPoints.push_back({curve[i], curve[i + 1] - curve[i - 1], 0.f});
    }
    extrusionPoints.push_back({curve[curve.size() - 1], curve[curve.size() - 1] - curve[curve.size() - 2], 0.f});

    return extrudeProfile(profile, extrusionPoints);
}