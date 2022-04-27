#include "curve_mesh.hpp"

#include <glm/gtx/rotate_vector.hpp>

#include <algorithm> // std::for_each
#include <cmath> // std::acos
#include <cstdio>


const glm::vec3 PROFILE_NORMAL = glm::vec3(0.f, 0.f, 1.f);

CurveMeshData extrudeProfile(std::vector<glm::vec2> profile, const std::vector<ExtrusionPoint>& extrusionPoints)
{
    CurveMeshData mesh{};

    if(extrusionPoints.size() < 2)
    {
        printf("[ERROR][%s(%d)] Not enough points to construct a mesh", __FILE__, __LINE__);
        return mesh;
    }

    // so that indexing is easier and texture wrapping across a segment is possible
    profile.push_back(profile[0]);
    const int profileSize = profile.size();



    // ============= VERTICES ============= //
    // each vertex of the profile is being transformed for every extrusion point
    // and added to the `vertices` vector
    for(const auto& ep : extrusionPoints)
    {
        glm::vec3 direction = glm::normalize(ep.direction);
        // the angle of rotation of the direction vector
        float rotationAngle = std::acos(glm::dot(PROFILE_NORMAL, direction));
        // the vector around which said direction vector is rotated
        glm::vec3 rotationNormal;
        // avoid calculating with NaN value
        if(direction == PROFILE_NORMAL || direction == -PROFILE_NORMAL)
        {
            rotationNormal = {1.f, 0.f, 0.f};
        }
        else
        {
            rotationNormal = glm::cross(PROFILE_NORMAL, direction);
        }
        
        for (size_t j = 0; j < profileSize - 1; j++)
        {
            // initialize the vertex with profile coords
            glm::vec3 vert = {profile[j].x, profile[j].y, 0.f};
            
            // rotate the vertex by the angle the direction vector makes with base vector of the profile
            vert = glm::rotate(vert, rotationAngle, rotationNormal);
            // now rotate the vertex by the roll angle
            vert = glm::rotate(vert, ep.roll, direction);
            // move vertex to the position
            vert += ep.position;
            
            mesh.vertices.push_back(vert);
        }
        mesh.vertices.push_back( *(mesh.vertices.end() - (profileSize - 1)) ); // for that one repeated vertex
    }



    // ============= UVS ============= //
    for (size_t i = 0; i < extrusionPoints.size(); i++)
    {
        // a texture will be wrapped around a single segment and will repeat with every segment
        for (size_t j = 0; j < profileSize; j++)
        {
            mesh.uvs.push_back(glm::vec2(
                float(j) / float(profileSize - 1),
                float(i)
            ));
        }
    }



    // ============= NORMALS ============= //
    // calculate normal based on the faces of the next curve mesh segment
    auto calcNormalAfter = [&](int i, int j) -> glm::vec3 {
        glm::vec3 vThis = mesh.vertices[i * profileSize + j];

        glm::vec3 vRight = mesh.vertices[i * profileSize + j + 1];
        
        glm::vec3 vUp = mesh.vertices[(i + 1) * profileSize + j];
        
        glm::vec3 vLeft;
        if(j > 0) {
            vLeft = mesh.vertices[i * profileSize + j - 1];
        } else {
            vLeft = mesh.vertices[(i + 1) * profileSize - 2];
        }

        glm::vec3 n1 = glm::cross(vRight - vThis, vUp - vThis);
        glm::vec3 n2 = glm::cross(vUp - vThis, vLeft - vThis);

        return glm::normalize(n1 + n2);
    };

    // calculate normal based on the faces of the previous curve mesh segment
    auto calcNormalBefore = [&](int i, int j) -> glm::vec3 {
        glm::vec3 vThis = mesh.vertices[i * profileSize + j];

        glm::vec3 vLeft;
        if(j > 0) {
            vLeft = mesh.vertices[i * profileSize + j - 1];
        } else {
            vLeft = mesh.vertices[(i + 1) * profileSize - 2];
        }

        glm::vec3 vDown = mesh.vertices[(i - 1) * profileSize + j];

        glm::vec3 vRight = mesh.vertices[i * profileSize + j + 1];

        glm::vec3 n1 = glm::cross(vLeft - vThis, vDown - vThis);
        glm::vec3 n2 = glm::cross(vDown - vThis, vRight - vThis);

        return glm::normalize(n1 + n2);
    };


    for (size_t j = 0; j < profile.size(); j++)
    {
        mesh.normals.push_back(calcNormalAfter(0, j));
    }
    mesh.normals.push_back( *(mesh.normals.end() - (profileSize - 1)) ); // for that one repeated vertex

    for (size_t i = 1; i < extrusionPoints.size() - 1; i++)
    {
        for (size_t j = 0; j < profile.size() - 1; j++)
        {
            glm::vec3 nBefore = calcNormalBefore(i, j);
            glm::vec3 nAfter = calcNormalAfter(i, j);
            mesh.normals.push_back(glm::normalize(nBefore + nAfter));
        }
        mesh.normals.push_back( *(mesh.normals.end() - (profileSize - 1)) );
    }

    for (size_t j = 0; j < profile.size(); j++)
    {
        mesh.normals.push_back(calcNormalBefore(extrusionPoints.size() - 1, j));
    }
    mesh.normals.push_back( *(mesh.normals.end() - (profileSize - 1)) );


    // ============= INDICES ============= //
    for (size_t i = 0; i < extrusionPoints.size() - 1; i++)
    {
        for (size_t j = 0; j < profileSize - 1; j++)
        {
            mesh.indices.push_back(i * profileSize + j);
            mesh.indices.push_back(i * profileSize + (j + 1));
            mesh.indices.push_back((i + 1) * profileSize + (j + 1));

            mesh.indices.push_back(i * profileSize + j);
            mesh.indices.push_back((i + 1) * profileSize + (j + 1));
            mesh.indices.push_back((i + 1) * profileSize + j);
        }
    }



    return mesh;
}

// All elements besides the first and last in curvePoints are treated as control points
CurveMeshData extrudeProfileWithCurve(const std::vector<glm::vec2>& profile, const std::vector<BezierCurvePoint>& curvePoints, unsigned int segmentCount)
{
    auto curve = plotBezierCurve(curvePoints, segmentCount);

    if(curve.size() < 2)
    {
        printf("[ERROR][%s(%d)] Not enough points to plot a curve", __FILE__, __LINE__);
        return CurveMeshData{};
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