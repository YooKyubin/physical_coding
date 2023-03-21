#include "line.h"

LineUPtr Line::Create(std::vector<glm::vec3>& position) {

	auto line = LineUPtr(new Line());
	line->Init(position);

	return std::move(line);
}

void Line::Init(std::vector<glm::vec3>& position) {

	m_vertexLayout = VertexLayout::Create();
	m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW,
	    position.data(), sizeof(glm::vec3), position.size());

	m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(float) * 3, 0);

}

void Line::UpdatePosition(glm::vec3 position){

}

void Line::Draw(const Program* program) const {
	m_vertexLayout->Bind();

	glDrawArrays(m_primitiveType, 0, m_vertexBuffer->GetCount());
}

LineUPtr Line::CreateLine(uint32_t pointCount) {
    std::vector<glm::vec3> position(pointCount);

    for (int i=0; i < pointCount; i++){
        position[i] = glm::vec3(1.0f* i, 0.3f * i * i, 0.0f);
    }


    return Create(position);
}

