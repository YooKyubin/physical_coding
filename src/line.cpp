#include "line.h"

LineUPtr Line::Create(std::vector<glm::vec3>& vertexPos) {

	auto line = LineUPtr(new Line());
	line->Init(vertexPos);

	return std::move(line);
}

void Line::Init(std::vector<glm::vec3>& vertexPos) {

	m_vertexLayout = VertexLayout::Create();
	m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW,
	    vertexPos.data(), sizeof(glm::vec3), vertexPos.size());

	m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(float) * 3, 0);
	m_vertexPos = vertexPos;

}

void Line::UpdatePosition(glm::vec3 newPosition){
	std::vector<glm::vec3> temp = std::vector<glm::vec3>(m_vertexPos.begin()+1, m_vertexPos.end());
	m_vertexPos.clear();
	temp.push_back(newPosition);
	m_vertexPos = temp;
	m_vertexBuffer->Bind();
	// glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_vertexPos.data()), m_vertexPos.data());
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * m_vertexPos.size(), m_vertexPos.data());
}

void Line::Draw(const Program* program) const {
	m_vertexLayout->Bind();

	glDrawArrays(m_primitiveType, 0, m_vertexBuffer->GetCount());
}

LineUPtr Line::CreateLine(uint32_t pointCount) {
    std::vector<glm::vec3> vertexPos(pointCount);

    for (int i=0; i < pointCount; i++){
		// 임시 코드입니다 조금 더러워도 양해부탁! 데헷뀽
		float t = i;
		t = t - pointCount / 2;
		t = t * 0.1;
        vertexPos[i] = glm::vec3(1.0f * t, 0.3f * t * t, 0.0f);
    }

    return Create(vertexPos);
}

