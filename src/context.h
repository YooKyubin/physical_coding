#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"
#include "mesh.h"
#include "model.h"
#include "framebuffer.h"
#include "shadow_map.h"
#include "matter.h"


CLASS_PTR(Context)
class Context {
public:
	static ContextUPtr Create();
	void Render();
	void ProcessInput(GLFWwindow* window);
	void Reshape(int width, int height);
	void MouseMove(double x, double y);
	void MouseButton(int button, int action, double x, double y);

private:
	Context() {}
	bool Init();
	
	ProgramUPtr m_simpleProgram;
	ProgramUPtr m_directLightProgram;
	ProgramUPtr m_lightProgram;
	
	MeshUPtr m_box;
	MeshUPtr m_plane;
    MeshUPtr m_sphere;

    struct Light {
	    glm::vec3 position { glm::vec3(0.0f, 0.0f, 0.0f) };
	    glm::vec3 color { glm::vec3(1.0f, 1.0f, 1.0f) };
	};
	std::vector<Light> m_lights;
	Light m_light;

	MaterialUPtr m_material;
	
	// screen size
	int m_width {640};
	int m_height {480};
	
	// camera parameter
	bool m_cameraControl { false };
	glm::vec2 m_prevMousePos { glm::vec2(0.0f) };
	float m_cameraPitch { 0.0f };
	float m_cameraYaw { 0.0f };
	glm::vec3 m_cameraFront { glm::vec3(0.0f, -1.0f, 0.0f) };
	glm::vec3 m_cameraPos { glm::vec3(0.0f, 0.0f, 40.0f) };
	glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };

	const double timeStep = 1.0 / 50.0; // 60fps
	double currentTime;
    double accumulator;

	MatterUPtr ball;
	MatterUPtr plane;
	MatterUPtr ball2;
};

#endif // __CONTEXT_H__