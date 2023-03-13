#ifndef __MATTER_H__
#define __MATTER_H__

#include "common.h"
#include "mesh.h"
#include "program.h"

CLASS_PTR(Matter);
class Matter {
public:
	static MatterUPtr Create(
        const Mesh* mesh, Program* program, 
        glm::vec3 size = glm::vec3(1.0f),
        glm::vec3 position = glm::vec3(0.0f),
        glm::vec3 velocity = glm::vec3(0.0f),
        glm::vec3 acc = glm::vec3(0.0f),
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f)
    );

    void Draw(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& viewPos);

    glm::vec3 m_size;
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_acc;
	
private:
	Matter() {}
	void Init(
        const Mesh* mesh, Program* program, 
        glm::vec3 size,
        glm::vec3 position,
        glm::vec3 velocity,
        glm::vec3 acc,
        glm::vec3 color
    );

    void setColor(glm::vec3 color) { m_color = color; };

    const Mesh* m_mesh;
    Program* m_program;
    glm::vec3 m_color;

};

#endif // __MATTER_H__