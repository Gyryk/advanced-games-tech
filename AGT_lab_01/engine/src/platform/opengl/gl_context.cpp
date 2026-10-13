#include "pch.h"
#include "platform/opengl/gl_context.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"

engine::gl_context::gl_context(GLFWwindow* window_handle)
	: m_window_handle(window_handle)
{
	CORE_ASSERT(m_window_handle, "[gl_context] window handle is null!");
}

void engine::gl_context::init()
{
	glfwMakeContextCurrent(m_window_handle);
	const int status = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	CORE_ASSERT(status, "Could not load Glad!");

	// glGetString returns const GLubyte*. fmt 11+ refuses to format non-void
	// pointers, so say explicitly that we want the string, not the address.
	LOG_CORE_INFO("OpenGL Info:");
	LOG_CORE_INFO("\tVendor: {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
	LOG_CORE_INFO("\tRenderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
	LOG_CORE_INFO("\tVersion: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
}

void engine::gl_context::swap_buffers()
{
	glfwSwapBuffers(m_window_handle);
}
