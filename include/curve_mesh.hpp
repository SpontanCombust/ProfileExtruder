#pragma once

#include "bezier_curve.hpp"

#include <glm/glm.hpp>

#include <vector>


struct CurveMesh
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;
};


struct ExtrusionPoint
{
    glm::vec3 position;
    glm::vec3 direction;
    float roll;
};

CurveMesh extrudeProfile(std::vector<glm::vec2> profile, const std::vector<ExtrusionPoint>& extrusionPoints);

// All elements besides the first and last in curvePoints are treated as control points
CurveMesh extrudeProfileWithCurve(std::vector<glm::vec2> profile, const std::vector<BezierCurvePoint>& curvePoints, unsigned int segmentCount);