#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>


class Mesh
{
private:
    GLuint m_vboVertices;
    GLuint m_vboNormals;
    GLuint m_vboUVs;
    GLuint m_ibo;
    GLuint m_vao;

    size_t m_iboSize;
    size_t m_iboCapacity;

public:
    Mesh();
    ~Mesh();

    void load(const std::vector<glm::vec3>& vertices, 
              const std::vector<glm::vec3>& normals,
              const std::vector<glm::vec2>& uvs,
              const std::vector<unsigned int>& indices);

    void draw();
};