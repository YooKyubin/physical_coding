#include "context.h"
#include "image.h"
#include <imgui.h>

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

void Context::ProcessInput(GLFWwindow* window) {
    if (!m_cameraControl)
	    return;

	const float cameraSpeed = 0.05f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		m_cameraPos += cameraSpeed * m_cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m_cameraPos -= cameraSpeed * m_cameraFront;
	
	auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m_cameraPos += cameraSpeed * cameraRight;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		m_cameraPos -= cameraSpeed * cameraRight;    
	
	auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		m_cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		m_cameraPos -= cameraSpeed * cameraUp;
}

void Context::Reshape(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Context::MouseMove(double x, double y) {
    if (!m_cameraControl)
	    return;
	auto pos = glm::vec2((float)x, (float)y);
	auto deltaPos = pos - m_prevMousePos;
	
	const float cameraRotSpeed = 0.5f;
	m_cameraYaw -= deltaPos.x * cameraRotSpeed;
	m_cameraPitch -= deltaPos.y * cameraRotSpeed;
	
	if (m_cameraYaw < 0.0f)   m_cameraYaw += 360.0f;
	if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;
	
	if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
	if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

	m_prevMousePos = pos;    
}

void Context::MouseButton(int button, int action, double x, double y) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
	    if (action == GLFW_PRESS) {
			// 마우스 조작 시작 시점에 현재 마우스 커서 위치 저장
			m_prevMousePos = glm::vec2((float)x, (float)y);
			m_cameraControl = true;
	    }
		else if (action == GLFW_RELEASE) {
			m_cameraControl = false;
	    }
	}
}

bool Context::Init() {
   glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	m_box = Mesh::CreateBox();
	m_plane = Mesh::CreatePlane();
	m_sphere = Mesh::CreateSphere();

	
	m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");
	m_lightProgram = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
	m_directLightProgram = Program::Create("./shader/lighting.vs", "./shader/direct_lighting.fs");
 

	Framebuffer::BindToDefault();
	glViewport(0, 0, m_width, m_height);

    // 시뮬레이션 상태 초기화
    currentTime = glfwGetTime();
    accumulator = 0.0;

	ball = Matter::Create(
		m_sphere.get(), m_directLightProgram.get(), 
		glm::vec3(1.0f),
		glm::vec3(1.0f, 0.5f, 0.0f),
		glm::vec3(-2.0f, 0.0f, 0.0f),
		glm::vec3(0.0f),
		glm::vec3(1.0f, 0.01f, 0.31f)
	);
	ball->m_position.x = 10.0f;
	ball->m_acc.x = 0.1f;

	plane = Matter::Create(m_box.get(), m_directLightProgram.get());
	plane->m_size = glm::vec3(10.0f, 0.3f, 10.0f);

	ball2 = Matter::Create(m_box.get(), m_directLightProgram.get());
	ball2->m_position = glm::vec3(-10.0f, 0.5f, -0.1f);

	return true;
}



void Context::Render() {
	if (ImGui::Begin("ui window")) {
	    ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
	    ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
	    ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);
	    ImGui::Separator();
	    if (ImGui::Button("reset camera")) {
			m_cameraYaw = 0.0f;
			m_cameraPitch = 0.0f;
			m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		}

		ImGui::Text("current time:: %.2f", currentTime);
		ImGui::DragFloat3("ball pos", glm::value_ptr(ball->m_position), 0.0f);

	}
	ImGui::End();

	m_cameraFront =
	    glm::rotate(glm::mat4(1.0f),
			glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
	    glm::rotate(glm::mat4(1.0f),
			glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
	    glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

	auto projection = glm::perspective(glm::radians(45.0f),
		(float)m_width / (float)m_height, 0.01f, 150.0f);
	auto view = glm::lookAt(
		m_cameraPos,
		m_cameraPos + m_cameraFront,
		m_cameraUp);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 0.85f, 0.89f, 1.0f);

	// 이전 프레임 이후 경과 시간 구하기
	double newTime = glfwGetTime();
	double frameTime = newTime - currentTime;
	currentTime = newTime;

	// 프레임 간격을 누적 변수에 추가
	accumulator += frameTime;

	// 고정된 시간 간격으로 시뮬레이션 상태 업데이트
	int log = 0;
	while (accumulator >= timeStep)
	{
		++log;
		// 시간 간격 크기를 사용하여 시뮬레이션 업데이트
		ball->m_velocity += ball->m_acc * (float)timeStep;
		ball->m_position += ball->m_velocity * (float)timeStep;

		// 누적 변수에서 시간 간격 크기를 뺌
		accumulator -= timeStep;
	}
	SPDLOG_INFO("cnt: {}", log);

	ball->Draw(view, projection, m_cameraPos);
	plane->Draw(view, projection, m_cameraPos);
	ball2->Draw(view, projection, m_cameraPos);


}
