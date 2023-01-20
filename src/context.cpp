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
    m_framebuffer = Framebuffer::Create(Texture::Create(width, height, GL_RGBA));
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

    m_box = Mesh::CreateBox();
    m_plane = Mesh::CreatePlane();
    
    // program 선언
    m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");
    if (!m_simpleProgram)
        return false;
    SPDLOG_INFO("program id: {}", m_simpleProgram->Get()); 

    m_program = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get()); 

    m_textureProgram = Program::Create("./shader/texture.vs", "./shader/texture.fs");
	if (!m_textureProgram)
		return false;
    SPDLOG_INFO("program id: {}", m_textureProgram->Get()); 

    m_postProgram = Program::Create("./shader/texture.vs", "./shader/gamma.fs");
	if (!m_postProgram)
	    return false;

    m_skyboxProgram = Program::Create("./shader/skybox.vs", "./shader/skybox.fs");
	if (!m_skyboxProgram)
	    return false;

    m_envMapProgram = Program::Create("./shader/env_map.vs", "./shader/env_map.fs");

    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

    TexturePtr darkGrayTexture = Texture::CreateFromImage(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)).get());

    TexturePtr grayTexture = Texture::CreateFromImage(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());
            
    m_planeMaterial = Material::Create();
    m_planeMaterial->diffuse = Texture::CreateFromImage(Image::Load("./image/marble.png").get());
    m_planeMaterial->specular = grayTexture;
    m_planeMaterial->shininess = 128.0f;

    m_box1Material = Material::Create();
    m_box1Material->diffuse = Texture::CreateFromImage(Image::Load("./image/container.jpg").get());
    m_box1Material->specular = darkGrayTexture;
    m_box1Material->shininess = 16.0f;

    m_box2Material = Material::Create();
    m_box2Material->diffuse = Texture::CreateFromImage(Image::Load("./image/container2.png").get());
    m_box2Material->specular = Texture::CreateFromImage(Image::Load("./image/container2_specular.png").get());
    m_box2Material->shininess = 64.0f;
    
    m_windowTexture = Texture::CreateFromImage(
        Image::Load("./image/blending_transparent_window.png").get());

    auto cubeRight = Image::Load("./image/skybox/right.jpg", false);
	auto cubeLeft = Image::Load("./image/skybox/left.jpg", false);
	auto cubeTop = Image::Load("./image/skybox/top.jpg", false);
	auto cubeBottom = Image::Load("./image/skybox/bottom.jpg", false);
	auto cubeFront = Image::Load("./image/skybox/front.jpg", false);
	auto cubeBack = Image::Load("./image/skybox/back.jpg", false);
	m_cubeTexture = CubeTexture::CreateFromImages({
	    cubeRight.get(),
	    cubeLeft.get(),
	    cubeTop.get(),
	    cubeBottom.get(),
	    cubeFront.get(),
	    cubeBack.get(),
	});

    return true;
}





