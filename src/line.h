#ifndef __LINE_H__
#define __LINE_H__

#include "common.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "program.h"

CLASS_PTR(Line);
class Line {
public:
	static LineUPtr Create(std::vector<glm::vec3>& position);
	static LineUPtr CreateLine(uint32_t pointCount = 100);


	const VertexLayout* GetVertexLayout() const { return m_vertexLayout.get(); }
	BufferPtr GetVertexBuffer() const { return m_vertexBuffer; }

	void UpdatePosition(glm::vec3 position);

	void Draw(const Program* program) const;

private:
	Line() {}
	void Init(std::vector<glm::vec3>& vertices);

	uint32_t m_primitiveType { GL_LINE_STRIP };
	VertexLayoutUPtr m_vertexLayout;
	BufferPtr m_vertexBuffer;

};

#endif // __LINE_H__

/*
선의 경로를 결정합니다: 선이 객체를 추적하기 위해 따라야 할 궤적을 결정합니다. 이것은 단순한 직선일 수도 있고 곡선과 같은 더 복잡한 경로일 수도 있다.

경로 점을 저장합니다: 선의 경로를 구성하는 점을 저장합니다. 이러한 점은 OpenGL에서 선을 긋는 데 사용됩니다.

정점 버퍼 객체 생성: OpenGL에서 VBO(Vertex Buffer Object)를 생성하여 경로 점을 저장합니다.

정점 셰이더 만들기: 정점 셰이더를 만들어 경로 점을 월드 좌표에서 화면 좌표로 변환합니다.

조각 셰이더 만들기: 조각 셰이더를 만들어 선의 색상과 투명도를 설정합니다.

정점 및 조각 셰이더 바인딩: 정점 및 조각 셰이더를 함께 바인딩하여 셰이더 프로그램을 만듭니다.

VBO 바인딩: VBO를 바인딩하여 경로 포인트를 저장합니다.

선 긋기: 셰이더 프로그램과 VBO를 사용하여 적절한 인수로 glDrawArrays 함수를 호출하여 선을 그립니다.

경로 지점 업데이트: 개체의 현재 위치와 궤적을 기준으로 경로 지점을 업데이트합니다.

반복: 추적 라인을 계속 업데이트하고 그리려면 8단계와 9단계를 계속 반복하십시오.

glBufferSubData로 VBO의 데이털르 변경한다

vector 슬라이싱을 이용해서 큐처럼 구현이 가능하다. v.begin(), v.end()+1
*/