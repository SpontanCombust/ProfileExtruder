#pragma once

#include <glm/glm.hpp>

#include <vector>


struct BezierCurvePoint
{
    glm::vec3 position;
    float ratio;
};

// All elements besides the first and last are treated as control points
std::vector<glm::vec3> plotBezierCurve(const std::vector<BezierCurvePoint>& points, unsigned int segmentCount);