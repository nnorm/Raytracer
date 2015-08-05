#include "Common.h"

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

#include "Camera.h"
#include "Sphere.h"
#include "Raytracer.h"
#include "Ray.h"
#include "Light.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/noise.hpp>
#include <ctime>
#include <sstream>
#include <omp.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

#define MAX_REFLECT_BOUNCES 5

void onKeyPressed(GLFWwindow* w, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS)
		return;

	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(w, GL_TRUE);
		break;

	case GLFW_KEY_L:
		std::cout << "lol" << std::endl;
		break;

	default:
		break;
	}
}

int main(int argc, char** argv)
{
	using namespace glm;

	int glfwInited = glfwInit();
#ifdef _DEBUG
	if (!glfwInited)
		std::cout << "Enable to initialize GLFW" << std::endl;
#endif // _DEBUG

	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raytracer", NULL, NULL);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, onKeyPressed);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
#ifdef _DEBUG
		std::cout << "Unable to initialize GLEW." << std::endl;
		Sleep(2000);
		exit(EXIT_FAILURE);
#endif // _DEBUG
	}

	Raytracer raytracer(WINDOW_WIDTH, WINDOW_HEIGHT);

	Camera c;
	c.viewMatrix = lookAt(vec3(4.5f, 1.5f, 3.5f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
	c.projMatrix = perspective(45.0f, float(WINDOW_WIDTH) / float(WINDOW_HEIGHT), 0.1f, 100.0f);

	Sphere s;
	s.center = vec3(0.0f, 1.0f, 0.0f);
	s.material.albedo = vec3(0.0f);
	s.material.shininess = 512.0f;
	s.material.reflectivity = 1.0f;
	s.material.F0 = vec3(0.72f, 0.45f, 0.2f);
	s.radius = 1.0f;
	raytracer.addObject(&s);

	Sphere s2;
	s2.center = vec3(2.0f, 1.0f, 0.0f);
	s2.material.albedo = vec3(1.0f, 0.5f, 0.5f);
	s2.material.shininess = 512.0f;
	s2.material.reflectivity = 1.0f;
	s2.radius = 1.0f;
	raytracer.addObject(&s2);

	Plane p;
	p.n = vec3(0.0f, 1.0f, 0.0f);
	p.d = 0.0f;
	p.material.albedo = vec3(0.8f);
	p.material.reflectivity = 0.5f;
	raytracer.addObject(&p);

	Light light1;
	light1.position = vec3(10.0f, 10.0f, 0.0f);
	light1.size = 2.5f;
	light1.intensity = 50.0f;
	light1.color = vec3(0.5f, 0.5f, 1.0f);
	raytracer.addLight(&light1);
	
	Light light2;
	light2.position = vec3(-10.0f, 10.0f, 10.0f);
	light2.size = 2.5f;
	light2.intensity = 50.0f;
	light2.color = vec3(1.0f, 0.5f, 0.5f);
	raytracer.addLight(&light2);

	srand(time(NULL));

	float percentage = 0.0f;

	#pragma omp parallel
	#pragma omp master
	{
		std::cout << "Starting on " << omp_get_num_threads() << " threads" << std::endl;
	}

	#pragma omp parallel for
	for (int y = 0; y < WINDOW_HEIGHT; y++)
	{
		for (int x = 0; x < WINDOW_WIDTH; x++)
		{
			float fx = float(x) / float(WINDOW_WIDTH);
			float fy = float(WINDOW_HEIGHT-y) / float(WINDOW_HEIGHT);
			fx = fx * 2.0f - 1.0f;
			fy = fy * 2.0f - 1.0f;
			vec3 ro = c.getO();
			vec3 rd = c.calculateRd(vec2(fx, fy));
			Ray r;
			r.ro = ro;
			r.rd = rd;
			
			raytracer.trace(x, y, r, MAX_REFLECT_BOUNCES);
		}

		percentage += 1.0f / float(WINDOW_HEIGHT);

		std::ostringstream oss;
		oss << (percentage * 100.0f) << " %" << std::endl;
		std::cout << oss.str();
	}

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, raytracer.getData());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	unsigned int vaoId;
	glGenVertexArrays(1, &vaoId);

	unsigned int fgtShd = glCreateShader(GL_FRAGMENT_SHADER);
	unsigned int vtxShd = glCreateShader(GL_VERTEX_SHADER);
	unsigned int program = glCreateProgram();

	const char * vsSource = "#version 330 core\n"
		"in vec3 attrPosition;"
		"out vec2 uv;"
		"vec3 vertices[4] = vec3[]("
			"vec3(-1.0,  1.0, 0.0),"
			"vec3(-1.0, -1.0, 0.0),"
			"vec3( 1.0,  1.0, 0.0),"
			"vec3( 1.0, -1.0, 0.0)"
		");"
		"void main() {"
			"gl_Position = vec4(vertices[gl_VertexID], 1.0);"
			"uv = vertices[gl_VertexID].xy * 0.5 + vec2(0.5);"
			"uv.y = 1.0 - uv.y;"
		"}"
		;

	const char* fsSource = "#version 330 core\n"
		"uniform sampler2D uRaytracedTexture;"
		"in vec2 uv;"
		"out vec4 fragColor;"
		"void main() {"
		"fragColor = texture(uRaytracedTexture, uv);"
		"}"
		;

	glShaderSource(vtxShd, 1, &vsSource, NULL);
	glCompileShader(vtxShd);
	int vsCompiled = 0;
	glGetShaderiv(vtxShd, GL_COMPILE_STATUS, &vsCompiled);
	if (!vsCompiled)
	{
		std::cout << "VS failed to compile" << std::endl;

		char infoLog[8000];
		glGetShaderInfoLog(vtxShd, 8000, nullptr, infoLog);
		std::cout << infoLog << std::endl;
	}

	glShaderSource(fgtShd, 1, &fsSource, NULL);
	glCompileShader(fgtShd);
	int fsCompiled = 0;
	glGetShaderiv(fgtShd, GL_COMPILE_STATUS, &fsCompiled);
	if (!fsCompiled)
	{
		std::cout << "FS failed to compile" << std::endl;
	}

	glAttachShader(program, vtxShd);
	glAttachShader(program, fgtShd);
	glLinkProgram(program);
	int prgLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &prgLinked);
	if (!prgLinked)
	{
		std::cout << "program failed to link" << std::endl;
	}

	do
	{
		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(program, "uRaytracedTexture"), 0);
		glUseProgram(program);
		glBindVertexArray(vaoId);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		glUseProgram(0);

		glBindTexture(GL_TEXTURE_2D, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (!glfwWindowShouldClose(window));

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
