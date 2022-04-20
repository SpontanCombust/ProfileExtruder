#pragma once

#include <glm/glm.hpp>

#include <vector>


struct SBezierCurvePoint
{
    glm::vec3 position;
    float ratio;
};

// All elements besides the first and last are treated as control points
std::vector<glm::vec3> plotBezierCurve(const std::vector<SBezierCurvePoint>& points, unsigned int segmentCount);