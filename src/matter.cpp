#include "matter.h"

MatterUPtr Matter::Create(
    const Mesh* mesh, Program* program, 
    glm::vec3 size,
    glm::vec3 position,
    glm::vec3 velocity,
    glm::vec3 acc,
    glm::vec3 color
){
    auto matter = MatterUPtr(new Matter());
    matter->Init(
        mesh, program, size, position, velocity, acc, color
    );

    return std::move(matter);
}

void Matter::Init(
    const Mesh* mesh, Program* program, 
    glm::vec3 size,
    glm::vec3 position,
    glm::vec3 velocity,
    glm::vec3 acc,
    glm::vec3 color
){
    m_mesh = mesh;
    m_program = program;
    m_size = size;
    m_position = position;
    m_velocity = velocity;
    m_acc = acc;
    m_color = color;
}

void Matter::Draw(const glm::mat4& view, const glm::mat4& projection){
    glm::vec3 direction = glm::vec3(0.5f, 1.0f, 0.5f);
    m_program->Use();
    auto modelTransform = 
        glm::translate(glm::mat4(1.0f), m_position) * 
        glm::scale(glm::mat4(1.0f), m_size);
    auto transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    // m_program->SetUniform("color", glm::vec4(m_color, 1.0f));
    m_program->SetUniform("modelTransform", modelTransform);
    m_program->SetUniform("color", m_color);
    m_program->SetUniform("direction", direction);
    m_mesh->Draw(m_program);
}