#include "mesh.hpp"

Mesh::Mesh()
{
    glCreateBuffers(1, &m_vboVertices);
    glCreateBuffers(1, &m_vboNormals);
    // glCreateBuffers(1, &m_vboUVs);
    glCreateBuffers(1, &m_ibo);
    glCreateVertexArrays(1, &m_vao);

    glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboNormals);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        // glBindBuffer(GL_ARRAY_BUFFER, m_vboUVs);
        // glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
        // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
    glBindVertexArray(0);

    m_iboSize = m_iboCapacity = 0;
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_vboVertices);
    glDeleteBuffers(1, &m_vboNormals);
    // glDeleteBuffers(1, &m_vboUVs);
    glDeleteBuffers(1, &m_ibo);
    glDeleteVertexArrays(1, &m_vao);
}

void Mesh::load(const std::vector<glm::vec3>& vertices, 
                const std::vector<glm::vec3>& normals,
                // const std::vector<glm::vec2>& uvs,
                const std::vector<unsigned int>& indices)
{
    m_iboSize = indices.size();

    if(m_iboSize > m_iboCapacity)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboNormals);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);

        // glBindBuffer(GL_ARRAY_BUFFER, m_vboUVs);
        // glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        m_iboCapacity = m_iboSize;
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), vertices.data());

        glBindBuffer(GL_ARRAY_BUFFER, m_vboNormals);
        glBufferSubData(GL_ARRAY_BUFFER, 0, normals.size() * sizeof(glm::vec3), normals.data());

        // glBindBuffer(GL_ARRAY_BUFFER, m_vboUVs);
        // glBufferSubData(GL_ARRAY_BUFFER, 0, uvs.size() * sizeof(glm::vec2), uvs.data());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(unsigned int), indices.data());
    }
}

void Mesh::setMaterial(const glm::vec3& diffuse, const glm::vec3& specular, float shininess)
{
    m_diffuse = diffuse;
    m_specular = specular;
    m_shininess = shininess;
}

const glm::vec3& Mesh::getMaterialDiffuse() const
{
    return m_diffuse;
}

const glm::vec3& Mesh::getMaterialSpecular() const
{
    return m_specular;
}

float Mesh::getMaterialShininess() const
{
    return m_shininess;
}

void Mesh::draw()
{
    glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_iboSize, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