void Context::Render() {
    if (ImGui::Begin("ui window")) {
        if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor))) {
            glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
        }
        ImGui::DragFloat("gamma", &m_gamma, 0.01f, 0.0f, 2.0f);
        ImGui::Separator();
        ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
        ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);
        ImGui::Separator();
        if (ImGui::Button("reset camera")) {
            m_cameraYaw = 0.0f;
            m_cameraPitch = -20.0f;
            m_cameraPos = glm::vec3(0.0f, 2.5f, 8.0f);
        }
        if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
            ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction), 0.01f);
            ImGui::DragFloat("l.cutoff_inner", &m_light.cutoff[0], 0.5f, 0.0f, 150.0f);
            ImGui::DragFloat("l.cutoff_offset", &m_light.cutoff[1], 0.5f, 0.0f, 30.0f);
            ImGui::DragFloat("l.distance", &m_light.distance, 0.5f, 0.0f, 3000.0f);
            ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
            ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
            ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));
            ImGui::Checkbox("flash light", &m_flashLightMode);
        }
        ImGui::Checkbox("animation", &m_animation);
        ImGui::Checkbox("depth test", &m_depthTest);

        float aspectRatio = (float)m_width / (float)m_height;
        // ImGui::Image((void*)m_framebuffer->GetColorAttachment()->Get(),
        ImGui::Image((ImTextureID)m_framebuffer->GetColorAttachment()->Get(),
            ImVec2(150 * aspectRatio, 150));
    }
    ImGui::End();

    m_framebuffer->Bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    m_cameraFront =
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) * 
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) * 
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    
    // projection
    auto projection = glm::perspective(glm::radians(45.0f),
        (float)m_width / (float)m_height, 0.1f, 80.0f); // near, far
    
    // view
	auto view = glm::lookAt(
		m_cameraPos,
		m_cameraPos + m_cameraFront,
		m_cameraUp);
    
    // cube map
    glDepthMask(GL_FALSE);
    auto skyboxModelTransform =
		glm::translate(glm::mat4(1.0), m_cameraPos * 0.8f) *
		glm::scale(glm::mat4(1.0), glm::vec3(50.0f));
	m_skyboxProgram->Use();
	m_cubeTexture->Bind();
	m_skyboxProgram->SetUniform("skybox", 0);
	m_skyboxProgram->SetUniform("transform", projection * view * skyboxModelTransform);
	m_box->Draw(m_skyboxProgram.get());
    glDepthMask(GL_TRUE);


    //손전등 시뮬레이션
    glm::vec3 lightPos = m_light.position;
    glm::vec3 lightDir = m_light.direction;
    if(m_flashLightMode) {
        lightPos = m_cameraPos;
        lightDir = m_cameraFront;
    }
    else{
        // 광원 큐브
        auto lightModelTransform =
            glm::translate(glm::mat4(1.0), m_light.position) *
            glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
        m_simpleProgram->Use();
        m_simpleProgram->SetUniform("color", glm::vec4(m_light.ambient + m_light.diffuse, 1.0f));
        m_simpleProgram->SetUniform("transform", projection * view * lightModelTransform);
        // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        m_box->Draw(m_simpleProgram.get());
    }

    // model
    m_program->Use();

    m_program->SetUniform("viewPos", m_cameraPos);
    m_program->SetUniform("light.position", lightPos);
    m_program->SetUniform("light.direction", lightDir);
    m_program->SetUniform("light.cutoff", glm::vec2(
        cosf(glm::radians(m_light.cutoff[0])),
        cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
    m_program->SetUniform("light.attenuation",  GetAttenuationCoeff(m_light.distance));
    m_program->SetUniform("light.ambient", m_light.ambient);
    m_program->SetUniform("light.diffuse", m_light.diffuse);
    m_program->SetUniform("light.specular", m_light.specular);
    
    //plane
    auto modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));
    auto transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_planeMaterial->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());

    // envmap cube
    modelTransform =
	    glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.75f, -2.0f)) *
	    glm::rotate(glm::mat4(1.0f), glm::radians(40.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
	    glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
	m_envMapProgram->Use();
	m_envMapProgram->SetUniform("model", modelTransform);
	m_envMapProgram->SetUniform("view", view);
	m_envMapProgram->SetUniform("projection", projection);
	m_envMapProgram->SetUniform("cameraPos", m_cameraPos);
	m_cubeTexture->Bind();
	m_envMapProgram->SetUniform("skybox", 0);
	m_box->Draw(m_envMapProgram.get());

    // box1
    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.75f, -4.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    m_program->Use();
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_box1Material->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    // box2
    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.75f, 2.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * 
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    m_program->Use();
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_box2Material->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    if (!m_depthTest){
        glDisable(GL_DEPTH_TEST);
    }
    
    // box2 stencil
    m_simpleProgram->Use();
    m_simpleProgram->SetUniform("color", glm::vec4(1.0f, 1.0f, 0.5f, 1.0f));
    m_simpleProgram->SetUniform(
        "transform", transform * glm::scale(glm::mat4(1.0f), glm::vec3(1.05f, 1.05f, 1.05f)));
    m_box->Draw(m_simpleProgram.get());

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    // blending
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	m_textureProgram->Use();
	m_windowTexture->Bind();
	m_textureProgram->SetUniform("tex", 0);
	
	modelTransform =
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 4.0f));
	transform = projection * view * modelTransform;
	m_textureProgram->SetUniform("transform", transform);
	m_plane->Draw(m_textureProgram.get());

    modelTransform =
		glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.5f, 5.0f));
	transform = projection * view * modelTransform;
	m_textureProgram->SetUniform("transform", transform);
	m_plane->Draw(m_textureProgram.get());
	
	modelTransform =
		glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.5f, 6.0f));
	transform = projection * view * modelTransform;
	m_textureProgram->SetUniform("transform", transform);
	m_plane->Draw(m_textureProgram.get());
    glDisable(GL_BLEND);

    Framebuffer::BindToDefault();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    m_postProgram->Use();
    m_postProgram->SetUniform("transform",
        glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f)));
    m_framebuffer->GetColorAttachment()->Bind();
    m_postProgram->SetUniform("tex", 0);
    m_postProgram->SetUniform("gamma", m_gamma);
    m_plane->Draw(m_postProgram.get()); 
}